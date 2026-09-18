#include "formpartel.h"
#include "ui_formpartel.h"

FormPartEl::FormPartEl(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormPartEl)
{
    ui->setupUi(this);

    QDoubleValidator *validator = new QDoubleValidator(0,10,1,this);
    validator->setLocale(QLocale::English);
    ui->lineEditDiam->setValidator(validator);

    loadSettings();

    ui->dateEditBeg->setDate(QDate(QDate::currentDate().year(),1,1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    ui->comboBoxMark->setModel(RelModels::instance()->getModel("mark"));

    modelTu = new RestRoTableModel(this);
    ui->listViewTu->setModel(modelTu);

    modelNote = new RestRoTableModel(this);

    modelShip = new RestRoTableModel(this);
    ui->tableViewShip->setModel(modelShip);

    modelChemSrc = new RestTableModel("el_parti_chem",this);
    modelChemSrc->setPath("api/elrtr/lab/chem/parti");
    modelChemSrc->setDefaultValue("id_dev",1);
    ui->tableViewChemSrc->setModel(modelChemSrc);

    modelMechSrc = new RestTableModel("el_parti_mech",this);
    modelMechSrc->setPath("api/elrtr/lab/mech/parti");
    ui->tableViewMechSrc->setModel(modelMechSrc);

    modelChem = new RestTableModel("el_sert_chem",this);
    modelChem->setPath("api/elrtr/lab/chem/sert");
    ui->tableViewChem->setModel(modelChem);

    modelMech = new RestTableModel("el_sert_mech",this);
    modelMech->setPath("api/elrtr/lab/mech/sert");
    ui->tableViewMech->setModel(modelMech);

    modelMechx = new RestTableModel("el_sert_mechx",this);
    ui->tableViewMechx->setModel(modelMechx);

    modelPart = new ModelPart(this);
    ui->tableViewPart->setModel(modelPart);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxMarkOnly,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxMark,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->lineEditDiam,SIGNAL(textChanged(QString)),this,SLOT(upd()));
    connect(ui->checkBoxMarkOnly,SIGNAL(clicked(bool)),ui->comboBoxMark,SLOT(setEnabled(bool)));
    connect(ui->checkBoxMarkOnly,SIGNAL(clicked(bool)),ui->lineEditDiam,SLOT(setEnabled(bool)));
    connect(modelPart,SIGNAL(sigRefresh()),this,SLOT(updFinished()));
    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updData(QModelIndex)));
    connect(modelNote,SIGNAL(sigRefresh()),this,SLOT(updNoteFinished()));
    connect(ui->plainTextEditPrim,SIGNAL(textChanged()),this,SLOT(onPrimChanged()));
    connect(ui->plainTextEditPrimProd,SIGNAL(textChanged()),this,SLOT(onPrimProdChanged()));
    connect(ui->checkBoxOk,SIGNAL(clicked(bool)),this,SLOT(setOk(bool)));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(savePrim()));
    connect(ui->toolButtonChem,SIGNAL(clicked(bool)),this,SLOT(copyChem()));
    connect(ui->toolButtonMech,SIGNAL(clicked(bool)),this,SLOT(copyMech()));
    connect(modelChem,SIGNAL(sigUpd()),this,SLOT(updCurrentState()));
    connect(modelMech,SIGNAL(sigUpd()),this,SLOT(updCurrentState()));
    connect(ui->pushButtonImport,SIGNAL(clicked(bool)),this,SLOT(importVals()));

    upd();
}

FormPartEl::~FormPartEl()
{
    saveSettings();
    delete ui;
}

void FormPartEl::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->ui->splitter->restoreState(settings.value("part_el_splitter_width").toByteArray());
}

void FormPartEl::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("part_el_splitter_width",ui->splitter->saveState());
}

