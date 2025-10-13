#ifndef SEVENT_H
#define SEVENT_H

#include <QEvent>
#include <QVariantMap>

/**
 * @brief
 *
 */
class SEvent : public QEvent
{
    QVariantMap m_params; /**< TODO */
public:
    /**
     * @brief
     *
     * @param type
     */
    SEvent(Type type);
    /**
     * @brief
     *
     * @return QVariantMap
     */
    QVariantMap& params();
    /**
     * @brief
     *
     * @param name
     * @param value
     */
    void setParam(const QString& name, const QVariant& value);
};

#endif // SEVENT_H
