#ifndef srlsS_H
#define srlsS_H

#include <QPluginLoader>
#include <QDir>

#if defined(SCORE_LIBRARY)
#  define SCORE_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define SCORE_SHARED_EXPORT Q_DECL_IMPORT
#endif

#define SRLS_ATTRIBUTE_ROLE "role"
#define SRLS_ATTRIBUTE_ROLEPARAM "role_value"

#define SRLS_ATTRIBUTE_ROLE_SELECTSIGNLE "select_signle"
#define SRLS_ATTRIBUTE_ROLE_SELECTSIGNLE "select_signle"
#define SRLS_ATTRIBUTE_ROLE_LAYOUTPARAM "layout_param"

#define SRLS_LAYOUTS    "grid,vbox,hbox,stack,form,geometry"

class SObject;
class SMainWindow;

/**
 * @brief SPluginInterface class is all of the GICAP plugin interface.
 *
 */
class SPluginInterface
{
public:
    /**
     * @brief Destructor
     *
     */
    virtual ~SPluginInterface() { }

    /**
     * @brief Get all of the key in this plugin
     *
     * @return QStringList  Keys' list
     */
    virtual QStringList keys() const = 0;

    /**
     * @brief Create an object from the plugin
     *
     * @param strKey        The key specify the key which be created class. It references the returnned keys from function keys().
     * @param pMainWindow   The main window handle
     * @param pParent       The parent object handle
     * @return QObject      Created object handle. If it's failed. It's NULL.
     */
    virtual QObject* create(const QString &strKey, SMainWindow *pMainWindow, QObject* pParent) = 0;

    /**
     * @brief Create a SObject from the plugin
     *
     * @param strKey    The key specify the key which be created SObject. It references the returnned keys from function keys().
     * @param pParent   The parent SObject handle
     * @return SObject  Created SObject handle. If it's failed. It's NULL.
     */
    virtual SObject* sobject(const QString &strKey, SObject* pParent) = 0;
};
Q_DECLARE_INTERFACE(SPluginInterface, "slrs.gicap.interface/1.0")

#define SRLS_CS const char*
#define SRLS_ECS extern const char*

namespace srls{

namespace attribute{
    SRLS_ECS Role;
    SRLS_ECS RoleParam;
    SRLS_ECS Tip;
    namespace role {
        SRLS_ECS SelectSignle;
        SRLS_ECS SelectMul;
        SRLS_ECS LayoutParam;
        SRLS_ECS LayoutValue;
        SRLS_ECS Image;
        SRLS_ECS Action;
        SRLS_ECS Map2Image;
        SRLS_ECS Map2Action;
        SRLS_ECS Map2Object;
        SRLS_ECS Map2Property;
    }
}


template <class T>
/**
 * @brief
 *
 */
struct SDataBuffer{
    quint64 capacity; /**< TODO */
    quint64 size; /**< TODO */
    T* data; /**< TODO */

    /**
     * @brief
     *
     */
    SDataBuffer(){
        capacity = 0;
        size = 0;
        data = NULL;
    }
    /**
     * @brief
     *
     */
    void release(){
        if(capacity > 0
                && data != NULL)
            free(data);
        capacity = 0;
        size = 0;
        data = NULL;
    }
    /**
     * @brief
     *
     * @param uSize
     */
    void resize(quint64 uSize){
        if(capacity >= uSize){
            size = uSize;
            return;
        }
        release();
        data = (T*)malloc(sizeof(T) * uSize);
        capacity = uSize;
        size = uSize;
    }
    /**
     * @brief
     *
     */
    void zero(){
        memset(data, 0, size * sizeof(T));
    }
};

template <class T>
struct ThreadSafeBuffer{
    volatile quint64 capacity; /**< TODO */
    volatile quint64 mSize; /**< TODO */
    T* data; /**< TODO */

    ThreadSafeBuffer(){
        capacity = 1000;
        mSize = 0;
        data = (T*)malloc(sizeof(T) * capacity);
    }

    int size()
    {
        return mSize;
    }

    void release(){
        if(capacity > 0
                && data != NULL)
            free(data);
        capacity = 0;
        mSize = 0;
        data = NULL;
    }

    void resize(quint64 uSize){
        if(capacity >= uSize){
            mSize = uSize;
            return;
        }
        release();
        data = (T*)malloc(sizeof(T) * uSize);
        capacity = uSize;
        mSize = uSize;
    }
    bool append(T newData){
        if(data == NULL){
            capacity = 1000;
            mSize = 1;
            data = (T*)malloc(sizeof(T) * capacity);
            memcpy(data, &newData, sizeof(T));
            return true;
        }else{
            if(mSize>=capacity)
                return false;
            else{
                memcpy(data+mSize, &newData, sizeof(T));
                mSize++;
                return true;
            }
        }
    }

