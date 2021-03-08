#include "dependencies.h"

#include <iostream>
#include <vector>
#include <filesystem>

#include <cxxopts.hpp>

int main(int argc, char *argv[]) {
    cxxopts::Options options{"my_ldd", "Resolve dynamic dependencies of an ELF binary"};

    options.add_options()
        ("h,help", "Print usage")
        ("p,path", "Add directory to the library search path",
            cxxopts::value<std::vector<std::filesystem::path>>()->default_value({}))
        ("s,symbols", "Show dynamic symbols")
    ;

    cxxopts::ParseResult parse_result;

    try {
        parse_result = options.parse(argc, argv);

        if (parse_result.unmatched().size() != 1) {
            throw cxxopts::OptionParseException{"Expected one positional argument"};
        }
    } catch (cxxopts::OptionParseException& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << options.help();
        return 1;
    }

    if (parse_result.count("help")) {
        std::cerr << options.help();
        return 0;
    }

    auto result = resolve(parse_result.unmatched()[0], parse_result["path"].as<std::vector<std::filesystem::path>>());

    for (const auto &library : result.dependencies) {
        std::cout << library.get_name() << " => ";
        if (library.is_resolved()) {
            std::cout << library.get_path();
        } else {
            std::cout << "not found";
        }

        std::cout << "\n";
    }

    if (parse_result.count("symbols")) {
        std::cout << "\n";

        for (const auto& symbol : result.symbols) {
            std::cout << symbol.get_name() << " : ";

            if (symbol.get_library_names().empty()) {
                std::cout << "not found";
            } else {
                bool first = true;

                for (const auto& library : symbol.get_library_names()) {
                    if (first) {
                        first = false;
                    } else {
                        std::cout << ", ";
                    }

                    std::cout << library;
                }
            }

            std::cout << "\n";
        }
    }

    return 0;
}
