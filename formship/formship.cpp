#include "formship.h"
#include "ui_formship.h"

FormShip::FormShip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormShip)
{
    ui->setupUi(this);
    loadSettings();

    QString rsrcPath=":/images/icons/";

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png"));
    ui->cmdSaveAll->setIcon(saveIcon);
    ui->pushButtonSaveDsAll->setIcon(saveIcon);

    const QIcon printIcon = QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png"));
    ui->cmdPrintAll->setIcon(printIcon);
    ui->pushButtonPrintDsAll->setIcon(printIcon);

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(ui->dateEditBeg->date().addYears(1));

    certificate = new CertDocument(this);
    editor = new CertEditor(certificate,this);
    ui->tab_1->layout()->addWidget(editor);

    processor = new CertProcessor(this);
    processor->setTextWidth(certificate->textWidth());

    //reader = new Reader(this);
    //ui->tab_2->layout()->addWidget(reader);

    ui->tableViewShip->verticalHeader()->setDefaultSectionSize(ui->tableViewShip->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableViewShip->verticalHeader()->hide();
    ui->tableViewShipData->verticalHeader()->setDefaultSectionSize(ui->tableViewShipData->verticalHeader()->fontMetrics().height()*1.5);
    modelShip = new RestRoTableModel(this);
    ui->tableViewShip->setModel(modelShip);
    modelDataShip = new RestRoTableModel(this);
    ui->tableViewShipData->setModel(modelDataShip);

    connect(modelShip,SIGNAL(sigRefresh()),this,SLOT(refreshFinished()));
    connect(modelDataShip,SIGNAL(sigRefresh()),this,SLOT(refreshDataShipFinished()));

    connect(ui->tableViewShip->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(refreshDataShip(QModelIndex)));
    connect(ui->tableViewShipData->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(refreshShipSert(QModelIndex)));
    connect(ui->tableViewShipData,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(partReq(QModelIndex)));

    connect(ui->cmdPrintAll,SIGNAL(clicked()),this,SLOT(printAll()));
    connect(ui->cmdSaveAll,SIGNAL(clicked()),this,SLOT(pdfAll()));
    connect(ui->cmdUpd,SIGNAL(clicked()),this,SLOT(refresh()));
    connect(ui->pushButtonPdf,SIGNAL(clicked(bool)),this,SLOT(multiPagePdf()));

    connect(editor,SIGNAL(signFinished()),this,SLOT(signFinished()));
    connect(editor,SIGNAL(signEnChanged(bool)),ui->cmdDsAll,SLOT(setEnabled(bool)));

    connect(ui->tabWidget,SIGNAL(currentChanged(int)),ui->stackedWidget,SLOT(setCurrentIndex(int)));
    connect(ui->cmdDsAll,SIGNAL(clicked(bool)),this,SLOT(signAll()));
    connect(ui->pushButtonSaveDsAll,SIGNAL(clicked(bool)),this,SLOT(signPdfAll()));
    connect(ui->pushButtonPrintDsAll,SIGNAL(clicked(bool)),this,SLOT(signPrintAll()));

    refresh();
}

FormShip::~FormShip()
{
    saveSettings();
    delete ui;
}

QVector<CertJobTask> FormShip::getTasks()
{
    QVector<CertJobTask> tasks;
    for (int i=0; i<modelDataShip->rowCount(); i++){
        CertJobTask job;
        job.id_part=modelDataShip->getModelData(i,"id_part").toInt();
        job.id_ship=modelDataShip->getModelData(i,"id").toInt();
        job.lang=certificate->getLang();
        job.mapSert=certificate->getMapSert();
        job.prefix=modelDataShip->getModelData(i,"prefix").toString();
        job.sertType=certificate->getType();
        tasks.push_back(job);
    }
    return tasks;
}

void FormShip::loadSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    this->ui->splitterVert->restoreState(settings.value("main_splitter_width").toByteArray());
}

void FormShip::saveSettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("main_splitter_width",ui->splitterVert->saveState());
}

void FormShip::refresh()
{
    if (sender()==ui->cmdUpd){
        RelModels::instance()->getModel("cert_type")->refresh();
        certificate->clearCache();
    }
    modelShip->setPath("api/generalship/list/"+ui->dateEditBeg->date().toString("yyyy-MM-dd")+"/"+ui->dateEditEnd->date().toString("yyyy-MM-dd"));

    modelShip->select();
}

