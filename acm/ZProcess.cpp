//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#include "ZProcess.h"
#include "Utilities.h"


#define BUFSIZE 1024

ZProcessBase::ZProcessBase()
    : m_hProcess(NULL)
    , m_hThread(NULL)
{
    
}

ZProcessBase::~ZProcessBase(void)
{
    SAFE_CLOSE_HANDLE(m_hProcess);
    SAFE_CLOSE_HANDLE(m_hThread);
}

DWORD ZProcessBase::waitExit(int time/* = INFINITE*/)
{
    if (isRunning())
    {
        WaitForSingleObject( m_hProcess, time ); 
    }
    return getExitCode();
}

void ZProcessBase::kill(DWORD exitCode/*=0*/)
{
    TerminateProcess(m_hProcess, exitCode);
}

DWORD ZProcessBase::getExitCode()
{
    DWORD code = 0;
    GetExitCodeProcess(m_hProcess, &code);
    return code;
}

bool ZProcessBase::isRunning()
{
    return getExitCode() == STILL_ACTIVE;
}

bool ZProcessBase::createProcess(LPTSTR szCMD,
    BOOL inheriteHandle, 
    DWORD creationFlag,
    STARTUPINFO & si, 
    PROCESS_INFORMATION & pi)
{
    return !!CreateProcess( NULL,   //   No module name (use command line).   
        szCMD,                      //   Command line.   
        NULL,                       //   Process handle not inheritable.   
        NULL,                       //   Thread handle not inheritable.   
        inheriteHandle,             //   Set handle inheritance to FALSE.   
        creationFlag,               //   No creation  flags.   
        NULL,                       //   Use parent 's environment block.   
        NULL,                       //   Use parent 's starting  directory.   
        &si,                        //   Pointer to STARTUPINFO structure. 
        &pi   );                    //   Pointer to PROCESS_INFORMATION structure.
}

//////////////////////////////////////////////////////////////////////////
bool ZProcess::create(const tstring &  cmd, bool start_)
{
    TCHAR szCMD[BUFSIZE];
    lstrcpy(szCMD, cmd.c_str());

    STARTUPINFO   si; 
    PROCESS_INFORMATION   pi; 

    ZeroMemory(&si, sizeof(si)); 
    si.cb   =   sizeof(si); 
    ZeroMemory(&pi, sizeof(pi)); 
  
    if(!createProcess(szCMD, FALSE, 0, si, pi))   
    { 
        return false;
    }

    m_hProcess = pi.hProcess;
    m_hThread = pi.hThread;

    if(start_)
    {
        start();
    }

    return true;
}

bool ZProcess::start()
{
    ResumeThread(m_hThread);
    return true;
}


//////////////////////////////////////////////////////////////////////////
ZProcessInOut::ZProcessInOut(const tstring &  inFile, const tstring &  outFile)
    : m_inputFile(inFile)
    , m_outputFile(outFile)
    , m_hInput(NULL)
    , m_hOutput(NULL)
{
}

ZProcessInOut::~ZProcessInOut()
{
    SAFE_CLOSE_HANDLE(m_hInput);
    SAFE_CLOSE_HANDLE(m_hOutput);
}

