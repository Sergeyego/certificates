#ifndef FORMMARKEL_H
#define FORMMARKEL_H

#include <QWidget>
#include "rest/resttablemodel.h"
#include "rest/restmapper.h"
#include "rest/resttabledialog.h"
#include "rest/restitemdelegate.h"

namespace Ui {
class FormMarkEl;
}

class FormMarkEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormMarkEl(QWidget *parent = 0);
    ~FormMarkEl();

private:
    Ui::FormMarkEl *ui;
    RestMapper *mapper;
    RestTableModel *modelMark;
    RestTableModel *modelAmp;
    RestTableModel *modelAmpTu;
    RestTableModel *modelChemTu;
    RestTableModel *modelMechTu;
    RestTableModel *modelPlav;
    RestTableModel *modelEan;
    RestTableModel *modelProvol;
    RestTableModel *modelGost;
    RestTableModel *modelIntClass;

    void loadsettings();
    void savesettings();
    int id_el();
    int id_diam();
    int id_var();

private slots:
    void updEanRel(QModelIndex index);
    void refreshCont(int index);
    void updImg();
    void updVarsList();
    void gelLbl();
    void blockVar(bool b);
    void loadVars();
    void createVar();
    void saveVar();
    void deleteVar();
    void varChanged();
    void copyTableData();
    void upd();
    void edtVar();

signals:
    void reqUpdSite();
};

class CustomDelegate : public RestItemDelegate
{
    Q_OBJECT
public:
    explicit CustomDelegate(QObject *parent=0);
    void setEditorData(QWidget *editor,const QModelIndex &index) const;
    void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;
};

#endif // FORMMARKEL_H
