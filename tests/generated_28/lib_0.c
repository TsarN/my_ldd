#include <stdio.h>
void fun_lib_0_1(void);
void fun_lib_0_0(void) {
	puts("fun_lib_0_0");
	fun_lib_0_1();
}
void fun_lib_0_1(void) {
	puts("fun_lib_0_1");
	fun_lib_0_1();
}
void fun_lib_0_2(void) {
	puts("fun_lib_0_2");
}
void fun_lib_0_3(void) {
	puts("fun_lib_0_3");
}
