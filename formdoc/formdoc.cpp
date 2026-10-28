#include "formdoc.h"
#include "ui_formdoc.h"

FormDoc::FormDoc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDoc)
{
    ui->setupUi(this);

    loadsettings();

    RelModels::instance()->getModel("gost_new")->setEditable(true);
    RelModels::instance()->getModel("zvd_grade")->setEditable(true);

    modelEl = new RestTableModel("zvd_els",this);
    ui->tableViewEl->setModel(modelEl);

    modelElDim = new RestTableModel("zvd_eldim",this);
    ui->tableViewElDim->setModel(modelElDim);

    modelTu = new RestTableModel("zvd_sert_tu",this);
    ui->tableViewTu->setModel(modelTu);


    modelEn = new ModelEn(this);
    ui->listViewEn->setModel(modelEn);

    modelWireDiam = new RestTableModel("zvd_wire_diam_sert",this);
    ui->tableViewWireDiam->setModel(modelWireDiam);

    modelWire = new RestTableModel("zvd_wire_sert",this);
    ui->tableViewWire->setModel(modelWire);

    modelDoc = new ModelDoc(this);
    ui->tableViewDoc->setModel(modelDoc);

    mapper = new RestMapper(ui->tableViewDoc,this);
    mapper->addMapping(ui->lineEditDoc,"nom_doc");
    mapper->addMapping(ui->lineEditNam,"nazv");
    mapper->addMapping(ui->dateEditDoc,"dat_doc");
    mapper->addMapping(ui->dateEditBeg,"dat_beg");
    mapper->addMapping(ui->dateEditEnd,"dat_end");
    mapper->addMapping(ui->lineEditNumBl,"nom_bl");
    mapper->addMapping(ui->lineEditNumNak,"nom_sl");
    mapper->addMapping(ui->lineEditGtu,"gr_tech_ust");
    mapper->addMapping(ui->comboBoxVed,"id_ved");
    mapper->addMapping(ui->comboBoxVid,"id_doc");

    mapper->addEmptyLock(ui->tableViewWireDiam);
    mapper->addEmptyLock(ui->tableViewEl);
    mapper->addEmptyLock(ui->tableViewElDim);
    mapper->addEmptyLock(ui->pushButtonUpload);
    mapper->addEmptyLock(ui->tableViewWire);
    mapper->addEmptyLock(ui->tableViewTu);
    mapper->addEmptyLock(ui->listViewEn);
    mapper->addEmptyLock(ui->toolButtonCheckAll);
    mapper->addEmptyLock(ui->toolButtonUnCheckAll);
    mapper->addLock(ui->pushButtonSite);
    ui->horizontalLayoutMap->insertWidget(0,mapper);

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshData(int)));
    connect(ui->checkBoxActive,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->cmdUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));

    connect(ui->pushButtonUpload,SIGNAL(clicked(bool)),this,SLOT(upload()));
    connect(ui->pushButtonView,SIGNAL(clicked(bool)),this,SLOT(viewSert()));
    connect(ui->pushButtonSaveAs,SIGNAL(clicked(bool)),this,SLOT(saveAs()));
    connect(ui->pushButtonDel,SIGNAL(clicked(bool)),this,SLOT(delSert()));
    connect(modelDoc,SIGNAL(sigList()),this,SLOT(updState()));
    connect(ui->toolButtonCheckAll,SIGNAL(clicked(bool)),modelEn,SLOT(checkAll()));
    connect(ui->toolButtonUnCheckAll,SIGNAL(clicked(bool)),modelEn,SLOT(unCheckAll()));
    connect(ui->pushButtonSite,SIGNAL(clicked(bool)),this,SIGNAL(reqUpdSite()));

    upd();
}

FormDoc::~FormDoc()
{
    savesettings();
    delete ui;
}

void FormDoc::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("doc_splitter").toByteArray());
}

void FormDoc::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("doc_splitter", ui->splitter->saveState());
}

void FormDoc::setOk(bool ok)
{
    ui->pushButtonSaveAs->setEnabled(ok);
    ui->pushButtonView->setEnabled(ok);
    ui->pushButtonDel->setEnabled(ok);
    QString pixname= ok ? QString(":/images/ok.png") : QString(":/images/notok.png");
    ui->labelSert->setPixmap(QPixmap(pixname));
}

