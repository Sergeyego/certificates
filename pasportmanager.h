#ifndef PASPORTMANAGER_H
#define PASPORTMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDir>
#include <QDesktopServices>
#include "rest/restconnection.h"

class PasportManager : public QObject
{
    Q_OBJECT
public:
    explicit PasportManager(QObject *parent = nullptr);
    void getPasport(int id_part, QString filename, int year, const QByteArray &json);

private slots:
    void onResult();

signals:
};

#endif // PASPORTMANAGER_H
