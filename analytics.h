#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QObject>
#include <QtNetwork>

#include "mathutil.h"
#include "webasset.h"

class Analytics : public QObject
{
    Q_OBJECT

public:
    Analytics();
    ~Analytics();
    static void Initialize();
    static void PostPageview(QString dh = NULL, QString dp = NULL, QString dt = NULL);
    static void PostEvent(QString ec, QString ea, QString el = NULL, QString sc = NULL);
    static void Post(QHash<QString,QString> analytics);

private:    
    static QUuid client_id;
};

#endif // ANALYTICS_H
