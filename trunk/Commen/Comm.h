//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <Windows.h>

#include <tchar.h>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#ifdef _UNICODE

typedef std::wstring    tstring;
typedef std::wifstream  tifstream;
typedef std::wofstream  tofstream;

#else

typedef std::string     tstring;
typedef std::ifstream   tifstream;
typedef std::ofstream  tofstream;

#endif

#ifndef LZDLL_API
#   define LZDLL_API
#endif

#define SAFE_CLOSE_HANDLE(h) if(h){ CloseHandle(h); h = NULL; }

typedef unsigned int        uint;
typedef __int64             int64;
typedef unsigned __int64    uint64;

#define OutputMsgA(format, ...) printf(format"\n", __VA_ARGS__)
#define OutputMsgW(format, ...) wprintf(format _T("\n"), __VA_ARGS__)

#ifdef _UNICODE

#define msprintf_s      swprintf_s
#define OutputMsg       OutputMsgW

#else

#define msprintf_s      sprintf_s
#define OutputMsg       OutputMsgA

#endif