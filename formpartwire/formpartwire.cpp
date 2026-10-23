#include "formpartwire.h"
#include "ui_formpartwire.h"

FormPartWire::FormPartWire(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormPartWire)
{
    ui->setupUi(this);
    loadSettings();

    saved_id_part=-1;

    certDocument = new CertDocument(this);
    certEditor = new CertEditor(certDocument,this);
    certEditor->setWindowFlags(Qt::Window);

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(ui->dateEditBeg->date().addYears(1));

    ui->comboBoxMarka->setModel(RelModels::instance()->getModel("provol"));
    ui->comboBoxDiam->setModel(RelModels::instance()->getModel("wire_diam"));
    colVal empVal;
    empVal.val=-1;
    ui->comboBoxMarka->setCurrentData(empVal);
    ui->comboBoxDiam->setCurrentData(empVal);

    modelTuSrc = new RestRoTableModel(this);
    ui->tableViewSrcTu->setModel(modelTuSrc);

    modelTu = new RestTableModel("wire_parti_gost",this);
    ui->tableViewTu->setModel(modelTu);

    modelShip = new RestRoTableModel(this);
    ui->tableViewShip->setModel(modelShip);

    modelNote = new RestRoTableModel(this);

    modelSrcChem = new RestTableModel("prov_chem",this);
    modelSrcChem->setPath("api/wire/parti/chem/buth");
    ui->tableViewSrcChem->setModel(modelSrcChem);

    modelChem = new RestTableModel("wire_parti_chem",this);
    modelChem->setPath("api/wire/parti/chem/part");
    ui->tableViewChem->setModel(modelChem);

    modelSrcMech = new RestTableModel("wire_parti_mech",this);
    ui->tableViewSrcMech->setModel(modelSrcMech);

    modelMech = new ModelWirePartiMech(this);
    ui->tableViewMech->setModel(modelMech);

    modelMechx = new RestTableModel("wire_sert_mechx",this);
    ui->tableViewMechx->setModel(modelMechx);

    modelPartWire = new ModelPartWire(this);
    ui->tableViewPart->setModel(modelPartWire);

    connect(modelPartWire,SIGNAL(sigRefresh()),this,SLOT(updFinished()));
    connect(modelNote,SIGNAL(sigRefresh()),this,SLOT(updNoteFinished()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(currentIndexChanged(QModelIndex)));
    connect(ui->checkBoxOnly,SIGNAL(clicked(bool)),ui->comboBoxMarka,SLOT(setEnabled(bool)));
    connect(ui->checkBoxOnly,SIGNAL(clicked(bool)),ui->comboBoxDiam,SLOT(setEnabled(bool)));
    connect(ui->comboBoxMarka,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->comboBoxDiam,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->checkBoxOnly,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->toolButtonCopyTu,SIGNAL(clicked(bool)),this,SLOT(copyTu()));
    connect(ui->tableViewShip,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(showShipCertificate(QModelIndex)));
    connect(ui->pushButtonCert,SIGNAL(clicked(bool)),this,SLOT(showPartCertificate()));
    connect(ui->pushButtonSavePrim,SIGNAL(clicked(bool)),this,SLOT(savePrim()));
    connect(ui->lineEditPrimProd,SIGNAL(textChanged(QString)),this,SLOT(enPrimSave()));
    connect(ui->toolButtonCopyChem,SIGNAL(clicked(bool)),this,SLOT(copyChem()));
    connect(modelChem,SIGNAL(sigUpd()),this,SLOT(refreshState()));
    connect(modelMech,SIGNAL(sigUpd()),this,SLOT(refreshState()));
    upd();
}

FormPartWire::~FormPartWire()
{
    saveSettings();
    delete ui;
}

void FormPartWire::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->ui->splitter->restoreState(settings.value("wire_parti_splitter_width").toByteArray());
}

void FormPartWire::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("wire_parti_splitter_width",ui->splitter->saveState());
}

void FormPartWire::setLock(bool b)
{
    ui->tableViewTu->setEnabled(!b);
    ui->toolButtonCopyTu->setEnabled(!b);
    ui->lineEditPrimProd->setEnabled(!b);
    ui->tableViewSrcChem->setEnabled(!b);
    ui->tableViewChem->setEnabled(!b);
    ui->toolButtonCopyChem->setEnabled(!b);
    ui->tableViewSrcMech->setEnabled(!b);
    ui->tableViewMech->setEnabled(!b);
    ui->tableViewMechx->setEnabled(!b);
    ui->pushButtonCert->setEnabled(!b);
}

