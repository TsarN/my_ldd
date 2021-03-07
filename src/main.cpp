#include <iostream>

#include "dependencies.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "usage: my_ldd <filename>";
        return 0;
    }
    std::cout << "DEPENDENCIES:\n";
    std::unordered_map<symbol, std::string> symbols;
    auto dependencies = resolve(argv[1], symbols);
    for (const auto &library : dependencies) {
        std::cout << library.get_name() << " => ";
        if (library.is_resolved()) {
            std::cout << library.get_path();
        } else {
            std::cout << "not found";
        }

        std::cout << "\n";
    }
    std::cout << "DYNAMIC SYMBOLS:\n";
    for (const auto& s: symbols) {
        std::cout << s.first << " => " << s.second << "\n";
    }
    return 0;
}
