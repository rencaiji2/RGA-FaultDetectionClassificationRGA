#ifndef STHREADTASK_H
#define STHREADTASK_H

#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include <QThread>
#include <QVariant>
#include <QVariantList>
#include "ssafedata.h"
#include "sglobal.h"

/**
 * @brief
 *
 */
typedef void (*SThreadTaskFunc)(const QString& strTaskName, QVariantList& taskParam, QObject* pGlobalParam, void* pUserParam, const bool& bRunning);
/**
 * @brief
 *
 */
typedef QObject* (*SThreadGlobalFunc)();

/**
 * @brief
 *
 */
class SCORE_SHARED_EXPORT STask : public QObject{
    QString m_name; /**< TODO */
public:
    /**
     * @brief
     *
     * @param name
     * @param parent
     */
    STask(const QString& name = "", QObject* parent = 0) : QObject(parent) , m_name(name){}
    /**
     * @brief
     *
     */
    virtual ~STask(){}
    /**
     * @brief
     *
     * @return QString
     */
    QString& name(){ return m_name;}
    /**
     * @brief
     *
     * @param param
     * @param global
     * @param bRunning
     */
    virtual void execute(QVariantList& param, QObject* global, const bool& bRunning) = 0;
};


/**
 * @brief
 *
 */
class SCORE_SHARED_EXPORT SThreadTask : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     */
    struct TaskT{
        QString name; /**< TODO */
        QVariantList param; /**< TODO */
        STask* task = NULL; /**< TODO */
        int index = 0; /**< TODO */
    };
    /**
     * @brief
     *
     */
    enum WaitT{W_ADDED, W_DONE, W_LENGTH};

    /**
     * @brief
     *
     * @param parent
     */
    SThreadTask(QObject *parent = 0);

    /**
     * @brief
     *
     */
    ~SThreadTask();

    /**
     * @brief
     *
     * @param strName
     * @param param
     * @param bRepeat
     * @return int
     */
    int addTask(const QString& strName, const QVariantList& param, bool bRepeat = false);

    /**
     * @brief
     *
     * @param task
     * @param param
     * @param bRepeat
     * @return int
     */
    int addTask(STask* task, const QVariantList& param, bool bRepeat = false);

    /**
     * @brief
     *
     * @param strName
     * @param param
     */
    void excuteTask(const QString& strName, QVariantList& param);
    /**
     * @brief
     *
     * @param task
     * @param param
     */
    void excuteTask(STask* task, QVariantList& param);

    /**
     * @brief
     *
     * @return SThreadTaskFunc
     */
    SThreadTaskFunc userFunction() const;
    /**
     * @brief
     *
     * @param userFunction
     */
    void setUserFunction(const SThreadTaskFunc &userFunction);

    /**
     * @brief
     *
     */
    void *userParam() const;
    /**
     * @brief
     *
     * @param userParam
     */
    void setUserParam(void *userParam);

    /**
     * @brief
     *
     * @return SThreadGlobalFunc
     */
    SThreadGlobalFunc globalFunction() const;
    /**
     * @brief
     *
     * @param globalFunction
     */
    void setGlobalFunction(const SThreadGlobalFunc &globalFunction);

    /**
     * @brief
     *
     */
    void stop();
protected:
    /**
     * @brief
     *
     */
    void run();
signals:
    /**
     * @brief
     *
     * @param task
     */
    void doneTask(TaskT* task);
private:
    QQueue<TaskT*> m_tasks; /**< TODO */
    QMutex m_mutex[W_LENGTH]; /**< TODO */
    int m_index = 0; /**< TODO */
    QWaitCondition m_wait[W_LENGTH]; /**< TODO */
    SThreadTaskFunc m_userFunction = NULL; /**< TODO */
    void* m_userParam = NULL; /**< TODO */
    SThreadGlobalFunc m_globalFunction = NULL; /**< TODO */
    bool m_running = false; /**< TODO */
    SSafeData<QMap<int, QVariantList*> > m_excuteTask; /**< TODO */
};


#endif // STHREADTASK_H
