#include <iostream>

#include "dependencies.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "usage: my_ldd <filename>";
        return 0;
    }

    auto dependencies = resolve(argv[1]);
    for (const auto &[name, path]: dependencies.resolved_dependencies) {
        if (name != path) {
            std::cout << name << " => ";
        }
        std::cout << path << "\n";
    }
    for (const auto &name: dependencies.unresolved_dependencies) {
        std::cout << name << " => not found\n";
    }

    return 0;
}
