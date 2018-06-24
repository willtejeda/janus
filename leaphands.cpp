#include "leaphands.h"

void LeapHand::SetJSON(const QMap<QString, QVariant> & map)
{    
    //hand
    QStringList state = map["state"].toString().trimmed().split(" ");

    is_active = true;

    if (state.length() == 16) {
        finger_tracking = false;
        int cur_index = 0;
        for (int k=0; k<16; ++k) {
            basis.data()[k] = state[cur_index].toFloat();
            ++cur_index;
        }
    }
    else if (state.length() == 336) {
        finger_tracking = true;

        int cur_index = 0;
        for (int k=0; k<16; ++k) {
            basis.data()[k] = state[cur_index].toFloat();
            ++cur_index;
        }

        for (int i=0; i<5; ++i) {
            for (int j=0; j<4; ++j) {
                for (int k=0; k<16; ++k) {
                    fingers[i][j].data()[k] = state[cur_index].toFloat();
                    ++cur_index;
                }
            }
        }
    }
}

QString LeapHand::GetJSON() const
{
    QString json_str;
    json_str += "{\"state\":\"";

    for (int k=0; k<16; ++k) {
        json_str += QString::number(basis.data()[k], 'f', 3) + " ";
    }

    if (finger_tracking) {
        for (int i=0; i<5; ++i) {
            for (int j=0; j<4; ++j) {
                for (int k=0; k<16; ++k) {
                    json_str += QString::number(fingers[i][j].data()[k], 'f', 3) + " ";
                }
            }
        }
    }

//    json_str += "\"pos\":\"";
//    json_str += QString::number(hand.pos.x(), 'f', 3) + " ";
//    json_str += QString::number(hand.pos.y(), 'f', 3) + " ";
//    json_str += QString::number(hand.pos.z(), 'f', 3) + "\",";

//    json_str += "\"state\":\"";

//    for (int i=0; i<5; ++i) {
//        for (int j=0; j<4; ++j) {

//            if (j == 0) {
//                json_str += QString::number(hand.fingers[i].bone[j].width, 'f', 3) + " ";
//                json_str += QString::number(hand.fingers[i].bone[j].start.x(), 'f', 3) + " ";
//                json_str += QString::number(hand.fingers[i].bone[j].start.y(), 'f', 3) + " ";
//                json_str += QString::number(hand.fingers[i].bone[j].start.z(), 'f', 3) + " ";
//            }
//            json_str += QString::number(hand.fingers[i].bone[j].end.x(), 'f', 3) + " ";
//            json_str += QString::number(hand.fingers[i].bone[j].end.y(), 'f', 3) + " ";
//            json_str += QString::number(hand.fingers[i].bone[j].end.z(), 'f', 3);

//            if (!(i == 4 && j == 3)) {
//                json_str += " ";
//            }

//        }
//    }

    json_str += "\"}";

    //qDebug() << "hands_json:" << json_str;

    return json_str;

}

LeapHand LeapHand::Interpolate(const LeapHand & h1, const LeapHand & h2, const float t)
{   
    LeapHand h;

    h.is_active = (h1.is_active && h2.is_active);
    h.finger_tracking = (h1.finger_tracking && h2.finger_tracking);
    h.finger_tracking_leap_hmd = (h1.finger_tracking_leap_hmd && h2.finger_tracking_leap_hmd);
    h.basis = MathUtil::InterpolateMatrices(h1.basis, h2.basis, t);

    for (int j=0; j<5; ++j) {
        for (int b=0; b<4; ++b) {
            h.fingers[j][b] = MathUtil::InterpolateMatrices(h1.fingers[j][b], h2.fingers[j][b], t);
        }
    }

    return h;
}

