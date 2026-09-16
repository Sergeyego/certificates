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

void FormPartEl::upd()
{

}
