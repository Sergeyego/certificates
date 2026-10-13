#ifndef DIALOGCOPYVALEL_H
#define DIALOGCOPYVALEL_H

#include <QDialog>
#include "rest/restrotablemodel.h"
#include "rest/resttablemodel.h"

namespace Ui {
class DialogCopyValEl;
}

class DialogCopyValEl : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCopyValEl(int id_part_src, QWidget *parent = nullptr);
    ~DialogCopyValEl();
    int currentIdPart();

private:
    Ui::DialogCopyValEl *ui;
    RestRoTableModel *modelPart;
    RestTableModel *modelChem;
    RestTableModel *modelMech;
    RestTableModel *modelMechx;

private slots:

    void updFinished();
    void updData(QModelIndex index);
};

#endif // DIALOGCOPYVALEL_H
