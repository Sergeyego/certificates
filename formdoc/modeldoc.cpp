#include "modeldoc.h"


ModelDoc::ModelDoc(QObject *parent) : RestTableModel("zvd_sert",parent)
{
    connect(this,SIGNAL(sigUpd()),this,SLOT(updateList()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(updateList()));
}

QVariant ModelDoc::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (role==Qt::BackgroundRole){
        if (fileExist(index.row()) && isActive(index.row())){
            return QVariant(QColor(170,255,170));
        } else {
            return QVariant(QColor(255,170,170));
        }
    }
    return RestTableModel::data(index,role);
}

bool ModelDoc::fileExist(int ind) const
{
    if (ind < 0 || ind >= rowCount()) return false;
    QString nom=this->getModelData(ind,"nom_doc").toString();
    docInfo def;
    def.id="-1";
    def.file="";
    return docMap.contains(nom) && !docMap.value(nom,def).file.isEmpty();
}

bool ModelDoc::isActive(int ind) const
{ 
    if (ind < 0 || ind >= rowCount()) return false;

    QDate current=QDate::currentDate();
    QDate beg=this->getModelData(ind,"dat_beg").toDate();
    QDate end=this->getModelData(ind,"dat_end").toDate();

    if (end.isNull()) end = current.addDays(1);

    return (current>=beg && current<=end);
}

void ModelDoc::refresh(bool activeOnly)
{
    if (activeOnly){
        RestFilter filter = RestFilter(RestFilter::Group::Or);
        filter.addRule(this->tableName(),"dat_end",RestFilter::Op::Null);
        filter.addRule(this->tableName(),"dat_end",RestFilter::Op::Gte,QDate::currentDate().toString("yyyy-MM-dd"));
        setFilter(filter);
    } else {
        setFilter(RestFilter::rule(this->tableName(),"id",RestFilter::Op::Gte,0));
    }
    select();
}

void ModelDoc::viewDoc(int ind)
{
    if (fileExist(ind)){
        QString nom=this->getModelData(ind,"nom_doc").toString();
        QNetworkReply *reply = RestConnection::instance()->sendGet(QUrl::fromUserInput(docMap.value(nom).file));
        connect(reply,SIGNAL(finished()),this,SLOT(replyViewFinished()));
    }
}

void ModelDoc::saveAs(int ind)
{
    if (fileExist(ind)){
        QString nom=this->getModelData(ind,"nom_doc").toString();
        QNetworkReply *reply = RestConnection::instance()->sendGet(QUrl::fromUserInput(docMap.value(nom).file));
        reply->setProperty("nom",nom);
        connect(reply,SIGNAL(finished()),this,SLOT(replySaveAsFinished()));
    }
}

void ModelDoc::upload(int ind)
{
    if (ind < 0) return;
    QSettings settings("szsm", QApplication::applicationName());
    QDir dir(settings.value("savePath",QDir::homePath()).toString());
    QString filename=QFileDialog::getOpenFileName(QApplication::activeWindow(), tr("Открыть файл"),dir.path(),tr("pdf (*.pdf)"));
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)){
        QFileInfo info(file);
        settings.setValue("savePath",info.path());
        QString nom = this->getModelData(ind,"nom_doc").toString();
        int id_doc = this->getModelData(ind,"id").toInt();
        QUrl url;
        if (docMap.contains(nom)){
            docInfo info = docMap.value(nom);
            QUrlQuery query;
            query.addQueryItem("id",QString::number(id_doc));
            query.addQueryItem("elementId",info.id);
            url = RestConnection::instance()->buildUrl("api/site/docs/upd",query);
        } else {
            QUrlQuery query;
            query.addQueryItem("id",QString::number(id_doc));
            url = RestConnection::instance()->buildUrl("api/site/docs/add",query);
        }
        QNetworkReply *reply = RestConnection::instance()->sendRequest(url,"POST",file.readAll(),"application/pdf");
        reply->setProperty("id_doc",id_doc);
        connect(reply,SIGNAL(finished()),this,SLOT(replyUpdFinished()));
        file.close();
    }
}

