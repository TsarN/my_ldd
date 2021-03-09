#!/usr/bin/env python3

import os
import sys
import random
import shutil
import subprocess


class Function:
    def __init__(self, name, binary):
        self.name = name
        self.calls = []
        self.binary = binary

    def add_call(self, name):
        self.calls.append(name)


class Binary:
    def __init__(self, name, is_executable):
        self.name = name
        self.is_executable = is_executable
        self.funs = []
        self.deps = []

    def add_fun(self, name):
        self.funs.append(Function(name, self))

    def generate_funs(self, fun_count):
        for i in range(fun_count):
            self.add_fun(f"fun_{self.name}_{i}")

    def add_dep(self, binary):
        if not self.funs:
            return
        self.deps.append(binary)
        random.choice(self.funs).add_call(random.choice(binary.funs))

    def get_fun_deps(self):
        fun_deps = set()
        for fun in self.funs:
            for call in fun.calls:
                fun_deps.add(call.name)

        return sorted(list(fun_deps))

    def get_lib_deps(self):
        deps = [f"libc.so.6 => \"{get_libc_path()}\""]

        for dep in self.deps:
            if dep.name != self.name:
                deps.append(f"lib{dep.name}.so => \"./lib{dep.name}.so\"")
                deps += dep.get_lib_deps()

        return sorted(set(deps))

    def get_symbol_deps(self):
        deps = ["__libc_start_main : libc.so.6", "puts : libc.so.6"]

        for fun in self.funs:
            for call in fun.calls:
                deps.append(f"{call.name} : lib{call.binary.name}.so")

        return sorted(set(deps))

    def get_answer(self):
        return "\n".join(self.get_lib_deps()) + "\n\n" + "\n".join(self.get_symbol_deps())

    def get_source(self):
        lines = []
        lines.append("#include <stdio.h>")

        for dep in self.get_fun_deps():
            lines.append(f"void {dep}(void);")

        for fun in self.funs:
            lines.append(f"void {fun.name}(void) {{")
            lines.append(f"\tputs(\"{fun.name}\");")

            for call in fun.calls:
                lines.append(f"\t{call.name}();")

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
        if i.is_executable:
            with open(os.path.join(name, i.name + ".ans"), "w") as f:
                print(i.get_answer(), file=f)

    with open(os.path.join(name, "CMakeLists.txt"), "w") as f:
        print("\n".join(cmake), file=f)


def get_libc_path():
    if not hasattr(get_libc_path, 'path'):
        get_libc_path.path = subprocess.check_output(
            "ldd /bin/bash | grep -Po '(?<=libc\.so\.6 => )([^\s]*)'", shell=True
        ).decode("ascii").rstrip()
    return get_libc_path.path


def main():
    random.seed(420)

    for i in range(5):
        generate_test(f"generated_no_deps_{i}", 1, 0, i, 0)

    for i in range(5):
        generate_test(f"generated_one_dep_{i}", 1, 1, i, 1)

    for i in range(50):
        generate_test(f"generated_{i}", range(1, 3), range(1, 5), range(1, 5), range(1, 5))


if __name__ == "__main__":
    main()
