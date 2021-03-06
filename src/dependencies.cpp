#include <elfio/elfio.hpp>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

#include "dependencies.h"

static void find_dependency_locations(const ELFIO::elfio &reader,
                                      std::unordered_set<std::string> &unresolved_dependencies,
                                      std::unordered_map<std::string, std::string> &resolved_dependencies) {
    // TODO DT_RPATH & DT_RUNPATH

    using namespace std::filesystem;
    for (const auto &directory : {"/lib", "/usr/lib"}) {
        for (auto &p: recursive_directory_iterator(directory)) {
            auto path = p.path().string();
            auto filename = p.path().filename().string();
            if (unresolved_dependencies.contains(filename) && ELFIO::elfio().load(path)) {
                unresolved_dependencies.erase(filename);
                resolved_dependencies[filename] = path;
            }
        }
    }
}

Dependencies resolve(const std::string &filepath) {
    ELFIO::elfio reader;
    reader.load(filepath);

    std::unordered_set<std::string> unresolved_dependencies;
    std::unordered_map<std::string, std::string> resolved_dependencies;

    for (auto &section: reader.sections) {
        if (section->get_type() == SHT_DYNAMIC) {
            ELFIO::dynamic_section_accessor accessor(reader, section);

            for (ELFIO::Elf_Xword i = 0; i < accessor.get_entries_num(); i++) {
                ELFIO::Elf_Xword tag, value;
                std::string str;
                accessor.get_entry(i, tag, value, str);
                if (tag == DT_NEEDED) {
                    if (str.find('/') != std::string::npos) {
                        resolved_dependencies[str] = str;
                    } else {
                        unresolved_dependencies.insert(str);
                    }
                }
            }
        }
    }
    find_dependency_locations(reader, unresolved_dependencies, resolved_dependencies);

    for (const auto &[name, path]: resolved_dependencies) {
        auto result = resolve(path);
        unresolved_dependencies.merge(result.unresolved_dependencies);
        resolved_dependencies.merge(result.resolved_dependencies);
    }

    return {unresolved_dependencies, resolved_dependencies};
}
