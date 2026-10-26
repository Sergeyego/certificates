#ifndef MODELWIREPARTIMECH_H
#define MODELWIREPARTIMECH_H

#include <QObject>
#include "rest/resttablemodel.h"
#include "rest/restrotablemodel.h"

class ModelWirePartiMech : public RestTableModel
{
    Q_OBJECT
public:
    ModelWirePartiMech(QObject *parent=0);
    void refresh(int id_part);
    bool setData(const QModelIndex &index, const QVariant &value, int role);

public slots:
    void refreshCats();

private:
    RestRoTableModel *modelCats;
    QMap <int, int> mapCat;

private slots:
    void refreshCatsFinished();
};

#endif // MODELWIREPARTIMECH_H