void FormPartEl::savePrim()
{
    if (!(primChanged || primChanged)){
        ui->pushButtonSave->setEnabled(false);
        return;
    }

    int currentIndex = ui->tableViewPart->currentIndex().row();
    int id_part = modelPart->getModelData(currentIndex,"id").toInt();

    QByteArray data;

    QJsonDocument doc;
    QJsonObject object;
    if (primChanged){
        object.insert("prim",ui->plainTextEditPrim->toPlainText());
    }
    if (primProdChanged){
        object.insert("prim_prod",ui->plainTextEditPrimProd->toPlainText());
    }
    doc.setObject(object);
    QByteArray body=doc.toJson();

    bool res = RestConnection::instance()->sendSyncRequest("api/elrtr/parti/patch/"+QString::number(id_part),"PATCH",body,data);
    if (res){
        primChanged=false;
        primProdChanged=false;
        ui->pushButtonSave->setEnabled(false);
    }
}

void FormPartEl::copyChem()
{
    if (!modelChem->isEmpty()){
        QMessageBox::information(this,tr("Предупреждение"),tr("Сначала удалите все уже существующие элементы!"),QMessageBox::Ok);
        return;
    }
    int n=QMessageBox::question(this,tr("Подтвердите действия"),tr("Сгенерировать значения на основе испытаний и похожих партий?"),QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
        int currentIndex = ui->tableViewPart->currentIndex().row();
        int id_part = modelPart->getModelData(currentIndex,"id").toInt();
        QByteArray body, data;

        bool res = RestConnection::instance()->sendSyncRequest("api/elrtr/parti/genchem/"+QString::number(id_part),"POST",body,data);
        if (res){
            modelChem->select();
            updCurrentState();
        }
    }
}

void FormPartEl::copyMech()
{
    if (!modelMech->isEmpty()){
        QMessageBox::information(this,tr("Предупреждение"),tr("Сначала удалите все уже существующие элементы!"),QMessageBox::Ok);
        return;
    }
    int n=QMessageBox::question(this,tr("Подтвердите действия"),tr("Сгенерировать значения на основе испытаний и похожих партий?"),QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
        int currentIndex = ui->tableViewPart->currentIndex().row();
        int id_part = modelPart->getModelData(currentIndex,"id").toInt();
        QByteArray body, data;

        bool res = RestConnection::instance()->sendSyncRequest("api/elrtr/parti/genmech/"+QString::number(id_part),"POST",body,data);
        if (res){
            modelMech->select();
            updCurrentState();
        }
    }
}

void FormPartEl::updCurrentState()
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    modelPart->refreshState(currentIndex);
}

void FormPartEl::onPrimChanged()
{
    primChanged=true;
    ui->pushButtonSave->setEnabled(true);
}

void FormPartEl::onPrimProdChanged()
{
    primProdChanged=true;
    ui->pushButtonSave->setEnabled(true);
}

void FormPartEl::upd()
{
    if (sender()==ui->pushButtonUpd){
        QVector<RestTableModel*> arrMod;
        arrMod.push_back(modelChemSrc);
        arrMod.push_back(modelMechSrc);
        arrMod.push_back(modelChem);
        arrMod.push_back(modelMech);
        arrMod.push_back(modelMechx);
        RelModels::instance()->updateRels(arrMod);
        RelModels::instance()->getModel("mark")->refresh();
    }
    int id_el=-1;
    QString diam="";
    if (ui->checkBoxMarkOnly->isChecked()){
        id_el=ui->comboBoxMark->getCurrentData().val.toInt();
        diam=ui->lineEditDiam->text();
        diam=diam.replace(",",".");
    }
    modelPart->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),id_el,diam);
}

void FormPartEl::updFinished()
{
    //qDebug()<<"upd finished";
    if (ui->tableViewPart->model()->rowCount()){
        ui->tableViewPart->selectRow(ui->tableViewPart->model()->rowCount()-1);
        ui->tableViewPart->scrollToBottom();
    }
}

void FormPartEl::updNoteFinished()
{
    if (modelNote->rowCount()){
        ui->plainTextEditPrim->setPlainText(modelNote->getModelData(0,"prim").toString());
        ui->plainTextEditPrimProd->setPlainText(modelNote->getModelData(0,"prim_prod").toString());
        ui->checkBoxOk->setChecked(modelNote->getModelData(0,"ok").toBool());
    } else {
        ui->plainTextEditPrim->clear();
        ui->plainTextEditPrimProd->clear();
        ui->checkBoxOk->setChecked(false);
    }
    primChanged=false;
    primProdChanged=false;
    ui->pushButtonSave->setEnabled(false);
}

