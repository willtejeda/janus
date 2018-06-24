#ifndef LEAPHANDS_H
#define LEAPHANDS_H

#include <QtGui>
#include <cfloat>

#include "mathutil.h"

class LeapHand
{
public:

    LeapHand() :
        is_active(false),        
        finger_tracking(false),
        finger_tracking_leap_hmd(false)
    {
    }

    void SetJSON(const QMap<QString, QVariant> & map);
    QString GetJSON() const;

    static LeapHand Interpolate(const LeapHand & h1, const LeapHand & h2, const float t);

    bool is_active;    
    bool finger_tracking;
    bool finger_tracking_leap_hmd;
    QMatrix4x4 basis;    
    QMatrix4x4 fingers[5][4]; //order is 0-thumb, 1-index, 2-middle, 3-third, 4-pinky
};

#endif // LEAPHANDS_H
