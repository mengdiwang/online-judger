//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2013.01
 * 
 * This file is part of Online Judger for HUSTOJ. about HUSTOJ 
 * see <http://code.google.com/p/hustoj>.
 *
 * HUSTOJ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * HUSTOJ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HUSTOJ. if not, see <http://www.gnu.org/licenses/>.
 */
//////////////////////////////////////////////////////////////////////////

#include "Comm.h"
#include "Utilities.h"
#include "Log.h"
#include "ZJobObject.h"
#include "Watcher.h"

#include "Excuter.h"
#include "Config.h"


#define GCC_ARG     _T("gcc.exe %s -o %s -O2 -Wall -lm --static -std=c99 -DONLINE_JUDGE")
#define GPLUS_ARG   _T("g++.exe %s -o %s -O2 -Wall -lm --static -DONLINE_JUDGE")

#define MAX_LEN     1024
#define KB_BYTES    1024
#define MB_BYTES    1048576
#define S_100NS     10000000
#define MS_100NS    10000

#define MATCHER_ACCEPT              0
#define MATCHER_WRONG_ARGS          1
#define MATCHER_NOT_EXIST_SRC       2
#define MATCHER_NOT_EXIST_DEST      3
#define MATCHER_WRONG_ANWSER        4
#define MATCHER_PRESENT_ERROR       5

const tstring whiteSpaces = _T(" \t\r\n");

extern Config config;

