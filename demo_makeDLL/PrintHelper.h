#pragma once

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>

#ifdef __cplusplus
EXTERN_C
{
#endif
	__declspec(dllexport) bool Print(unsigned char* buffer,int buffersize);
#ifdef __cplusplus
}
#endif
