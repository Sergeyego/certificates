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

    const QByteArray data = reply->readAll();
    if (err != QNetworkReply::NoError) {
        RestError::critical(tr("Ошибка"), errStr + "\n" + data);
        return;
    }

    QPixmap pix;
    pix.loadFromData(data);
    if (!pix.isNull()){
        int id_pix = reply->property("id_pix").toInt();
        mapPix.insert(id_pix,pix);
        int index = ui->tableViewVed->currentIndex().row();
        if (index>=0){
            int current_id_pix = modelVed->getModelData(index,"id_pos").toInt();
            if (id_pix==current_id_pix){
                viewPix(pix);
            } else {
                pix=mapPix.value(current_id_pix);
                if (!pix.isNull()){
                    viewPix(pix);
                }
            }
        }
    }
}

void FormVed::loadImg(QModelIndex index)
{
    int id_pix=modelVed->getModelData(index.row(),"id_pos").toInt();
    QPixmap pix=mapPix.value(id_pix);
    if (!pix.isNull()){
        viewPix(pix);
    } else {
        QNetworkReply *reply = RestConnection::instance()->sendGet(RestConnection::instance()->buildUrl("api/pos-"+QString::number(id_pix)+".png"));
        reply->setProperty("id_pix",id_pix);
        connect(reply,SIGNAL(finished()),this,SLOT(updImgFinished()));
    }


    //ui->labelSimb->setPixmap(QPixmap());
    //int id=ui->tableViewVed->model()->data(ui->tableViewVed->model()->index(index.row(),0),Qt::EditRole).toInt();
    //QPixmap pix;
    /*pix.loadFromData(Rels::instance()->mapVedPix.value(id));
    if (!pix.isNull()){
        viewPix(pix);
    }*/
    ui->cmdDel->setEnabled(!pix.isNull());
}

void FormVed::newImg()
{
    QDir dir(QDir::homePath());
    QString filename = QFileDialog::getOpenFileName(nullptr,QString::fromUtf8("Выберите изображение"),
                                                    dir.path(),
                                                    QString::fromUtf8("Images (*.png *.xpm *.jpg *.jpeg *.bmp)"));
    QImage img(filename);
    if (!img.isNull()){
        /*int row=ui->tableViewVed->currentIndex().row();
        int id=ui->tableViewVed->model()->data(ui->tableViewVed->model()->index(row,0),Qt::EditRole).toInt();
        QByteArray arr;
        QBuffer buffer(&arr);
        buffer.open(QIODevice::WriteOnly);
        img.save(&buffer,"PNG");
        buffer.close();

        QSqlQuery query;
        query.prepare("update zvd_ved set simb = :data where id= :id");
        query.bindValue(":id",id);
        query.bindValue(":data",arr);
        if (query.exec()){
            viewPix(QPixmap::fromImage(img));
            ui->cmdDel->setEnabled(!img.isNull());
            Rels::instance()->refreshVedPix();
        } else {
            QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
        }*/
    }
}

void FormVed::delImg()
{
    /*int row=ui->tableViewVed->currentIndex().row();
    int id=ui->tableViewVed->model()->data(ui->tableViewVed->model()->index(row,0),Qt::EditRole).toInt();
    QSqlQuery query;
    query.prepare("update zvd_ved set simb = NULL where id= :id");
    query.bindValue(":id",id);
    if (query.exec()){
        viewPix(QPixmap());
        Rels::instance()->refreshVedPix();
    } else {
        QMessageBox::critical(this,"Error",query.lastError().text(),QMessageBox::Cancel);
    }*/
}

void FormVed::upd()
{
    modelVed->select();
    modelDoc->select();
}
