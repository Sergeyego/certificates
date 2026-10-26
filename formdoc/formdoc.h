#ifndef FORMDOC_H
#define FORMDOC_H

#include <QWidget>
#include "modeldoc.h"
#include "rest/resttablemodel.h"
#include "rest/restrotablemodel.h"
#include "rest/restmapper.h"

namespace Ui {
class FormDoc;
}

class ModelEn : public RestRoTableModel
{
    Q_OBJECT
public:
    explicit ModelEn(QWidget *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void refresh(int id_doc);
    QVariant data(const QModelIndex &item, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

public slots:
    void checkAll();
    void unCheckAll();

private:
    int id_d;
    QList<int> disList;
    bool writeSql(int id_var, bool value);
};

class FormDoc : public QWidget
{
    Q_OBJECT

public:
    explicit FormDoc(QWidget *parent = 0);
    ~FormDoc();

private:
    Ui::FormDoc *ui;
    ModelDoc *modelDoc;
    RestTableModel *modelEl;
    RestTableModel *modelElDim;
    RestTableModel *modelWireDiam;
    RestTableModel *modelWire;
    RestTableModel *modelTu;
    RestMapper *mapper;
    void loadsettings();
    void savesettings();
    void setOk(bool ok);
    QString currentFname;
    ModelEn *modelEn;

private slots:
    void refreshData(int index);
    void upd();
    void upload();
    void viewSert();
    void saveAs();
    void updState();
    void delSert();

signals:
    void reqUpdSite();
};

#endif // FORMDOC_H
