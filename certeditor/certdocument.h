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
    explicit CertDocument(QObject *parent = 0);
    int getType();
    svData getSData();
    void build(int id_part, int id_ship, QString name, QString prefix);
    QString getName();

public slots:
    void rebuild();
    void setType(int t);
    void setLang(QString l);
    void setDocEn(int id_doc, bool en);
    void setDefaultDoc();
    void clearCache();
    int getIdShip();
    QString getLang();
    QString getPrefix();
    void loadDoc(const QString &html);

private slots:
    void processNextRequest();
    void rebuildFinished();

private:
    int sertType;
    int current_id_part;
    int current_id_ship;
    QString lang;
    void updSData();
    QMap<QString,QByteArray> map;
    svData sdata;
    QMap <int, bool> mapSert;
    QString sname;
    QString spref;
    QQueue<QUrl> queue;
    bool isProcessing;

signals:
    void sigRefresh();
};


#endif // CERTDOCUMENT_H
