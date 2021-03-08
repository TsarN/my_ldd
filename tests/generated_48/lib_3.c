#include <stdio.h>
void fun_lib_1_1(void);
void fun_lib_2_2(void);
void fun_lib_3_0(void) {
	puts("fun_lib_3_0");
}
void fun_lib_3_1(void) {
	puts("fun_lib_3_1");
	fun_lib_2_2();
}
void fun_lib_3_2(void) {
	puts("fun_lib_3_2");
	fun_lib_1_1();
}
