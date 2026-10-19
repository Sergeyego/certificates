#ifndef CERTPROCESSOR_H
#define CERTPROCESSOR_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>
#include <QQueue>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QPainter>
#include "certdocument.h"

// Структура параметров для одного сертификата
struct CertJobTask {
    int id_part;
    int id_ship;
    QString lang;
    QString prefix;
    QMap<int, bool> mapSert;
    int sertType;
};

class CertProcessor : public QObject
{
    Q_OBJECT
public:
    // Режимы работы процессора
    enum WorkMode {
        SaveSeparatePDF,  // Сохранить каждый файл отдельно
        SaveMultiPagePDF, // Сохранить все в один многостраничный PDF
        PrintAll          // Напечатать все на принтере
    };

    explicit CertProcessor(QObject *parent = nullptr);
    ~CertProcessor();
    void setTextWidth(qreal width);

    // Запуск пакетной обработки
    bool startProcessing(const QVector<CertJobTask> &tasks, WorkMode mode, const QString &outputDirOrFile = "");

signals:
    void finished(); // Сигнал завершения всей пачки
    void progress(int current, int total); // Сигнал прогресса для UI

private slots:
    void onDocRefreshed(); // Вызывается, когда CertDocument полностью загрузил HTML и картинки

private:
    void processNext();
    void renderCurrentDocument();
    void finalizeProcessing();

    WorkMode m_mode;
    QString m_basePath;
    QQueue<CertJobTask> m_taskQueue;
    int m_totalTasks;
    int m_currentProgress;

    CertDocument *m_certDoc;
    QPrinter *m_printer;
    QPainter *m_painter;
    bool m_isFirstPage; // Для многостраничного режима
};

#endif // CERTPROCESSOR_H
