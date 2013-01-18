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
#include "RefPtr.h"
#include "ZThread.h"
#include "ZJobObject.h"

class ZProcessBase : public IBase
{
public:
    ZProcessBase();

    virtual ~ZProcessBase(void);

    virtual bool create(const tstring & cmd, bool start_ = true) = 0;

    virtual bool start() = 0;

    DWORD waitExit(int time = INFINITE);

    virtual DWORD getExitCode();

    void kill(DWORD exitCode=0);

    bool isRunning();

    HANDLE hprocess(){ return m_hProcess; }

    HANDLE hthread(){ return m_hThread; }

protected:

    virtual bool createProcess(
        LPTSTR szCMD,
        BOOL inheriteHandle, 
        DWORD creationFlag,
        STARTUPINFO & si, 
        PROCESS_INFORMATION & pi);

protected:
    HANDLE  m_hProcess;
    HANDLE  m_hThread;
};

typedef RefPtr<ZProcessBase> ZProcessPtr;
//////////////////////////////////////////////////////////////////////////

class ZProcess : public ZProcessBase
{
public:
    virtual bool create(const tstring & cmd, bool start_ = true);

    virtual bool start();

};

//////////////////////////////////////////////////////////////////////////
class ZProcessInOut : public ZProcessBase
{
public:
    ZProcessInOut(const tstring &  inFile, const tstring &  outFile);
    ~ZProcessInOut();

    virtual bool create(const tstring &  cmd, bool start_ = true);

    virtual bool start();

protected:

    virtual HANDLE createInputFile();
    virtual HANDLE createOutputFile();

protected:
    HANDLE  m_hInput;
    HANDLE  m_hOutput;

    tstring m_inputFile;
    tstring m_outputFile;
};

class ZProcessDebug : public ZProcessInOut
{
public:
    ZProcessDebug(const tstring &  inFile, const tstring &  outFile);
    ~ZProcessDebug();

    virtual bool create(const tstring &  cmd, bool start_ = true);

    virtual bool start();
};

class ZProcessJob : public ZProcessInOut
{
public:

    ZProcessJob(const tstring &  inFile, const tstring &  outFile,
        int timeMS, int memoryKB);

    ~ZProcessJob();

    virtual bool create(const tstring &  cmd, bool start_ = true);

    virtual bool start();

    virtual DWORD getExitCode(){ return m_exitCode; }

    int getRunTime(){ return m_runTime; }
    int getRunMemory(){ return m_runMemory; }

protected:
    int         m_limitTime;
    int         m_limitMemory;
    int         m_runTime;
    int         m_runMemory;

    ZJobObject  m_job;
    HANDLE      m_ioCPHandle;
    DWORD       m_exitCode;
};