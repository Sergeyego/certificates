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
    connect(ui->plainTextEditPrim,SIGNAL(textChanged()),this,SLOT(enPrimSave()));
    connect(ui->plainTextEditPrimProd,SIGNAL(textChanged()),this,SLOT(enPrimSave()));

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

void FormPartEl::enPrimSave()
{
    ui->pushButtonSave->setEnabled(true);
}

void FormPartEl::upd()
{
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
