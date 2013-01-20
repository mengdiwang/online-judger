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
#include "DBManager.h"
#include "Log.h"
#include "Utilities.h"
#include "Config.h"

extern Config config;
bool g_enableDBMDebugMsg = false;

void enableDBMDebugMsg(bool e)
{
    g_enableDBMDebugMsg = e;
}

const std::string sqlSelectProblem = "SELECT solution.solution_id, solution.problem_id, "
    "solution.user_id, solution.language, "
    "problem.time_limit, problem.memory_limit, solution.result "
    "FROM solution, problem "
    "WHERE solution.problem_id = problem.problem_id and "
    "(solution.result=%d or solution.result=%d) limit 20";

const std::string sqlUpdateSolutionCompiling = "UPDATE `solution` SET `result`=%d "
	"WHERE `solution_id` = %d";

const std::string sqlUpdateSolution = "UPDATE `solution` SET `result`=%d, `time`=%d, "
    "`memory`=%d, `judgetime`=NOW(), `pass_rate`=%f "
    "WHERE `solution_id` = %d";

const std::string sqlSelectCode = "SELECT source FROM source_code WHERE solution_id=%d";

const std::string sqlDeleteCompile = "DELETE FROM `compileinfo` WHERE `solution_id`=%d";
const std::string sqlInsertCompile = "INSERT INTO `compileinfo`(solution_id, error)VALUES(%d, \"%s\")";

const std::string sqlUpdateUserSolved = "UPDATE `users` SET `solved`="
    "(SELECT count(DISTINCT `problem_id`) FROM `solution` WHERE `user_id`=\'%s\' AND `result`=%d) "
    "WHERE `user_id`=\'%s\'";

const std::string sqlUpdateUserSubmit = "UPDATE `users` SET `submit`="
    "(SELECT count(*) FROM `solution` WHERE `user_id`=\'%s\') "
    "WHERE `user_id`=\'%s\'";

const std::string sqlUpdateProblemAccept = "UPDATE `problem` SET `accepted`="
    "(SELECT count(*) FROM `solution` WHERE `problem_id`=\'%d\' AND `result`=\'%d\') "
    "WHERE `problem_id`=\'%d\'";

const std::string sqlUpdateProblemSubmit = "UPDATE `problem` SET `submit`="
    "(SELECT count(*) FROM `solution` WHERE `problem_id`=\'%d\')"
    "WHERE `problem_id`=\'%d\'";


#define ECHO_SQL_ERROR(ok) if(!ok)     \
{   \
    XWRITE_LOGA("SQLERROR:更新数据库失败：%s", m_sql->error().c_str());   \
}

