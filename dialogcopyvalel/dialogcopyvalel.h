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

private:
    Ui::DialogCopyValEl *ui;
    RestRoTableModel *modelPart;

private slots:

    void updFinished();
};

#endif // DIALOGCOPYVALEL_H
