//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Comm.h"


//多字节字符串转换成宽字符字符串
std::wstring charToWChar(const std::string & str, DWORD code=CP_ACP);

//宽字符串转换成多字节字符串
std::string wcharToChar(const std::wstring & wstr, DWORD code=CP_ACP);

///改变工作路径
bool changeCurDirectory(LPCTSTR szPath = NULL);

//生成guid
void generateGUID(tstring & guid);

//生成guid
tstring generateGUID();

//生成唯一的整数id。线程安全。
int generateID();

//将string截取掉ch以后的部分
void trimPartStringA(std::string & str, char ch='\0');

//将string截取掉ch以后的部分
void trimPartStringW(std::wstring & str, wchar_t ch='\0');

//去除string尾部的特定字符。 filter : 要过滤掉的字符集和
void trimRStringA(std::string & str, const std::string & filter=" \t\r\n");

//去除string尾部的特定字符。 filter : 要过滤掉的字符集和
void trimRStringW(std::wstring & str, const std::wstring & filter=L" \t\r\n");

#ifdef _UNICODE
#   define trimRString trimRStringW
#   define trimPartString trimPartStringW
#else
#   define trimRString trimRStringA
#   define trimPartString trimPartStringA
#endif

//////////////////////////////////////////////////////////////////////////
//字符串工具
//用于将字符串，转换成其他类型数值。
//////////////////////////////////////////////////////////////////////////
class StringTool
{
public:
    StringTool(){};
    StringTool(const std::string & v) : m_value(v) {}
    StringTool(const StringTool& st) : m_value(st.value()) {}

    const StringTool& operator=(const StringTool& st)
    {
        m_value = st.m_value;
        return *this;
    }

    const StringTool& operator=(std::string & v)
    {
        m_value = v;
        return *this;
    }

public:
    const std::string & value() const { return m_value; }
    const char* cvalue()const { return m_value.c_str(); }

    int asInt() const;
    bool asBool() const;
    float asFloat() const;
    double asDouble() const;
    const char* asText() const;
    const std::string& asString () const;
    std::string asStringEx () const;

    void setInt(int data);
    void setBool(bool data);
    void setFloat(float data);
    void setDouble(double data);
    void setText(const char* data);
    void setString(const std::string& data);

protected:
    std::string m_value;
};