void FormShip::refreshFinished()
{
    if (ui->tableViewShip->model()->rowCount()){
        ui->tableViewShip->selectRow(ui->tableViewShip->model()->rowCount()-1);
    } else {
        modelDataShip->clear();
        certificate->clear();
    }
}

void FormShip::refreshDataShip(QModelIndex index)
{
    if (index.row()<0){
        modelDataShip->clear();
        certificate->clear();
        return;
    }
    int id_ship=modelShip->getModelData(index.row(),"id").toInt();
    modelDataShip->setPath("api/generalship/data/"+QString::number(id_ship));
    modelDataShip->select();
}

void FormShip::refreshDataShipFinished()
{
    if (ui->tableViewShipData->model()->rowCount()){
        ui->tableViewShipData->selectRow(0);
    } else {
        certificate->clear();
    }
}

void FormShip::refreshShipSert(QModelIndex index)
{
    if (index.row()<0){
        certificate->clear();
        return;
    }
    int id_part=modelDataShip->getModelData(index.row(),"id_part").toInt();
    int id_ship=modelDataShip->getModelData(index.row(),"id").toInt();
    QString prefix=modelDataShip->getModelData(index.row(),"prefix").toString();
    certificate->build(id_part,id_ship,prefix);
    //reader->setCurrentIdShip(id_ship,name,prefix);
}

void FormShip::printAll()
{
    QVector<CertJobTask> tasks=getTasks();
    bool ok = processor->startProcessing(tasks,CertProcessor::WorkMode::PrintAll);
    if (ok){
        createProgressDialog(tasks.size(),tr("Печать сертификатов..."));
    }
}

void FormShip::pdfAll()
{
    if (ui->tableViewShip->currentIndex().row() < 0) return;
    QVector<CertJobTask> tasks=getTasks();
    QDir dir(QDir::homePath()+"/sertificat");
    if (!dir.exists()) dir.mkdir(dir.path());
    dir.setPath(dir.path()+"/"+QString::number(modelShip->getModelData(ui->tableViewShip->currentIndex().row(),"dat").toDate().year()));
    if (!dir.exists()) dir.mkdir(dir.path());
    dir.setPath(dir.path()+"/"+modelShip->getModelData(ui->tableViewShip->currentIndex().row(),"num").toString());
    if (!dir.exists()) dir.mkdir(dir.path());
    bool ok = processor->startProcessing(tasks,CertProcessor::WorkMode::SaveSeparatePDF,dir.path());
    if (ok){
        createProgressDialog(tasks.size(),tr("Сохранение сертификатов..."));
    }
}

void FormShip::multiPagePdf()
{
    if (!ui->tableViewShipData->model()->rowCount() || (ui->tableViewShip->currentIndex().row() < 0)){
        return;
    }
    QString name=modelShip->getModelData(ui->tableViewShip->currentIndex().row(),"num").toString();
    QSettings settings("szsm", QApplication::applicationName());
    QDir dir(settings.value("sertPath",QDir::homePath()).toString());
    QString fname = QFileDialog::getSaveFileName(this,tr("Сохранить PDF"),dir.path()+"/"+name+".pdf", "*.pdf");
    if (!fname.isEmpty()){
        QVector<CertJobTask> tasks=getTasks();
        bool ok = processor->startProcessing(tasks,CertProcessor::WorkMode::SaveMultiPagePDF,fname);
        if (ok) {
            createProgressDialog(tasks.size(),tr("Формирование документа..."));
        }
        QFile file(fname);
        QFileInfo info(file);
        settings.setValue("sertPath",info.path());
    }
}

void FormShip::partReq(QModelIndex index)
{
    if (index.row()<0){
        return;
    }
    int id_part=modelDataShip->getModelData(index.row(),"id_part").toInt();
    QString prefix=modelDataShip->getModelData(index.row(),"prefix").toString();
    emit partReq(id_part,prefix);
}

void FormShip::signFinished()
{
    /*reader->setLang(sertificat->getLang());
    reloadDataShip();
    ui->tabWidget->setCurrentIndex(1);*/
}


