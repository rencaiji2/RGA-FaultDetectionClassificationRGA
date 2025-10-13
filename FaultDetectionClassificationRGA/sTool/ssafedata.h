#pragma once
#include <QMutex>
#include <QReadWriteLock>
//************************************
// Class:	SSafeData
// Brief:	安全数据		
// Details: 用于实现数据安全操作，每次要操作数据前先获得锁，操作完成后释放锁
//************************************
template<class T>
/**
 * @brief   SSafeData can be used to protects any object.
 *
 */
class SSafeData: public QReadWriteLock{
public:
    T m_Data; /**< Object */
    /**
     * @brief   Override the object's pointer operation.
     *
     * @return T *operator ->
     */
    T* operator ->(void)
	{
        return &m_Data;
	}
};

template<class T>
/**
 * @brief SSafePtrData can be used to protects any pointer.
 *
 */
class SSafePtrData: public QReadWriteLock{
public:
    // 数据
    T m_Data = NULL; /**< Pointer */
    /**
     * @brief   Override the pointer's pointer operation.
     *
     * @return T &operator ->
     */
    T& operator ->(void)
    {
        return m_Data;
    }
};

