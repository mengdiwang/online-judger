//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "Comm.h"
#include "ZJobObject.h"

ZJobObject::ZJobObject(void)
    : m_handle(NULL)
{
}

ZJobObject::~ZJobObject(void)
{
    close();
}

//创建作业对象
bool ZJobObject::create(const tstring & name, 
                        LPSECURITY_ATTRIBUTES lpJobAttributes/* = NULL*/)
{
    if (m_handle)
    {
        return false;
    }
    m_handle = CreateJobObject(lpJobAttributes, name.c_str());
    if (m_handle == INVALID_HANDLE_VALUE)
    {
        m_handle = 0;
    }
    return m_handle != 0;
}

void ZJobObject::close()
{
    SAFE_CLOSE_HANDLE(m_handle);
}

//等待作业结束
DWORD ZJobObject::wait(DWORD time/* = INFINITE*/)
{
    return WaitForSingleObject(m_handle, time);
}

//终止作业
bool ZJobObject::terminate(DWORD exitCode/* = 4*/)
{
    return !!TerminateJobObject(m_handle, exitCode);
}

//设置作业信息
bool ZJobObject::setInformation(JOBOBJECTINFOCLASS infoClass,
                    LPVOID lpInfo,
                    DWORD cbInfoLength)
{
    return !!SetInformationJobObject(m_handle, infoClass, lpInfo, cbInfoLength);
}

bool ZJobObject::assinProcess(HANDLE handel)
{
    return !!AssignProcessToJobObject(m_handle, handel);
}