#define ECHO_SQL_ERROR_2(ok, action) if(!ok) \
{\
    XWRITE_LOGA("SQLERROR:更新数据库失败：%s", m_sql->error().c_str());   \
    action; \
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
DBManager::DBManager(ZMySqlPtr sql, RecivTaskCallBack recivCB)
    : m_sql(sql)
    , m_recivCB(recivCB)
    , m_running(true)
{
    m_readThread = new ZThreadEx<DBManager>(this, &DBManager::doRead);
    m_writeThread = new ZThreadEx<DBManager>(this, &DBManager::doWrite);
}

DBManager::~DBManager(void)
{
}

void DBManager::stop()
{ 
    m_running = false; 
    m_finishTask.addFront(NULL);
    
    //等待线程结束
    m_readThread = NULL;
    m_writeThread = NULL;
}


void DBManager::tryLoadCode(TaskPtr task)
{
    char buffer[1024];
    sprintf_s(buffer, 1024, sqlSelectCode.c_str(), task->getSolutionID());
    
    ECHO_SQL_ERROR_2(m_sql->query(buffer), return);
    
    ZMySqlResPtr res = m_sql->storeResult();
    if (!res)
    {
        return;
    }

    ZMysqlRowPtr rowData = res->fetchRow();
    if (!rowData)
    {
        return;
    }
    std::string code = rowData->col(0).asString();

    tstring codePath = getFilePath(task->getCodeFile());
    makeDir(codePath);

#ifdef _UNICODE
    std::string path = wcharToChar(task->getCodeFile());
#else
    std::string path = task->getCodeFile();
#endif

    std::ofstream fout(path.c_str());
    if (!fout.good())
    {
        return ;
    }
    fout.write(code.c_str(), code.size());
    fout.close();

    XWRITE_LOGA("INFO: create code file : %s", path.c_str());
}

void DBManager::doRead()
{
    XWRITE_LOGA("读数据库线程启动...");
    char buffer[1024];
    while (isRunning())
    {
        Sleep(1000);

        sprintf_s(buffer, 1024, sqlSelectProblem.c_str(), config.JE_PENDING, config.JE_REJUDGE);

        ECHO_SQL_ERROR_2(m_sql->query(buffer), break);
        
        ZMySqlResPtr res = m_sql->storeResult();
        if (!res)
        {
            continue;
        }

        if (res->rows() > 0 && g_enableDBMDebugMsg)
        {
            int cols = res->cols();
            for (int i=0; i<cols; ++i)
            {
                MYSQL_FIELD * field = res->getField(i);
                std::cout<< field->name<<" | ";
            }
            std::cout<<std::endl;
        }

        ZMysqlRowPtr row = NULL;
        while(row = res->fetchRow())
        {
            if (g_enableDBMDebugMsg)
            {
                int cols = res->cols();
                for (int i=0; i<cols; ++i)
                {
                    std::cout<< row->col(i).asString() <<" \t| ";
                }
                std::cout<<std::endl;
            }

#ifdef _UNICODE
            std::wstring userName = charToWChar(row->col(2).asString());
#else
            std::string userName = row->col(2).asString();
#endif

            TaskPtr task = new TaskBase(
                row->col(0).asInt(),    //solutionid
                row->col(1).asInt(),    //problemid
                userName,
                row->col(3).asInt(),    //language
                10000,                  //comile time
                row->col(4).asInt(),   //runtime
                row->col(5).asInt()    //runmemory
                ); 

            tryLoadCode(task);

			sprintf_s(buffer, 1024, sqlUpdateSolutionCompiling.c_str(), 
				config.JE_COMPILING, task->getSolutionID());
			ECHO_SQL_ERROR_2(m_sql->query(buffer), break);

            onRecivTask(task);
        }
    }
    XWRITE_LOGA("读数据库线程退出。");
}

void DBManager::doWrite()
{
    XWRITE_LOGA("写数据库线程启动...");

    while(isRunning())
    {
        TaskPtr task = getFinishTask();
        if (!task)
        {
            break;
        }

        //continue; //用于测试，不提交到数据库

        updateSolution(task);

        updateUser(task);

        updateProblem(task);

        updateInfo(task);
    }
    XWRITE_LOGA("写数据库线程退出。");
}

void DBManager::onRecivTask(TaskPtr task)
{
    assert(m_recivCB!=NULL && "recivCB不能为空！");
    
    m_recivCB(task);
}

void DBManager::updateSolution(TaskPtr task)
{
    char buffer[1024];
    
    sprintf_s(buffer, 1024, sqlUpdateSolution.c_str(), task->getResult(), 
        task->getRunTime(), task->getRunMemory(), task->getPassRate(), task->getSolutionID());

    ECHO_SQL_ERROR(m_sql->query(buffer));
}

void DBManager::updateProblem(TaskPtr task)
{
    char buffer[1024];
    sprintf_s(buffer, 1024, sqlUpdateProblemAccept.c_str(), 
        task->getProblemID(),
        config.JE_ACCEPT, 
        task->getProblemID());
    ECHO_SQL_ERROR(m_sql->query(buffer));

    sprintf_s(buffer, 1024, sqlUpdateProblemSubmit.c_str(), 
        task->getProblemID(), 
        task->getProblemID());
    ECHO_SQL_ERROR(m_sql->query(buffer));
}

void DBManager::updateUser(TaskPtr task)
{
#ifdef _UNICODE
    const std::string userName = wcharToChar(task->getUserName());
#else
    const std::string userName = task->getUserName();
#endif

    char buffer[1024];
    sprintf_s(buffer, 1024, sqlUpdateUserSolved.c_str(), 
        userName.c_str(),
        config.JE_ACCEPT, 
        userName.c_str());
    ECHO_SQL_ERROR(m_sql->query(buffer));

    sprintf_s(buffer, 1024, sqlUpdateUserSubmit.c_str(), 
        userName.c_str(),
        userName.c_str());
    ECHO_SQL_ERROR(m_sql->query(buffer));
}

void DBManager::updateInfo(TaskPtr task)
{
    char buffer[1024];
    sprintf_s(buffer, 1024, sqlDeleteCompile.c_str(), task->getSolutionID());
    ECHO_SQL_ERROR(m_sql->query(buffer));

    //编译错误
    if (task->getResult() == config.JE_COMPLIE)
    {
#ifdef _UNICODE
        std::string errorStr = wcharToChar(task->getCompileErrorStr());
#else
        std::string errorStr = task->getCompileErrorStr();
#endif
        errorStr = m_sql->escapeString(errorStr);

        sprintf_s(buffer, 1024, sqlInsertCompile.c_str(), task->getSolutionID(), errorStr.c_str());
        ECHO_SQL_ERROR(m_sql->query(buffer));
    }
}