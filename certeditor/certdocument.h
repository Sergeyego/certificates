#ifndef CERTDOCUMENT_H
#define CERTDOCUMENT_H

#include <QTextDocument>
#include <QtGui>
#include <QFont>
#include <QTextCharFormat>
#include <QMessageBox>
#include <QTextDocumentFragment>
#include <QTextLength>
#include <QDebug>
#include <QQueue>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QPointer>
#include "rest/restconnection.h"

struct sertData {
    int id_doc;
    QString nom_doc;
    QString ved_short;
    bool en;
};

typedef QVector<sertData> svData;

class CertDocument : public QTextDocument
{
    Q_OBJECT
public:
    explicit CertDocument(QObject *parent = nullptr);
    int getType();
    svData getSData();
    QMap<int, bool> getMapSert();
    bool getDocEn(int id_doc);
    void build(int id_part, int id_ship, QString name, QString prefix);
    QString getName();

public slots:
    void rebuild();
    void setType(int t);
    void setLang(QString l);
    void setDocEn(int id_doc, bool en);
    void setDefaultDoc();
    void setBatchMode(bool batch);
    void clearCache();
    int getIdShip();
    QString getLang();
    QString getPrefix();
    void loadDoc(const QString &html);

private slots:
    void processNextRequest();
    void rebuildFinished();
    void updSDataFinished();
    void imageDownloadFinished();

private:
    void updSData();
    void downloadNextImage();
    void abortReply(QPointer<QNetworkReply> &reply);

    int sertType = 0;
    int current_id_part = -1;
    int current_id_ship = -1;
    QString lang = "ru";
    QMap<QString, QByteArray> map;
    svData sdata;
    QMap<int, bool> mapDocSert;
    QMap<int, bool> mapDocOrig;
    QString sname;
    QString spref;

    QUrl nextUrl;
    bool isProcessing = false;

    QPointer<QNetworkReply> currentSertDataReply;
    QPointer<QNetworkReply> currentImageReply;

    QString currentHtml;
    QStringList pendingImages;
    QMap<QString, QByteArray> tmpRes;
    bool m_batchMode = false;

signals:
    void sigDocRefresh();
    void buildFinished();
};

#endif // CERTDOCUMENT_H
