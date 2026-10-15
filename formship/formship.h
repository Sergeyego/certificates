#ifndef FORMSHIP_H
#define FORMSHIP_H

#include <QWidget>
#include <QFileDialog>
#include <QProgressDialog>
#include <QPrintDialog>
#include "certeditor/editor.h"
#include "certeditor/certdocument.h"
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
    Editor *editor;
    //Reader *reader;
    CertDocument *sertificat;
    RestRoTableModel *modelShip;
    RestRoTableModel *modelDataShip;
    void loadSettings();
    void saveSettings();

private slots:
    void refreshDataShip(QModelIndex index);
    void refreshShipSert(QModelIndex index);
    void printAll();
    void printAll(QPagedPaintDevice *printer);
    void pdfAll();
    void multiPagePdf();
    void refresh();
    void partReq(QModelIndex index);
    void signFinished();
    void reloadDataShip();
    void signAll();
    void signPdfAll();
    void signPrintAll();
};

#endif // FORMSHIP_H
