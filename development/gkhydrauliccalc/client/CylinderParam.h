#ifndef CYLINDERPARAM_H
#define CYLINDERPARAM_H

#include <QObject>

class CylinderParam
{
public:
    CylinderParam();
    CylinderParam(qreal d1, qreal l, qreal vd2, qreal vdeta, qreal vl, qreal p, qreal t);

public:
    qreal _d1;
    qreal _l;
    qreal _valveD2;
    qreal _valveDeta;
    qreal _valveL;

    qreal _p;
    qreal _t;

    qreal _volume;
    qreal _u1;
    qreal _Q;
    qreal _delayT;
};

inline CylinderParam::CylinderParam() { }

inline CylinderParam::CylinderParam(qreal d1, qreal l, qreal vd2, qreal vdeta, qreal vl, qreal p, qreal t) :
    _d1(d1), _l(l), _valveD2(vd2), _valveDeta(vdeta), _valveL(vl), _p(p), _t(t) { }

#endif // CYLINDERPARAM_H
