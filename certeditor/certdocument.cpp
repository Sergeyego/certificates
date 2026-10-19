#include "certdocument.h"
#include <QApplication>

CertDocument::CertDocument(QObject *parent) :
    QTextDocument(parent)
{
}

int CertDocument::getType()                 { return sertType; }
svData CertDocument::getSData()             { return sdata; }
QMap<int, bool> CertDocument::getMapSert()  { return mapDocSert; }
QString CertDocument::getName()             { return metaInformation(QTextDocument::DocumentTitle); }
int CertDocument::getIdShip()               { return current_id_ship; }
QString CertDocument::getLang()             { return lang; }
QString CertDocument::getPrefix()           { return spref; }
void CertDocument::clearCache()             { map.clear(); }

void CertDocument::renderToOnePage(QPainter *painter)
{
    QRect rect = painter->viewport();
    QSize size (rect.size());
    size.scale(this->size().toSize(),Qt::KeepAspectRatioByExpanding);
    painter->setWindow(0,0,size.width(),size.height());
    if (sertType==1){
        painter->drawImage(painter->window(),QImage(imBackPath()));
    }
    this->drawContents(painter);
}

QString CertDocument::imBackPath()
{
    QString p;
    if (lang=="ru"){
        p=":images/bgr.png";
    } else if (lang=="en"){
        p=":images/bge.png";
    } else {
        p=":images/bgm.png";
    }
    return p;
}

bool CertDocument::getDocEn(int id_doc)
{
    return mapDocSert.value(id_doc, mapDocOrig.value(id_doc, false));
}

void CertDocument::abortReply(QPointer<QNetworkReply> &reply)
{
    if (!reply) return;

    QNetworkReply *old = reply.data();
    reply = nullptr;

    if (old) {
        old->abort();
        old->deleteLater();
    }
}

// ============================================================
// Публичный вход
// ============================================================
void CertDocument::build(int id_part, int id_ship, QString prefix)
{
    current_id_part = id_part;
    current_id_ship = id_ship;
    spref = prefix;
    updSData();
}

// ============================================================
// Загрузка списка аттестаций (Метаданные)
// ============================================================
void CertDocument::updSData()
{
    abortReply(currentSertDataReply);

    sdata.clear();
    mapDocOrig.clear();

    if (spref.isEmpty()) {
        rebuild();
        return;
    }

    const QUrl url = RestConnection::instance()->buildUrl(
        "api/" + spref + "/sertdata/" + QString::number(current_id_part));

    QNetworkReply *reply = RestConnection::instance()->sendGet(url);
    if (!reply) {
        rebuild();
        return;
    }
    currentSertDataReply = reply;
    connect(reply, &QNetworkReply::finished, this, &CertDocument::updSDataFinished);
}

// ============================================================
// Загрузка списка аттестаций
// ============================================================
void CertDocument::updSDataFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    if (currentSertDataReply == reply) {
        currentSertDataReply = nullptr;
    }

    const QString errStr = reply->errorString();
    const auto err = reply->error();
    reply->deleteLater();

    if (err == QNetworkReply::OperationCanceledError) {
        return;
    }

    const QByteArray resp = reply->readAll();

    if (err == QNetworkReply::NoError) {
        const QJsonArray json = QJsonDocument::fromJson(resp).array();
        for (const QJsonValue &v : json) {
            sertData s;
            s.id_doc    = v.toObject().value("id_doc").toInt();
            s.nom_doc   = v.toObject().value("nom_doc").toString();
            s.ved_short = v.toObject().value("ved_short").toString();
            s.en        = v.toObject().value("en").toBool();
            sdata.push_back(s);
            mapDocOrig.insert(s.id_doc, s.en);
        }
        emit sigDocRefresh();
        rebuild();
    } else {
        this->clear();
        emit sigDocRefresh();
        emit buildFinished();
        RestError::critical(tr("Ошибка загрузки метаданных"), errStr + "\n" + resp);
    }
}

// ============================================================
// Построение URL сертификата
// ============================================================
void CertDocument::rebuild()
{
    if (spref.isEmpty()) {
        this->clear();
        emit sigDocRefresh();
        emit buildFinished();
        return;
    }

    QString docs;
    for (const sertData &d : sdata) {
        if (getDocEn(d.id_doc)) {
            if (!docs.isEmpty()) docs += "|";
            docs += QString::number(d.id_doc);
        }
    }

    const int id = (current_id_ship >= 0) ? current_id_ship : current_id_part;

    QUrlQuery query;
    query.addQueryItem("lang", lang);
    query.addQueryItem("part", (current_id_ship >= 0) ? "false" : "true");
    query.addQueryItem("docs", docs);

    nextUrl = RestConnection::instance()->buildUrl(
        "api/" + spref + "/certificates/" + QString::number(sertType) + "/" + QString::number(id),
        query);

    if (!isProcessing) {
        processNextRequest();
    }
}

