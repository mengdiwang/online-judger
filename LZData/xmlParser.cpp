//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#include "xmlParser.h"
#include "xml.h"


//////////////////////////////////////////////////////////////////////////
xmlParser::xmlParser(const char* buffer, int size)
    : m_stream(buffer, size)
{
}

xmlParser::~xmlParser(void)
{
}

bool xmlParser::parse(LZDataPtr parent)
{
    if (!parent)
    {
        m_root = new xml("root");
    }
    else
    {
        m_root = parent;
    }
    return parseReal(m_root);
}

bool xmlParser::parseReal(LZDataPtr parent)
{
    while(!m_stream.empty())
    {
        int begin = m_stream.find('<');
        if (begin == -1)
        {
            break;
        }
        int end = m_stream.find('>');
        if (end == -1)
        {
            printf("couldn't find end mark! '>'\n");
            return false;
        }
        m_stream.seek(end+1);

        std::string tag ;
        m_stream.getRange(tag, begin+1, end);
        trimString(tag);

        if (tag[0] == '/') //是结束标签
        {
            tag.erase(0, 1);
            rtrimString(tag);

            if (tag != parent->tag())
            {
                printf("end tag </%s> not match with begin tag <%s>\n", tag.c_str(), parent->tag().c_str());
                return false;
            }
            return true; //匹配成功
        }
        else
        {
            bool isAttr = false;
            int lastIndex = (int)tag.length()-1;
            if (lastIndex > 0 && tag[lastIndex] == '/') //属性
            {
                isAttr = true;
                size_t pos = tag.find_first_of(' ');
                if (pos == tag.npos)
                {
                    tag.erase(lastIndex);
                }
                else
                {
                    tag.erase(pos);
                }
            }

            LZDataPtr node = new xml(tag);
            parent->addChild(node);

            if (!isAttr)
            {
                begin = m_stream.index();
                end = m_stream.find('<'); //找到结束标签的位置
                if (end == -1)
                {
                    printf("couldn't find next mark '<'!\n");
                    return false;
                }
                m_stream.seek(end);

                std::string value_;
                m_stream.getRange(value_, begin, end);
                trimString(value_);

                node->setString(value_);

                //解析子节点
                if (!parseReal(node))
                {
                    return false;
                }
            }
        }
    }
    if(parent != m_root)
    {
        printf("tag not closed : <%s>\n", parent->tag());
        return false;
    }
    return true;
}