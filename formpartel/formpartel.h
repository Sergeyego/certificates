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
    RestRoTableModel *modelTu;
    RestRoTableModel *modelNote;
    RestRoTableModel *modelShip;
    RestTableModel *modelChemSrc;
    RestTableModel *modelMechSrc;
    RestTableModel *modelChem;
    RestTableModel *modelMech;
    RestTableModel *modelMechx;

private slots:
    void enPrimSave();
    void upd();
    void updFinished();
    void updNoteFinished();
    void updData(QModelIndex index);

};

#endif // FORMPARTEL_H