void FormDoc::refreshData(int index)
{
    if (index<0){
        modelEl->clear();
        modelElDim->clear();
        modelWire->clear();
        modelWireDiam->clear();
        modelTu->clear();
        modelEn->clear();
        setOk(false);
        return;
    }

    int id=mapper->modelData(index,"id").toInt();

    modelEl->setDefaultValue("id_sert",id);
    modelEl->setFilter(RestFilter::rule(modelEl->tableName(),"id_sert",RestFilter::Op::Eq,id));
    modelEl->select();

    modelElDim->setDefaultValue("id_sert",id);
    modelElDim->setFilter(RestFilter::rule(modelElDim->tableName(),"id_sert",RestFilter::Op::Eq,id));
    modelElDim->select();

    modelWireDiam->setDefaultValue("id_sert",id);
    modelWireDiam->setFilter(RestFilter::rule(modelWireDiam->tableName(),"id_sert",RestFilter::Op::Eq,id));
    modelWireDiam->select();

    modelWire->setDefaultValue("id_sert",id);
    modelWire->setFilter(RestFilter::rule(modelWire->tableName(),"id_sert",RestFilter::Op::Eq,id));
    modelWire->select();

    modelTu->setDefaultValue("id_sert",id);
    modelTu->setFilter(RestFilter::rule(modelTu->tableName(),"id_sert",RestFilter::Op::Eq,id));
    modelTu->select();

    modelEn->refresh(id);

    updState();
}

void FormDoc::upd()
{
    modelDoc->refresh(ui->checkBoxActive->isChecked());
}

void FormDoc::upload()
{
    modelDoc->upload(mapper->currentIndex());
}

void FormDoc::viewSert()
{
    modelDoc->viewDoc(mapper->currentIndex());
}

void FormDoc::saveAs()
{
    modelDoc->saveAs(mapper->currentIndex());
}

void FormDoc::updState()
{
    setOk(modelDoc->fileExist(mapper->currentIndex()));
}

void FormDoc::delSert()
{
    modelDoc->clearFile(mapper->currentIndex());
}

ModelEn::ModelEn(QWidget *parent) : RestRoTableModel(parent)
{
    id_d=-1;
    connect(this,SIGNAL(sigRefresh()),this,SLOT(refreshFinished()));
}

Qt::ItemFlags ModelEn::flags(const QModelIndex &index) const
{
    return index.column()==0 ? (Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled) : (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void ModelEn::refresh(int id_doc)
{
    this->setPath("api/docs/variants/get/"+QString::number(id_doc));
    this->select();
    id_d=id_doc;
}

QVariant ModelEn::data(const QModelIndex &item, int role) const
{
    if (!item.isValid()){
        return QVariant();
    }
    if (role==Qt::CheckStateRole && item.column()==0){
        return disList.contains(this->getModelData(item.row(),"id").toInt()) ? Qt::Unchecked : Qt::Checked;
    }
    return RestRoTableModel::data(item,role);
}

bool ModelEn::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role==Qt::CheckStateRole && index.column()==0){
        int id_var=this->getModelData(index.row(),"id").toInt();
        QByteArray data;
        QJsonDocument doc;
        QJsonObject object
            {
                {"id", id_var},
                {"value", value.toBool()},
            };
        doc.setObject(object);
        QByteArray body=doc.toJson();
        bool ok = RestConnection::instance()->sendSyncRequest("api/docs/variants/patch/"+QString::number(id_d),"PATCH",body,data);
        if (ok) {
            if (value.toBool()){
                if (disList.contains(id_var)){
                    disList.remove(id_var);
                    emit dataChanged(index,index);
                }
            } else {
                if (!disList.contains(id_var)){
                    disList.insert(id_var);
                    emit dataChanged(index,index);
                }
            }
        }
        return ok;
    }
    return RestRoTableModel::setData(index,value,role);
}

void ModelEn::refreshFinished()
{
    disList.clear();
    for (int i=0; i<rowCount();i++){
        if (!this->getModelData(i,"stat").toBool()){
            disList.insert(this->getModelData(i,"id").toInt());
        }
    }
    if (rowCount() > 0) {
        emit dataChanged(index(0, 0), index(rowCount() - 1, 0), {Qt::CheckStateRole});
    }
}

void ModelEn::checkAll()
{
    QByteArray data;
    bool ok = RestConnection::instance()->sendSyncRequest("api/docs/variants/delete/"+QString::number(id_d),"DELETE","{}",data);
    if (ok){
        refresh(id_d);
    }
}

void ModelEn::unCheckAll()
{
    QByteArray data;
    bool ok = RestConnection::instance()->sendSyncRequest("api/docs/variants/post/"+QString::number(id_d),"POST","{}",data);
    if (ok){
        refresh(id_d);
    }
}
