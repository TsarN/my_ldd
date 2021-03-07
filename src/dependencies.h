#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using LibraryName = std::string; // e.g. "libc.so.6"

class Library {
public:
    explicit Library(std::string name);
    
    bool is_resolved() const noexcept;
    void resolve(std::filesystem::path path) noexcept;

    const std::string& get_name() const noexcept;
    const std::filesystem::path& get_path() const noexcept;

private:
    std::string name;
    std::optional<std::filesystem::path> path;
};

class Resolver {
    friend std::vector<Library> resolve(const std::filesystem::path& filepath);

private:
    void resolve(const std::filesystem::path& filepath);
    void collect_dependencies(const std::filesystem::path& filepath);

private:
    std::unordered_map<LibraryName, Library> libraries;
    std::vector<std::filesystem::path> search_paths { "/lib", "/usr/lib" };
};

std::vector<Library> resolve(const std::filesystem::path& filepath);

