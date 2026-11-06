#ifndef CHECKFORM_H
#define CHECKFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include "rest/restconnection.h" // Подключаем RestConnection

namespace Ui {
class CheckForm;
}

class CheckForm : public QWidget
{
    Q_OBJECT

public:
    explicit CheckForm(QWidget *parent = 0);
    ~CheckForm();
    void setCheckEl(bool b);

private:
    Ui::CheckForm *ui;

private slots:
    void go();
    void onCheckFinished();

};

#endif // CHECKFORM_H