HANDLE ZProcessInOut::createInputFile()
{
    if (m_inputFile.empty())
    {
        return NULL;
    }
    SECURITY_ATTRIBUTES saAttr = {0};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE handle = CreateFile(
        m_inputFile.c_str(),
        GENERIC_READ,          	
        FILE_SHARE_READ,       	
        &saAttr,                  
        OPEN_EXISTING,         	
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (!handle)
    {
        OutputMsg(_T("open file faild:%d"), GetLastError());
    }

    return handle;
}

HANDLE ZProcessInOut::createOutputFile()
{
    if (m_outputFile.empty())
    {
        return NULL;
    }

    SECURITY_ATTRIBUTES saAttr = {0};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE handle = CreateFile(
        m_outputFile.c_str(),
        GENERIC_WRITE, 
        FILE_SHARE_READ,
        &saAttr,                   	
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (!handle)
    {
        OutputMsg(_T("create file faild:%d"), GetLastError());
    }

    return handle;
}

bool ZProcessInOut::create(const tstring & cmd, bool start_ /*= true*/)
{
    if (NULL != m_hProcess)
    {
        OutputMsg(_T("process has been created!"));
        return false;
    }

    TCHAR cmd_[BUFSIZE];
    lstrcpy(cmd_, cmd.c_str());

    m_hInput = createInputFile();
    m_hOutput = createOutputFile();

	/*CreateProcess的第一个参数
	将标准输出和错误输出定向到我们建立的ChildOut_Write上
	将标准输入定向到我们建立的ChildIn_Read上
	设置子进程接受StdIn以及StdOut的重定向
	*/
	STARTUPINFO StartupInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.hStdOutput = m_hOutput;
    StartupInfo.hStdError = m_hOutput;
	StartupInfo.hStdInput = m_hInput;
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION ProcessInfo;
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
    
//    int time = GetTickCount();
    if(!createProcess(cmd_, TRUE, CREATE_SUSPENDED, StartupInfo, ProcessInfo))
    { 
        return false;
    }
//    time = GetTickCount() - time;
//    OutputMsg("create process use time:%d", time);

    m_hProcess = ProcessInfo.hProcess;
    m_hThread = ProcessInfo.hThread;

    if (start_)
    {
        start();
    }

    return true;
}

bool ZProcessInOut::start()
{
    ResumeThread(m_hThread);
    return true;
}


ZProcessDebug::ZProcessDebug(const tstring &  inFile, const tstring &  outFile)
    : ZProcessInOut(inFile, outFile)
{

}
ZProcessDebug::~ZProcessDebug()
{

}

bool ZProcessDebug::create(const tstring &  cmd, bool start_/* = true*/)
{
    if (NULL != m_hProcess)
    {
        OutputMsg(_T("process has been created!"));
        return false;
    }

    TCHAR cmd_[BUFSIZE];
    lstrcpy(cmd_, cmd.c_str());

    m_hInput = createInputFile();
    m_hOutput = createOutputFile();

	/*CreateProcess的第一个参数
	将标准输出和错误输出定向到我们建立的ChildOut_Write上
	将标准输入定向到我们建立的ChildIn_Read上
	设置子进程接受StdIn以及StdOut的重定向
	*/
	STARTUPINFO StartupInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.hStdOutput = m_hOutput;
    StartupInfo.hStdError = m_hOutput;
	StartupInfo.hStdInput = m_hInput;
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION ProcessInfo;
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
    
//    int time = GetTickCount();
    if(!createProcess(cmd_, TRUE, CREATE_SUSPENDED | DEBUG_ONLY_THIS_PROCESS,
        StartupInfo, ProcessInfo))
    { 
        return false;
    }
//    time = GetTickCount() - time;
//    OutputMsg("create process use time:%d", time);

    m_hProcess = ProcessInfo.hProcess;
    m_hThread = ProcessInfo.hThread;

    if (start_)
    {
        start();
    }

    return true;
}

bool ZProcessDebug::start()
{
//    OutputMsgA("start run");
    ResumeThread(m_hThread);

    SAFE_CLOSE_HANDLE(m_hInput);
    SAFE_CLOSE_HANDLE(m_hOutput);
    SAFE_CLOSE_HANDLE(m_hThread);

    DEBUG_EVENT de;
    ZeroMemory(&de, sizeof(de));
    DWORD dwContinueStatus = DBG_CONTINUE;

    while (WaitForDebugEvent(&de, INFINITE)) 
    {
        if (de.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
        {
            //强制关闭exe文件
            SAFE_CLOSE_HANDLE(de.u.CreateProcessInfo.hFile);
        }
        else if (de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) 
        {
            if (de.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)
            {
            }
            else
            {
                switch(de.u.Exception.ExceptionRecord.ExceptionCode) 
                { 
                case EXCEPTION_INT_DIVIDE_BY_ZERO: //整数除法的除数是0时引发该异常。
                    OutputMsgA("INT_DIVIDE_BY_ZERO");
                    break;

                case EXCEPTION_INT_OVERFLOW://整数操作的结果溢出时引发该异常。
                    OutputMsgA("INT_OVERFLOW");
                    break;

                case EXCEPTION_ACCESS_VIOLATION: //程序企图读写一个不可访问的地址时引发的异常。例如企图读取0地址处的内存。
                    OutputMsgA("ACCESS_VIOLATION");
                    break;

                case EXCEPTION_DATATYPE_MISALIGNMENT://程序读取一个未经对齐的数据时引发的异常。
                    OutputMsgA("DATATYPE_MISALIGNMENT");
                    break;

                case EXCEPTION_FLT_STACK_CHECK: //进行浮点数运算时栈发生溢出或下溢时引发该异常。
                    OutputMsgA("FLT_STACK_CHECK");
                    break;

                case EXCEPTION_INVALID_DISPOSITION: //异常处理器返回一个无效的处理的时引发该异常。
                    OutputMsgA("INVALID_DISPOSITION");
                    break;

                case EXCEPTION_STACK_OVERFLOW: //栈溢出时引发该异常。
                    OutputMsgA("STACK_OVERFLOW");
                    break;

                default:
                    OutputMsgA("UNKNOW_EXCEPTION");
                    break;
                } 

                if (de.u.Exception.dwFirstChance)
                {
                    OutputMsgA("exception at 0x%08x, exception-code: 0x%08x",
                        de.u.Exception.ExceptionRecord.ExceptionAddress,
                        de.u.Exception.ExceptionRecord.ExceptionCode);
                }
                dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
            }
        }
        else if(de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) 
        {
            //OutputMsgA("progress exit with code:%u，%u", de.u.ExitProcess.dwExitCode, GetLastError());

            DebugSetProcessKillOnExit(TRUE);
            DebugActiveProcessStop(de.dwProcessId);


            if (de.u.ExitProcess.dwExitCode != 0)
            {
                return false;
            }
            break;
        }

        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, dwContinueStatus); 
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

ZProcessJob::ZProcessJob(const tstring &  inFile, const tstring &  outFile,
                         int timeMS, int memoryKB)
    : ZProcessInOut(inFile, outFile)
    , m_ioCPHandle(NULL)
    , m_exitCode(0)
    , m_runTime(0)
    , m_runMemory(0)
    , m_limitTime(timeMS)
    , m_limitMemory(memoryKB)
{

}

ZProcessJob::~ZProcessJob()
{
    SAFE_CLOSE_HANDLE(m_ioCPHandle);
}

bool ZProcessJob::create(const tstring &  cmd, bool start_/* = true*/)
{
    if (NULL != m_hProcess)
    {
        OutputMsg(_T("process has been created!"));
        return false;
    }

    int64 limitTime = m_limitTime * 10000; //100ns (1s = 10^9ns)
    int limitMemory = m_limitMemory * 1024; //bytes
    if (limitMemory < 0)//超出int范围了
    {
        limitMemory = 128*1024*1024; //默认128M
    }

    //////////////////////////////////////////////////////////////////////////
    //创建作业沙箱（job）
    //////////////////////////////////////////////////////////////////////////
    
    tstring jobName;
    generateGUID(jobName);
    if(!m_job.create(jobName))
    {
        OutputMsg(_T("create job faild!"));
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //设置job信息
    //////////////////////////////////////////////////////////////////////////

    //设置基本限制信息
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION subProcessLimitRes;
    ZeroMemory(&subProcessLimitRes, sizeof(subProcessLimitRes));

    JOBOBJECT_BASIC_LIMIT_INFORMATION & basicInfo = subProcessLimitRes.BasicLimitInformation;
    basicInfo.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_TIME| \
        JOB_OBJECT_LIMIT_PRIORITY_CLASS| \
        JOB_OBJECT_LIMIT_PROCESS_MEMORY| \
        JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
    basicInfo.PriorityClass = NORMAL_PRIORITY_CLASS;      //优先级为默认
    basicInfo.PerProcessUserTimeLimit.QuadPart = limitTime; //子进程执行时间ns(1s=10^9ns)
    subProcessLimitRes.ProcessMemoryLimit = limitMemory;    //内存限制

    m_job.setInformation(
        JobObjectExtendedLimitInformation,
        &subProcessLimitRes, 
        sizeof(subProcessLimitRes));


    //让完成端口发出时间限制的消息
    JOBOBJECT_END_OF_JOB_TIME_INFORMATION timeReport;
    ZeroMemory(&timeReport, sizeof(timeReport));

    timeReport.EndOfJobTimeAction = JOB_OBJECT_POST_AT_END_OF_JOB;

    m_job.setInformation(
        JobObjectEndOfJobTimeInformation, 
        &timeReport,
        sizeof(JOBOBJECT_END_OF_JOB_TIME_INFORMATION));


    //UI限制
    JOBOBJECT_BASIC_UI_RESTRICTIONS subProcessLimitUi;
    ZeroMemory(&subProcessLimitUi, sizeof(subProcessLimitUi));

    subProcessLimitUi.UIRestrictionsClass = JOB_OBJECT_UILIMIT_NONE| \
        JOB_OBJECT_UILIMIT_DESKTOP| \
        JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS| \
        JOB_OBJECT_UILIMIT_DISPLAYSETTINGS| \
        JOB_OBJECT_UILIMIT_EXITWINDOWS| \
        JOB_OBJECT_UILIMIT_GLOBALATOMS| \
        JOB_OBJECT_UILIMIT_HANDLES| \
        JOB_OBJECT_UILIMIT_READCLIPBOARD;

    m_job.setInformation(
        JobObjectBasicUIRestrictions,
        &subProcessLimitUi,
        sizeof(subProcessLimitUi));


    //将作业关联到完成端口，以确定其运行情况，及退出的原因
    int id = generateID();
    m_ioCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, id, 0);

    JOBOBJECT_ASSOCIATE_COMPLETION_PORT jobCP;
    ZeroMemory(&jobCP, sizeof(jobCP));

    jobCP.CompletionKey = (PVOID)id;
    jobCP.CompletionPort = m_ioCPHandle;

    m_job.setInformation(
        JobObjectAssociateCompletionPortInformation,
        &jobCP,
        sizeof(jobCP));

    //////////////////////////////////////////////////////////////////////////
    //创建子进程
    //////////////////////////////////////////////////////////////////////////

    TCHAR cmd_[BUFSIZE];
    lstrcpy(cmd_, cmd.c_str());

    m_hInput = createInputFile();
    m_hOutput = createOutputFile();

	/*CreateProcess的第一个参数
	将标准输出和错误输出定向到我们建立的m_hOutput上
	将标准输入定向到我们建立的m_hInput上
	设置子进程接受StdIn以及StdOut的重定向
	*/
	STARTUPINFO StartupInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.hStdOutput = m_hOutput;
    StartupInfo.hStdError = m_hOutput;
	StartupInfo.hStdInput = m_hInput;
	StartupInfo.dwFlags = STARTF_USESTDHANDLES;

    PROCESS_INFORMATION ProcessInfo;
    ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));
    
    if(!createProcess(cmd_, TRUE, CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB,
        StartupInfo, ProcessInfo))
    { 
        return false;
    }

    m_hProcess = ProcessInfo.hProcess;
    m_hThread = ProcessInfo.hThread;

    //////////////////////////////////////////////////////////////////////////
    //将子进程与job关联
    //////////////////////////////////////////////////////////////////////////

    if (start_)
    {
        start();
    }

    return true;
}

bool ZProcessJob::start()
{
    OutputMsgA("start run.");

    if(!m_job.assinProcess(m_hProcess))
    {
        OutputMsg(_T("应用进程到job失败!%d"), GetLastError());
        return false;
    }

    //启动子进程
    ResumeThread(m_hThread);
    
    //关闭标准输入文件和零时输出文件的句柄
    SAFE_CLOSE_HANDLE(m_hInput);
    SAFE_CLOSE_HANDLE(m_hOutput);

    //关闭主进程主线程句柄
    SAFE_CLOSE_HANDLE(m_hThread);

    //等待进程子进程处理完毕或耗尽资源退出

    DWORD ExecuteResult = -1;
    unsigned long completeKey;
    LPOVERLAPPED processInfo;
    bool done = false;
    while(!done)
    {
        GetQueuedCompletionStatus(
            m_ioCPHandle,
            &ExecuteResult, 
            &completeKey, 
            &processInfo, 
            INFINITE);

        switch (ExecuteResult) 
        {
        case JOB_OBJECT_MSG_NEW_PROCESS: 
            {
                OutputMsg(TEXT("New process (Id=%d) in Job"), processInfo);
            }
            break;

        case JOB_OBJECT_MSG_END_OF_JOB_TIME:
            {
                OutputMsg(TEXT("Job time limit reached"));
                m_exitCode = 1;
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_END_OF_PROCESS_TIME: 
            {
                OutputMsg(TEXT("Job process (Id=%d) time limit reached"), processInfo);
                m_exitCode = 1;
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_PROCESS_MEMORY_LIMIT: 
            {
                OutputMsg(TEXT("Process (Id=%d) exceeded memory limit"), processInfo);
                m_exitCode = 2;
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_JOB_MEMORY_LIMIT: 
            {
                OutputMsg(TEXT("Process (Id=%d) exceeded job memory limit"), processInfo);
                m_exitCode = 2;
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT:
            {
                OutputMsg(TEXT("Too many active processes in job"));
            }
            break;

        case JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO:
            {
                OutputMsg(TEXT("Job contains no active processes"));
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_EXIT_PROCESS: 
            {
                OutputMsg(TEXT("Process (Id=%d) terminated"), processInfo);
                done = true;
            }
            break;

        case JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS: 
            {
                OutputMsg(TEXT("Process (Id=%d) terminated abnormally"), processInfo);
                m_exitCode = 3;
                done = true;
            }
            break;

        default:
            OutputMsg(TEXT("Unknown notification: %d"), ExecuteResult);
            m_exitCode = 99;
            break;
        }
    }

    JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION jobai;
    ZeroMemory(&jobai, sizeof(jobai));
    QueryInformationJobObject(m_job.handle(), JobObjectBasicAndIoAccountingInformation, 
        &jobai, sizeof(jobai), NULL);
#if 0
    OutputMsg(_T("total user time : %dms"), jobai.BasicInfo.TotalUserTime.LowPart/10000);
    OutputMsg(_T("total kernel time : %dms"), jobai.BasicInfo.TotalKernelTime.LowPart/10000);
    OutputMsg(_T("Period user time : %dms"), jobai.BasicInfo.ThisPeriodTotalUserTime.LowPart/10000);
    OutputMsg(_T("Period kernel time : %dms"), jobai.BasicInfo.ThisPeriodTotalKernelTime.LowPart/10000);
#endif

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION joeli;
    ZeroMemory(&joeli, sizeof(joeli));
    QueryInformationJobObject(m_job.handle(), JobObjectExtendedLimitInformation, 
        &joeli, sizeof(joeli), NULL);

#if 0
    OutputMsg(_T("peek process memory : %dk"), joeli.PeakProcessMemoryUsed/1024);
    OutputMsg(_T("peek job memory : %dk"), joeli.PeakJobMemoryUsed/1024);
#endif

    m_runTime = jobai.BasicInfo.TotalUserTime.LowPart/10000;
    m_runMemory = joeli.PeakProcessMemoryUsed/1024;

    //关闭进程句柄
    SAFE_CLOSE_HANDLE(m_hProcess);

    //关闭完成端口
    SAFE_CLOSE_HANDLE(m_ioCPHandle);

    //为了安全，杀死作业内所有进程
    while(!m_job.terminate(0))
    {
        OutputMsg(_T("停止job失败!%d"), GetLastError());
        Sleep(1000);
    }
    //关闭作业句柄
    m_job.close();

    OutputMsgA("end run.");
    return true;
}