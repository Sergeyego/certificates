#include "pasportmanager.h"
#include <QRegularExpression>
#include <QApplication>
#include <QMessageBox>
#include "rest/resterror.h"

PasportManager::PasportManager(QObject *parent)
    : QObject{parent}
{

}

void PasportManager::getPasport(int id_part, QString filename, int year, const QByteArray &json)
{
    QUrl url = RestConnection::instance()->buildUrl("api/elrtr/pasport/create/" + QString::number(id_part));
    QNetworkReply *reply = RestConnection::instance()->sendRequest(url, "POST", json);

    reply->setProperty("filename", filename);
    reply->setProperty("year", year);

    connect(reply, SIGNAL(finished()), this, SLOT(onResult()));
}

void PasportManager::onResult()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    if (reply->error()) {
        RestError::critical(tr("Ошибка"),reply->errorString() + "\n" + reply->readAll());
        reply->deleteLater();
        return;
    }

    QString filename = reply->property("filename").toString();
    const int y = reply->property("year").toInt();
    const QString year = (y > 0) ? QString::number(y)
                                 : QString::number(QDate::currentDate().year());

    if (filename.isEmpty()) {
        filename = QStringLiteral("temp.pdf");
    }

    QDir dir(QDir::homePath() + QStringLiteral("/Паспорта"));
    if (!dir.exists()) dir.mkpath(dir.path());

    if (!dir.cd(year)) {
        dir.mkdir(year);
        dir.cd(year);
    }

    filename.replace(QRegularExpression("[^\\w]", QRegularExpression::UseUnicodePropertiesOption), "_");
    const QString totalName = dir.path() + "/" + filename + ".pdf";

    QFile file(totalName);
    if (!file.open(QIODevice::WriteOnly)) {
        RestError::critical(tr("Ошибка"), tr("Не удалось сохранить файл: ") + totalName);
        reply->deleteLater();
        return;
    }

    const QByteArray data = reply->readAll();
    if (file.write(data) != data.size()) {
        RestError::critical(tr("Ошибка"), tr("Файл записан не полностью: ") + totalName);
        file.close();
        reply->deleteLater();
        return;
    }
    file.close();

    QDesktopServices::openUrl(QUrl::fromLocalFile(totalName));
    reply->deleteLater();
}