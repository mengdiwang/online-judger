//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
//Utilities.cpp 

#include "Utilities.h"
#include "FileTool.h"
#include "ZLock.h"

std::wstring charToWChar(const std::string & str, DWORD code/*=CP_ACP*/)
{
    long lLen = MultiByteToWideChar(code, 0, str.c_str(), -1, NULL, 0); 
    std::wstring wstr(lLen, 0);
    MultiByteToWideChar(code, 0, str.c_str(), -1, &wstr[0], lLen);
    trimPartStringW(wstr);
    return wstr;
}

std::string wcharToChar(const std::wstring & wstr, DWORD code/*=CP_ACP*/)
{
    long lLen = WideCharToMultiByte(code, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(lLen, 0);
    WideCharToMultiByte(code, 0, wstr.c_str(), -1, &str[0], lLen, NULL, NULL);
    trimPartStringA(str);
    return str;
}

///改变工作路径
bool changeCurDirectory(LPCTSTR szPath)
{
    tstring path;
    if (szPath != NULL)
    {
        path = szPath;
    }
    else
    {
        TCHAR buffer[MAX_PATH];
        GetModuleFileName(NULL, buffer, MAX_PATH);
        path = buffer;
        path = getFilePath(path);
    }
    return !!SetCurrentDirectory(path.c_str());
}

void generateGUID(tstring & guidStr)
{
    guidStr.resize(64, 0);
    GUID guid;
    if (S_OK == ::CoCreateGuid(&guid))
    {
        msprintf_s(&guidStr[0], 64
            , _T("%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X")
            , guid.Data1, guid.Data2, guid.Data3
            , guid.Data4[0], guid.Data4[1]
            , guid.Data4[2], guid.Data4[3]
            , guid.Data4[4], guid.Data4[5]
            , guid.Data4[6], guid.Data4[7]
        );
    }
    trimPartString(guidStr);
}

//生成guid
tstring generateGUID()
{
    tstring str;
    generateGUID(str);
    return str;
}

static int g_idIndex = 1;
ZSimpleLock g_idLocker;
//生成唯一的整数id。线程安全。
int generateID()
{
    ZLockHolder holder(&g_idLocker);
    ++g_idIndex;
    return g_idIndex;
}

//将string截取掉ch以后的部分
void trimPartStringA(std::string & str, char ch/*='\0'*/)
{
    size_t pos = str.find_first_of(ch);
    if (pos != str.npos)
    {
        str.erase(pos);
    }
}

void trimPartStringW(std::wstring & str, wchar_t ch/*='\0'*/)
{
    size_t pos = str.find_first_of(ch);
    if (pos != str.npos)
    {
        str.erase(pos);
    }
}

template<typename STRING>
void _trimRString(STRING & str, const STRING & filter)
{
    int i = int(str.size()) - 1;
    for(; i>=0 ; --i)
    {
        if (filter.find(str[i]) == filter.npos)
        {
            break;
        }
    }
    i += 1;
    if (i < int(str.size()))
    {
        str.erase(i);
    }
}

//去除string尾部的特定字符。 filter : 要过滤掉的字符集和
void trimRStringA(std::string & str, const std::string & filter/*=" \t\r\n"*/)
{
    _trimRString(str, filter);
}

void trimRStringW(std::wstring & str, const std::wstring & filter/*=L" \t\r\n"*/)
{
    _trimRString(str, filter);
}

//////////////////////////////////////////////////////////////////////////
//String Tool
//////////////////////////////////////////////////////////////////////////
template<typename type>
void convertToString(std::string & str, type data, const char* format)
{
    str.resize(64);
    int n = sprintf_s(&str[0], 64, format, data);
    if (n >= 0)
    {
        str.resize(n);
    }
}

template<>
void convertToString(std::string & str, bool data, const char* format)
{
    str = (data ? "true" : "false");
}

int StringTool::asInt() const
{
    return atoi(cvalue());
}

bool StringTool::asBool() const
{
    if (m_value == "true")
    {
        return true;
    }
    else if(m_value == "false")
    {
        return false;
    }
    return !!asInt();
}

float StringTool::asFloat() const
{
    return (float)asDouble();
}

const char* StringTool::asText() const
{
    return cvalue();
}

double StringTool::asDouble() const
{
    return atof(cvalue());
}

const std::string& StringTool::asString() const
{
    return m_value;
}

void StringTool::setInt(int data)
{
    convertToString(m_value, data,"%d");
}

void StringTool::setBool(bool data)
{
    convertToString(m_value, data, "");
}

void StringTool::setFloat(float data)
{
    convertToString(m_value, data,"%f");
}

void StringTool::setDouble(double data)
{
    convertToString(m_value, data,"%lf");
}

void StringTool::setText(const char* data)
{
    m_value = data;
}


void StringTool::setString(const std::string& data)
{
    m_value = data;
}

//////////////////////////////////////////////////////////////////////////