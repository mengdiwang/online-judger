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

#pragma once

#include "RefPtr.h"
#include "..\LZData\LZData.h"

//////////////////////////////////////////////////////////////////////////
//常量配置参数
//////////////////////////////////////////////////////////////////////////
class Config : public IBase
{
public:
    Config();

    virtual bool loadConfig(const std::string & config);

    virtual bool loadConfig(LZDataPtr root);

public:
    //状态类型

    int JE_PENDING;             //待定
    int JE_ACCEPT;              //接受
    int JE_REJUDGE;             //等待重判
    int JE_COMPLIE;             //编译错误
    int JE_WRONG_ANWSER;        //答案错误
    int JE_TIME_LIMITED;        //超时
    int JE_MEMORY_LIMITED;      //内存超出
    int JE_PRESENT;             //显示错误
    int JE_SYSTEM;              //系统错误
    int JE_RUNTIME;             //运行时错误
    int JE_UNKNOWN;             //未知错误

    //编译器类型
    int CL_GPLUS;   //g++
    int CL_GCC;     //gcc
    int CL_JAVA;    //java

    int         cores;
    std::string testDataPath;

    std::string dbusername;
    std::string dbpassword;
    std::string dbip;
    int         dbport;
    std::string dbname;
};
