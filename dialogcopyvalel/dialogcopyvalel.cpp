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

    connect(modelPart,SIGNAL(sigRefresh()),this,SLOT(updFinished()));
    modelPart->select();
}

DialogCopyValEl::~DialogCopyValEl()
{
    delete ui;
}

void DialogCopyValEl::updFinished()
{
    if (modelPart->rowCount()){
        ui->tableViewPart->selectRow(0);
    }
}
