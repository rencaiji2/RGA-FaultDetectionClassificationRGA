#ifndef STHREAD_H
#define STHREAD_H
#include <GlobalTools_global.h>
#include <QThread>
/**
 * @brief
 *
 */
typedef int (*SThreadFunc)(void* pParam, const bool& bRunning);

/**
 * @brief
 *
 */
class GLOBALTOOLS_EXPORT SThread : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param parent
     */
    explicit SThread(QObject *parent = 0);

    /**
     * @brief
     *
     * @return SThreadFunc
     */
    SThreadFunc userFunction() const;
    /**
     * @brief
     *
     * @param fnUserFunction
     */
    void setUserFunction(const SThreadFunc &fnUserFunction);

    /**
     * @brief
     *
     */
    void *userParam() const;
    /**
     * @brief
     *
     * @param pUserParam
     */
    void setUserParam(void *pUserParam);
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
    virtual void run();

    SThreadFunc m_userFunction; /**< TODO */
    void* m_userParam; /**< TODO */
    bool m_running = false; /**< TODO */
};

#endif // STHREAD_H
