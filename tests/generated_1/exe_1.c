#include <stdio.h>
void fun_lib_2_0(void);
void fun_lib_3_1(void);
void fun_exe_1_0(void) {
	puts("fun_exe_1_0");
}
void fun_exe_1_1(void) {
	puts("fun_exe_1_1");
	fun_lib_2_0();
	fun_lib_3_1();
}
int main() {
	puts("main (exe_1)");
	fun_exe_1_0();
	fun_exe_1_1();
}
