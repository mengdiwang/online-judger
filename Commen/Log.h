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
#include "ZLock.h"

/**日志系统*/
class cLog
{
public:

	cLog(void);

	virtual ~cLog(void);

    bool init(const tstring & fileName);

    void unload(void);

    bool isUsefull(void){ return m_bUsefull; }

    void writeA(const std::string & buffer);

    void writeW(const std::wstring & buffer);

    void xwriteA(LPCSTR format, ...);

    void xwriteW(LPCWSTR format, ...);

protected:

    void generateTimeHead(tstring & header);

    void write(const tstring & buffer);

private:
    tofstream	    m_fLog;		//输出文件流
	bool			m_bUsefull;	//日志系统是否可用
	SYSTEMTIME		m_systm;

    ZMutex          m_writeMutex;
    ZMutex          m_formatWriteMutex;
};

extern cLog g_globle_log_;

/**初始日志系统。*/
#define  INIT_LOG(fileName)       g_globle_log_.init(fileName)

/**写日志*/
#define  WRITE_LOGA(buffer)       g_globle_log_.writeA(buffer)

/**写日志*/
#define  WRITE_LOGW(buffer)       g_globle_log_.writeW(buffer)

/**写日志*/
#define  XWRITE_LOGA(format, ...)   g_globle_log_.xwriteA(format, __VA_ARGS__)

/**写日志*/
#define  XWRITE_LOGW(format, ...)   g_globle_log_.xwriteW(format, __VA_ARGS__)

/**卸载日志系统*/
#define  UNLOAD_LOG()               g_globle_log_.unload()

#ifdef _UNICODE
#   define XWRITE_LOG   XWRITE_LOGW
#   define WRITE_LOG    WRITE_LOGW
#else
#   define XWRITE_LOG   XWRITE_LOGA
#   define WRITE_LOG    WRITE_LOGA
#endif
