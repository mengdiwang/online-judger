//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "RefPtr.h"
#include "Utilities.h"
#include <mysql.h>

//////////////////////////////////////////////////////////////////////////
//mysql一行数据
//////////////////////////////////////////////////////////////////////////
class ZMysqlRow : public IBase
{
public:
    typedef std::vector<StringTool> StringVector;

    ZMysqlRow(MYSQL_ROW rowData, int numCols);

    int cols() const { return (int)m_rowData.size();}

    const StringTool & col(int i) const { return m_rowData[i]; }

private:
    StringVector m_rowData;
};

typedef RefPtr<ZMysqlRow> ZMysqlRowPtr;

//////////////////////////////////////////////////////////////////////////
//mysql查询得到的数据
//////////////////////////////////////////////////////////////////////////
class ZMySqlRes : public IBase
{
public:
    explicit ZMySqlRes(MYSQL_RES * res);

    ~ZMySqlRes();
   
    //行数
    uint64 rows();

    //列数
    int cols();

    //获得标题域
    MYSQL_FIELD* getField(int i);

    //域转换成索引
    int fieldIndex(const std::string & field);

    ZMysqlRowPtr fetchRow();

private:

    //禁止拷贝构造，复制
    ZMySqlRes(const ZMySqlRes & );
    const ZMySqlRes & operator=(const ZMySqlRes &);

private:
    MYSQL_RES* m_result;
};

typedef RefPtr<ZMySqlRes> ZMySqlResPtr;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class ZMySql : public IBase
{
public:
    ZMySql(void);
    virtual ~ZMySql(void);

    bool loadService();

    void unloadService();

    bool setOption(mysql_option option, const std::string & arg);

    bool setCharSet(const std::string & charset);

    bool connect(const std::string & host, uint port,
        const std::string & user,
        const std::string & passwd,
        const std::string & db);

    bool selectDB(const std::string & db);

    bool disconect();

    bool query(const std::string & sql);

    std::string escapeString(const std::string & str);

    uint64 getAffectedRows();

    uint64 getInsertID();

    ZMySqlResPtr storeResult();

    unsigned int errorNo();

    std::string error();

protected:
    MYSQL       m_mysql;
};

typedef RefPtr<ZMySql> ZMySqlPtr;