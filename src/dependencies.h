#pragma once

#include <compare>
#include <filesystem>
#include <string>
#include <optional>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using LibraryName = std::string; // e.g. "libc.so.6"
using SymbolName = std::string;

class Symbol {
public:
    explicit Symbol(SymbolName name, bool exported) : name(std::move(name)), exported(exported) {}

    const SymbolName& get_name() const noexcept {
        return name;
    }

    const std::vector<LibraryName>& get_library_names() const noexcept {
        return library_names;
    }

    bool is_exported() const noexcept {
        return exported;
    }

    void exported_from(LibraryName library) {
        library_names.emplace_back(std::move(library));
    }

    std::strong_ordering operator<=>(const Symbol& that) const noexcept {
        return name <=> that.name;
    }

private:
    SymbolName name;
    std::vector<LibraryName> library_names;
    bool exported;
};

class Library {
public:
    explicit Library(SymbolName name = {}) : name(std::move(name)) {}
    
    bool is_resolved() const noexcept {
        return path.has_value();
    }

    void resolve(std::filesystem::path path) noexcept {
        this->path = std::move(path);
    }

    void add_symbol(Symbol symbol) {
        auto name = symbol.get_name();
        symbols.emplace(std::move(name), std::move(symbol));
    }

    const SymbolName& get_name() const noexcept {
        return name;
    }

    const std::filesystem::path& get_path() const noexcept {
        return path.value();
    }

    const std::unordered_map<SymbolName, Symbol>& get_symbols() const noexcept {
        return symbols;
    }

    std::strong_ordering operator<=>(const Library& that) const noexcept {
        return name <=> that.name;
    }

private:
    SymbolName name;
    std::optional<std::filesystem::path> path;
    std::unordered_map<SymbolName, Symbol> symbols;
};

struct ResolveResult {
    std::vector<Library> dependencies;
    std::vector<Symbol> symbols;
};

ResolveResult resolve(const std::filesystem::path& filepath, const std::vector<std::filesystem::path>& search_paths = {});