void FormPartWire::refreshState()
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPartWire->rowCount() || currentIndex < 0) {
        return;
    }
    int id_m = modelPartWire->getModelData(currentIndex,"id_m").toInt();
    modelPartWire->refreshState(id_m);
}

void FormPartWire::upd()
{
    if (sender()==ui->pushButtonUpd){
        QVector<RestTableModel*> arrMod;
        arrMod.push_back(modelTu);
        arrMod.push_back(modelSrcChem);
        arrMod.push_back(modelSrcMech);
        arrMod.push_back(modelChem);
        arrMod.push_back(modelMech);
        arrMod.push_back(modelMechx);
        RelModels::instance()->updateRels(arrMod);
        RelModels::instance()->getModel("provol")->refresh();
        RelModels::instance()->getModel("wire_diam")->refresh();
    }

    bool b = ui->checkBoxOnly->isChecked();
    if (b && sender()==ui->checkBoxOnly && ui->tableViewPart->model()->rowCount()){
        int currRow = ui->tableViewPart->currentIndex().row();
        if (currRow >= 0) {
            colVal d;
            d.val = modelPartWire->getModelData(currRow, "id_prov");
            ui->comboBoxMarka->setCurrentData(d);
        }
    }

    // id_pr: -1 если фильтр не нужен или ничего не выбрано.
    // Если выбран элемент (включая ключ 0), currentIndex() будет >= 0.
    int id_pr = -1;
    if (b && ui->comboBoxMarka->currentIndex() >= 0) {
        id_pr = ui->comboBoxMarka->getCurrentData().val.toInt();
    }

    // id_diam: аналогично.
    int id_diam = -1;
    if (ui->comboBoxDiam->currentIndex() >= 0) {
        id_diam = ui->comboBoxDiam->getCurrentData().val.toInt();
    }

    modelPartWire->refresh(ui->dateEditBeg->date(), ui->dateEditEnd->date(), id_pr, id_diam);
}

void FormPartWire::updFinished()
{
    bool ok = ui->tableViewPart->model()->rowCount();
    if (ok){
        if (saved_id_part>=0){
            int row=modelPartWire->findRowByKey(saved_id_part,"id");
            if (row<0) {
                row=ui->tableViewPart->model()->rowCount()-1;
            }
            ui->tableViewPart->selectRow(row);
            ui->tableViewPart->scrollTo(ui->tableViewPart->model()->index(row,0));
        } else {
            ui->tableViewPart->selectRow(ui->tableViewPart->model()->rowCount()-1);
            ui->tableViewPart->scrollToBottom();
        }
    } else {
        ui->lineEditSrcPart->clear();
        ui->lineEditPodt->clear();
        ui->lineEditVol->clear();
        ui->lineEditLine->clear();
        ui->lineEditKvo->clear();
        ui->lineEditComment->clear();
        modelTuSrc->clear();
        modelTu->clear();
        modelNote->clear();
        modelShip->clear();
        modelSrcChem->clear();
        modelChem->clear();
        modelSrcMech->clear();
        modelMech->clear();
        modelMechx->clear();
    }
    setLock(!ok);
}

void FormPartWire::updNoteFinished()
{
    if (modelNote->rowCount()){
        ui->lineEditPrimProd->setText(modelNote->getModelData(0,"prim_prod").toString());
    } else {
        ui->lineEditPrimProd->clear();
    }
    ui->pushButtonSavePrim->setEnabled(false);
}

