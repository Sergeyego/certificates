#include "certprocessor.h"
#include <QDebug>
#include <QDir>
#include <QApplication>

CertProcessor::CertProcessor(QObject *parent)
    : QObject(parent), m_certDoc(nullptr), m_printer(nullptr), m_painter(nullptr), m_totalTasks(0), m_currentProgress(0), m_isFirstPage(true)
{
    m_certDoc = new CertDocument(this);
    // Включаем пакетный режим работы для документа, чтобы он не сбрасывал задачи на асинхронных шагах
    m_certDoc->setBatchMode(true);
    connect(m_certDoc, &CertDocument::buildFinished, this, &CertProcessor::onDocRefreshed);
}

CertProcessor::~CertProcessor()
{
    finalizeProcessing();
}

void CertProcessor::startProcessing(const QVector<CertJobTask> &tasks, WorkMode mode, const QString &outputDirOrFile)
{
    // Защита от повторного запуска, пока конвейер занят
    if (!m_taskQueue.isEmpty() || m_painter || m_printer) {
        QMessageBox::warning(QApplication::activeWindow(), tr("Уже выполняется"), tr("Дождитесь завершения текущей операции."));
        return;
    }

    if (tasks.isEmpty()) {
        emit finished();
        return;
    }

    m_taskQueue.clear();
    for (const auto &task : tasks) {
        m_taskQueue.enqueue(task);
    }

    m_mode = mode;
    m_basePath = outputDirOrFile;
    m_totalTasks = tasks.size();
    m_currentProgress = 0;
    m_isFirstPage = true;

    m_printer = new QPrinter(QPrinter::HighResolution);
    m_printer->setPageSize(QPageSize(QPageSize::A4));
    m_printer->setPageOrientation(QPageLayout::Portrait);
    m_printer->setPageMargins(QMarginsF(0, 0, 0, 0));

    if (m_mode == SaveMultiPagePDF) {
        if (m_basePath.isEmpty()) {
            m_basePath = QFileDialog::getSaveFileName(QApplication::activeWindow(), tr("Сохранить PDF"), "", "PDF Files (*.pdf)");
            if (m_basePath.isEmpty()) {
                finalizeProcessing();
                emit finished(); // ИСПРАВЛЕНО: Обязательно уведомляем UI об остановке
                return;
            }
        }
        m_printer->setOutputFormat(QPrinter::PdfFormat);
        m_printer->setOutputFileName(m_basePath);

        m_painter = new QPainter();
        if (!m_painter->begin(m_printer)) {
            qWarning() << "Failed to begin painting on multi-page PDF printer";
            finalizeProcessing();
            emit finished();
            return;
        }
    }
    else if (m_mode == PrintAll) {
        QPrintDialog dialog(m_printer, QApplication::activeWindow());
        if (dialog.exec() != QDialog::Accepted) {
            finalizeProcessing();
            emit finished(); // ИСПРАВЛЕНО: Спиннер в UI больше не зависнет при нажатии "Отмена" в диалоге печати
            return;
        }
        m_painter = new QPainter();
        if (!m_painter->begin(m_printer)) {
            qWarning() << "Failed to begin painting on system printer";
            finalizeProcessing();
            emit finished();
            return;
        }
    }

    // Запуск конвейера задач
    processNext();
}

void CertProcessor::processNext()
{
    if (m_taskQueue.isEmpty()) {
        finalizeProcessing();
        emit finished(); // Успешное окончание всей пачки задач
        return;
    }

    m_currentProgress++;
    emit progress(m_currentProgress, m_totalTasks);

    CertJobTask task = m_taskQueue.dequeue();

    // Сбрасываем старое состояние документа под новую задачу
    m_certDoc->setDefaultDoc();

    QMapIterator<int, bool> i(task.mapSert);
    while (i.hasNext()) {
        i.next();
        m_certDoc->setDocEn(i.key(), i.value());
    }

    m_certDoc->setLang(task.lang);
    m_certDoc->setType(task.sertType);

    // Асинхронный запуск сборки. Ждем сигнал buildFinished -> слот onDocRefreshed()
    m_certDoc->build(task.id_part, task.id_ship, "", task.prefix);
}

void CertProcessor::onDocRefreshed()
{
    renderCurrentDocument();
    processNext(); // Переходим строго к следующему элементу очереди
}

void CertProcessor::renderCurrentDocument()
{
    // Защита от пустых/недогруженных размеров документа
    if (m_certDoc->idealWidth() <= 0 || m_certDoc->size().height() <= 0) {
        qWarning() << "CertProcessor: Document geometry is invalid. Height:" << m_certDoc->size().height();
    }

    if (m_mode == SaveSeparatePDF) {
        QPrinter separatePrinter(QPrinter::HighResolution);
        separatePrinter.setPageSize(QPageSize(QPageSize::A4));
        separatePrinter.setPageOrientation(QPageLayout::Portrait);
        separatePrinter.setPageMargins(QMarginsF(0, 0, 0, 0));
        separatePrinter.setOutputFormat(QPrinter::PdfFormat);

        QString fileName = QString("cert_part%1_type%2.pdf")
                               .arg(m_certDoc->getIdShip() >= 0 ? m_certDoc->getIdShip() : m_currentProgress)
                               .arg(m_certDoc->getType());

        QString fullPath = m_basePath.isEmpty() ? fileName : QDir(m_basePath).filePath(fileName);
        separatePrinter.setOutputFileName(fullPath);

        QPainter painter(&separatePrinter);
        if (painter.isActive()) {
            m_certDoc->setPageSize(separatePrinter.pageRect(QPrinter::DevicePixel).size());

            double scaleX = double(separatePrinter.pageRect(QPrinter::DevicePixel).width()) / m_certDoc->idealWidth();
            double scaleY = double(separatePrinter.pageRect(QPrinter::DevicePixel).height()) / m_certDoc->size().height();
            double scale = qMin(scaleX, scaleY);

            painter.scale(scale, scale);
            m_certDoc->drawContents(&painter);
            painter.end();
        }
    }
    else {
        // Режимы: Многостраничный PDF или Прямая печать
        if (!m_painter || !m_painter->isActive()) return;

        if (!m_isFirstPage) {
            m_printer->newPage();
        }
        m_isFirstPage = false;

        m_certDoc->setPageSize(m_printer->pageRect(QPrinter::DevicePixel).size());

        double scaleX = double(m_printer->pageRect(QPrinter::DevicePixel).width()) / m_certDoc->idealWidth();
        double scaleY = double(m_printer->pageRect(QPrinter::DevicePixel).height()) / m_certDoc->size().height();
        double scale = qMin(scaleX, scaleY);

        m_painter->save();
        m_painter->scale(scale, scale);
        m_certDoc->drawContents(m_painter);
        m_painter->restore();
    }
}

void CertProcessor::finalizeProcessing()
{
    if (m_painter) {
        if (m_painter->isActive()) {
            m_painter->end();
        }
        delete m_painter;
        m_painter = nullptr;
    }
    if (m_printer) {
        delete m_printer;
        m_printer = nullptr;
    }
}

