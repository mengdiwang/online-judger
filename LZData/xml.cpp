//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#include "xml.h"
#include "xmlParser.h"

//////////////////////////////////////////////////////////////////////////
/*static*/ LZDataPtr xml::createFromBuffer(const char* buffer, int length)
{
    xmlParser parser(buffer, length);
    parser.parse();
    return parser.root();
}

/*static*/ LZDataPtr xml::createFromFile(const std::string & fname)
{
    std::vector<char> buffer;
    if(!readFileBuffer(buffer, fname))
    {
        return NULL;
    }
    return createFromBuffer(&buffer[0], buffer.size());
}


//////////////////////////////////////////////////////////////////////////
xml::xml(void)
{
}

xml::~xml(void)
{
}

xml::xml(const std::string & tag)
    : LZData(tag)
{

}
xml::xml(const std::string & tag, const std::string & value)
    : LZData(tag, value)
{

}


void xml::print(std::ostream & out, int depth/*=0*/)
{
    for (DataConstIterator it = m_children.begin(); it != m_children.end(); ++it)
    {
        LZDataPtr ptr = *it;
        if ( ptr->countChildren() > 0)
        {
            printTable(out, depth);
            out<< "<" << ptr->tag() << ">" <<std::endl;

            ptr->print(out, depth+1);

            printTable(out, depth);
            out<< "</" << ptr->tag() << ">" <<std::endl;
        }
        else
        {
            printTable(out, depth);
            out<< "<" << ptr->tag() << ">\t" << ptr->asString();
            out<< "\t</" << ptr->tag() << ">" <<std::endl;
        }
    }
}


LZDataPtr xml::newOne(const std::string & tag, const std::string & value_)
{
    return new xml(tag, value_);
}