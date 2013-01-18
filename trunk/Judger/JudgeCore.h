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
#pragma once

#include "Excuter.h"

typedef void (*JudgeFinishCallBack)(int id, TaskPtr task);

//////////////////////////////////////////////////////////////////////////
//评判程序核心
//进行一次评判
//////////////////////////////////////////////////////////////////////////
class JudgeCore : public IBase
{
public:

    JudgeCore(int id, JudgeFinishCallBack cb);

    void setAllExcuter(ExcuterPtr compiler, ExcuterPtr runner, ExcuterPtr matcher)
    {
        setCompiler(compiler);
        setRunner(runner);
        setMatcher(matcher);
    }

    void setFinishCallBack(JudgeFinishCallBack cb){ m_finishCB = cb; }

    void setCompiler(ExcuterPtr compiler)
    {
        m_compiler = compiler;
    }

    void setRunner(ExcuterPtr runner)
    {
        m_runner = runner;
    }

    void setMatcher(ExcuterPtr matcher)
    {
        m_mather = matcher;
    }

    //成功返回true，出现致命错误返回false。
    virtual bool judge(TaskPtr task);

protected:

    virtual bool doJudge(TaskPtr task);

    virtual void onTaskFinish(TaskPtr task);

    virtual bool doCleanUp(TaskPtr task);

protected:
    int         m_id;
    ExcuterPtr  m_compiler;
    ExcuterPtr  m_runner;
    ExcuterPtr  m_mather;
    JudgeFinishCallBack m_finishCB;
};

typedef RefPtr<JudgeCore> JudgeCorePtr;

JudgeCorePtr createJudgeCore(int id, JudgeFinishCallBack cb);

JudgeCorePtr createTestJudgeCore();