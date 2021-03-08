#include "dependencies.h"
#include "util.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_map>

#include <elfio/elfio.hpp>

class Resolver {
    friend ResolveResult resolve(const std::filesystem::path& filepath, const std::vector<std::filesystem::path>& search_paths);

private:
    void resolve(const std::filesystem::path& filepath);
    void collect_dependencies(const std::filesystem::path& filepath);
    void load_symbols(Library& library);
    std::unordered_map<SymbolName, std::string> predict_dependencies() const noexcept;

private:
    std::unordered_map<LibraryName, Library> libraries;
    std::vector<std::filesystem::path> search_paths { "/lib", "/usr/lib", "/lib/x86_64-linux-gnu" };
};

void Resolver::load_symbols(Library& library) {
    if (!library.is_resolved()) {
        throw std::logic_error{"Cannot load symbols from unresolved library"};
    }

    ELFIO::elfio reader;
    reader.load(library.get_path());

    std::unordered_set<SymbolName> my_symbols;
    for (auto &section: reader.sections) {
        if (section->get_type() == SHT_DYNSYM) {
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

                bool is_exported = section_index != SHN_UNDEF;

                if (!is_exported && bind != STB_GLOBAL) continue;

                my_symbols.insert(name);
                library.add_symbol(Symbol { name, is_exported });
            }
        }
    }
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

ResolveResult resolve(const std::filesystem::path& filepath, const std::vector<std::filesystem::path>& search_paths) {
    ResolveResult result;

    Resolver resolver;
    resolver.search_paths.insert(resolver.search_paths.begin(), search_paths.begin(), search_paths.end());
    resolver.resolve(filepath);

    std::unordered_map<SymbolName, Symbol> dynamic_symbols;

    for (auto& [_, library] : resolver.libraries) {
        result.dependencies.push_back(library);
    }

    std::sort(result.dependencies.begin(), result.dependencies.end());

    for (auto& library : result.dependencies) {
        if (library.is_resolved()) {
            resolver.load_symbols(library);
            for (const auto& [_, symbol] : library.get_symbols()) {
                if (symbol.is_exported()) {
                    if (!dynamic_symbols.contains(symbol.get_name())) {
                        dynamic_symbols.emplace(symbol.get_name(), symbol);
                    }

                    dynamic_symbols.at(symbol.get_name()).exported_from(library.get_name());
                }
            }
        }
    }

    Library me;
    me.resolve(filepath);
    resolver.load_symbols(me);

    for (const auto& [_, symbol] : me.get_symbols()) {
        if (symbol.get_name().empty()) {
            continue;
        }

        if (dynamic_symbols.contains(symbol.get_name())) {
            result.symbols.emplace_back(dynamic_symbols.at(symbol.get_name()));
        }
    }

    std::sort(result.symbols.begin(), result.symbols.end());

    return result;
}