void FormPartWire::currentIndexChanged(QModelIndex index)
{
    if (index.row() < 0) {
        ui->lineEditSrcPart->clear();
        ui->lineEditPodt->clear();
        ui->lineEditVol->clear();
        ui->lineEditLine->clear();
        ui->lineEditKvo->clear();
        ui->lineEditComment->clear();

        modelTuSrc->clear();
        modelTu->clear();
        modelShip->clear();
        modelNote->clear();
        modelSrcChem->clear();
        modelChem->clear();
        modelSrcMech->clear();
        modelMech->clear();
        modelMechx->clear();

        saved_id_part = -1;
        return;
    }

    ui->lineEditSrcPart->setText(modelPartWire->getModelData(index.row(),"buht").toString());
    ui->lineEditPodt->setText(modelPartWire->getModelData(index.row(),"podt").toString());
    ui->lineEditVol->setText(modelPartWire->getModelData(index.row(),"rab").toString());
    ui->lineEditLine->setText(modelPartWire->getModelData(index.row(),"line").toString());
    ui->lineEditKvo->setText(QLocale().toString(modelPartWire->getModelData(index.row(),"kvo").toDouble(),'f',1));
    ui->lineEditComment->setText(modelPartWire->getModelData(index.row(),"note").toString());

    int id_part = modelPartWire->getModelData(index.row(),"id").toInt();
    int id_prov = modelPartWire->getModelData(index.row(),"id_prov").toInt();
    int id_m = modelPartWire->getModelData(index.row(),"id_m").toInt();
    int id_buht = modelPartWire->getModelData(index.row(),"id_buht").toInt();

    saved_id_part=id_part;

    modelTuSrc->setPath("api/wire/parti/tusrc/"+QString::number(id_prov));
    modelTuSrc->select();

    modelTu->setFilter(RestFilter::rule(modelTu->tableName(),"id_parti",RestFilter::Op::Eq,id_m));
    modelTu->setDefaultValue("id_parti",id_m);
    modelTu->select();

    modelShip->setPath("api/wire/parti/ship/"+QString::number(id_part));
    modelShip->select();

    modelNote->setPath("api/wire/parti/note/"+QString::number(id_part));
    modelNote->select();

    modelSrcChem->setFilter(RestFilter::rule(modelSrcChem->tableName(),"id_buht",RestFilter::Op::Eq,id_buht));
    modelSrcChem->setDefaultValue("id_buht",id_buht);
    modelSrcChem->select();

    modelChem->setFilter(RestFilter::rule(modelChem->tableName(),"id_part",RestFilter::Op::Eq,id_m));
    modelChem->setDefaultValue("id_part",id_m);
    modelChem->select();

    modelSrcMech->setFilter(RestFilter::rule(modelSrcMech->tableName(),"id_part",RestFilter::Op::Eq,id_m));
    modelSrcMech->setDefaultValue("id_part",id_m);
    modelSrcMech->select();

    modelMechx->setFilter(RestFilter::rule(modelMechx->tableName(),"id_part",RestFilter::Op::Eq,id_m));
    modelMechx->setDefaultValue("id_part",id_m);
    modelMechx->select();

    modelMech->refresh(id_m);
}

void FormPartWire::copyTu()
{
    int row=ui->tableViewPart->currentIndex().row();
    if (row<0) return;
    int id_m = modelPartWire->getModelData(row,"id_m").toInt();
    QByteArray data;
    bool ok = RestConnection::instance()->sendSyncRequest("api/wire/parti/copytu/"+QString::number(id_m),"POST","{}",data);
    if (ok){
        modelTu->select();
    }
}

void FormPartWire::showShipCertificate(QModelIndex index)
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPartWire->rowCount() || currentIndex < 0) {
        return;
    }
    int id_part = modelPartWire->getModelData(currentIndex,"id").toInt();
    int id_ship=modelShip->getModelData(index.row(),"id").toInt();
    if (id_ship>0){
        certDocument->build(id_part,id_ship,"wire");
        certEditor->showNormal();
        certEditor->show();
        certEditor->raise();
        certEditor->activateWindow();
    }
}

void FormPartWire::showPartCertificate()
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPartWire->rowCount() || currentIndex < 0) {
        return;
    }
    int id_part = modelPartWire->getModelData(currentIndex,"id").toInt();
    certDocument->build(id_part,-1,"wire");
    certEditor->showNormal();
    certEditor->show();
    certEditor->raise();
    certEditor->activateWindow();
}

void FormPartWire::savePrim()
{
    int currentIndex = ui->tableViewPart->currentIndex().row();
    if (!modelPartWire->rowCount() || currentIndex < 0) {
        ui->pushButtonSavePrim->setEnabled(false);
        return;
    }

    int id_part = modelPartWire->getModelData(currentIndex,"id").toInt();

    QByteArray data;

    QJsonDocument doc;
    QJsonObject object;
    object.insert("prim_prod",ui->lineEditPrimProd->text());
    doc.setObject(object);
    QByteArray body=doc.toJson();

    bool res = RestConnection::instance()->sendSyncRequest("api/wire/parti/patch/"+QString::number(id_part),"PATCH",body,data);
    if (res) {
        ui->pushButtonSavePrim->setEnabled(false);
    }
}

void FormPartWire::enPrimSave()
{
    ui->pushButtonSavePrim->setEnabled(true);
}

