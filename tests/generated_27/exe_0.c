#include <stdio.h>
void fun_lib_0_0(void);
void fun_lib_0_1(void);
void fun_exe_0_0(void) {
	puts("fun_exe_0_0");
}
void fun_exe_0_1(void) {
	puts("fun_exe_0_1");
	fun_lib_0_1();
	fun_lib_0_0();
	fun_lib_0_1();
}
int main() {
	puts("main (exe_0)");
	fun_exe_0_0();
	fun_exe_0_1();
}
