#include <stdio.h>
void fun_lib_1_2(void);
void fun_lib_2_0(void);
void fun_lib_2_1(void);
void fun_lib_2_0(void) {
	puts("fun_lib_2_0");
}
void fun_lib_2_1(void) {
	puts("fun_lib_2_1");
	fun_lib_1_2();
}
void fun_lib_2_2(void) {
	puts("fun_lib_2_2");
	fun_lib_2_1();
	fun_lib_2_0();
}
void fun_lib_2_3(void) {
	puts("fun_lib_2_3");
	fun_lib_2_1();
}