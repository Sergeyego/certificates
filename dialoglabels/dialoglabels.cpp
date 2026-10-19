#include "dialoglabels.h"
#include "ui_dialoglabels.h"
#include <QDir>
#include <QFileInfo>

DialogLabels::DialogLabels(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogLabels)
{
    ui->setupUi(this);
    loadSettings();

    ui->labelView->setMinimumSize(100, 100);

    connect(ui->pushButtonClose,SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->comboBoxLabels,SIGNAL(currentIndexChanged(int)),this,SLOT(updLabel(int)));
    connect(ui->pushButtonSaveAs,SIGNAL(clicked(bool)),this,SLOT(saveLabel()));
    connect(ui->pushButtonPrint,SIGNAL(clicked(bool)),this,SLOT(printLabel()));
}

DialogLabels::~DialogLabels()
{
    saveSettings();
    delete ui;
}

void DialogLabels::setLabel(const QString &basePath, int id_part)
{
    _basePath=basePath;
    _id_part=id_part;

    QNetworkReply *reply = RestConnection::instance()->sendGet(QUrl(RestConnection::instance()->getUrl()+"/"+_basePath+"/list"));
    connect(reply,SIGNAL(finished()),this,SLOT(onListResult()));
}

void DialogLabels::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event); // Вызываем базовый обработчик

    if (!currentPixmap.isNull()) {
        ui->labelView->setPixmap(currentPixmap.scaled(ui->labelView->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void DialogLabels::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->restoreGeometry(settings.value("dialoglabels_geometry").toByteArray());
}

void DialogLabels::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("dialoglabels_geometry", this->saveGeometry());
}

void DialogLabels::onListResult()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    const QByteArray data = reply->readAll();
    const QString errStr  = reply->errorString();
    const auto err        = reply->error();
    reply->deleteLater();

    if (err != QNetworkReply::NoError) {
        RestError::critical(tr("Ошибка"), errStr + "\n" + data);
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        RestError::critical(tr("Ошибка"), tr("Некорректный ответ сервера."));
        return;
    }

    const QJsonArray arr = doc.array();

    ui->comboBoxLabels->clear();

    for (const QJsonValue &val : arr){
        ui->comboBoxLabels->addItem(val.toObject().value("name").toString(),val.toObject().value("id").toInt());
    }
}

void DialogLabels::onLabelResult()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    const QByteArray data = reply->readAll();
    const QString errStr  = reply->errorString();
    const auto err        = reply->error();
    reply->deleteLater();

    if (err != QNetworkReply::NoError) {
        RestError::critical(tr("Ошибка"), errStr + "\n" + data);
        return;
    }

    if (reply->property("id_lbl").toInt() != ui->comboBoxLabels->currentData().toInt()) {
        return;   // ответ устарел, выбран другой
    }

    currentPixmap.loadFromData(data);

    if (currentPixmap.isNull()) {
        ui->labelView->clear();
    } else {
        // Отображаем картинку, смасштабированную под текущий размер labelView
        ui->labelView->setPixmap(currentPixmap.scaled(ui->labelView->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void DialogLabels::updLabel(int index)
{
    int id_lbl = ui->comboBoxLabels->itemData(index).toInt();
    if (id_lbl <= 0 || _id_part<=0) {
        currentPixmap = QPixmap();
        ui->labelView->clear();
        return;
    }

    QUrlQuery query;
    query.addQueryItem("dpi", "600");
    query.addQueryItem("checkOk", "false");
    QUrl url = RestConnection::instance()->buildUrl(_basePath + "/get/" + QString::number(id_lbl) + "/" + QString::number(_id_part), query);
    QNetworkReply *reply = RestConnection::instance()->sendGet(url);
    reply->setProperty("id_lbl", id_lbl);
    connect(reply, SIGNAL(finished()), this, SLOT(onLabelResult()));
}

void DialogLabels::saveLabel()
{

    if (currentPixmap.isNull()) {
        QMessageBox::warning(this, tr("Внимание"), tr("Нет изображения для сохранения."));
        return;
    }

    QSettings settings("szsm", QApplication::applicationName());

    // Получаем прошлый путь, если его нет — по умолчанию берем системную папку Документов
    QString lastPath = settings.value("last_label_save_path", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();

    QString defaultName = ui->comboBoxLabels->currentText().trimmed().replace(" ", "_") + ".png";

    QString initialPath = QDir(lastPath).filePath(defaultName);

    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Сохранить этикетку"),
        initialPath,
        tr("Изображения (*.png);;Все файлы (*.*)")
        );

    if (filePath.isEmpty()) return; // Пользователь отменил сохранение

    if (currentPixmap.save(filePath, "PNG")) {
        QFileInfo fileInfo(filePath);
        settings.setValue("last_label_save_path", fileInfo.absolutePath());
        //QMessageBox::information(this, tr("Успех"), tr("Этикетка успешно сохранена."));
    } else {
        RestError::critical(tr("Ошибка"), tr("Не удалось сохранить файл."));
    }
}

void DialogLabels::printLabel()
{
    if (currentPixmap.isNull()) {
        QMessageBox::warning(this, tr("Внимание"), tr("Нет изображения для печати."));
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setDocName(tr("Печать этикетки: ") + ui->comboBoxLabels->currentText());

    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() != QDialog::Accepted) {
        return;
    }

    QPainter painter(&printer);
    if (!painter.isActive()) {
        RestError::critical(tr("Ошибка"), tr("Не удалось инициализировать печать."));
        return;
    }

    // Универсальное получение печатной области в пикселях для Qt5 и Qt6
    QRect pageRect = printer.pageLayout().paintRectPixels(printer.resolution());

    QPixmap scaledPixmap = currentPixmap.scaled(
        pageRect.size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    int x = pageRect.left() + (pageRect.width() - scaledPixmap.width()) / 2;
    int y = pageRect.top() + (pageRect.height() - scaledPixmap.height()) / 2;

    painter.drawPixmap(x, y, scaledPixmap);
    painter.end();
}
