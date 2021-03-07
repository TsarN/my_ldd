#include <string>
#include <array>
#include <fstream>
#include "util.h"

bool is_elf(const std::string &filename) {
    static const std::size_t SIGNATURE_SIZE = 4;
    static std::array<char, SIGNATURE_SIZE> elf_signature{
        0x7f, 'E', 'L', 'F'
    };

    std::ifstream is(filename, std::istream::binary);
    std::array<char, SIGNATURE_SIZE> prefix{};

    is.read(prefix.data(), SIGNATURE_SIZE);
    return is.good() && prefix == elf_signature;
}