void FormPartEl::updData(QModelIndex index)
{
    int id_part = modelPart->getModelData(index.row(),"id").toInt();

    modelTu->setPath("api/elrtr/parti/tu/"+QString::number(id_part));
    modelTu->select();

    modelNote->setPath("api/elrtr/parti/note/"+QString::number(id_part));
    modelNote->select();

    modelShip->setPath("api/elrtr/parti/ship/"+QString::number(id_part));
    modelShip->select();

    modelChemSrc->setFilter(modelChemSrc->tableName()+".id_part = "+QString::number(id_part));
    modelChemSrc->setDefaultValue("id_part",id_part);
    modelChemSrc->select();

    modelChem->setFilter(modelChem->tableName()+".id_part = "+QString::number(id_part));
    modelChem->setDefaultValue("id_part",id_part);
    modelChem->select();

    modelMechSrc->setFilter(modelMechSrc->tableName()+".id_part = "+QString::number(id_part));
    modelMechSrc->setDefaultValue("id_part",id_part);
    modelMechSrc->select();

    modelMech->setFilter(modelMech->tableName()+".id_part = "+QString::number(id_part));
    modelMech->setDefaultValue("id_part",id_part);
    modelMech->select();

    modelMechx->setFilter(modelMechx->tableName()+".id_part = "+QString::number(id_part));
    modelMechx->setDefaultValue("id_part",id_part);
    modelMechx->select();
}

void FormPartEl::setOk(bool ok)
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    int id_part = modelPart->getModelData(currentIndex,"id").toInt();

    QByteArray data;

    QJsonDocument doc;
    QJsonObject object
        {
            {"ok", ok}
        };
    doc.setObject(object);
    QByteArray body=doc.toJson();

    bool res = RestConnection::instance()->sendSyncRequest("api/elrtr/parti/patch/"+QString::number(id_part),"PATCH",body,data);
    if (!res){
        ui->checkBoxOk->setChecked(!ok);
    } else {
        updCurrentState();
    }
}

void FormPartEl::importVals()
{
    if (!modelPart->rowCount()){
        return;
    }

    int currentIndex = ui->tableViewPart->currentIndex().row();
    int id_part = modelPart->getModelData(currentIndex,"id").toInt();

    DialogCopyValEl d(id_part);
    if (d.exec()==QDialog::Accepted){

    }
}

ModelPart::ModelPart(QObject *parent) : RestRoTableModel(parent)
{

}

void ModelPart::refresh(QDate beg, QDate end, int id_el, QString diam)
{
    QUrlQuery query;
    query.addQueryItem("id_el",QString::number(id_el));
    query.addQueryItem("diam",diam);
    setPath("api/elrtr/parti/list/"+beg.toString("yyyy-MM-dd")+"/"+end.toString("yyyy-MM-dd")+"?"+query.toString());
    this->select();
}

void ModelPart::refreshState(int index)
{
    int id_part = this->getModelData(index,"id").toInt();
    QUrl url = QUrl(RestConnection::instance()->getUrl()+"/api/elrtr/parti/color/"+QString::number(id_part));

    QNetworkReply *reply = RestConnection::instance()->sendGet(url);
    reply->setProperty("index",index);
    connect(reply,SIGNAL(finished()),this,SLOT(refreshStateFinished()));
}

void ModelPart::refreshStateFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply){
        QByteArray data=reply->readAll();
        bool ok=(reply->error()==QNetworkReply::NoError);
        if (!ok){
            QMessageBox::critical(nullptr,tr("Ошибка"),reply->errorString()+"\n"+data,QMessageBox::Cancel);
        } else {
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QColor color = QColor(doc.object().value("color").toString());
            int id_part = doc.object().value("id").toInt();

            int index = reply->property("index").toInt();

            int current_id_part = this->getModelData(index,"id").toInt();

            if (id_part==current_id_part) {
                for (int j=0; j<this->columnCount(); j++){
                    modelData[index][j].background = color;
                }
                emit dataChanged(this->index(index,0),this->index(index,columnCount()-1));
            }
        }
        reply->deleteLater();
    }
}
