#include <stdio.h>
void fun_lib_1_0(void);
void fun_lib_2_0(void) {
	puts("fun_lib_2_0");
	fun_lib_1_0();
}
