#ifndef DIALOGAUTHOR_H
#define DIALOGAUTHOR_H

#include <QDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include "rest/restrotablemodel.h"

namespace Ui {
class DialogAuthor;
}

class DialogAuthor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAuthor(QWidget *parent = nullptr);
    ~DialogAuthor();
    QByteArray getJson();

private:
    Ui::DialogAuthor *ui;
    RestRoTableModel *modelAuthor;

private slots:
    void updInfo();
    void updFinished();
};

#endif // DIALOGAUTHOR_H
