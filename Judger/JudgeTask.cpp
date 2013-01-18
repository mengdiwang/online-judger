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

#include "JudgeTask.h"
#include "Config.h"


#define WORK_DIR        "work\\"

#define MAX_NAME_SIZE  1024

extern Config config;

//////////////////////////////////////////////////////////////////////////

//获得编程语言的扩展名(含'.')
LPCTSTR getLanguageExt(int t)
{
    if(t == config.CL_GCC) return _T(".c");

    if(t == config.CL_GPLUS) return _T(".cpp");

    if(t == config.CL_JAVA) return _T(".java");

    return _T(".unknown");
}

//////////////////////////////////////////////////////////////////////////
//任务
//////////////////////////////////////////////////////////////////////////

TaskBase::TaskBase(int solutionID, int problemID, const tstring & userName,
                   int language, int limitCompileTime, int limitTime, int limitMemory)
                   : m_solutionID(solutionID)
                   , m_problemID(problemID)
                   , m_userName(userName)
                   , m_language(language)
                   , m_limitCompileTime(limitCompileTime)
                   , m_limitTime(limitTime)
                   , m_limitMemory(limitMemory)
                   , m_runTime(0)
                   , m_runMemory(0)
                   , m_result(0)
                   , m_curTestDataIndex(0)
                   , m_passRate(0.0f)
{
    initializeData();
}

void TaskBase::initializeData()
{
    if (m_solutionID == 0)
    {
        return;
    }

    TCHAR buffer[MAX_NAME_SIZE];

#if _UNICODE
    std::wstring testDataPath = charToWChar(config.testDataPath);
#else
    std::string testDataPath = config.testDataPath;
#endif
    formatDirName(testDataPath);
    testDataPath += _T("%d");

    //搜索测试数据
    msprintf_s(buffer, testDataPath.c_str(), m_problemID);
    if(!findFiles(m_testData, buffer, _T("*.out")))
    {
        XWRITE_LOG(_T("ERROR: %u, not found test data in :%s "), GetLastError(), buffer);
    }

    //临时数据文件
    msprintf_s(buffer, MAX_NAME_SIZE, _T("%d"), getSolutionID());
    tstring tempName = buffer;

    m_codeFile = _T(WORK_DIR) + tempName + getLanguageExt(m_language);
    m_compilerOutput = _T(WORK_DIR) + tempName + _T(".txt");
    m_exeName = _T(WORK_DIR) + tempName + _T(".exe");
    m_exeOutput = _T(WORK_DIR) + tempName + _T(".out");

    if (getTestDataCount() > 0)
    {
        m_curTestDataIndex = -1;
        setTestDataIndex(0);
    }
}


void TaskBase::setTestDataIndex(int i)
{
    if (i == m_curTestDataIndex)
    {
        return;
    }
    assert(i>=0 && i<getTestDataCount() && "测试数据越界");
    m_curTestDataIndex = i;

    m_curOutputTestData = m_testData[i];

    m_curInputTestData = getFileName(m_curOutputTestData);
    m_curInputTestData += _T(".in");


}


//////////////////////////////////////////////////////////////////////////
TaskPtr createTestTask()
{
    return new TaskBase(1001, 1000, _T("zhoubao"), 1, 10000, 1000, 32);
}