void FormPartWire::copyChem()
{
    if (!modelSrcChem->rowCount()) return;
    int row=ui->tableViewPart->currentIndex().row();
    if (row<0) return;
    int id_m=modelPartWire->getModelData(row,"id_m").toInt();
    if (!modelChem->isEmpty()){
        QMessageBox::information(this,tr("Предупреждение"),tr("Сначала удалите все уже существующие элементы!"),QMessageBox::Ok);
        return;
    }
    int n=QMessageBox::question(this,tr("Подтвердите действия"),tr("Скопировать значения?"),QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
        QByteArray data;
        bool ok = RestConnection::instance()->sendSyncRequest("api/wire/parti/copychem/"+QString::number(id_m),"POST","{}",data);
        if (ok){
            this->refreshState();
            modelChem->select();
        }
    }
}

void FormPartWire::findPart(int id_part, QString prefix)
{
    if (prefix!="wire"){
        return;
    }
    int row = modelPartWire->findRowByKey(id_part,"id");
    if (row>=0){
        ui->tableViewPart->selectRow(row);
    } else {
        QMessageBox::information(this,tr("Информация"),tr("Не найдено такой партии за указанный период"),QMessageBox::Ok);
    }
}



ModelPartWire::ModelPartWire(QWidget *parent) : RestRoTableModel(parent)
{

}

void ModelPartWire::refresh(QDate beg, QDate end, int id_provol, int id_diam)
{
    QUrlQuery query;
    query.addQueryItem("id_prov",QString::number(id_provol));
    query.addQueryItem("id_diam",QString::number(id_diam));
    setPath("api/wire/parti/list/"+beg.toString("yyyy-MM-dd")+"/"+end.toString("yyyy-MM-dd")+"?"+query.toString());
    this->select();
}


void ModelPartWire::refreshState(int id_m)
{
    QUrl url = QUrl(RestConnection::instance()->getUrl()+"/api/wire/parti/color/"+QString::number(id_m));

    QNetworkReply *reply = RestConnection::instance()->sendGet(url);
    connect(reply,SIGNAL(finished()),this,SLOT(refreshStateFinished()));
}

void ModelPartWire::refreshStateFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    const auto err = reply->error();
    const QString errStr = reply->errorString();
    reply->deleteLater();

    if (err == QNetworkReply::OperationCanceledError) return;

    const QByteArray data = reply->readAll(); // readAll только после проверки отмены
    if (err != QNetworkReply::NoError) {
        RestError::critical(tr("Ошибка"), errStr + "\n" + data);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QColor color = QColor(doc.object().value("color").toString());
    int id_m = doc.object().value("id").toInt();

    for (int i=0; i<rowCount(); i++){
        if (getModelData(i,"id_m").toInt()==id_m){
            for (int j = 0; j < this->columnCount(); ++j) {
                modelData[i][j].background = color;
            }
            emit dataChanged(this->index(i, 0), this->index(i, columnCount() - 1));
        }
    }
}

ModelWirePartiMech::ModelWirePartiMech(QObject *parent):RestTableModel("wire_sert_mech",parent)
{
    modelCats = new RestRoTableModel(this);
    modelCats->setPath("api/wire/parti/mechcats/get");

    connect(modelCats,SIGNAL(sigRefresh()),this,SLOT(refreshCatsFinished()));

    modelCats->select();
}

void ModelWirePartiMech::refresh(int id_part)
{
    setFilter(RestFilter::rule(tableName(),"id_part",RestFilter::Op::Eq,id_part));
    setDefaultValue("id_part",id_part);
    select();
}

bool ModelWirePartiMech::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ((index.column()==columnIndex("id_mech")) && role==Qt::EditRole){
        int id_mech=value.toInt();
        int id_cat=mapCat.value(id_mech,2);
        int columnCat = columnIndex("id_cat");
        if (columnCat>=0 && isColumnRel(columnCat)){
            RestRelModel *relModel = RelModels::instance()->getModel(columnInfo(columnCat).relnam);
            int row = relModel->findRowByKey(id_cat);
            if (row>=0){
                colVal cat = relModel->getModelData(row);
                RestTableModel::setData(this->index(index.row(),columnCat),cat.val,Qt::EditRole);
                RestTableModel::setData(this->index(index.row(),columnCat),cat.disp,Qt::DisplayRole);
            }
        }
    }
    return RestTableModel::setData(index,value,role);
}

void ModelWirePartiMech::refreshCats()
{
    modelCats->select();
}

void ModelWirePartiMech::refreshCatsFinished()
{
    mapCat.clear();
    for (int i=0; i<modelCats->rowCount();i++){
        mapCat.insert(modelCats->getModelData(i,"id").toInt(),modelCats->getModelData(i,"id_wire_cat").toInt());
    }
}
