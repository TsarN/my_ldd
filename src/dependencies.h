#pragma once

#include <compare>
#include <filesystem>
#include <string>
#include <optional>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using LibraryName = std::string; // e.g. "libc.so.6"

using symbol = std::string;

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

    std::unordered_set<symbol> get_symbols(bool dynamic_only) const noexcept;

private:
    std::string name;
    std::optional<std::filesystem::path> path;
};

std::vector<Library> resolve(const std::filesystem::path& filepath, std::unordered_map<symbol, std::string>& symbols);

