#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tabmanager.h"
#include "rest/restconnection.h"
#include "formpartel/formpartel.h"
#include "formpartwire/formpartwire.h"
#include "formship/formship.h"
#include "formdoc/formdoc.h"
#include "progressreportdialog/progressreportdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    TabManager *tabManager;
    void actAction(QAction *a, void (MainWindow::*sl)(), int lev);
    void loadSettings();
    void saveSettings();
    void clearEmptyMenus();

private slots:
    void updSite();
    void updSiteFinished();
    void partReq(int id_part, QString prefix);
    void newFormPartEl();
    void newFormPartWire();
    void newFormShip();
    void newFormDoc();

signals:
    void findPart(int id_part, QString prefix);
};
#endif // MAINWINDOW_H
