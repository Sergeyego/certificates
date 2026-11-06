#include "formved.h"
#include "ui_formved.h"

FormVed::FormVed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormVed)
{
    ui->setupUi(this);

    modelVed = new RestTableModel("zvd_ved",this);
    ui->tableViewVed->setModel(modelVed);

    modelDoc = new RestTableModel("zvd_doc",this);
    ui->tableViewDocVid->setModel(modelDoc);

    connect(ui->tableViewVed->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(loadImg(QModelIndex)));
    connect(ui->cmdBrowse,SIGNAL(clicked(bool)),this,SLOT(newImg()));
    connect(ui->cmdDel,SIGNAL(clicked(bool)),this,SLOT(delImg()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(modelVed,SIGNAL(sigRefresh()),this,SLOT(updVedFinished()));

    upd();
}

FormVed::~FormVed()
{
    delete ui;
}

void FormVed::viewPix(const QPixmap &pix)
{
    if (pix.isNull()){
        ui->labelSimb->setPixmap(pix);
    } else {
        ui->labelSimb->setPixmap(pix.scaled(ui->labelSimb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    ui->cmdDel->setEnabled(!pix.isNull());
}

void FormVed::updVedFinished()
{
    if (modelVed->rowCount()){
        ui->tableViewVed->selectRow(0);
    }
}

void FormVed::updImgFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    const auto err = reply->error();
    const QString errStr = reply->errorString();
    reply->deleteLater();

    if (err == QNetworkReply::OperationCanceledError) return;

    // Игнорируем 404 — если картинки нет, просто показываем пустоту
    if (err == QNetworkReply::ContentNotFoundError) {
        int id_pix = reply->property("id_pix").toInt();
        mapPix.insert(id_pix, QPixmap()); // кэшируем пустоту, чтобы не дергать сервер каждый раз
        viewPix(QPixmap());
        return;
    }

    const QByteArray data = reply->readAll();
    if (err != QNetworkReply::NoError) {
        RestError::critical(tr("Ошибка"), errStr + "\n" + data);
        viewPix(QPixmap());
        return;
    }

    QPixmap pix;
    pix.loadFromData(data);
    int id_pix = reply->property("id_pix").toInt();
    mapPix.insert(id_pix, pix);

    int index = ui->tableViewVed->currentIndex().row();
    if (index >= 0) {
        int current_id_pix = modelVed->getModelData(index,"id").toInt();
        if (id_pix == current_id_pix) {
            viewPix(pix);
        }
    }
}

void FormVed::onUploadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    const auto err = reply->error();
    const QString errStr = reply->errorString();
    reply->deleteLater();

    if (err == QNetworkReply::OperationCanceledError) return;

    const QByteArray data = reply->readAll();
    if (err != QNetworkReply::NoError) {
        RestError::critical(tr("Ошибка"), errStr + "\n" + data);
        viewPix(QPixmap());
        return;
    }
    int id_pix = reply->property("id_pix").toInt();
    mapPix.remove(id_pix);
    loadImg(ui->tableViewVed->currentIndex());
}

void FormVed::loadImg(QModelIndex index)
{
    if (!index.isValid()){
        viewPix(QPixmap());
        return;
    }
    QVariant id = modelVed->getModelData(index.row(),"id");
    if (id.isNull()){
        viewPix(QPixmap());
        return;
    }
    int id_pix=id.toInt();
    if (mapPix.contains(id_pix)){
        viewPix(mapPix.value(id_pix));
    } else {
        QNetworkReply *reply = RestConnection::instance()->sendGet(RestConnection::instance()->buildUrl("api/depimages/"+QString::number(id_pix)+".png"));
        reply->setProperty("id_pix",id_pix);
        connect(reply,SIGNAL(finished()),this,SLOT(updImgFinished()));
    }
}

void FormVed::newImg()
{
    int index = ui->tableViewVed->currentIndex().row();
    if (index<0){
        return;
    }
    QVariant id = modelVed->getModelData(index,"id");
    if (id.isNull()){
        return;
    }
    QDir dir(QDir::homePath());
    QString filename = QFileDialog::getOpenFileName(nullptr,tr("Выберите изображение"),
                                                    dir.path(),
                                                    tr("Images (*.png *.xpm *.jpg *.jpeg *.bmp)"));
    QImage img(filename);
    if (!img.isNull()){
        QByteArray arr;
        QBuffer buffer(&arr);
        buffer.open(QIODevice::WriteOnly);
        img.save(&buffer,"PNG");
        buffer.close();

        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

        QHttpPart filePart;

        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"img.png\""));
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
        filePart.setBody(arr);

        multiPart->append(filePart);

        QUrl url = RestConnection::instance()->buildUrl("api/depimages/"+id.toString()+".png");
        QNetworkReply *reply = RestConnection::instance()->sendRequest(url,"PUT",multiPart);
        reply->setProperty("id_pix",id);
        multiPart->setParent(reply);
        connect(reply,SIGNAL(finished()),this,SLOT(onUploadFinished()));

    }
}

void FormVed::delImg()
{
    int row=ui->tableViewVed->currentIndex().row();
    if (row<0){
        return;
    }
    QString nam = modelVed->getModelData(row,"nam").toString();
    int n = QMessageBox::question(this,tr("Подтвердите удаление"),tr("Подтверждаете уданение логотипа ")+nam+"?",QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
        int id=modelVed->getModelData(row,"id").toInt();
        QByteArray data;
        bool ok = RestConnection::instance()->sendSyncRequest("api/depimages/"+QString::number(id)+".png","DELETE","{}",data);
        if (ok){
            mapPix.remove(id);
            viewPix(QPixmap());
        }
    }
}

void FormVed::upd()
{
    if (sender()==ui->pushButtonUpd){
        mapPix.clear();
        QVector<RestTableModel*> arrMod;
        arrMod.push_back(modelVed);
        arrMod.push_back(modelDoc);
        RelModels::instance()->updateRels(arrMod);
    }
    modelVed->select();
    modelDoc->select();
}
