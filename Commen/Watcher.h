//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "ZProcess.h"

//////////////////////////////////////////////////////////////////////////
//进程监视器
//作用：监视进程占用的时间和内存，如果超出范围，则强制结束。
//原理：启动一个线程，每隔一段时间检测一次进程占用的时间和内存。
//////////////////////////////////////////////////////////////////////////
class Watcher
{
public:
    Watcher(ZProcessBase* proc, uint64 time, uint64 memory);

    virtual ~Watcher();

    //等待监视线程结束。
    void waitExit() {  threadWatcher = NULL; }

    bool isTimeOut() { return timeLimited; }

    bool isMemoryOut() { return memoryLimited; }

    int64 getRunMemory(){ return runMemory; }

    uint64 getRunTime(){ return runTime; }

    //线程工作函数
    virtual void _doWatch();

protected:

    virtual bool updateTime();

    virtual bool updateMemory();

private:
    ZProcessBase*   process;    //被监视的进程
    uint64          limitTime;  //限制的时间 (单位100ns)
    uint64          runTime;    //运行时间 (单位100ns)
    uint64          limitMemory;//限制的内存 (单位byte)
    uint64          runMemory;  //运行占用内存 (单位byte)
    bool            timeLimited;//是否超时
    bool            memoryLimited;//是否超出内存
    ZThreadPtr      threadWatcher;//监视线程
};

//获得进程占用内存
uint64 getProcessMemory(HANDLE hprocess);

//获得进程耗费的时间
uint64 getProcessTime(HANDLE hprocess);