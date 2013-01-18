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
#include "ZMySql.h"
#include "ZThread.h"
#include "JudgeTask.h"
#include "ZLock.h"

typedef void (*RecivTaskCallBack)(TaskPtr task);

class DBManager : public IBase
{
public:
    DBManager(ZMySqlPtr sql, RecivTaskCallBack recivCB);
    virtual ~DBManager(void);

    void doRead();

    void doWrite();

    void addFinishTask(TaskPtr task){ m_finishTask.add(task);}

    void stop();

protected:
    //是否正在运行
    bool isRunning(){ return m_running; }

    virtual void onRecivTask(TaskPtr task);

    void tryLoadCode(TaskPtr task);

    //是否正在评判。如果正在评判，则不能继续读取数据，因为结果还未写入数据库。
    bool isJudging();

    void addWorkRef();

    void delWorkRef();

    TaskPtr getFinishTask(){ return m_finishTask.get(); }

    void updateSolution(TaskPtr task);
    void updateProblem(TaskPtr task);
    void updateUser(TaskPtr task);
    void updateInfo(TaskPtr task);

protected:
    ZMySqlPtr m_sql;
    RecivTaskCallBack m_recivCB;
    ZThreadPtr m_readThread;
    ZThreadPtr m_writeThread;

    bool            m_running;
    JudgeTaskPool   m_finishTask;
    int             m_workRef;
    ZSimpleLock     m_workLock;
};
typedef RefPtr<DBManager> DBManagerPtr;


void enableDBMDebugMsg(bool e);