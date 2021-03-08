#include "dependencies.h"
#include "util.h"

#include <algorithm>
#include <unordered_map>

#include <elfio/elfio.hpp>

class Resolver {
    explicit Resolver(const std::filesystem::path& filepath);

    friend std::vector<Library> resolve(const std::filesystem::path& filepath, std::unordered_map<symbol, std::string>& symbols);

private:
    void resolve(const std::filesystem::path& filepath);
    void collect_dependencies(const std::filesystem::path& filepath);
    std::unordered_map<symbol, std::string> predict_dependencies() const noexcept;

private:
    std::filesystem::path target;
    std::unordered_map<LibraryName, Library> libraries;
    std::vector<std::filesystem::path> search_paths { "/lib", "/usr/lib", "/lib/x86_64-linux-gnu/" };
};

std::unordered_set<symbol> Library::get_symbols(bool dynamic_only) const noexcept {
    ELFIO::elfio reader;
    if (!is_resolved()) return std::unordered_set<symbol>();
    reader.load(get_path());

    std::unordered_set<symbol> my_symbols;
    for (auto &section: reader.sections) {
        if ((!dynamic_only && section->get_type() == SHT_SYMTAB) || section->get_type() == SHT_DYNSYM) {
            ELFIO::symbol_section_accessor accessor(reader, section);

            for (ELFIO::Elf_Xword i = 0; i < accessor.get_symbols_num(); i++) {
                // TODO do i need some other params?
                std::string name;
                ELFIO::Elf64_Addr value;
                ELFIO::Elf_Xword size;
                unsigned char bind;
                unsigned char type;
                ELFIO::Elf_Half section_index;
                unsigned char other;

                accessor.get_symbol(i, name, value, size, bind, type, section_index, other);

                if (!dynamic_only && bind != STB_GLOBAL) continue;

                my_symbols.insert(name);
            }
        }
    }

    return my_symbols;
}

void Resolver::resolve(const std::filesystem::path& filename) {
    collect_dependencies(filename);

    for (const auto &directory : search_paths) {
        try {
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
        } catch(std::filesystem::filesystem_error& error) {
            continue;
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

std::vector<Library> resolve(const std::filesystem::path& filepath, std::unordered_map<symbol, std::string>& symbols) {
    std::vector<Library> result;

    Resolver resolver(filepath);
    resolver.resolve(filepath);

    for (auto& [_, library] : resolver.libraries) {
        result.push_back(library);
    }

    std::sort(result.begin(), result.end());

    symbols = resolver.predict_dependencies();

    return result;
}

std::unordered_map<symbol, std::string> Resolver::predict_dependencies() const noexcept {
    auto my_lib = Library("my_lib");
    my_lib.resolve(target);
    auto symbols = my_lib.get_symbols(true);

    std::unordered_map<symbol, std::vector<std::string>> result;
    for (auto& [_, library]: libraries) {
        auto lib_symbols = library.get_symbols(false);
        std::unordered_set<symbol> intersection;
        for (auto& elem: symbols) {
            if (lib_symbols.contains(elem)) {
                if (!result.contains(elem)) {
                    result.insert({elem, std::vector<std::string>()});
                }
                result[elem].push_back(library.get_name());
            }
        }
    }
    std::unordered_map<symbol, std::string> predictions;
    for (auto& symbol: symbols) {
        if (result.contains(symbol)) {
            if (result[symbol].size() == 1) {
                predictions[symbol] = result[symbol][0];
            } else {
                predictions[symbol] = "# Several libraries contain this symbol";
            }
        }
        else {
            predictions[symbol] = "# Unknown";
        }
    }
    return predictions;
}

Resolver::Resolver(const std::filesystem::path& filepath) :
    target(filepath) {}