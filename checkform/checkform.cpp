#include "checkform.h"
#include "ui_checkform.h"

CheckForm::CheckForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckForm)
{
    ui->setupUi(this);
    connect(ui->cmdGo, SIGNAL(clicked()), this, SLOT(go()));
}

CheckForm::~CheckForm()
{
    delete ui;
}

void CheckForm::setCheckEl(bool b)
{
    if (b){
        ui->radioButtonEl->setChecked(b);
    } else {
        ui->radioButtonWire->setChecked(true);
    }
}

void CheckForm::go()
{
    ui->textEdit->clear();
    ui->cmdGo->setEnabled(false); // Блокируем кнопку на время запроса

    QString code = ui->lineEditCod->text().trimmed();
    if (code.isEmpty()) {
        ui->textEdit->setText(tr("Введите код!"));
        ui->cmdGo->setEnabled(true);
        return;
    }

    QString type = ui->radioButtonEl->isChecked() ? "el" : "wire";
    
    QUrlQuery query;
    query.addQueryItem("type", type);

    QUrl url = RestConnection::instance()->buildUrl("api/checkcode/" + code, query);
    QNetworkReply *reply = RestConnection::instance()->sendGet(url);
    connect(reply, SIGNAL(finished()), this, SLOT(onCheckFinished()));
}

void CheckForm::onCheckFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    ui->cmdGo->setEnabled(true); // Разблокируем кнопку

    const auto err = reply->error();
    const QString errStr = reply->errorString();
    reply->deleteLater();

    if (err == QNetworkReply::OperationCanceledError) return;

    const QByteArray data = reply->readAll();

    if (err != QNetworkReply::NoError) {
        ui->textEdit->setText(tr("Ошибка сети: ") + errStr + "\n" + QString::fromUtf8(data));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject() && doc.object().contains("text")) {
        ui->textEdit->setText(doc.object().value("text").toString());
    } else {
        ui->textEdit->setText(tr("Некорректный ответ сервера"));
    }
}