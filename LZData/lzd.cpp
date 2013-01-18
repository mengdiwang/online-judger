//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "lzd.h"
#include "lzdParser.h"



//////////////////////////////////////////////////////////////////////////

//写入文件时，需要对#、}、；转义
std::string convertValue(std::string src)
{
    std::string dest;
    for (size_t i=0; i<src.size(); ++i)
    {
        char ch = src[i];
        if (ch=='#' || ch=='}' || ch==';')
        {
            dest += '\\';
        }
        dest += ch;
    }
    return dest;
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/*static*/ LZDataPtr lzd::createFromBuffer(const char* buffer, int length)
{
    LZDataPtr root = new lzd("root");
    cParser parser(buffer, root);
    parser.parse();
    return root;
}

/*static*/ LZDataPtr lzd::createFromFile(const std::string & fname)
{
    std::vector<char> buffer;
    if(!readFileBuffer(buffer, fname))
    {
        return NULL;
    }
    return createFromBuffer(&buffer[0], buffer.size());
}

lzd::lzd(void)
{
}

lzd::lzd(const std::string & tag)
    : LZData(tag)
{
}

lzd::lzd(const std::string & tag, const std::string & value_)
    : LZData(tag, value_)
{
}

lzd::~lzd(void)
{
}

LZDataPtr lzd::newOne(const std::string & tag, const std::string & value_)
{
    return new lzd(tag, value_);
}


void lzd::print(std::ostream & out, int depth)
{
    for (DataConstIterator it = m_children.begin(); it != m_children.end(); ++it)
    {
        LZDataPtr ptr = *it;
        if ( ptr->countChildren() > 0)
        {
            printTable(out, depth);
            out<<ptr->tag()<<std::endl;

            printTable(out, depth);
            out<<"{"<<std::endl;

            ptr->print(out, depth+1);

            printTable(out, depth);
            out<<"}"<<std::endl;
        }
        else
        {
            printTable(out, depth);
            out<<ptr->tag()<<" = "<<convertValue(ptr->asString())<<std::endl;
        }
    }
}