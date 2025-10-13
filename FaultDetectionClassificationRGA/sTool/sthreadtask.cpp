#include "sthreadtask.h"

SThreadTask::SThreadTask(QObject *parent):
    QThread(parent)
{
}

SThreadTask::~SThreadTask(){
    m_running = false;
    m_mutex[W_DONE].lock();
    m_wait[W_DONE].wakeAll();
    m_mutex[W_DONE].unlock();
    m_mutex[W_ADDED].lock();
    while(!m_tasks.isEmpty())
        delete m_tasks.dequeue();
    m_wait[W_ADDED].wakeOne();
    m_mutex[W_ADDED].unlock();
    wait();
}

int SThreadTask::addTask(const QString &strName, const QVariantList &param, bool bRepeat){
    if(strName.isEmpty())
        return -1;
    QMutexLocker lock(&m_mutex[W_ADDED]);

    if(!bRepeat){
        foreach (TaskT* item, m_tasks) {
            if(item->name == strName)
                return item->index;
        }
    }
    TaskT* pTask = new TaskT();
    pTask->name = strName;
    pTask->param = param;
    pTask->index = m_index;
    m_tasks.enqueue(pTask);
    m_wait[W_ADDED].wakeOne();
    return m_index++;
}

int SThreadTask::addTask(STask *task, const QVariantList &param, bool bRepeat){
    if(task == NULL)
        return -1;
    QMutexLocker lock(&m_mutex[W_ADDED]);

    if(!bRepeat){
        foreach (TaskT* item, m_tasks) {
            if(item->task == task)
                return item->index;
        }
    }
    TaskT* pTask = new TaskT();
    pTask->task = task;
    pTask->param = param;
    pTask->index = m_index;
    m_tasks.enqueue(pTask);
    m_wait[W_ADDED].wakeOne();
    return m_index++;
}

void SThreadTask::excuteTask(const QString &strName, QVariantList &param)
{
    int nID;
    {
        if(strName.isEmpty())
            return;
        QMutexLocker lock(&m_mutex[W_ADDED]);

        TaskT* pTask = new TaskT();
        pTask->name = strName;
        pTask->param = param;
        nID = pTask->index = m_index;
        m_excuteTask.lockForWrite();
        m_excuteTask->insert(m_index, &param);
        m_excuteTask.unlock();
        m_mutex[W_DONE].lock();
        m_tasks.enqueue(pTask);
        m_wait[W_ADDED].wakeOne();
        m_index++;
    }
    forever{
        m_wait[W_DONE].wait(&m_mutex[W_DONE]);
        m_mutex[W_DONE].unlock();
        if(!m_running)
            return;
        m_excuteTask.lockForRead();
        if(m_excuteTask->find(nID) == m_excuteTask->end()){
            m_excuteTask.unlock();
            return;
        }
        m_excuteTask.unlock();
        m_mutex[W_DONE].lock();
    }
}

void SThreadTask::excuteTask(STask *task, QVariantList &param)
{
    int nID;
    {
        if(task == NULL)
            return;
        QMutexLocker lock(&m_mutex[W_ADDED]);

        TaskT* pTask = new TaskT();
        pTask->task = task;
        pTask->param = param;
        nID = pTask->index = m_index;
        m_excuteTask.lockForWrite();
        m_excuteTask->insert(m_index, &param);
        m_excuteTask.unlock();
        m_mutex[W_DONE].lock();
        m_tasks.enqueue(pTask);
        m_wait[W_ADDED].wakeOne();
        m_index++;
    }
    forever{
        m_wait[W_DONE].wait(&m_mutex[W_DONE]);
        m_mutex[W_DONE].unlock();
        if(!m_running)
            return;
        m_excuteTask.lockForRead();
        if(m_excuteTask->find(nID) == m_excuteTask->end()){
            m_excuteTask.unlock();
            return;
        }
        m_excuteTask.unlock();
        m_mutex[W_DONE].lock();
    }
}

SThreadTaskFunc SThreadTask::userFunction() const{
    return m_userFunction;
}

void SThreadTask::setUserFunction(const SThreadTaskFunc &userFunction){
    m_userFunction = userFunction;
}

void *SThreadTask::userParam() const{
    return m_userParam;
}

void SThreadTask::setUserParam(void *userParam){
    m_userParam = userParam;
}

void SThreadTask::stop(){
    m_running = false;
    m_mutex[W_DONE].lock();
    m_wait[W_DONE].wakeAll();
    m_mutex[W_DONE].unlock();
    m_mutex[W_ADDED].lock();
    m_wait[W_ADDED].wakeOne();
    m_mutex[W_ADDED].unlock();
}


void SThreadTask::run(){
    QObject* pGlobal = NULL;
    QMap<int, QVariantList*>::iterator iterExcute;
    if(m_globalFunction)
        pGlobal = m_globalFunction();
    m_running = true;
    while(m_running){
        QMutexLocker lock(&m_mutex[W_ADDED]);
        while(m_tasks.isEmpty()){
            m_wait[W_ADDED].wait(&m_mutex[W_ADDED]);
            if(!m_running){
                if(pGlobal)
                    delete pGlobal;
                return;
            }
        }
        TaskT* pTask = m_tasks.dequeue();
        if(pTask->task != NULL)
            pTask->task->execute(pTask->param, pGlobal, m_running);
        else if(m_userFunction != NULL){
            m_userFunction(pTask->name, pTask->param, pGlobal, m_userParam, m_running);
        }
        m_excuteTask.lockForWrite();
        if((iterExcute = m_excuteTask->find(pTask->index)) != m_excuteTask->end()){
            *iterExcute.value() = pTask->param;
            m_excuteTask->erase(iterExcute);
        }
        m_excuteTask.unlock();

        m_mutex[W_DONE].lock();
        m_wait[W_DONE].wakeAll();
        m_mutex[W_DONE].unlock();
        emit doneTask(pTask);
        delete pTask;
    }
}
SThreadGlobalFunc SThreadTask::globalFunction() const
{
    return m_globalFunction;
}

void SThreadTask::setGlobalFunction(const SThreadGlobalFunc &globalFunction)
{
    m_globalFunction = globalFunction;
}