void CertDocument::setType(int t)                { sertType = t; }
void CertDocument::setLang(QString l)            { lang = l; }
void CertDocument::setDocEn(int id_doc, bool en) { mapDocSert[id_doc] = en; }

void CertDocument::setDefaultDoc()
{
    mapDocSert.clear();
    emit sigDocRefresh();
}

void CertDocument::setBatchMode(bool batch)
{
    m_batchMode=batch;
}

// ============================================================
// Конвейер запросов HTML документа
// ============================================================
void CertDocument::processNextRequest()
{
    if (nextUrl.isEmpty()) {
        isProcessing = false;
        return;
    }

    isProcessing = true;

    const QUrl url = nextUrl;
    nextUrl.clear();

    QNetworkReply *reply = RestConnection::instance()->sendGet(url);
    if (!reply) {
        isProcessing = false;
        emit buildFinished();
        return;
    }
    connect(reply, &QNetworkReply::finished, this, &CertDocument::rebuildFinished);
}

void CertDocument::rebuildFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    const QString errStr = reply->errorString();
    const auto err = reply->error();
    reply->deleteLater();

    if (err == QNetworkReply::OperationCanceledError) {
        processNextRequest();
        if (!isProcessing) {
            emit buildFinished();
        }
        return;
    }

    const QByteArray data = reply->readAll();

    // В пакетном режиме игнорируем прерывание, дорабатываем задачу
    if (!m_batchMode && !nextUrl.isEmpty()) {
        processNextRequest();
        return;
    }

    if (err != QNetworkReply::NoError) {
        this->clear();
        emit sigDocRefresh();
        emit buildFinished();
        RestError::critical(tr("Ошибка загрузки документа"), errStr + "\n" + data);
        processNextRequest();
        return;
    }

    loadDoc(QString::fromUtf8(data));
}

// ============================================================
// Парсинг HTML и загрузка картинок
// ============================================================
void CertDocument::loadDoc(const QString &html)
{
    currentHtml = html;
    pendingImages.clear();
    tmpRes.clear();

    abortReply(currentImageReply);

    QTextDocument tmpDoc;
    tmpDoc.setHtml(html);

    QTextBlock bl = tmpDoc.begin();
    while (bl.isValid()) {
        for (auto it = bl.begin(); !it.atEnd(); ++it) {
            const QTextFragment frag = it.fragment();
            if (frag.isValid() && frag.charFormat().isImageFormat()) {
                const QString name = frag.charFormat().toImageFormat().name();
                if (!name.isEmpty() && !pendingImages.contains(name)) {
                    pendingImages.push_back(name);
                }
            }
        }
        bl = bl.next();
    }

    downloadNextImage();
}

void CertDocument::downloadNextImage()
{
    // В пакетном режиме не уходим на обработку новой задачи раньше времени
    if (!m_batchMode && !nextUrl.isEmpty()) {
        processNextRequest();
        return;
    }

    while (!pendingImages.isEmpty()) {
        const QString nextImg = pendingImages.first();

        if (map.contains(nextImg)) {
            this->addResource(QTextDocument::ImageResource, nextImg, map.value(nextImg));
            pendingImages.removeFirst();
        } else if (tmpRes.contains(nextImg)) {
            this->addResource(QTextDocument::ImageResource, nextImg, tmpRes.value(nextImg));
            pendingImages.removeFirst();
        } else {
            const QUrl url = RestConnection::instance()->buildUrl(nextImg);
            QNetworkReply *reply = RestConnection::instance()->sendGet(url);
            if (!reply) {
                pendingImages.removeFirst();
                continue;
            }
            currentImageReply = reply;
            connect(reply, &QNetworkReply::finished, this, &CertDocument::imageDownloadFinished);
            return;
        }
    }

    this->setHtml(currentHtml);
    emit buildFinished();
    processNextRequest();
}

void CertDocument::imageDownloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    if (currentImageReply == reply) {
        currentImageReply = nullptr;
    }

    const QString errStr = reply->errorString();
    const auto err = reply->error();
    reply->deleteLater();

    if (err == QNetworkReply::OperationCanceledError) {
        return;
    }

    const QByteArray resp = reply->readAll();

    // В пакетном режиме игнорируем прерывание загрузки картинок
    if (!m_batchMode && !nextUrl.isEmpty()) {
        processNextRequest();
        return;
    }

    if (!pendingImages.isEmpty()) {
        const QString expectedName = pendingImages.first();

        if (err == QNetworkReply::NoError) {
            if (!expectedName.contains("qrcode")) {
                map.insert(expectedName, resp);
            } else {
                tmpRes.insert(expectedName, resp);
            }
            this->addResource(QTextDocument::ImageResource, expectedName, resp);
        } else {
            qWarning() << "Failed to load image resource:" << expectedName << "Reason:" << errStr;
        }
        pendingImages.removeFirst();
    }

    downloadNextImage();
}