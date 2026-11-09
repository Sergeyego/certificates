#include "mainwindow.h"

#include <QApplication>
#include "rest/restlogin.h"
#include <QDir>
#include <QFileInfoList>
#include <QFontDatabase>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/ico.ico"));
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || defined(Q_OS_MAC)
    QApplication::setStyle(QStyleFactory::create("Fusion"));
#endif
    RestLogin d(QObject::tr("Результаты испытаний и сертификаты качества"));
    d.setHost("https://192.168.1.10");
    if (d.exec()!=QDialog::Accepted) {
        exit(1);
    }

    QDir dir(":fonts");
    QFileInfoList list = dir.entryInfoList();
    foreach (QFileInfo i, list) {
        QFontDatabase::addApplicationFont(i.absoluteFilePath());
    }

    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