    bool insert(int index,T newData)
    {
        if(index< 0)
            return false;
        if(data == NULL){
            capacity = 1000;
            mSize = 1;
            data = (T*)malloc(sizeof(T) * capacity);
            memcpy(data, &newData, sizeof(T));
            return true;
        }else{
            memcpy(data+index, &newData, sizeof(T));
            return true;
        }
    }

    T first()
    {
        return data[0];
    }

    bool removeFirst()
    {
        if(data == NULL || (mSize<=0))
            return false;
        mSize--;
        memmove(data, data+1, sizeof(T)*mSize);
        return true;
    }

    void zero(){
        memset(data, 0, mSize * sizeof(T));
    }

    bool isEmpty()
    {
        if(mSize== 0)
            return true;
        return false;
    }
};
template <typename InterfaceType>
/**
 * @brief
 *
 * @param strPluginDir
 * @param QMap<QString
 * @param mapPlugin
 * @return int
 */
int loadPlugin(const QString& strPluginDir, QMap<QString, InterfaceType*>& mapPlugin)
{
    QDir pluginsDir(strPluginDir);
    InterfaceType *pInterface;

    foreach (QString fileName, pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
       loadPlugin(strPluginDir + "/" + fileName, mapPlugin);
    }
    pluginsDir.setNameFilters(QStringList() << "*.dll"<< "*.a" << "*.so");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
       QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
       if ((pInterface =
                   qobject_cast<InterfaceType *>(loader.instance())) != NULL)
       {
           QStringList lstIDs = pInterface->ids();
           for(int nIndex = 0; nIndex < lstIDs.size(); nIndex++)
           {
               mapPlugin[lstIDs[nIndex]] = pInterface;
           }
       }
    }
    return mapPlugin.size();
}

/**
 * @brief
 *
 * @param pObj
 * @param strType
 * @param strFileName
 * @param strComment
 * @param strAuthor
 * @return bool
 */
bool SCORE_SHARED_EXPORT saveSObject(
        SObject *pObj,
        const QString& strType,
        const QString& strFileName,
        const QString& strComment = "",
        const QString& strAuthor = ""
        );

/**
 * @brief
 *
 * @param strFileName
 * @param strType
 * @param strComment
 * @param strAuthor
 * @param parent
 * @return SObject
 */
SObject* SCORE_SHARED_EXPORT readSObject(
        const QString& strFileName,
        QString& strType,
        QString& strComment,
        QString& strAuthor,
        SObject *parent = NULL
        );

/**
 * @brief
 *
 * @param strName
 * @return bool
 */
bool SCORE_SHARED_EXPORT isValidName(const QString& strName);

/**
 * @brief
 *
 * @param strSrc
 * @param strName
 * @param strType
 * @param uID
 * @return int
 */
int SCORE_SHARED_EXPORT splitNameTypeID(const QString& strSrc, QString& strName, QString& strType, uint& uID);

/**
 * @brief
 *
 * @param pSrcObj
 * @param szIDName
 * @param strType
 * @param QMap<QString
 * @param mapExist
 * @param bInstance
 * @return QString
 */
QString SCORE_SHARED_EXPORT assignID(QObject* pSrcObj, const char* szIDName, const QString& strType, QMap<QString, QVariantMap>& mapExist, bool bInstance = false);
/**
 * @brief
 *
 * @param pSrcObj
 * @param QMap<QString
 * @param mapDes
 * @param strType
 */
void SCORE_SHARED_EXPORT analyseObjectID(QObject* pSrcObj, QMap<QString, QVariantMap>& mapDes, const QString strType = "sobject");

/**
 * @brief
 *
 * @param vl
 * @param ba
 */
void SCORE_SHARED_EXPORT vl2ba(const QVariantList& vl, QByteArray& ba);
/**
 * @brief
 *
 * @param ba
 * @param vl
 */
void SCORE_SHARED_EXPORT ba2vl(const QByteArray& ba, QVariantList& vl);

void SCORE_SHARED_EXPORT addWidgetParam(SObject *pSrc);

SObject* SCORE_SHARED_EXPORT createContainerSObject(const QString& strType, const QString& strName, QMap<QString, QVariantMap> &mapIDs, SObject* pParent = NULL);

SObject* SCORE_SHARED_EXPORT createProjectSObject(QMap<QString, QVariantMap> &mapIDs, const QString& strProjectName = "New Project");

}   // namespace slrs

#endif // srlsS_H
