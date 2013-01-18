//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#pragma once


#include "ZLock.h"
//////////////////////////////////////////////////////////////////////////
//任务管理器
//////////////////////////////////////////////////////////////////////////
template<typename Task>
class TaskMgr
{
public:
    typedef std::list<Task> TaskPool;
    typename TaskPool::iterator TaskIterator;

    TaskMgr() {}

    void add(Task task)
    {
        ZLockHolder hoder(&m_lock);
        m_pool.push_back(task);
        m_semap.push(); 
    }

    void addFront(Task task)
    {
        ZLockHolder hoder(&m_lock);
        m_pool.push_front(task); 
        m_semap.push();
    }

    Task get()
    {
        m_semap.pop();
        ZLockHolder hoder(&m_lock);
        Task task = m_pool.front();
        m_pool.pop_front();
        return task;
    }

    bool empty()
    {
        ZLockHolder hoder(&m_lock);
        return m_pool.empty();
    }
private:
    TaskPool        m_pool;
    ZSimpleLock     m_lock;
    ZSemaphore      m_semap;
};