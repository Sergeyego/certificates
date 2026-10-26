#include "modelwirepartimech.h"

ModelWirePartiMech::ModelWirePartiMech(QObject *parent):RestTableModel("wire_sert_mech",parent)
{
    modelCats = new RestRoTableModel(this);
    modelCats->setPath("api/wire/parti/mechcats/get");

    connect(modelCats,SIGNAL(sigRefresh()),this,SLOT(refreshCatsFinished()));

    modelCats->select();
}

void ModelWirePartiMech::refresh(int id_part)
{
    setFilter(RestFilter::rule(tableName(),"id_part",RestFilter::Op::Eq,id_part));
    setDefaultValue("id_part",id_part);
    select();
}

bool ModelWirePartiMech::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ((index.column()==columnIndex("id_mech")) && role==Qt::EditRole){
        int id_mech=value.toInt();
        int id_cat=mapCat.value(id_mech,2);
        int columnCat = columnIndex("id_cat");
        if (columnCat>=0 && isColumnRel(columnCat)){
            RestRelModel *relModel = RelModels::instance()->getModel(columnInfo(columnCat).relnam);
            int row = relModel->findRowByKey(id_cat);
            if (row>=0){
                colVal cat = relModel->getModelData(row);
                RestTableModel::setData(this->index(index.row(),columnCat),cat.val,Qt::EditRole);
                RestTableModel::setData(this->index(index.row(),columnCat),cat.disp,Qt::DisplayRole);
            }
        }
    }
    return RestTableModel::setData(index,value,role);
}

void ModelWirePartiMech::refreshCats()
{
    modelCats->select();
}

void ModelWirePartiMech::refreshCatsFinished()
{
    mapCat.clear();
    for (int i=0; i<modelCats->rowCount();i++){
        mapCat.insert(modelCats->getModelData(i,"id").toInt(),modelCats->getModelData(i,"id_wire_cat").toInt());
    }
}