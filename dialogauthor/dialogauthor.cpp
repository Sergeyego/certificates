#include "dialogauthor.h"
#include "ui_dialogauthor.h"

DialogAuthor::DialogAuthor(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogAuthor)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());

    modelAuthor = new RestRoTableModel(this);

    connect(modelAuthor,SIGNAL(sigRefresh()),this,SLOT(updFinished()));

    connect(ui->dateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(updInfo()));
    updInfo();
}

DialogAuthor::~DialogAuthor()
{
    delete ui;
}

QByteArray DialogAuthor::getJson()
{
    QJsonObject object;
    object.insert("authTitle",ui->lineEditAuthTitle->text());
    object.insert("auth",ui->lineEditAuth->text());
    object.insert("checkTitle",ui->lineEditCheckTitle->text());
    object.insert("check",ui->lineEditCheck->text());
    object.insert("appTitle",ui->lineEditAppTitle->text());
    object.insert("app",ui->lineEditApp->text());
    object.insert("norm",ui->lineEditNorm->text());
    object.insert("date",ui->dateEdit->date().toString("yyyy-MM-dd"));
    object.insert("isSign",ui->checkBoxSig->isChecked());
    QJsonDocument doc(object);
    return doc.toJson(QJsonDocument::Indented);
}

void DialogAuthor::updInfo()
{
    modelAuthor->setPath("api/elrtr/pasport/author/"+ui->dateEdit->date().toString("yyyy-MM-dd"));
    modelAuthor->select();
}

void DialogAuthor::updFinished()
{
    ui->lineEditAuthTitle->clear();
    ui->lineEditAuth->clear();
    ui->lineEditCheckTitle->clear();
    ui->lineEditCheck->clear();
    ui->lineEditAppTitle->clear();
    ui->lineEditApp->clear();
    ui->lineEditNorm->clear();

    for (int i=0; i<modelAuthor->rowCount();i++){
        int id_role=modelAuthor->getModelData(i,"id_role").toInt();
        QStringList el=modelAuthor->getModelData(i,"nam").toString().split("#");
        QString empl = (el.size())? el.at(0) : "";
        QString job = (el.size()>1)? el.at(1) : "";
        if (id_role==1){
            ui->lineEditAuthTitle->setText(job);
            ui->lineEditAuth->setText(empl);
        } else if (id_role==2){
            ui->lineEditCheckTitle->setText(job);
            ui->lineEditCheck->setText(empl);
        } else if (id_role==3){
            ui->lineEditAppTitle->setText(job);
            ui->lineEditApp->setText(empl);
        } else if (id_role==4){
            ui->lineEditNorm->setText(empl);
        }
    }
}
