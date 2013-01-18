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

#include "Watcher.h"

#include <psapi.h>

//获得进程占用内存
uint64 getProcessMemory(HANDLE hprocess)
{
    PROCESS_MEMORY_COUNTERS pmc;
    ZeroMemory(&pmc, sizeof(pmc));
    pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
    if ( GetProcessMemoryInfo( hprocess, &pmc, sizeof(pmc)) )
    {
        //return pmc.PeakWorkingSetSize;
        //return pmc.WorkingSetSize;
        return pmc.PeakPagefileUsage;
    }
    return 0;
}

//获得进程耗费的时间
uint64 getProcessTime(HANDLE hprocess)
{
    FILETIME userTime, temp;
    GetProcessTimes(hprocess, &temp, &temp, &temp, &userTime);
    ULARGE_INTEGER time;
    time.LowPart = userTime.dwLowDateTime;
    time.HighPart = userTime.dwHighDateTime;
    return time.QuadPart;
}

//////////////////////////////////////////////////////////////////////////


Watcher::Watcher(ZProcessBase* proc, uint64 time, uint64 memory)
    : process(proc)
    , limitTime(time)
    , limitMemory(memory)
    , runMemory(0)
    , runTime(0)
    , timeLimited(false)
    , memoryLimited(false)
{
    threadWatcher = new ZThreadEx<Watcher>(this, &Watcher::_doWatch);
}

Watcher::~Watcher()
{

}

void Watcher::_doWatch()
{
    while(process->isRunning())
    {
        Sleep(10);

        if (updateTime())
        {
            timeLimited = true;
            break;
        }
        if (updateMemory())
        {
            memoryLimited = true;
            break;
        }
    }
    if (process->isRunning())
    {
        XWRITE_LOGA("WATCHER: kill process! time:%I64u, memory:%I64u", runTime, runMemory);
        process->kill();
    }

    //最后再更新一次
    updateTime();
    updateMemory();
}

bool Watcher::updateTime()
{
    runTime = getProcessTime(process->hprocess());
    return runTime > limitTime;
}

bool Watcher::updateMemory()
{
    uint64 memory = getProcessMemory(process->hprocess());
    if (memory > runMemory)
    {
        runMemory = memory;
    }
    return runMemory > limitMemory;
}
