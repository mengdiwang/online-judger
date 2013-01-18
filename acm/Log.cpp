//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "Log.h"
#include "Utilities.h"

const int   MAX_SIZE_BUFFER = 4096;
char        g_szMutiBuffer[MAX_SIZE_BUFFER];
wchar_t     g_szWideBuffer[MAX_SIZE_BUFFER];
cLog        g_globle_log_ ;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

cLog::cLog(void)
{
	m_bUsefull=false;
}

cLog::~cLog(void)
{
    unload();
}

bool cLog::init(const tstring & fileName)
{
	if(m_bUsefull)
	{
		return false;
	}

	m_fLog.open(fileName.c_str());
	if(!m_fLog.good())
	{
		return false;
	}
	m_bUsefull = true;
    m_fLog.imbue(std::locale(std::locale(""), "", LC_CTYPE));
    
	GetLocalTime(&m_systm);
	TCHAR buffer[128];
	msprintf_s(buffer, 128, _T("---[%d-%d-%d]日志系统启动---\n"), 
        m_systm.wYear, m_systm.wMonth, m_systm.wDay);
	write(buffer);

	return true;
}

void cLog::unload(void)
{
    if(!m_bUsefull)
    {
        return ;
    }


    GetLocalTime(&m_systm);
    TCHAR buffer[128];
    msprintf_s(buffer, 128, _T("***[%d-%d-%d]日志系统停止***\n\n"), 
        m_systm.wYear, m_systm.wMonth, m_systm.wDay);
    write(buffer);

    m_fLog.close();

    m_bUsefull = false;
}

void cLog::generateTimeHead(tstring & header)
{
    GetLocalTime(&m_systm);
    
    header.resize(128);
    msprintf_s(&header[0], 128, _T("[%2.2d:%2.2d:%2.2d]: "), 
        m_systm.wHour, m_systm.wMinute, m_systm.wSecond);

    trimPartString(header);
}

void cLog::write(const tstring & buffer)
{
    if (!m_bUsefull)
    {
        //OutputMsg("log unusefull:%s", buffer.c_str());
        return;
    }

#ifdef _DEBUG

#ifdef _UNICODE
//     std::string tmpString = wcharToChar(buffer);
//     OutputMsg("%s", tmpString.c_str());
    OutputMsgW(_T("%s"), buffer.c_str());
#else
    OutputMsg("%s", buffer.c_str());
#endif

#endif

    ZLockHolder holder(&m_writeMutex);
    tstring header;
    generateTimeHead(header);
    m_fLog<<header<<buffer<<std::endl;
}

void cLog::writeA(const std::string & buffer)
{
#ifdef _UNICODE
    write(charToWChar(buffer));
#else
    write(buffer);
#endif
}

void  cLog::writeW(const std::wstring & buffer)
{
#ifdef _UNICODE
    write(buffer);
#else
    write(wcharToChar(buffer));
#endif
}

void cLog::xwriteA(LPCSTR format, ...  )
{
    ZLockHolder holder(&m_formatWriteMutex);

    va_list vl;
    va_start (vl, format);
    vsnprintf_s (g_szMutiBuffer, MAX_SIZE_BUFFER, MAX_SIZE_BUFFER, format, vl);
    va_end (vl);  

    writeA(g_szMutiBuffer);
}


void cLog::xwriteW(LPCWSTR format, ...  )
{
    ZLockHolder holder(&m_formatWriteMutex);

    va_list vl;
    va_start (vl, format);
    vswprintf_s(g_szWideBuffer, MAX_SIZE_BUFFER, format, vl);
    va_end (vl);  

    writeW(g_szWideBuffer);
}
