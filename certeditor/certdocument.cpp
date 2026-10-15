#include "certdocument.h"

CertDocument::CertDocument(QObject *parent) :
    QTextDocument(parent)
{
    current_id_part=-1;
    current_id_ship=-1;
    sertType=0;
    lang="ru";
    isProcessing=false;
}

int CertDocument::getType()
{
    return sertType;
}

svData CertDocument::getSData()
{
    return sdata;
}

void CertDocument::build(int id_part, int id_ship, QString name, QString prefix)
{
    current_id_part=id_part;
    current_id_ship=id_ship;
    sname=name;
    spref=prefix;
    //updSData();
    rebuild();
}

QString CertDocument::getName()
{
    return sname;
}

void CertDocument::rebuild()
{
    if (spref.isEmpty()){
        this->clear();
        emit sigRefresh();
        return;
    }

    updSData();
    QString docs;
    for (const sertData &d : sdata){
        int id_doc=d.id_doc;
        bool en = mapSert.value(id_doc,d.en);
        if (en){
            if (!docs.isEmpty()){
                docs+="|";
            }
            docs+=QString::number(id_doc);
        }
    }
    int id = (current_id_ship>=0) ? current_id_ship : current_id_part;

    QUrlQuery query;
    query.addQueryItem("lang",lang);
    query.addQueryItem("part",(current_id_ship>=0) ? "false" : "true");
    query.addQueryItem("docs",docs);

    QUrl url = RestConnection::instance()->buildUrl("api/"+spref+"/certificates/"+QString::number(sertType)+"/"+QString::number(id),query);

    queue.enqueue(url);

    if (!isProcessing) {
        processNextRequest();
    }
}

void CertDocument::setType(int t)
{
    sertType=t;
    this->rebuild();
}

void CertDocument::setLang(QString l)
{
    lang=l;
    this->rebuild();
}

void CertDocument::setDocEn(int id_doc, bool en)
{
    mapSert[id_doc]=en;
    this->rebuild();
}

void CertDocument::setDefaultDoc()
{
    mapSert.clear();
    this->rebuild();
}

void CertDocument::clearCache()
{
    map.clear();
}

int CertDocument::getIdShip()
{
    return current_id_ship;
}

QString CertDocument::getLang()
{
    return lang;
}

QString CertDocument::getPrefix()
{
    return spref;
}

void CertDocument::loadDoc(const QString &html)
{
    QMap <QString, QByteArray> tmpRes;
    QStringList resList;
    QTextDocument tmpDoc;
    tmpDoc.setHtml(html);
    QTextBlock bl = tmpDoc.begin();
    while(bl.isValid()){
        QTextBlock::iterator it;
        for(it = bl.begin(); !(it.atEnd()); ++it){
            QTextFragment currentFragment = it.fragment();
            if(currentFragment.isValid()){
                if(currentFragment.charFormat().isImageFormat()){
                    QTextImageFormat imgFmt = currentFragment.charFormat().toImageFormat();
                    QString name=imgFmt.name();
                    resList.push_back(name);
                    if (!map.contains(name)){
                        QByteArray resp;
                        bool ok = RestConnection::instance()->sendSyncGet(name,resp);
                        if (ok){
                            if (!name.contains("qrcode")){
                                map.insert(name,resp);
                            } else {
                                tmpRes.insert(name,resp);
                            }
                        }
                    }
                }
            }
        }
        bl = bl.next();
    }
    for (QString res : resList){
        if (map.contains(res)){
            this->addResource(QTextDocument::ImageResource,res,map.value(res));
        } else {
            this->addResource(QTextDocument::ImageResource,res,tmpRes.value(res));
        }
    }
    this->setHtml(html);
    emit sigRefresh();
}

void CertDocument::processNextRequest()
{
    if (queue.isEmpty()) {
        isProcessing = false;
        return;
    }

    isProcessing = true;
    QUrl url = queue.dequeue();

    QNetworkReply *reply = RestConnection::instance()->sendGet(url);
    connect(reply,SIGNAL(finished()),this,SLOT(rebuildFinished()));
}

void CertDocument::rebuildFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    const QByteArray data = reply->readAll();
    const QString errStr  = reply->errorString();
    const auto err        = reply->error();
    reply->deleteLater();

    if (err != QNetworkReply::NoError) {
        this->clear();
        emit sigRefresh();
        QMessageBox::critical(QApplication::activeWindow(),tr("Ошибка"), errStr + "\n" + data,QMessageBox::Cancel);
        processNextRequest();
        return;
    }
    loadDoc(data);
    processNextRequest();
}

void CertDocument::updSData()
{
    sdata.clear();
    if (spref.isEmpty()){
        return;
    }
    QByteArray resp;
    bool ok = RestConnection::instance()->sendSyncGet("api/"+spref+"/sertdata/"+QString::number(current_id_part),resp);
    if (ok){
        QJsonDocument respDoc;
        respDoc=QJsonDocument::fromJson(resp);
        QJsonArray json=respDoc.array();
        for (const QJsonValue &v : json){
            sertData s;
            s.id_doc=v.toObject().value("id_doc").toInt();
            s.nom_doc=v.toObject().value("nom_doc").toString();
            s.ved_short=v.toObject().value("ved_short").toString();
            s.en=mapSert.value(s.id_doc,v.toObject().value("en").toBool());
            sdata.push_back(s);
        }
    }
}
