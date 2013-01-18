//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "comm.h"

enum LzDataType
{
    LZD_LZD,
    LZD_XML,
};

typedef RefPtr<class LZData> LZDataPtr;

class LZDATA_API LZData : public IBase
{
protected:
    LZData(void);
    
    explicit LZData(const std::string & tag);
    
    LZData(const std::string & tag, const std::string & value_);

    virtual ~LZData(void);

public:
    typedef std::vector<LZDataPtr>      DataPool;
    typedef DataPool::iterator          DataIterator;
    typedef DataPool::const_iterator    DataConstIterator;

public: //对自身的操作

    void setTag(const std::string &tag){ m_tag = tag; }
    const std::string& tag() const { return m_tag; }
    const char* ctag() const { return m_tag.c_str(); }

    void setValue(const std::string & v){ m_value = v; }
    const std::string & value() const { return m_value; }
    const char* cvalue()const { return m_value.c_str(); }

    int asInt() const;
    bool asBool() const;
    float asFloat() const;
    double asDouble() const;
    const std::string& asString () const;

    void        setInt      (int data);
    void        setBool     (bool data);
    void        setFloat    (float data);
    void        setDouble   (double data);
    void        setString   (const std::string& data);

public://对子孙节点的操作

    int         readInt     (const std::string & tag, int def = 0);
    bool        readBool    (const std::string & tag, bool def = false);
    float       readFloat   (const std::string & tag, float def = 0.0f);
    double      readDouble  (const std::string & tag, double def = 0.0);
    std::string readString  (const std::string & tag, std::string def = "");

    void        writeInt    (const std::string & tag, int data);
    void        writeBool   (const std::string & tag, bool data);
    void        writeFloat  (const std::string & tag, float data);
    void        writeDouble (const std::string & tag, double data);
    void        writeString (const std::string & tag, std::string data);

public://接口

    virtual LZDataPtr newOne(const std::string & tag, const std::string & value_) = 0;

    virtual void print(std::ostream & out, int depth=0) = 0;

public:
    DataIterator begin(){ return m_children.begin(); }
    DataConstIterator begin()const { return m_children.begin();}

    DataIterator end(){ return m_children.end(); }
    DataConstIterator end() const { return m_children.end(); }

    //获得子孙结点属性。支持a/b/c的形式。
    virtual LZDataPtr read(const std::string & tag);

    virtual LZDataPtr write(const std::string & tag, std::string value_="");

    virtual const DataPool& children(void){ return m_children; }

    virtual int countChildren(void){ return (int)m_children.size(); }

    virtual int addChild(LZDataPtr ptr){ m_children.push_back(ptr); return countChildren()-1;}

    virtual LZDataPtr getChild(int index){ return m_children[index]; }

    virtual int findChild(const std::string & name){ return findNextChild(-1, name); }

    virtual int findNextChild(int index, const std::string& name);

    virtual bool save(const std::string & fileName) ;

protected:
    std::string m_tag;
    std::string m_value;
    DataPool    m_children;
};

LZDataPtr createLzdFromBuffer(LzDataType type, const char* buffer, int length);

LZDataPtr createLzdFromFile(LzDataType type, const std::string & fname);

void printTable(std::ostream & cout, int n);
bool readFileBuffer(std::vector<char> & buffer, const std::string & file);

bool isWhiteSpace(char ch);
void ltrimString(std::string & str);
void rtrimString(std::string & str);
void trimString(std::string & str);