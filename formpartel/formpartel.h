#ifndef FORMPARTEL_H
#define FORMPARTEL_H

#include <QWidget>
#include "rest/resttablemodel.h"
#include "rest/restrotablemodel.h"
#include "dialogcopyvalel/dialogcopyvalel.h"
#include "dialoglabels/dialoglabels.h"
#include "dialogauthor/dialogauthor.h"
#include "pasportmanager.h"

namespace Ui {
class FormPartEl;
}

class ModelPart : public RestRoTableModel
{
    Q_OBJECT
public:
    explicit ModelPart(QObject *parent = nullptr);
    void refresh(QDate beg, QDate end, int id_el=-1, QString diam="");
    void refreshState(int index);

private slots:
    void refreshStateFinished();

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
    bool primChanged;
    bool primProdChanged;
    PasportManager *pasportManager;
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
    void setBlock(bool b);
    void onPrimChanged();
    void onPrimProdChanged();
    void savePrim();
    void copyChem();
    void copyMech();
    void updCurrentState();
    void upd();
    void updFinished();
    void updNoteFinished();
    void updData(QModelIndex index);
    void setOk(bool ok);
    void importVals();
    void showLabels();
    void createPasport();

};

#endif // FORMPARTEL_H
