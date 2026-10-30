#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tabManager = new TabManager(ui->tabWidget,this);

    actAction(ui->actionPartEl,&MainWindow::newFormPartEl,1);
    actAction(ui->actionPartWire,&MainWindow::newFormPartWire,1);
    actAction(ui->actionShipCert,&MainWindow::newFormShip,1);
    actAction(ui->actionDoc,&MainWindow::newFormDoc,1);
    actAction(ui->actionMarkEl,&MainWindow::newFormMarkEl,1);

    tabManager->loadSettings();
    loadSettings();
    clearEmptyMenus();
}

MainWindow::~MainWindow()
{
    tabManager->saveSettings();
    saveSettings();
    delete ui;
}

void MainWindow::actAction(QAction *a, void (MainWindow::*sl)(), int lev)
{
    if (RestConnection::instance()->groups().contains(lev)){
        connect(a, &QAction::triggered, this, sl);
        tabManager->actAction(a);
    } else {
        delete a;
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->restoreState(settings.value("main_state").toByteArray());
    this->restoreGeometry(settings.value("main_geometry").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("main_state", this->saveState());
    settings.setValue("main_geometry", this->saveGeometry());
}

void MainWindow::clearEmptyMenus()
{
    QMenuBar* menuBar = this->menuBar();
    QList<QAction*> actions = menuBar->actions();

    for (int i = actions.count() - 1; i >= 0; --i) {
        QAction* action = actions.at(i);
        QMenu* menu = action->menu();
        if (menu != nullptr && menu->isEmpty()) {
            menuBar->removeAction(action);
            menu->deleteLater();
        }
    }
}

void MainWindow::updSite()
{
    ProgressReportDialog *prd = new ProgressReportDialog(this);
    prd->setText(tr("Обновление данных на сайте"));
    QNetworkReply *reply = RestConnection::instance()->sendGet(RestConnection::instance()->buildUrl("api/site/sync"));
    connect(reply,SIGNAL(finished()),prd,SLOT(hide()));
    connect(reply,SIGNAL(finished()),prd,SLOT(deleteLater()));
    connect(reply,SIGNAL(finished()),this,SLOT(updSiteFinished()));
    prd->show();
}

void MainWindow::updSiteFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply){
        QByteArray data=reply->readAll();
        bool ok=(reply->error()==QNetworkReply::NoError);
        if (!ok){
            RestError::critical(tr("Ошибка"),reply->errorString()+"\n"+data);
        } else {
            QMessageBox::information(this,QString::fromUtf8("Обновление завершено"),data,QMessageBox::Ok);
        }
        reply->deleteLater();
    }
}

void MainWindow::partReq(int id_part, QString prefix)
{
    if (prefix=="elrtr"){
        ui->actionPartEl->trigger();
    } else {
        ui->actionPartWire->trigger();
    }
    emit findPart(id_part,prefix);
}

void MainWindow::newFormPartEl()
{
    if (!tabManager->exist(sender())){
        FormPartEl *formPartEl = new FormPartEl();
        connect(this,SIGNAL(findPart(int,QString)),formPartEl,SLOT(findPart(int,QString)));
        tabManager->addSubWindow(formPartEl,sender());
    }
}

void MainWindow::newFormShip()
{
    if (!tabManager->exist(sender())){
        FormShip *formShip = new FormShip();
        connect(formShip,SIGNAL(partReq(int,QString)),this,SLOT(partReq(int,QString)));
        tabManager->addSubWindow(formShip,sender());
    }
}

void MainWindow::newFormDoc()
{
    if (!tabManager->exist(sender())){
        FormDoc *formDoc = new FormDoc();
        connect(formDoc,SIGNAL(reqUpdSite()),this,SLOT(updSite()));
        tabManager->addSubWindow(formDoc,sender());
    }
}

void MainWindow::newFormPartWire()
{
    if (!tabManager->exist(sender())){
        FormPartWire *formPartWire = new FormPartWire();
        connect(this,SIGNAL(findPart(int,QString)),formPartWire,SLOT(findPart(int,QString)));
        tabManager->addSubWindow(formPartWire,sender());
    }
}

void MainWindow::newFormMarkEl()
{
    if (!tabManager->exist(sender())){
        FormMarkEl *formMarkEl = new FormMarkEl();
        connect(formMarkEl,SIGNAL(reqUpdSite()),this,SLOT(updSite()));
        tabManager->addSubWindow(formMarkEl,sender());
    }
}
