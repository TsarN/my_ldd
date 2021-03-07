#!/usr/bin/env python3

import os
import sys
import random
import shutil


class Function:
    def __init__(self, name):
        self.name = name
        self.calls = []

    def add_call(self, name):
        self.calls.append(name)


class Binary:
    def __init__(self, name, is_executable):
        self.name = name
        self.is_executable = is_executable
        self.funs = []
        self.deps = []

    def add_fun(self, name):
        self.funs.append(Function(name))

    def generate_funs(self, fun_count):
        for i in range(fun_count):
            self.add_fun(f"fun_{self.name}_{i}")

    def add_dep(self, binary):
        self.deps.append(binary)
        random.choice(self.funs).add_call(random.choice(binary.funs).name)

    def get_fun_deps(self):
        fun_deps = set()
        for fun in self.funs:
            for call in fun.calls:
                fun_deps.add(call)

        return sorted(list(fun_deps))

    def get_source(self):
        lines = []
        lines.append("#include <stdio.h>")

        for dep in self.get_fun_deps():
            lines.append(f"void {dep}(void);")

        for fun in self.funs:
            lines.append(f"void {fun.name}(void) {{")
            lines.append(f"\tputs(\"{fun.name}\");")

            for call in fun.calls:
                lines.append(f"\t{call}();")

            lines.append("}")
        
        if self.is_executable:
            lines.append("int main() {")
            lines.append(f"\tputs(\"main ({self.name})\");")

            for fun in self.funs:
                lines.append(f"\t{fun.name}();")

            lines.append("}")

        return "\n".join(lines)
    
    def get_cmake(self):
        lines = []
        if self.is_executable:
            lines.append(f"add_executable({self.name} {self.name}.c)")
        else:
            lines.append(f"add_library({self.name} SHARED {self.name}.c)")

        for dep in self.deps:
            if dep.name != self.name:
                lines.append(f"target_link_libraries({self.name} {dep.name})")

        return "\n".join(lines)


def sample(n):
    if callable(n):
        return n()
    elif type(n) is int:
        return n
    else:
        return random.choice(n)


def generate_test(name, exe_count, lib_count, fun_count, dep_count):
    exe_count = sample(exe_count)
    lib_count = sample(lib_count)
    fun_count = sample(fun_count)
    dep_count = sample(dep_count)

    bins = []
    libs = []

    for i in range(lib_count):
        lib = Binary(f"lib_{i}", False)
        lib.generate_funs(fun_count)
        libs.append(lib)

        for i in range(dep_count):
            that = random.choice(libs)
            lib.add_dep(that)

        bins.append(lib)

    for i in range(exe_count):
        exe = Binary(f"exe_{i}", True)
        exe.generate_funs(fun_count)

        for i in range(dep_count):
            that = random.choice(libs)
            exe.add_dep(that)

        bins.append(exe)


    if os.path.exists(name):
        shutil.rmtree(name)
    os.makedirs(name)

    cmake = []
    for i in bins:
        cmake.append(i.get_cmake())
        with open(os.path.join(name, i.name + ".c"), "w") as f:
            print(i.get_source(), file=f)

    with open(os.path.join(name, "CMakeLists.txt"), "w") as f:
        print("\n".join(cmake), file=f)



def main():
    generate_test("kek", 3, 5, 3, 3)


if __name__ == "__main__":
    main()
