#ifndef FORMPARTEL_H
#define FORMPARTEL_H

#include <QWidget>
#include "rest/resttablemodel.h"
#include "rest/restrotablemodel.h"

namespace Ui {
class FormPartEl;
}

class ModelPart : public RestRoTableModel
{
    Q_OBJECT
public:
    explicit ModelPart(QObject *parent = nullptr);
    void refresh(QDate beg, QDate end, int id_el=-1, QString diam="");

};

class FormPartEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormPartEl(QWidget *parent = nullptr);
    ~FormPartEl();

private:
    Ui::FormPartEl *ui;
    void loadSettings();
    void saveSettings();
    ModelPart *modelPart;

private slots:
    void upd();
    void updFinished();

};

#endif // FORMPARTEL_H
