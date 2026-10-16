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

    certDocument = new CertDocument(this);
    certEditor = new CertEditor(certDocument);

    ui->dateEditBeg->setDate(QDate(QDate::currentDate().year(),1,1));
    ui->dateEditEnd->setDate(QDate(QDate::currentDate().year(),12,31));

    pasportManager = new PasportManager(this);

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
    connect(ui->pushButtonLabel,SIGNAL(clicked(bool)),this,SLOT(showLabels()));
    connect(ui->pushButtonPasport,SIGNAL(clicked(bool)),this,SLOT(createPasport()));
    connect(ui->pushButtonCert,SIGNAL(clicked(bool)),this,SLOT(showPartCertificate()));
    connect(ui->tableViewShip,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(showShipCertificate(QModelIndex)));

    upd();
}

FormPartEl::~FormPartEl()
{
    saveSettings();
    delete certEditor;
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

void FormPartEl::setBlock(bool b)
{
    ui->tableViewChem->setEnabled(!b);
    ui->tableViewChemSrc->setEnabled(!b);
    ui->tableViewMech->setEnabled(!b);
    ui->tableViewMechSrc->setEnabled(!b);
    ui->tableViewMechx->setEnabled(!b);
    ui->plainTextEditPrim->setEnabled(!b);
    ui->plainTextEditPrimProd->setEnabled(!b);
    ui->checkBoxOk->setEnabled(!b);
    ui->toolButtonChem->setEnabled(!b);
    ui->toolButtonMech->setEnabled(!b);
    ui->pushButtonImport->setEnabled(!b);
    ui->pushButtonLabel->setEnabled(!b);
    ui->pushButtonPasport->setEnabled(!b);
    ui->pushButtonSave->setEnabled(!b);
    ui->pushButtonCert->setEnabled(!b);
}

void FormPartEl::savePrim()
{

    if (!(primChanged || primProdChanged)){
        ui->pushButtonSave->setEnabled(false);
        return;
    }

    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPart->rowCount() || currentIndex < 0) {
        ui->pushButtonSave->setEnabled(false);
        return;
    }

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
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPart->rowCount() || currentIndex < 0) {
        return;
    }

    if (!modelChem->isEmpty()){
        QMessageBox::information(this,tr("Предупреждение"),tr("Сначала удалите все уже существующие элементы!"),QMessageBox::Ok);
        return;
    }
    int n=QMessageBox::question(this,tr("Подтвердите действия"),tr("Сгенерировать значения на основе испытаний и похожих партий?"),QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
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
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPart->rowCount() || currentIndex < 0) {
        return;
    }
    if (!modelMech->isEmpty()){
        QMessageBox::information(this,tr("Предупреждение"),tr("Сначала удалите все уже существующие элементы!"),QMessageBox::Ok);
        return;
    }
    int n=QMessageBox::question(this,tr("Подтвердите действия"),tr("Сгенерировать значения на основе испытаний и похожих партий?"),QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
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
    if (currentIndex < 0) {
        return;
    }
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
    bool ok = ui->tableViewPart->model()->rowCount();
    if (ok){
        ui->tableViewPart->selectRow(ui->tableViewPart->model()->rowCount()-1);
        ui->tableViewPart->scrollToBottom();
    } else {
        modelTu->clear();
        modelNote->clear();
        modelShip->clear();
        modelChemSrc->clear();
        modelChem->clear();
        modelMechSrc->clear();
        modelMech->clear();
        modelMechx->clear();
    }
    setBlock(!ok);
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
    int id_part = (index.row()>=0) ? modelPart->getModelData(index.row(),"id").toInt() : -1;

    modelTu->setPath("api/elrtr/parti/tu/"+QString::number(id_part));
    modelTu->select();

    modelNote->setPath("api/elrtr/parti/note/"+QString::number(id_part));
    modelNote->select();

    modelShip->setPath("api/elrtr/parti/ship/"+QString::number(id_part));
    modelShip->select();

    modelChemSrc->setFilter(RestFilter::rule(modelChemSrc->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelChemSrc->setDefaultValue("id_part",id_part);
    modelChemSrc->select();

    modelChem->setFilter(RestFilter::rule(modelChem->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelChem->setDefaultValue("id_part",id_part);
    modelChem->select();

    modelMechSrc->setFilter(RestFilter::rule(modelMechSrc->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelMechSrc->setDefaultValue("id_part",id_part);
    modelMechSrc->select();

    modelMech->setFilter(RestFilter::rule(modelMech->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelMech->setDefaultValue("id_part",id_part);
    modelMech->select();

    modelMechx->setFilter(RestFilter::rule(modelMechx->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelMechx->setDefaultValue("id_part",id_part);
    modelMechx->select();
}

void FormPartEl::setOk(bool ok)
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPart->rowCount() || currentIndex < 0) {
        ui->checkBoxOk->setChecked(!ok);
        return;
    }

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
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPart->rowCount() || currentIndex<0){
        return;
    }

    int id_part = modelPart->getModelData(currentIndex,"id").toInt();

    DialogCopyValEl d(id_part);
    if (d.exec()==QDialog::Accepted){
        int id_part_src=d.currentIdPart();
        if (id_part_src>0){

            QByteArray data;

            QJsonDocument doc;
            QJsonObject object
                {
                    {"id_part_src", id_part_src}
                };
            doc.setObject(object);
            QByteArray body=doc.toJson();

            RestConnection::instance()->sendSyncRequest("api/elrtr/parti/importval/"+QString::number(id_part),"POST",body,data);
            modelChem->select();
            modelMech->select();
            modelMechx->select();
            updCurrentState();
        }
    }
}

void FormPartEl::showLabels()
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPart->rowCount() || currentIndex < 0) {
        return;
    }
    int id_part = modelPart->getModelData(currentIndex,"id").toInt();
    DialogLabels d;
    d.setLabel("api/elrtr/labels",id_part);
    d.exec();
}

void FormPartEl::showPartCertificate()
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPart->rowCount() || currentIndex < 0) {
        return;
    }
    int id_part = modelPart->getModelData(currentIndex,"id").toInt();
    certDocument->build(id_part,-1,"","elrtr");
    certEditor->show();
}

void FormPartEl::showShipCertificate(QModelIndex index)
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPart->rowCount() || currentIndex < 0) {
        return;
    }
    int id_part = modelPart->getModelData(currentIndex,"id").toInt();
    int id_ship=modelShip->getModelData(index.row(),"id").toInt();
    if (id_ship>0){
        certDocument->build(id_part,id_ship,"","elrtr");
        certEditor->show();
    }
}

void FormPartEl::createPasport()
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPart->rowCount() || currentIndex < 0) {
        return;
    }
    DialogAuthor d;
    if(d.exec()==QDialog::Accepted){
        int id_part = modelPart->getModelData(currentIndex,"id").toInt();
        int year = modelPart->getModelData(currentIndex,"dat_part").toDate().year();
        QString n_s = modelPart->getModelData(currentIndex,"n_s").toString();
        QString marka = modelPart->getModelData(currentIndex,"marka").toString();
        pasportManager->getPasport(id_part,n_s+"_"+marka,year,d.getJson());
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
            QMessageBox::critical(QApplication::activeWindow(),tr("Ошибка"),reply->errorString()+"\n"+data,QMessageBox::Cancel);
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
