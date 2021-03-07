#include <iostream>

#include "dependencies.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "usage: my_ldd <filename>";
        return 0;
    }

    auto dependencies = resolve(argv[1]);
    for (const auto &library : dependencies) {
        std::cout << library.get_name() << " => ";
        if (library.is_resolved()) {
            std::cout << library.get_path();
        } else {
            std::cout << "not found";
        }

        std::cout << "\n";
    }

    return 0;
}
