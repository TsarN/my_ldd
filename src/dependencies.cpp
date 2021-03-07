#include "dependencies.h"
#include "util.h"

#include <elfio/elfio.hpp>

Library::Library(std::string name) : name(std::move(name)) {}

bool Library::is_resolved() const noexcept {
    return path.has_value();
}

void Library::resolve(std::filesystem::path path) noexcept {
    this->path = std::move(path);
}

const std::string& Library::get_name() const noexcept {
    return name;
}

const std::filesystem::path& Library::get_path() const noexcept {
    return path.value();
}

void Resolver::resolve(const std::filesystem::path& filename) {
    collect_dependencies(filename);

    for (const auto &directory : search_paths) {
        for (auto &p: std::filesystem::directory_iterator(directory)) {
            auto path = p.path();

            if (!is_elf(path)) {
                continue;
            }

            auto name = p.path().filename().string();

            auto lib = libraries.find(name);
            if (lib != libraries.end() && !lib->second.is_resolved()) {
                lib->second.resolve(path);
                resolve(path);
            }
        }
    }
}

void Resolver::collect_dependencies(const std::filesystem::path& filename) {
    ELFIO::elfio reader;
    reader.load(filename);

    for (auto &section: reader.sections) {
        if (section->get_type() == SHT_DYNAMIC) {
            ELFIO::dynamic_section_accessor accessor(reader, section);

            for (ELFIO::Elf_Xword i = 0; i < accessor.get_entries_num(); i++) {
                ELFIO::Elf_Xword tag, value;
                std::string name;
                accessor.get_entry(i, tag, value, name);
                if (tag == DT_NEEDED) {
                    libraries.emplace(name, Library(name));
                    if (std::filesystem::path{name}.is_absolute()) {
                        libraries.at(name).resolve(name);
                    }
                }
            }
        }
    }
}

std::vector<Library> resolve(const std::filesystem::path& filepath) {
    std::vector<Library> result;

    Resolver resolver;
    resolver.resolve(filepath);

    for (auto& [_, library] : resolver.libraries) {
        result.emplace_back(std::move(library));
    }

    return result;
}
