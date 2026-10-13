#include "dialogcopyvalel.h"
#include "ui_dialogcopyvalel.h"

DialogCopyValEl::DialogCopyValEl(int id_part_src, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogCopyValEl)
{
    ui->setupUi(this);

    modelPart = new RestRoTableModel(this);
    modelPart->setPath("api/elrtr/parti/perepacklist/"+QString::number(id_part_src));

    ui->tableViewPart->setModel(modelPart);

    modelChem = new RestTableModel("el_sert_chem",this);
    modelChem->setPath("api/elrtr/lab/chem/sert");
    ui->tableViewChem->setModel(modelChem);

    modelMech = new RestTableModel("el_sert_mech",this);
    modelMech->setPath("api/elrtr/lab/mech/sert");
    ui->tableViewMech->setModel(modelMech);

    modelMechx = new RestTableModel("el_sert_mechx",this);
    ui->tableViewMechx->setModel(modelMechx);

    connect(modelPart,SIGNAL(sigRefresh()),this,SLOT(updFinished()));
    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updData(QModelIndex)));

    modelPart->select();
}

DialogCopyValEl::~DialogCopyValEl()
{
    delete ui;
}

int DialogCopyValEl::currentIdPart()
{
    int id_part=-1;
    int currentRow=ui->tableViewPart->currentIndex().row();
    if (currentRow>=0 && currentRow<modelPart->rowCount()){
        id_part = modelPart->getModelData(currentRow,"id").toInt();
    }
    return id_part;
}

void DialogCopyValEl::updFinished()
{
    if (modelPart->rowCount()){
        ui->tableViewPart->selectRow(0);
    }
}

void DialogCopyValEl::updData(QModelIndex index)
{
    int id_part = modelPart->getModelData(index.row(),"id").toInt();

    modelChem->setFilter(RestFilter::rule(modelChem->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelChem->setDefaultValue("id_part",id_part);
    modelChem->select();

    modelMech->setFilter(RestFilter::rule(modelMech->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelMech->setDefaultValue("id_part",id_part);
    modelMech->select();

    modelMechx->setFilter(RestFilter::rule(modelMechx->tableName(),"id_part",RestFilter::Op::Eq,id_part));
    modelMechx->setDefaultValue("id_part",id_part);
    modelMechx->select();
}
