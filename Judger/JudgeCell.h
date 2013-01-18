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

#include "JudgeCore.h"
#include "ZThread.h"

class JudgeCell;

//////////////////////////////////////////////////////////////////////////
//核心线程对象
//负责链接JudgeCell和JudgeCore.
//////////////////////////////////////////////////////////////////////////
class CoreThreadObj : public IBase
{
public:

    CoreThreadObj(JudgeCell* cell, int coreID, JudgeFinishCallBack cb);

    void work();

private:
    int             m_id;
    JudgeCell*      m_cell;//不使用智能指针，防止循环引用。
    JudgeCorePtr    m_core;
    ZThreadPtr      m_thread;
};

typedef RefPtr<CoreThreadObj> CoreThreadPtr;


//////////////////////////////////////////////////////////////////////////
//评测单元
//一个
//////////////////////////////////////////////////////////////////////////
class JudgeCell : public IBase
{
public:
    typedef std::vector<CoreThreadPtr> JudgeCorePool;

    JudgeCell(int cores, JudgeFinishCallBack cb);
    virtual ~JudgeCell(void);

    //添加一个任务
    void addTask(TaskPtr task){ m_tasks.add(task); }

    //获得任务
    TaskPtr getTask(){ return m_tasks.get(); }

    //停止工作
    void stop();

protected:

    void create(int cores, JudgeFinishCallBack cb);

protected:
    JudgeCorePool m_cores;
    JudgeTaskPool m_tasks;
};
typedef RefPtr<JudgeCell> JudgeCellPtr;