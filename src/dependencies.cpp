#include "dependencies.h"
#include "util.h"

#include <algorithm>
#include <unordered_map>

#include <elfio/elfio.hpp>

class Resolver {
    friend std::vector<Library> resolve(const std::filesystem::path& filepath);

private:
    void resolve(const std::filesystem::path& filepath);
    void collect_dependencies(const std::filesystem::path& filepath);

private:
    std::unordered_map<LibraryName, Library> libraries;
    std::vector<std::filesystem::path> search_paths { "/lib", "/usr/lib" };
};

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

    std::sort(result.begin(), result.end());

    return result;
}
