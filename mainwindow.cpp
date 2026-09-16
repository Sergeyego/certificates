#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tabManager = new TabManager(ui->tabWidget,this);

    actAction(ui->actionPartEl,&MainWindow::newFormPartEl,1);

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
        a->setEnabled(false);
        a->deleteLater();
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

void MainWindow::newFormPartEl()
{
    if (!tabManager->exist(sender())){
        tabManager->addSubWindow(new FormPartEl(),sender());
    }
}
