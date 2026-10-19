#ifndef FORMSHIP_H
#define FORMSHIP_H

#include <QWidget>
#include <QFileDialog>
#include <QProgressDialog>
#include <QPrintDialog>
#include "certeditor/certeditor.h"
#include "certeditor/certdocument.h"
#include "certeditor/certprocessor.h"
#include "rest/restrotablemodel.h"

namespace Ui {
class FormShip;
}

class FormShip : public QWidget
{
    Q_OBJECT

public:
    explicit FormShip(QWidget *parent = 0);
    ~FormShip();

private:
    Ui::FormShip *ui;
    CertEditor *editor;
    CertProcessor *processor;
    //Reader *reader;
    CertDocument *certificate;
    RestRoTableModel *modelShip;
    RestRoTableModel *modelDataShip;
    QVector<CertJobTask> getTasks();
    void loadSettings();
    void saveSettings();

private slots:
    void refresh();
    void refreshFinished();
    void refreshDataShip(QModelIndex index);
    void refreshDataShipFinished();
    void refreshShipSert(QModelIndex index);
    void printAll();
    void pdfAll();
    void multiPagePdf();
    void partReq(QModelIndex index);
    void signFinished();
    void signAll();
    void signPdfAll();
    void signPrintAll();
    void createProgressDialog(int maximum, const QString &message);

signals:
    void partReq(int id_part, QString prefix);
};

#endif // FORMSHIP_H
