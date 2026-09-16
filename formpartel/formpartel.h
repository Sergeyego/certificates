#ifndef FORMPARTEL_H
#define FORMPARTEL_H

#include <QWidget>
#include "rest/resttablemodel.h"

namespace Ui {
class FormPartEl;
}

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

private slots:
    void upd();

};

#endif // FORMPARTEL_H
