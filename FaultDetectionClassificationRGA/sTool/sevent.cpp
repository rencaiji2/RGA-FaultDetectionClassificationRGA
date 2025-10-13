#include "sevent.h"

SEvent::SEvent(Type type)
    :QEvent(type)
{
}

QVariantMap& SEvent::params()
{
    return m_params;
}

void SEvent::setParam(const QString &name, const QVariant &value)
{
    m_params[name] = value;
}

