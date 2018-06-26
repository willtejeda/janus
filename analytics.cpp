#include "analytics.h"

QUuid Analytics::client_id = NULL;

Analytics::Analytics()
{

}

Analytics::~Analytics()
{

}

void Analytics::Initialize()
{    
    const QString filename = MathUtil::GetAppDataPath() + "clientid.txt";
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        client_id = QUuid::createUuid();
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Analytics::Initialize() - File" << filename << " can't be saved.";
            return;
        }
        QTextStream ofs(&file);
        ofs << client_id.toString();
        file.close();
        //qDebug() << "Analytics::Initialize() - Wrote to file: "<< client_id;
    }
    else{
        const QByteArray ba = file.readAll();
        client_id = QUuid(ba);
        file.close();
        //qDebug() << "Analytics::Initialize() - Read from file: "<< client_id;
    }
}

void Analytics::PostPageview(QString dh, QString dp, QString dt)
{
    if (!WebAsset::GetNetworkManager().isNull() && !client_id.isNull()){
        QHash<QString, QString> analytics;

        analytics["t"] = "pageview";
        if (!dh.isNull()){
            analytics["dh"] = dh;
        }
        if (!dp.isNull()){
            analytics["dp"] = dp;
        }
        if (!dt.isNull()){
            analytics["dt"] = dt;
        }
        Post(analytics);
    }
}

void Analytics::PostEvent(QString ec, QString ea, QString el, QString sc)
{
    if (!WebAsset::GetNetworkManager().isNull() && !client_id.isNull()){
        QHash<QString, QString> analytics;

        analytics["t"] = "event";
        analytics["ec"] = ec;
        analytics["ea"] = ea;
        if (!el.isNull()){
            analytics["el"] = el;
        }
        if (!sc.isNull()){
            analytics["sc"] = sc;
        }
        Post(analytics);
    }
}

void Analytics::Post(QHash<QString, QString> analytics)
{
    if (!WebAsset::GetNetworkManager().isNull() && !client_id.isNull()){
        //Create an http network request
        QNetworkRequest req(QUrl("http://www.google-analytics.com/collect"));
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        //Build up the query parameters.
        QUrlQuery query;
        query.addQueryItem("v", "1"); // Version

        query.addQueryItem("tid", GOOGLE_ANALYTICS_ID); // Tracking ID use value assigned by Google Analytics
        query.addQueryItem("cid", client_id.toString().remove("{").remove("}")); // Client ID

        if (analytics.contains("t")){
            query.addQueryItem("t", analytics["t"]);

            // Pageview
            if (analytics["t"].toLower() == "pageview"){
                if (analytics.contains("dh")){
                    query.addQueryItem("dh", QString::fromUtf8(QUrl::toPercentEncoding(analytics["dh"]).data())); // Document hostname
                }
                if (analytics.contains("dp")){
                    query.addQueryItem("dp", QString::fromUtf8(QUrl::toPercentEncoding(analytics["dp"]).data())); // Page
                }
                if (analytics.contains("dt")){
                    query.addQueryItem("dt", QString::fromUtf8(QUrl::toPercentEncoding(analytics["dt"]).data())); // Title
                }
            }
            // Event
            else if (analytics["t"].toLower() == "event"){
                if(analytics.contains("ec") && analytics.contains("ea")){
                    //Session control
                    if (analytics.contains("sc")){
                        query.addQueryItem("sc", QString::fromUtf8(QUrl::toPercentEncoding(analytics["sc"]).data())); // Session control
                    }

                    // Required fields
                    query.addQueryItem("ec", QString::fromUtf8(QUrl::toPercentEncoding(analytics["ec"]).data())); // Event category
                    query.addQueryItem("ea", QString::fromUtf8(QUrl::toPercentEncoding(analytics["ea"]).data())); // Event action

                    //Optional info
                    if (analytics.contains("el")){
                        query.addQueryItem("el", QString::fromUtf8(QUrl::toPercentEncoding(analytics["el"]).data())); // Event label
                    }
                }
                else{
                    qDebug() << "Analytics::Post(): Missing required info for event hit";
                    return;
                }
            }
            else{
                qDebug() << "Analytics::Post(): No valid hit type specified";
                return;
            }
        }
        else{
            qDebug() << "Analytics::Post(): No hit type specified";
            return;
        }

        //Post data
        QByteArray data;
        data.append(query.query());
        //qDebug() << "Analytics::Post() - Posting" << query.toString();
        QNetworkReply * reply = WebAsset::GetNetworkManager()->post(req, data);
        if (analytics.contains("sc") && analytics["sc"] == "end"){
            QEventLoop loop;
            connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();
        }
    }
}
