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

//////////////////////////////////////////////////////////////////////////
//windows 作业对象
//////////////////////////////////////////////////////////////////////////
class ZJobObject :  public IBase
{
public:
    ZJobObject(void);

    virtual ~ZJobObject(void);

    HANDLE handle(){ return m_handle; }

    bool valid() const { return m_handle != 0; }

    void close();

    //创建作业对象
    bool create(const tstring & name, LPSECURITY_ATTRIBUTES lpJobAttributes = NULL);

    //等待作业结束
    DWORD wait(DWORD time = INFINITE);

    //终止作业
    bool terminate(DWORD exitCode = 4);

    //设置作业信息
    bool setInformation(JOBOBJECTINFOCLASS infoClass,
        LPVOID lpInfo,
        DWORD cbInfoLength);

    //将进程应用到job
    bool assinProcess(HANDLE handel);

protected:
    HANDLE m_handle;
};
