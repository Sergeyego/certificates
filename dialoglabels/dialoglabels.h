#ifndef DIALOGLABELS_H
#define DIALOGLABELS_H

#include <QDialog>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QSettings>
#include <QStandardPaths>
#include "rest/restconnection.h"

namespace Ui {
class DialogLabels;
}

class DialogLabels : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLabels(QWidget *parent = nullptr);
    ~DialogLabels();
    void setLabel(const QString &basePath, int id_part);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::DialogLabels *ui;
    QPixmap currentPixmap;
    QString _basePath;
    int _id_part;
    void loadSettings();
    void saveSettings();

private slots:
    void onListResult();
    void onLabelResult();
    void updLabel(int index);
    void saveLabel();
    void printLabel();
};

#endif // DIALOGLABELS_H
