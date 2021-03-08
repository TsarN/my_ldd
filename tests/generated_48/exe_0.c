#include <stdio.h>
void fun_lib_2_0(void);
void fun_lib_3_0(void);
void fun_exe_0_0(void) {
	puts("fun_exe_0_0");
	fun_lib_2_0();
}
void fun_exe_0_1(void) {
	puts("fun_exe_0_1");
	fun_lib_3_0();
}
void fun_exe_0_2(void) {
	puts("fun_exe_0_2");
}
int main() {
	puts("main (exe_0)");
	fun_exe_0_0();
	fun_exe_0_1();
	fun_exe_0_2();
}