void ModelDoc::clearFile(int ind)
{
    if (ind < 0) return;
    QString nom = this->getModelData(ind,"nom_doc").toString();
    int n=QMessageBox::question(QApplication::activeWindow(),tr("Подтвердите удаление"),tr("Подтветждаете удаление ")+nom+"?",QMessageBox::Yes| QMessageBox::No);
    if (n==QMessageBox::Yes) {
        if (docMap.contains(nom)){
            int id_doc = this->getModelData(ind,"id").toInt();
            docInfo info = docMap.value(nom);
            QUrlQuery query;
            query.addQueryItem("id",QString::number(id_doc));
            query.addQueryItem("elementId",info.id);
            QUrl url = RestConnection::instance()->buildUrl("api/site/docs/upd",query);
            QNetworkReply *reply = RestConnection::instance()->sendRequest(url,"POST",QByteArray(),"application/pdf");
            reply->setProperty("id_doc",id_doc);
            connect(reply,SIGNAL(finished()),this,SLOT(replyUpdFinished()));
        }
    }
}

void ModelDoc::updateList()
{
    if (isUpdatingList) return; // Уже запрашиваем, не дублируем
    isUpdatingList = true;
    QNetworkReply *reply = RestConnection::instance()->sendGet(RestConnection::instance()->buildUrl("api/site/docs/list"));
    connect(reply,SIGNAL(finished()),this,SLOT(replyListFinished()));
}

void ModelDoc::replyListFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply){
        QByteArray data=reply->readAll();
        bool ok=(reply->error()==QNetworkReply::NoError);
        if (!ok){
            RestError::critical(tr("Ошибка"),reply->errorString()+"\n"+data);
        } else {
            QJsonDocument respDoc;
            respDoc=QJsonDocument::fromJson(data);
            if (respDoc.isArray()){
                docMap.clear();
                QJsonArray arr = respDoc.array();
                for (const QJsonValue &v : arr){
                    QString name=v.toObject().value("NAME").toString();
                    docInfo info;
                    info.id=v.toObject().value("ID").toString();
                    info.file=v.toObject().value("FILE").toString();
                    docMap.insert(name,info);
                }
            }
            if (rowCount() > 0) {
                emit dataChanged(this->index(0,0),this->index(rowCount()-1,columnCount()-1));
            }
            emit sigList();
        }
        isUpdatingList = false;
        reply->deleteLater();
    }
}

void ModelDoc::replyViewFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply){
        QByteArray data=reply->readAll();
        bool ok=(reply->error()==QNetworkReply::NoError);
        if (!ok){
            RestError::critical(tr("Ошибка"),reply->errorString()+"\n"+data);
        } else {
            TempFileManager::instance().createFile(data,"pdf",true);
        }
        reply->deleteLater();
    }

}

void ModelDoc::replySaveAsFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply){
        QByteArray data=reply->readAll();
        bool ok=(reply->error()==QNetworkReply::NoError);
        if (!ok){
            RestError::critical(tr("Ошибка"),reply->errorString()+"\n"+data);
        } else {
            QString name=reply->property("nom").toString();
            QSettings settings("szsm", QApplication::applicationName());
            QDir dir(settings.value("savePath",QDir::homePath()).toString());
            QString filename=QFileDialog::getSaveFileName(QApplication::activeWindow(), tr("Сохранить файл"),dir.path()+QString("/")+name+".pdf",tr("pdf (*.pdf)"));
            if (!filename.isEmpty()){
                QFile file(filename);
                if (file.open(QFile::WriteOnly)){
                    if (file.write(data) != data.size()) {
                        RestError::critical(tr("Ошибка"), tr("Не удалось записать файл полностью. Возможно, диск переполнен."));
                    }
                    file.close();
                    QFileInfo info(file);
                    settings.setValue("savePath",info.path());
                }
            }
        }
        reply->deleteLater();
    }
}

void ModelDoc::replyUpdFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply){
        QByteArray data=reply->readAll();
        bool ok=(reply->error()==QNetworkReply::NoError);
        if (!ok){
            RestError::critical(tr("Ошибка"),reply->errorString()+"\n"+data);
        } else {
            int id_doc = reply->property("id_doc").toInt();
            QJsonDocument respDoc = QJsonDocument::fromJson(data);
            if (respDoc.isObject()){
                QJsonValue resultVal = respDoc.object().value("result");
                if (resultVal.isObject()) {
                    QJsonObject o = resultVal.toObject();
                    QString name = o.value("NAME").toString();
                    if (!name.isEmpty()) { // <--- ЗАЩИТА ОТ ПУСТОГО КЛЮЧА
                        docInfo info;
                        info.id = o.value("ID").toString();
                        info.file = o.value("FILE").toString();
                        docMap[name] = info;
                    }
                }
            }
            QVariantList id;
            id << id_doc;
            int index = this->findRowByPrimaryKeys(id);
            if (index >= 0) {
                emit dataChanged(this->index(index,0),this->index(index,columnCount()-1));
            }
            emit sigList();
        }
        reply->deleteLater();
    }
}
