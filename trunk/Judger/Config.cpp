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


#include "Comm.h"
#include "Config.h"


Config::Config()
    : JE_PENDING(0)
    , JE_REJUDGE(1)
    , JE_ACCEPT(2)
    , JE_COMPLIE(3)
    , JE_WRONG_ANWSER(4)
    , JE_TIME_LIMITED(5)
    , JE_MEMORY_LIMITED(6)
    , JE_PRESENT(7)
    , JE_SYSTEM(8)
    , JE_RUNTIME(9)
    , JE_UNKNOWN(99)
    , CL_GPLUS(0)
    , CL_GCC(1)
    , CL_JAVA(2)
{
}

bool Config::loadConfig(const std::string & config)
{
    LZDataPtr root = createLzdFromFile(LZD_XML, config);
    if(!root) 
        return false;

    return loadConfig(root);
}

bool Config::loadConfig(LZDataPtr root)
{
    cores = root->readInt("cores");
    testDataPath = root->readString("testDataPath");

    //错误类型
    LZDataPtr ptr = root->read("judgecode");
    if (!ptr)
    {
        return false;
    }
    JE_PENDING = ptr->readInt("pending");
    JE_REJUDGE = ptr->readInt("rejudge");
    JE_ACCEPT = ptr->readInt("accept");             //接受
    JE_COMPLIE = ptr->readInt("compile_error");             //编译错误
    JE_WRONG_ANWSER = ptr->readInt("wrong_anwser");        //答案错误
    JE_TIME_LIMITED = ptr->readInt("time_limited");        //超时
    JE_MEMORY_LIMITED = ptr->readInt("memory_limited");      //内存超出
    JE_PRESENT = ptr->readInt("present_error");             //显示错误
    JE_SYSTEM = ptr->readInt("system_error");              //系统错误
    JE_RUNTIME = ptr->readInt("runtime_error");             //运行时错误
    JE_UNKNOWN = ptr->readInt("unknow_error");             //未知错误

    //编译器类型
    ptr = root->read("compiler");
    if(!ptr)
    {
        return false;
    }
    CL_GCC = ptr->readInt("gcc", 0);     //gcc
    CL_GPLUS = ptr->readInt("g++", 1);   //g++
    CL_JAVA = ptr->readInt("java", 3);    //java

    //数据库信息
    ptr = root->read("mysql");
    if (!ptr)
    {
        return false;
    }
    dbip = ptr->readString("ip");
    dbport = ptr->readInt("port");
    dbusername = ptr->readString("user");
    dbpassword = ptr->readString("password");
    dbname = ptr->readString("dbname");

    return true;
}
