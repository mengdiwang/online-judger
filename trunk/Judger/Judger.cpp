//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2013.01
 * 
 * This file is part of Online Judger for HUSTOJ. about HUSTOJ 
 * see <http://code.google.com/p/hustoj>.
 *
 * HUSTOJ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * HUSTOJ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HUSTOJ. if not, see <http://www.gnu.org/licenses/>.
 */
//////////////////////////////////////////////////////////////////////////

#include "Judger.h"

#include "Utilities.h"
#include "Log.h"
#include "ZJobObject.h"

#ifdef WORK_DIR
#   undef WORK_DIR
#endif

#define WORK_DIR        "work\\"
#define TEST_DATA_DIR   "..\\testdata\\%d\\"
#define CODE_DIR        "..\\code\\%s\\%d\\%d.%s"

#define EXE_NAME            _T("work\\%d\\a.exe ")
#define EXE_OUT_FILE        _T("work\\%d\\a.out ")
#define CODE_NAME           _T("..\\code\\%s\\%d\\%d")
#define GCC_ARG             _T("gcc.exe %s -o %s -O2 -Wall -lm --static -std=c99 -DONLINE_JUDGE")
#define GPLUS_ARG           _T("g++.exe %s -o %s -O2 -Wall -lm --static -DONLINE_JUDGE")
#define ANWSER_FILE_IN      _T("..\\testdata\\%d\\test.in ")
#define ANWSER_FILE_OUT     _T("..\\testdata\\%d\\test.out ")
#define MATCHER_NAME        _T("matcher.exe ")

#define MAX_LEN 1024
#define KB_BYTES 1024
#define MB_BYTES 1048576