//////////////////////////////////////////////////////////////////////////
//c/c++编译程序
//////////////////////////////////////////////////////////////////////////
class CppCompiler : public ExcuterBase
{
public:
    virtual bool exc(TaskPtr task)
    {
        XWRITE_LOGA("[%d]compiling...", task->getSolutionID());

        if (!fileExist(task->getCodeFile()))
        {
            task->setResult(config.JE_SYSTEM);
            return false;
        }

        int language = task->getLanguage();
        if (language != config.CL_GCC && language != config.CL_GPLUS )
        {
            task->setResult(config.JE_COMPLIE);
            return false;
        }

        //目前只支持两种编译器gcc和g++
        LPCTSTR szArg = GPLUS_ARG;
        if(language == config.CL_GCC)
        {
            szArg = GCC_ARG;
        }

        TCHAR buffer[1024];
        msprintf_s(buffer, 1024, szArg, task->getCodeFile().c_str(), 
            task->getExeFile().c_str());

        XWRITE_LOG(_T("[%d]compile: %s"), task->getSolutionID(), buffer);

        ZProcessInOut proc(_T(""), task->getCompileOutputFile());
        if(!proc.create(buffer))
        {
			XWRITE_LOGA("[%d]create compile process error:%d", task->getSolutionID(), GetLastError());
            task->setResult(config.JE_SYSTEM);
            return false;
        }

        DWORD code = proc.waitExit(task->getLimitCompileTime());

        if (code != 0)
        {
            task->setResult(config.JE_COMPLIE);
            //////////////////////////////////////////////////////////////////////////
            tstring errorStr;
            tifstream fin(task->getCompileOutputFile().c_str());
            if (fin.good())
            {
                while(!fin.eof())
                {
                    fin.read(buffer, 1000);
                    buffer[fin.gcount()] = 0;
                    errorStr += buffer;
                }
                fin.close();
            }
            task->setCompileError(errorStr);
            //////////////////////////////////////////////////////////////////////////
            return false;
        }
        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
//在沙箱中运行
//暂未使用
//////////////////////////////////////////////////////////////////////////
class ExeRunner : public ExcuterBase
{
public:
    virtual bool exc(TaskPtr task)
    {
        XWRITE_LOGA("[%d]running...", task->getSolutionID());

        ZProcessJob process(task->getInputTestData(), 
            task->getExeOutputFile(),
            task->getLimitTime() * 1000,
            task->getLimitMemory() * KB_BYTES);

        if(!process.create(task->getExeFile(), false))
        {
            XWRITE_LOGA("[%d]create run process error!", task->getSolutionID());
            task->setResult(config.JE_SYSTEM);
            return false;
        }

        process.start();
        DWORD code = process.getExitCode();
        task->setRunTime(process.getRunTime());
        task->setRunMemory(process.getRunMemory());
      
        XWRITE_LOGA("[%d]end watch: time=%dms, memory=%dK",
            task->getSolutionID(), task->getRunTime(), task->getRunMemory());

        switch(code)
        {
        case 0:
            break;

        case 1:
            task->setResult(config.JE_TIME_LIMITED);
            break;

        case 2:
            task->setResult(config.JE_MEMORY_LIMITED);
            break;

        case 3:
            task->setResult(config.JE_RUNTIME);
            break;

         default:
            task->setResult(config.JE_RUNTIME);
            break;
        }

        return code == 0;
    }
};

//////////////////////////////////////////////////////////////////////////
//调试运行
//////////////////////////////////////////////////////////////////////////
class ExeDebugRunner : public ExcuterBase
{
public:
    virtual bool exc(TaskPtr task)
    {
        XWRITE_LOGA("[%d]running...", task->getSolutionID());

        ZProcessDebug process(task->getInputTestData(), task->getExeOutputFile());
        if(!process.create(task->getExeFile(), false))
        {
            XWRITE_LOGA("[%d]create run process error!", task->getSolutionID());
            task->setResult(config.JE_SYSTEM);
            return false;
        }

        uint64 maxMemory = uint64(task->getLimitMemory())*MB_BYTES;//bytes
        uint64 maxTime = uint64(task->getLimitTime())*S_100NS; //100ns

        Watcher watcher = Watcher(&process, maxTime, maxMemory);
        process.start();
        watcher.waitExit();
        
        task->setRunTime(int(watcher.getRunTime() / MS_100NS));
        task->setRunMemory(int(watcher.getRunMemory() / KB_BYTES));

        if (watcher.isMemoryOut())
        {
            task->setResult(config.JE_MEMORY_LIMITED);
            return false;
        }

        if (watcher.isTimeOut())
        {
            task->setResult(config.JE_TIME_LIMITED);
            return false;
        }

        DWORD code = process.getExitCode();
        //最后判短是否运行时错误
        if (code != 0)
        {
            task->setResult(config.JE_RUNTIME);
            return false;
        }

        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
//匹配结果
//////////////////////////////////////////////////////////////////////////
class Matcher : public ExcuterBase
{
public:
    virtual bool exc(TaskPtr task)
    {
        int result = compare(task->getOutputTestData(), task->getExeOutputFile());
        
        switch(result)
        {
        case MATCHER_ACCEPT :
            task->setResult(config.JE_ACCEPT);
            break;

        case MATCHER_WRONG_ANWSER:
            task->setResult(config.JE_WRONG_ANWSER);
            break;

        case MATCHER_PRESENT_ERROR:
            task->setResult(config.JE_PRESENT);
            break;

        default:
            task->setResult(config.JE_SYSTEM);
            break;
        }
        
        return result == MATCHER_ACCEPT;
    }

    int compare(const tstring & srcFile, const tstring & destFile)
    {
        std::vector<TCHAR> srcBuffer;
        if (!readFileBuffer(srcBuffer, srcFile, false))
        {
            return MATCHER_NOT_EXIST_SRC;
        }

        std::vector<TCHAR> dstBuffer;
        if (!readFileBuffer(dstBuffer, destFile, false))
        {
            return MATCHER_NOT_EXIST_DEST;
        }

        return compare(srcBuffer, dstBuffer);
    }

    bool isWhiteSpace(TCHAR ch, const tstring & white=whiteSpaces)
    {
        return ch==0 || (white.find(ch) != white.npos);
    }

    int compare(const std::vector<TCHAR> & srcBuffer, const std::vector<TCHAR> & dstBuffer)
    {
        int result = MATCHER_ACCEPT;

        int srcLen = int(srcBuffer.size());
        int dstLen = int(dstBuffer.size());

        TCHAR srcChr, dstChr;

        int i=0, k=0;
        while(i<srcLen && k<dstLen)
        {
            srcChr = srcBuffer[i];
            dstChr = dstBuffer[k];

            if (srcChr == dstChr)
            {
                ++i;
                ++k;
                continue;
            }

            //srcChr != dstChr

            if (srcChr==_T('\r') && dstChr == _T('\n'))
            {
                ++i;
            }
            else if (dstChr==_T('\r') && srcChr == _T('\n'))
            {
                ++k;
            }
            else if (isWhiteSpace(srcChr))
            {
                result = MATCHER_PRESENT_ERROR;
                ++i;
            }
            else if (isWhiteSpace(dstChr))
            {
                result = MATCHER_PRESENT_ERROR;
                ++k;
            }
            else
            {
                return MATCHER_WRONG_ANWSER;
            }
        }

        while(i < srcLen)
        {
            if (!isWhiteSpace(srcBuffer[i]))
            {
                return MATCHER_WRONG_ANWSER;
            }
            result = MATCHER_PRESENT_ERROR;
            ++i;
        }
        
        while(k < dstLen)
        {
            if (!isWhiteSpace(dstBuffer[k]))
            {
                return MATCHER_WRONG_ANWSER;
            }
            result = MATCHER_PRESENT_ERROR;
            ++k;
        }

        return result;
    }
};

//////////////////////////////////////////////////////////////////////////

ExcuterPtr createCppCompiler()
{
    return new CppCompiler();
}

ExcuterPtr createExeRunner()
{
    return new ExeDebugRunner();
    //  return new ExeRunner();
}

ExcuterPtr createMatcher()
{
    return new Matcher();
}