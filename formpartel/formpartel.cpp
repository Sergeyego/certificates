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

    modelPart = new ModelPart(this);
    ui->tableViewPart->setModel(modelPart);

    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->checkBoxMarkOnly,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->comboBoxMark,SIGNAL(currentIndexChanged(int)),this,SLOT(upd()));
    connect(ui->lineEditDiam,SIGNAL(textChanged(QString)),this,SLOT(upd()));
    connect(ui->checkBoxMarkOnly,SIGNAL(clicked(bool)),ui->comboBoxMark,SLOT(setEnabled(bool)));
    connect(ui->checkBoxMarkOnly,SIGNAL(clicked(bool)),ui->lineEditDiam,SLOT(setEnabled(bool)));
    connect(modelPart,SIGNAL(sigRefresh()),this,SLOT(updFinished()));

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
