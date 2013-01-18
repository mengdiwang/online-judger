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

#include "RefPtr.h"
#include "FileTool.h"
#include "TaskMgr.h"

class TaskBase : public IBase
{
public:
    TaskBase(int solutionID, int problemID, const tstring & userName,
        int language, int limitCompileTime, int limitTime, int limitMemory);

    virtual void initializeData();

    int getSolutionID(){ return m_solutionID; }

    int getProblemID(){ return m_problemID; }

    int getLanguage(){ return m_language; }

    int getLimitCompileTime(){ return m_limitCompileTime; }

    int getLimitTime() { return m_limitTime; }

    int getLimitMemory() { return m_limitMemory; }

    const tstring & getUserName(){ return m_userName; }

    //获得代码文件路径
    const tstring & getCodeFile(){ return m_codeFile; }

    //获得测试数据数量
    int getTestDataCount() { return (int)m_testData.size(); }

    virtual void setTestDataIndex(int i);

    const tstring & getInputTestData(){ return m_curInputTestData; }

    const tstring & getOutputTestData(){ return m_curOutputTestData; }

    bool isSpecialJudge() { return false; }

    const tstring & getSPJProgram() 
    { 
        assert(NULL && "getSPJProgram not implament"); 
        return m_exeName; 
    }

    const tstring & getCompileOutputFile() { return m_compilerOutput; }

    const tstring & getExeFile() { return m_exeName; }

    const tstring & getExeOutputFile(){ return m_exeOutput; }

    int getRunTime(){ return m_runTime; }
    void setRunTime(int time) { m_runTime = time; }

    int getRunMemory(){ return m_runMemory; }
    void setRunMemory(int memory) { m_runMemory = memory; }

    int getResult(){ return m_result; }
    void setResult(int result) { m_result = result; }

    void setCompileError(const tstring & errorStr){ m_compileErrorStr = errorStr; }
    const tstring & getCompileErrorStr(){return m_compileErrorStr; }

    void setPassRate(float r){ m_passRate = r; }
    float getPassRate(){ return m_passRate; }

private:
    tstring m_userName;     //用户名
    int m_solutionID;       //提交编号
    int m_problemID;        //问题编号
    int m_limitCompileTime; //编译时间限制
    int m_limitTime;        //运行时间限制
    int m_limitMemory;      //运行内存限制
    int m_language;         //编程语言类型

    int m_runTime;          //运行时间ms
    int m_runMemory;        //运行占用内存kb
    int m_result;           //评判结果
    float m_passRate;       //测试数据通过率

    StringPool m_testData;          //输入测试数据文件集和
    int     m_curTestDataIndex;     //当前测试数据组索引
    tstring m_curInputTestData;     //当前输入测试数据
    tstring m_curOutputTestData;    //当前输出测试数据

    tstring m_codeFile;         //代码文件
    tstring m_compilerOutput;   //编译信息输出文件
    tstring m_exeName;          //生成exe文件
    tstring m_exeOutput;        //exe输出文件
    tstring m_compileErrorStr;  //编译错误信息
};

typedef RefPtr<TaskBase> TaskPtr;
typedef TaskMgr<TaskPtr> JudgeTaskPool;

TaskPtr createTestTask();