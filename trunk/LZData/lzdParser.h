//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "LZData.h"

///语法分析器。仅分析一个模块，即从当前位置起，到第一个‘}’终止
class cParser : public IBase
{
public:
    /** 构造函数。
        @param text : 待解析文本。
        @pram curNode : 当前模块结点。
        */
    cParser(std::string text, LZDataPtr curNode);
    ~cParser(void);

    ///开始分析
    void parse(void);

    ///获得该模块分析后的文本
    std::string getText(void);

    ///获得行号
    int getLines(void);

    ///是否有错误
    bool error(void);

    ///错误编号
    int errorID(void);

    ///错误描述
    std::string errorDesc(void);

protected:
    ///左大括号
    void onLBraket(void);

    ///右花括号
    void onRBraket(void);

    ///等号
    void onEqual(void);

    ///空白符
    void onSpace(void);

    ///回车符
    void onReturn(void);

    ///注释‘#’
    void onComment(void);

    ///分号
    void onSemicolon(void);

    ///其他字符
    void onOther(char ch);

    ///错误处理
    void onError(void);

    ///结束符
    void onEnd(void);


    ///跳过空白符
    void skipSpace(void);

    ///跳到语句结尾
    void skipToEnd(void);

    ///跳到行尾
    void skipToReturn(void);

    ///从当前位置起，获得一个合法字符串。
    std::string getAStr(void);

    ///设置变量名
    void setVar(std::string name, int line = -1);

    ///分析变量名
    std::string parseVarName( void );

    ///设置错误
    void setError(int id_);

private:
    std::string     m_strText;
    LPCSTR          m_pText;
    LZDataPtr    m_pCurNode;
    bool            m_bEnd;

    int             m_errorID;
    std::string     m_errorDesc;

    std::string     m_variable;
    int             m_varLine;
    std::string     m_curVariable;
    int             m_curLine;
};
