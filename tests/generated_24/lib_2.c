#include <stdio.h>
void fun_lib_0_0(void);
void fun_lib_1_1(void);
void fun_lib_2_1(void);
void fun_lib_2_0(void) {
	puts("fun_lib_2_0");
	fun_lib_1_1();
	fun_lib_1_1();
	fun_lib_2_1();
	fun_lib_0_0();
}
void fun_lib_2_1(void) {
	puts("fun_lib_2_1");
}
