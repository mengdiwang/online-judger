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
#include "JudgeCell.h"

CoreThreadObj::CoreThreadObj(JudgeCell* cell, int coreID, JudgeFinishCallBack cb)
    : m_cell(cell)
    , m_id(coreID)
{
    m_core = createJudgeCore(coreID, cb);
    m_thread = new ZThreadEx<CoreThreadObj>(this, &CoreThreadObj::work);
}

void CoreThreadObj::work()
{
    XWRITE_LOG(_T("[%d]core thread start."), m_id);
    while(true)
    {
        TaskPtr task = m_cell->getTask();
        if (!task)
        {
            XWRITE_LOG(_T("[%d]found exit task."), m_id);
            break;
        }
        if(!m_core->judge(task))
        {
            XWRITE_LOG(_T("[%d]ÆÀÅÐÄÚºË³öÏÖÖÂÃü´íÎó£¬ÄÚºË¼´½«Í£Ö¹¡£"), m_id);
            break;
        }

        Sleep(100);
    }

    XWRITE_LOG(_T("[%d]core thread end."), m_id);
}

//////////////////////////////////////////////////////////////////////////

JudgeCell::JudgeCell(int cores, JudgeFinishCallBack cb)
{
    create(cores, cb);
}

JudgeCell::~JudgeCell(void)
{
    stop();
}

void JudgeCell::create(int cores, JudgeFinishCallBack cb)
{
    for (int i=0; i<cores; ++i)
    {
        m_cores.push_back( new CoreThreadObj(this, i, cb));
    }
}

//Í£Ö¹¹¤×÷
void JudgeCell::stop()
{
    for (size_t i=0; i<m_cores.size(); ++i)
    {
        m_tasks.addFront(NULL);
    }
    m_cores.clear();
}