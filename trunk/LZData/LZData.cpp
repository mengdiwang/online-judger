//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "LZData.h"
#include <fstream>

template<typename type>
LPCSTR convertToString(type data, LPCSTR format)
{
    static char buffer[256];
    sprintf_s(buffer, 256, format, data);
    return buffer;
}

template<>
LPCSTR convertToString(bool data, LPCTSTR format)
{
    return data ? "true" : "false";
}

void printTable(std::ostream & cout, int n)
{
    for (int i=0; i<n; ++i)
    {
        cout<<"\t";
    }
}

bool readFileBuffer(std::vector<char> & buffer, const std::string & fname)
{
    FILE* file = NULL;
    fopen_s(&file, fname.c_str(), "rb");
    if (NULL == file)
    {
        printf("打开文件失败！fname:%s", fname.c_str());
        return false;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer.resize(length);
    if(fread_s(&buffer[0], length, length, 1, file) != 1)
    {
        printf("读取文件数据失败！fname:%s, size:%ld", fname.c_str(), length);

        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

void ltrimString(std::string & str)
{
    for (std::string::iterator it=str.begin(); it!=str.end(); ++it)
    {
        if (!isWhiteSpace(*it ))
        {
            str.erase(str.begin(), it);
            return;
        }
    }
    str.clear();
}

void rtrimString(std::string & str)
{
    int end = (int)str.length()-1;
    for (int i=end; i>=0; --i)
    {
        if (!isWhiteSpace(str[i]))
        {
            if (i != end)
            {
                str.erase(i+1);
            }
            return;
        }
    }
    str.clear();
}

void trimString(std::string & str)
{
    ltrimString(str);
    rtrimString(str);
}

//////////////////////////////////////////////////////////////////////////

LZData::LZData(void)
{
}

LZData::~LZData(void)
{
}

LZData::LZData(const std::string & tag)
    : m_tag(tag)
{

}

LZData::LZData(const std::string & tag, const std::string & value_)
    : m_tag(tag)
    , m_value(value_)
{

}

//////////////////////////////////////////////////////////////////////////


int LZData::asInt() const
{
    return atoi(m_value.c_str());
}

bool LZData::asBool() const
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

float LZData::asFloat() const
{
    return (float)asDouble();
}

double LZData::asDouble() const
{
    return atof(m_value.c_str());
}

const std::string& LZData::asString() const
{
    return m_value;
}

void LZData::setInt(int data)
{
    m_value = convertToString(data,"%d");
}

void LZData::setBool(bool data)
{
    m_value = convertToString(data, "");
}

void LZData::setFloat(float data)
{
    m_value = convertToString(data,"%f");
}

void LZData::setDouble(double data)
{
    m_value = convertToString(data,"%lf");
}


void LZData::setString(const std::string& data)
{
    m_value = data;
}

//////////////////////////////////////////////////////////////////////////

int LZData::readInt(const std::string & tag, int def)
{
    LZDataPtr ptr = read(tag);
    if (!ptr)
    {
        return def;
    }
    return ptr->asInt();
}

bool LZData::readBool(const std::string & tag, bool def)
{
    LZDataPtr ptr = read(tag);
    if (!ptr)
    {
        return def;
    }
    return ptr->asBool();
}

float LZData::readFloat(const std::string & tag, float def)
{
    LZDataPtr ptr = read(tag);
    if (!ptr)
    {
        return def;
    }
    return ptr->asFloat();
}

double LZData::readDouble(const std::string & tag, double def)
{
    LZDataPtr ptr = read(tag);
    if (!ptr)
    {
        return def;
    }
    return ptr->asDouble();
}


std::string LZData::readString(const std::string & tag, std::string def /*= ""*/)
{
    LZDataPtr ptr = read(tag);
    if (!ptr)
    {
        return def;
    }
    return ptr->asString();
}

void LZData::writeInt(const std::string & tag, int data)
{
    write(tag)->setInt(data);
}

void LZData::writeBool(const std::string & tag, bool data)
{
    write(tag)->setBool(data);
}

void LZData::writeFloat(const std::string & tag, float data)
{
    write(tag)->setFloat(data);
}

void LZData::writeDouble(const std::string & tag, double data)
{
    write(tag)->setDouble(data);
}


void LZData::writeString(const std::string & tag, std::string data)
{
    write(tag, data);
}

//////////////////////////////////////////////////////////////////////////

//获得子孙结点属性。支持a.b.c的形式。
LZDataPtr LZData::read(const std::string & tag)
{
    size_t pos = tag.find_first_of('/');
    int i = findChild(tag.substr(0, pos));
    if (i < 0)
    {
        return NULL;
    }

    if (pos == tag.npos)
    { 
        return getChild(i);
    }
    else
    {
        return getChild(i)->read(tag.substr(pos+1));
    }
}
LZDataPtr LZData::write(const std::string & tag, std::string value_/*=""*/)
{
    size_t pos = tag.find_first_of('/');
    if (tag.npos == pos)
    {
        int i = findChild(tag);
        if (i > 0)
        {
            getChild(i)->setString(value_);
        }
        else
        {
            i = addChild(newOne(tag, value_));
        }
        return getChild(i);
    }
    else
    {
        std::string lName = tag.substr(0,pos);
        std::string rName = tag.substr(pos+1);
        int i = findChild(lName);
        if (i < 0)
        {
            i = addChild(newOne(lName, ""));
        }
        return getChild(i)->write(rName, value_);
    }
}

int LZData::findNextChild(int index, const std::string & tag)
{
    index += 1;
    if (index<0 || index>=countChildren())
    {
        return -1;
    }
    for (int i=index; (size_t)i<m_children.size(); ++i)
    {
        if (m_children[i]->tag() == tag)
        {
            return i;
        }
    }
    return -1;
}

bool LZData::save(const std::string & fileName)
{
    std::ofstream fout(fileName.c_str());
    if (!fout.good())
    {
        return false;
    }
    print(fout, 0);
    fout.close();
    return true;
}

//////////////////////////////////////////////////////////////////////////
#include "lzd.h"
#include "xml.h"

LZDataPtr createLzdFromBuffer(LzDataType type, const char* buffer, int length)
{
    if(type == LZD_LZD)
    {
        return lzd::createFromBuffer(buffer, length);
    }
    else if (type == LZD_XML)
    {
        return xml::createFromBuffer(buffer, length);
    }
    return NULL;
}

LZDataPtr createLzdFromFile(LzDataType type, const std::string & fname)
{
    if(type == LZD_LZD)
    {
        return lzd::createFromFile(fname);
    }
    else if (type == LZD_XML)
    {
        return xml::createFromFile(fname);
    }
    return NULL;
}