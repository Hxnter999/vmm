#include "vmmcall.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	printf("Hi\n");
	//for(uint64_t i = 0; i < 2; i++)
		_vmmcall(hypercall_code::PING);
	system("pause");

}