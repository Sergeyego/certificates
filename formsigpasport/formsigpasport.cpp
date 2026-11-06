#include "formsigpasport.h"
#include "ui_formsigpasport.h"

FormSigPasport::FormSigPasport(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FormSigPasport)
{
    ui->setupUi(this);
    modelSig = new RestTableModel("pasport_sig",this);
    modelSig->setDefaultValue("dat",QDate::currentDate());
    modelSig->select();
    ui->tableView->setModel(modelSig);
}

FormSigPasport::~FormSigPasport()
{
    delete ui;
}