void FormShip::signAll()
{
    /*DialogSignature d;
    if (d.exec()==QDialog::Accepted){
        QString sn=d.getSN();
        if (sn.isEmpty()){
            return;
        }
        QProgressDialog* pprd = new QProgressDialog(tr("Идет подписание документов..."),"", 0, modelDataShip->rowCount(), this);
        pprd->setCancelButton(0);
        pprd->setMinimumDuration(0);
        pprd->setWindowTitle(tr("Пожалуйста, подождите"));

        for (int i=0; i<modelDataShip->rowCount(); i++){
            QCoreApplication::processEvents();
            pprd->setValue(i);
            refreshShipSert(modelDataShip->index(i,0));
            editor->signDS(sn);
        }
        pprd->setLabelText("Синхронизация с облачным сервисом...");
        QByteArray st;
        HttpSyncManager::sendGet(Rels::instance()->appServer()+"/s3/sync",st);
        delete pprd;
        signFinished();
    }*/
}

void FormShip::signPdfAll()
{
    /*QProgressDialog* pprd = new QProgressDialog(tr("Идет сохранение документов..."),"", 0, modelDataShip->rowCount(), this);
    pprd->setCancelButton(0);
    pprd->setMinimumDuration(0);
    pprd->setWindowTitle(tr("Пожалуйста, подождите"));
    int yearSert=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(ui->tableViewShip->currentIndex().row(),2),Qt::EditRole).toDate().year();
    QString nomSert=ui->tableViewShip->model()->data(ui->tableViewShip->model()->index(ui->tableViewShip->currentIndex().row(),1),Qt::EditRole).toString();
    QString lang=reader->getCurrentLang();
    for (int i=0; i<modelDataShip->rowCount(); i++){
        QCoreApplication::processEvents();
        pprd->setValue(i);
        int id_ship=modelDataShip->data(modelDataShip->index(i,0),Qt::EditRole).toInt();
        QString prefix = modelDataShip->data(modelDataShip->index(i,1),Qt::EditRole).toString();
        QByteArray data;
        bool ok = HttpSyncManager::sendGet(Rels::instance()->appServer()+"/s3/local/"+prefix+"/"+QString::number(id_ship)+"/"+lang,data);
        if (ok && data.size()){
            QString name = prefix+"_"+modelDataShip->data(modelDataShip->index(i,2),Qt::EditRole).toString();
            name+="_"+nomSert;
            name=name.replace(QRegularExpression("[^\\w]"), "_");
            QDir dir(QDir::homePath()+"/sertificat");
            if (!dir.exists()) dir.mkdir(dir.path());
            dir.setPath(dir.path()+"/"+QString::number(yearSert));
            if (!dir.exists()) dir.mkdir(dir.path());
            dir.setPath(dir.path()+"/"+nomSert);
            if (!dir.exists()) dir.mkdir(dir.path());
            QFile file(dir.path()+"/"+name+"_"+lang+"_sig.pdf");
            if (file.open(QFile::WriteOnly)){
                file.write(data);
                file.close();
            }
        }
    }
    delete pprd;*/
}

void FormShip::signPrintAll()
{
    /*QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec()) {
        QProgressDialog* pprd = new QProgressDialog(tr("Идет формирование документа..."),"", 0, modelDataShip->rowCount(), this);
        pprd->setCancelButton(0);
        pprd->setMinimumDuration(0);
        pprd->setWindowTitle(tr("Пожалуйста, подождите"));

        QPainter painter(&printer);
        QString lang=reader->getCurrentLang();
        for (int i=0; i<modelDataShip->rowCount(); i++){
            QCoreApplication::processEvents();
            pprd->setValue(i);
            int id_ship=modelDataShip->data(modelDataShip->index(i,0),Qt::EditRole).toInt();
            QString prefix=modelDataShip->data(modelDataShip->index(i,1),Qt::EditRole).toString();
            QByteArray data;
            bool ok = HttpSyncManager::sendGet(Rels::instance()->appServer()+"/s3/img/"+prefix+"/"+QString::number(id_ship)+"/"+lang+"/300",data);
            if (ok && data.size()){
                QImage img = QImage::fromData(data,"png");
                painter.drawImage(painter.window(),img);
                if (i<modelDataShip->rowCount()-1){
                    printer.newPage();
                }
            }
        }
        delete pprd;
    }*/
}

void FormShip::createProgressDialog(int maximum, const QString &message)
{
    QProgressDialog *progressDialog = new QProgressDialog(message,nullptr,0,maximum,this);
    progressDialog->setModal(true);
    progressDialog->setWindowTitle(tr("Пожалуйста, подождите"));
    progressDialog->show();
    connect(processor,SIGNAL(progress(int,int)),progressDialog,SLOT(setValue(int)));
    connect(processor,SIGNAL(finished()),progressDialog,SLOT(deleteLater()));
}

