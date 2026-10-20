#ifndef FORMPARTWIRE_H
#define FORMPARTWIRE_H

#include <QWidget>
#include <QDataWidgetMapper>
#include "rest/restrotablemodel.h"
#include "rest/resttablemodel.h"
#include "certeditor/certeditor.h"
#include "certeditor/certdocument.h"

class ModelPartWire : public RestRoTableModel
{    Q_OBJECT

public:
    explicit ModelPartWire(QWidget *parent = nullptr);
    void refresh(QDate beg, QDate end, int id_provol);

public slots:
    void refreshState();

private:
    QDate dateBeg;
    QDate dateEnd;
    QMap <int,int> colorState;
};

class ModelWirePartiMech : public RestTableModel
{
    Q_OBJECT
public:
    ModelWirePartiMech(QObject *parent=0);
    void refresh(int id_part);
    bool setData(const QModelIndex &index, const QVariant &value, int role);
};

namespace Ui {
class FormPartWire;
}

class FormPartWire : public QWidget
{
    Q_OBJECT

public:
    explicit FormPartWire(QWidget *parent = nullptr);
    ~FormPartWire();

private:
    Ui::FormPartWire *ui;
    CertDocument *certDocument;
    CertEditor *certEditor;
    //Reader *readerPart;

    ModelPartWire *modelPartWire;
    QDataWidgetMapper *mapper;
    RestRoTableModel *modelTuSrc;
    RestTableModel *modelTu;
    RestRoTableModel *modelShip;
    RestTableModel *modelSrcChem;
    RestTableModel *modelChem;
    RestTableModel *modelSrcMech;
    ModelWirePartiMech *modelMech;
    void loadSettings();
    void saveSettings();
    void setLock(bool b);

private slots:
    void upd();
    void currentIndexChanged(QModelIndex index);
    void copyTu();
    void showShipSert(QModelIndex index);
    void showPartSert();
    void savePrim();
    void enPrimSave();
    void copyChem();
    void showCheckForm();
    void findPart(int id_part, QString prefix);
};

#endif // FORMPARTWIRE_H
