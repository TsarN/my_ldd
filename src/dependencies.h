#ifndef MY_LDD_DEPENDENCIES_H
#define MY_LDD_DEPENDENCIES_H

#include <unordered_map>
#include <unordered_set>

struct Dependencies {
    std::unordered_set<std::string> unresolved_dependencies;
    std::unordered_map<std::string, std::string> resolved_dependencies;
};

Dependencies resolve(const std::string &filepath);

#endif //MY_LDD_DEPENDENCIES_H
