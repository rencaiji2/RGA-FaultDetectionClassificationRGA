#pragma once

#include <QString>

struct _STATE_RGA{
    enum TYPE_EQ:quint32{NULL_EQ= 0, XKL_EQ, NAURA_EQ};
    bool usingSEMI_XY= false;
    TYPE_EQ EQ= NULL_EQ;
    quint32 AD_SVID= 0;
    quint32 AD_CEID= 0;
    quint32 AD_ALID= 0;
    quint32 LevelSoftware= 0;
    _STATE_RGA(){}
    _STATE_RGA(const _STATE_RGA &other):
        usingSEMI_XY(other.usingSEMI_XY),
        EQ(other.EQ),
        AD_SVID(other.AD_SVID),
        AD_CEID(other.AD_CEID),
        AD_ALID(other.AD_ALID){

    }
    _STATE_RGA &operator=(const _STATE_RGA &other) {
        if (this != &other) {
            usingSEMI_XY= other.usingSEMI_XY;
            EQ= other.EQ;
            AD_SVID= other.AD_SVID;
            AD_CEID= other.AD_CEID;
            AD_ALID= other.AD_ALID;
        }
        return *this;
    }
};
