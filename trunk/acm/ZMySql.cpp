//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "Comm.h"
#include "Log.h"
#include "Utilities.h"

#include "ZMySql.h"

#pragma comment(lib, "libmysql")


//////////////////////////////////////////////////////////////////////////

ZMysqlRow::ZMysqlRow(MYSQL_ROW rowData, int numCols)
    : m_rowData(numCols)
{
    for (int i=0; i<numCols; ++i)
    {
        m_rowData[i].setText(rowData[i]);
    }
}

//////////////////////////////////////////////////////////////////////////

ZMySqlRes::ZMySqlRes(MYSQL_RES * res)
    : m_result(res)
{

}

ZMySqlRes::~ZMySqlRes()
{
    if (m_result)
    {
        mysql_free_result(m_result);
        m_result = NULL;
    }
}

uint64 ZMySqlRes::rows()
{
    return mysql_num_rows(m_result);
}

int ZMySqlRes::cols()
{
    return mysql_num_fields(m_result);
}

MYSQL_FIELD* ZMySqlRes::getField(int i)
{
    assert(i>=0 && i<cols() && "getField out of range");
    return mysql_fetch_field_direct(m_result, i);
}

//Óò×ª»»³ÉË÷Òý
int ZMySqlRes::fieldIndex(const std::string & field)
{
    for (int i=0; i<cols(); ++i)
    {
        if (field == getField(i)->name)
        {
            return i;
        }
    }
    return -1;
}

ZMysqlRowPtr ZMySqlRes::fetchRow()
{
    MYSQL_ROW rowData = mysql_fetch_row(m_result);
    if (NULL == rowData)
    {
        return NULL;
    }
    return new ZMysqlRow(rowData, cols());
}

//////////////////////////////////////////////////////////////////////////
ZMySql::ZMySql(void)
{
    memset(&m_mysql, 0, sizeof(m_mysql));
}

ZMySql::~ZMySql(void)
{
}

bool ZMySql::loadService()
{
    return 0 == mysql_library_init(0, NULL, NULL);
}

void ZMySql::unloadService()
{
    mysql_library_end();
}

bool ZMySql::setOption(mysql_option option, const std::string & arg)
{
    return 0 == mysql_options(&m_mysql, option, arg.c_str());
}


bool ZMySql::setCharSet(const std::string & charset)
{
    return setOption(MYSQL_SET_CHARSET_NAME, charset.c_str());
}

bool ZMySql::connect(const std::string & host, uint port,
                     const std::string & user,
                     const std::string & passwd,
                     const std::string & db )
{
    if(!mysql_init(&m_mysql))
    {
        return false;
    }

    return NULL != mysql_real_connect(
        &m_mysql, 
        host.c_str(), 
        user.c_str(), 
        passwd.c_str(), 
        db.c_str(),
        port,
        NULL, 0);
}

bool ZMySql::selectDB(const std::string & db)
{
     return 0 == mysql_select_db(&m_mysql, db.c_str());
}

bool ZMySql::disconect()
{
    mysql_close(&m_mysql);
    return true;
}

unsigned int ZMySql::errorNo()
{
    return mysql_errno(&m_mysql);
}

std::string ZMySql::error()
{
    return mysql_error(&m_mysql);
}

bool ZMySql::query(const std::string & sql)
{
    return 0 == mysql_real_query(&m_mysql, sql.c_str(), sql.length());
}

ZMySqlResPtr ZMySql::storeResult()
{
    MYSQL_RES* result = mysql_store_result(&m_mysql);
    if (NULL == result)
    {
        return NULL;
    }
    
    return new ZMySqlRes(result);
}

uint64 ZMySql::getAffectedRows()
{
    return mysql_affected_rows(&m_mysql);
}

uint64 ZMySql::getInsertID()
{
    return mysql_insert_id(&m_mysql);
}

std::string ZMySql::escapeString(const std::string & str)
{
    std::string dest(str.size()*2, '\0');
    int len = (int)mysql_real_escape_string(&m_mysql, &dest[0], str.c_str(), str.size());
    dest[len] = 0;
    trimPartStringA(dest);
    return dest;
}