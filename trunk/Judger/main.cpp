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
#include <iostream>
#include <stdio.h>
#include <conio.h>

#include "Comm.h"
#include "Utilities.h"
#include "Log.h"

#include "Config.h"
#include "JudgeCore.h"
#include "JudgeCell.h"
#include "DBManager.h"
#include "ZMySql.h"

Config config;
DBManagerPtr dbMgr;
JudgeCellPtr judgeCell;

void onRecivTask(TaskPtr task)
{
    judgeCell->addTask(task);
}

void onFinishTask(int id, TaskPtr task)
{
    XWRITE_LOG(_T("[%d]judge finish. solution=%d result=%d"), 
        id, task->getSolutionID(), task->getResult());

    dbMgr->addFinishTask(task);
}


int main()
{
	system("title HUSTOJ Online Judger");

    setlocale(LC_CTYPE, ""); 

    changeCurDirectory();

    INIT_LOG(_T("oj.txt"));

    OutputMsg(_T("INFO: start..."));

    if (!config.loadConfig("config.xml"))
    {
        XWRITE_LOG(_T("读取配置文件失败！"));
        return 0;
    }

    ZMySqlPtr mysql = new ZMySql();
    if(!mysql->loadService())
    {
        OutputMsgA("加载mysql数据库服务失败！%s", mysql->error().c_str());
        return 0;
    }
    if(!mysql->connect(config.dbip, config.dbport, config.dbusername, 
        config.dbpassword, config.dbname))
    {
        OutputMsgA("链接数据库失败！%s", mysql->error().c_str());
        return 0;
    }
    dbMgr = new DBManager(mysql, onRecivTask);

    judgeCell = new JudgeCell(config.cores, onFinishTask);
    
    char buffer[256];
    std::string str;
    while(true)
    {
        std::cin.getline(buffer, 256);
        str = buffer;
		if (str.empty())
		{
			continue;
		}

		if (str[0] == '$')
		{
			std::string cmd = str.substr(1);
			if (!cmd.empty())
			{
				system(cmd.c_str());
			}
		}
		else if (str == "exit")
		{
			break;
		}
        else if (str == "dbmsg on")
        {
            enableDBMDebugMsg(true);
        }
        else if(str == "dbmsg off")
        {
            enableDBMDebugMsg(false);
        }
    };

    judgeCell->stop();
    dbMgr->stop();
    mysql->disconect();
    mysql->unloadService();

    OutputMsg(_T("INFO: end."));
    return 0;
}

//////////////////////////////////////////////////////////////////////////
//测试用部件
//////////////////////////////////////////////////////////////////////////

void onJudgeFinish(int id, TaskPtr task)
{
    XWRITE_LOG(_T("[%d]judge finish. solution=%d result=%d"), 
        id, task->getSolutionID(), task->getResult());
}

int testJudgeCore()
{
    JudgeCorePtr core = createTestJudgeCore();
    if (!core)
    {
        OutputMsg(_T("ERROR: no judge core!"));
        return 0;
    }

    for (int i=0; i<1; ++i)
    { 
        TaskPtr task = createTestTask();
        if (!task)
        {
            OutputMsg(_T("ERROR: no task!"));
            return 0;
        }

        XWRITE_LOG(_T("code:%s"), task->getCodeFile().c_str());

        int n = task->getTestDataCount();
        for (int i=0; i<n; ++i)
        {
            XWRITE_LOG(_T("test data in:%s"), task->getInputTestData().c_str());
            XWRITE_LOG(_T("test data out:%s"), task->getOutputTestData().c_str());
        }

        if(!core->judge(task))
        {
            OutputMsg(_T("ERROR: judge not pass!"));
        }
        else
        {
            OutputMsg(_T("INFO: Accept!"));
        }

    }
    _getch();
    return 1;
}

int testJudgeCell()
{
    OutputMsg(_T("评判单元启动"));

    JudgeCell cell(4, onJudgeFinish);

    for (int i=0; i<4; ++i)
    {
        cell.addTask(createTestTask());
    }
    _getch();
    return 1;
}