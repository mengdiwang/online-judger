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
#include "Log.h"
#include "JudgeCore.h"
#include "Config.h"

extern Config config;
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

bool safeRemoveFile(const tstring& name, int attemp=10, int delayTime=1000)
{
    for(int i=1; i<=attemp; ++i)
    {
        if(DeleteFile(name.c_str()))
        {
            return true;
        }

        DWORD code = GetLastError();
        if(ERROR_FILE_NOT_FOUND == code)
        {
            return true;;
        }

        Sleep(delayTime);
        XWRITE_LOG(_T("ERROR: %u Attempt(%d) to delete file: %s"), code, i, name.c_str());
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////

JudgeCore::JudgeCore(int id, JudgeFinishCallBack cb) 
    : m_id(id)
    , m_finishCB(cb) 
{ 
}

bool JudgeCore::judge(TaskPtr task)
{
    doJudge(task);

    onTaskFinish(task);

	return doCleanUp(task);
}

bool JudgeCore::doJudge(TaskPtr task)
{
    //编译程序
    if (!m_compiler->exc(task))
    {
        return false;
    }

    //评判多组测试数据

    int testCount = task->getTestDataCount();
    if (testCount == 0)//没有测试数据
    {
        task->setResult(config.JE_SYSTEM);
        return false;
    }

    int maxTime = 0;
    int maxMemory =0;
    int accepted = 0;
    for (int i=0; i<testCount; ++i)
    {
        task->setTestDataIndex(i);//设置当前测试组

        //运行程序
        bool result = m_runner->exc(task);
        
        if (result)
        {
            //评判结果
            if (m_mather->exc(task))
            {
                ++accepted;
            }
		}

		//强制删除临时文件
		if(!safeRemoveFile(task->getExeOutputFile()))
		{
			task->setResult(config.JE_SYSTEM);
			return false;
		}

        //记录最大占用时间和内存
        if (maxTime < task->getRunTime())
        {
            maxTime = task->getRunTime();
        }
        if (maxMemory < task->getRunMemory())
        {
            maxMemory = task->getRunMemory();
        }

        if (!result)
        {
            break;
        }
    }
    task->setPassRate(float(accepted)/task->getTestDataCount());
    task->setRunTime(maxTime);
    task->setRunMemory(maxMemory);
    return true;
}

bool JudgeCore::doCleanUp(TaskPtr task)
{
    if(!safeRemoveFile(task->getCodeFile()))
	{
		return false;
	}

    if(!safeRemoveFile(task->getCompileOutputFile()))
    {
        return false;
    }
    if(!safeRemoveFile(task->getExeOutputFile()))
    {
        return false;
    }
    if(!safeRemoveFile(task->getExeFile()))
    {
        return false;
    }

    return true;
}

void JudgeCore::onTaskFinish(TaskPtr task)
{
    if (m_finishCB)
    {
        m_finishCB(m_id, task);
    }
}

//////////////////////////////////////////////////////////////////////////

JudgeCorePtr createJudgeCore(int id, JudgeFinishCallBack cb)
{
    JudgeCorePtr ptr = new JudgeCore(id, cb);
    ptr->setAllExcuter(createCppCompiler(), createExeRunner(), createMatcher());
    return ptr;
}

JudgeCorePtr createTestJudgeCore()
{
    return createJudgeCore(0, NULL);
}