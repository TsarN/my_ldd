#include <stdio.h>
void fun_lib_0_0(void);
void fun_lib_0_2(void);
void fun_lib_2_1(void);
void fun_exe_1_0(void) {
	puts("fun_exe_1_0");
	fun_lib_0_0();
	fun_lib_0_2();
}
void fun_exe_1_1(void) {
	puts("fun_exe_1_1");
	fun_lib_2_1();
}
void fun_exe_1_2(void) {
	puts("fun_exe_1_2");
}
int main() {
	puts("main (exe_1)");
	fun_exe_1_0();
	fun_exe_1_1();
	fun_exe_1_2();
}
