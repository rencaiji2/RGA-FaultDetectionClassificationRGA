#pragma once

#include "cConfigRGA.h"
#include <QList>
//#include <sMethod/cCommandRGA.h>

struct _PARAM_CPM{
    double delayTimeS= 3600;
    bool autoCPM= true;
    cConfigRGA::_UNIT_GAUGE CPM_GaugeUnit= cConfigRGA::_UNIT_GAUGE_MBAR;
    double thresholdLP0= 0, thresholdLP1= 0;
    double thresholdHP0= 0, thresholdHP1= 0;
    double thresholdHC= 0;//高于就关，低于就开//0= 0, thresholdHC1= 0
    //double thresholdBYPASS0= 0, thresholdBYPASS1= 0;
    cConfigRGA::_UNIT_GAUGE ExternalGaugeUnit= cConfigRGA::_UNIT_GAUGE_MBAR;
    double thresholdPUMP= 0;
    double delayMs_BYPASS= 20;

    void thresholdSort(QList<double>& pList) const{
        pList.clear();
        pList << thresholdLP0 << thresholdLP1 << thresholdHP0
              << thresholdHP1 <<thresholdHC;
        std::sort(pList.begin(), pList.end());
    }
};
struct _IO_EXT{
    bool LP_ON= false;
    bool HP_ON= false;
    bool HC_ON= false;
    bool BYPASS_ON= false;
    bool PUMP_ON= false;
    _IO_EXT& operator==(const _IO_EXT& p_IO_EXT){
        if(this!= &p_IO_EXT){
            LP_ON= p_IO_EXT.LP_ON;
            HP_ON= p_IO_EXT.HP_ON;
            HC_ON= p_IO_EXT.HC_ON;
            BYPASS_ON= p_IO_EXT.BYPASS_ON;
            PUMP_ON= p_IO_EXT.PUMP_ON;
        }
        return *this;
    }
    quint16 toUint16(){
        quint16 v= 0;
        v|= static_cast<quint16>(PUMP_ON)<< 8;
        v|= static_cast<quint16>(LP_ON)<< 9;
        v|= static_cast<quint16>(HP_ON)<< 10;
        v|= static_cast<quint16>(HC_ON)<< 11;
        v|= static_cast<quint16>(BYPASS_ON)<< 12;
        return v;
    }
    void fromUint16(quint16 v){
        PUMP_ON= (v>>8)&0x01;
        LP_ON= (v>>9)&0x01;
        HP_ON= (v>>10)&0x01;
        HC_ON= (v>>11)&0x01;
        BYPASS_ON= (v>>12)&0x01;
    }
};
