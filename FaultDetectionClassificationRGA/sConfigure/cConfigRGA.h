#pragma once

class cConfigRGA{
public:
    enum _UNIT_GAUGE:int{_UNIT_CURRENT_A=0, _UNIT_GAUGE_MBAR=1, _UNIT_GAUGE_PA=2,
                     _UNIT_GAUGE_TORR=3, _UNIT_GAUGE_PPM_CAL=4, _UNIT_GAUGE_PPM=5,
                    _UNIT_GAUGE_MTORR=6, _UNIT_PP_N2,_UNIT_PPM_N2};
//    struct _struct_cleanliness{
//        int startMassNumerator=45;
//        int endMassNumerator=100;
//        int startMassDenominator=0;
//        int endMassDenominator=100;
//    };
    cConfigRGA();
    ~cConfigRGA();
};
