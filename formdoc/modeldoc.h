#ifndef MODELDOC_H
#define MODELDOC_H

#include <QObject>
#include "rest/resttablemodel.h"
#include "rest/tempfilemanager.h"
#include <QNetworkReply>
#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QSettings>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


struct docInfo {
    QString id;
    QString file;
};

class ModelDoc : public RestTableModel
{
    Q_OBJECT
public:
    ModelDoc(QObject *parent=0);
    QVariant data(const QModelIndex &index, int role) const;
    bool fileExist(int ind) const;
    bool isActive(int ind) const;

public slots:
    void refresh(bool activeOnly);
    void viewDoc(int ind);
    void saveAs(int ind);
    void upload(int ind);
    void clearFile(int ind);

private:
    QMap <QString, docInfo> docMap;
    bool isUpdatingList = false;

private slots:
    void updateList();
    void replyListFinished();
    void replyViewFinished();
    void replySaveAsFinished();
    void replyUpdFinished();

signals:
    void sigList();
};

#endif // MODELDOC_H
