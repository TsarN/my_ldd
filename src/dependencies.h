#pragma once

#include <compare>
#include <filesystem>
#include <string>
#include <optional>
#include <vector>

using LibraryName = std::string; // e.g. "libc.so.6"

class Library {
public:
    explicit Library(std::string name) : name(std::move(name)) {}
    
    bool is_resolved() const noexcept {
        return path.has_value();
    }

    void resolve(std::filesystem::path path) noexcept {
        this->path = std::move(path);
    }

    const std::string& get_name() const noexcept {
        return name;
    }

    const std::filesystem::path& get_path() const noexcept {
        return path.value();
    }

    std::strong_ordering operator<=>(const Library& that) const noexcept {
        return name <=> that.name;
    }

private:
    std::string name;
    std::optional<std::filesystem::path> path;
};

std::vector<Library> resolve(const std::filesystem::path& filepath);

