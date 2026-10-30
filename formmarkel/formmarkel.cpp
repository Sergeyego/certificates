#include "formmarkel.h"
#include "ui_formmarkel.h"

FormMarkEl::FormMarkEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMarkEl)
{
    ui->setupUi(this);
    loadsettings();

    ui->lineEditPr->setInputMask(QString("099±90°C 009 xxxxx"));
    ui->dateEdit->setDate(QDate::currentDate());

    id_el_src=-1;
    id_var_src=-1;

    RelModels::instance()->getModel("el_iso_types")->setEditable(true);
    RelModels::instance()->getModel("el_purpose")->setEditable(true);
    RelModels::instance()->getModel("el_aws_types")->setEditable(true);
    RelModels::instance()->getModel("el_types")->setEditable(true);
    RelModels::instance()->getModel("el_gost_types")->setEditable(true);
    RelModels::instance()->getModel("el_denominator")->setEditable(true);
    RelModels::instance()->getModel("el_grp")->setEditable(true);
    RelModels::instance()->getModel("el_plav_nams")->setEditable(true);
    RelModels::instance()->getModel("int_class")->setEditable(true);
    RelModels::instance()->getModel("positions")->setEditable(true);

    ui->comboBoxVarGost->setModel(RelModels::instance()->getModel("el_gost_types"));

    colVal defDim;
    defDim.val=4;

    ui->comboBoxDiam->setModel(RelModels::instance()->getModel("el_diam"));
    ui->comboBoxDiam->setCurrentData(defDim);
    ui->comboBoxDiam->setEditable(false);

    modelVarList = new RestRoTableModel(this);
    connect(modelVarList,SIGNAL(sigRefresh()),this,SLOT(updVarsList()));

    modelVar = new RestRoTableModel(this);
    connect(modelVar,SIGNAL(sigRefresh()),this,SLOT(loadVarsFinished()));

    modelProvol = new RestTableModel("el_provol",this);
    ui->tableViewProvol->setModel(modelProvol);

    colVal defVar;
    defVar.val=1;
    ui->comboBoxVar->setModel(RelModels::instance()->getModel("vars"));
    ui->comboBoxVar->setCurrentData(defVar);
    ui->comboBoxVar->setEditable(false);

    modelEan = new RestTableModel("ean_el",this);
    ui->tableViewPack->setModel(modelEan);
    connect(modelEan,SIGNAL(sigUpd()),RelModels::instance()->getModel("ean_ed"),SLOT(refresh()));
    connect(modelEan,SIGNAL(sigUpd()),RelModels::instance()->getModel("ean_gr"),SLOT(refresh()));

    modelPlav = new RestTableModel("el_plav",this);
    ui->tableViewPlav->setModel(modelPlav);

    modelGost = new RestTableModel("gost_el",this);
    modelGost->setDefaultValue("dat",QDate::currentDate());
    ui->tableViewGost->setModel(modelGost);

    modelAmp = new RestTableModel("el_amp",this);
    ui->tableViewAmp->setModel(modelAmp);

    modelAmpTu = new RestTableModel("el_amp_tu",this);
    ui->tableViewAmpTu->setModel(modelAmpTu);

    modelChemTu = new RestTableModel("el_chem_tu",this);
    ui->tableViewChem->setModel(modelChemTu);

    modelMechTu = new RestTableModel("el_mech_tu",this);
    ui->tableViewMech->setModel(modelMechTu);

    modelIntClass = new RestTableModel("el_int_class",this);
    ui->tableViewInt->setModel(modelIntClass);

    modelMark = new RestTableModel("elrtr",this);
    modelMark->setFilter(RestFilter::rule(modelMark->tableName(),"id",RestFilter::Op::Ne,0));
    ui->tableViewMark->setModel(modelMark);

    mapper = new RestMapper(ui->tableViewMark,this);
    mapper->setItemDelegate(new CustomDelegate(this));

    ui->horizontalLayoutMap->insertWidget(0,mapper);

    mapper->addMapping(ui->lineEditMark,"marka");
    mapper->addMapping(ui->lineEditMarkSert,"marka_sert");
    mapper->addMapping(ui->comboBoxGroup,"id_grp");
    mapper->addMapping(ui->comboBoxVid,"id_vid");
    mapper->addMapping(ui->comboBoxProv,"id_prov");
    mapper->addMapping(ui->comboBoxPol,"id_pos");
    mapper->addMapping(ui->comboBoxGost,"id_gost_type");
    mapper->addMapping(ui->comboBoxISO,"id_iso_type");
    mapper->addMapping(ui->comboBoxAWS,"id_aws_type");
    mapper->addMapping(ui->comboBoxZnam,"id_denominator");
    mapper->addMapping(ui->comboBoxBukv,"id_purpose");
    mapper->addMapping(ui->plainTextEditDescrCat,"descr");
    mapper->addMapping(ui->lineEditVl,"vl");
    mapper->addMapping(ui->lineEditPr,"pr2");
    mapper->addMapping(ui->lineEditSL,"shelf_life");
    mapper->addMapping(ui->checkBoxKat,"katalog");
    mapper->addMapping(ui->plainTextEditDescrSpec,"descr_spec");
    mapper->addMapping(ui->plainTextEditDescrFeature,"descr_feature");

    mapper->addEmptyLock(ui->tableViewAmp);
    mapper->addEmptyLock(ui->tableViewChem);
    mapper->addEmptyLock(ui->tableViewMech);
    mapper->addEmptyLock(ui->tableViewPlav);
    mapper->addEmptyLock(ui->groupBoxVar);
    mapper->addEmptyLock(ui->tableViewProvol);
    mapper->addEmptyLock(ui->pushButtonSite);
    mapper->addEmptyLock(ui->pushButtonLbl);
    mapper->addEmptyLock(ui->tableViewPack);
    mapper->addEmptyLock(ui->tableViewGost);

    connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshCont(int)));
    connect(modelMark,SIGNAL(sigUpd()),RelModels::instance()->getModel("mark"),SLOT(refresh()));
    connect(ui->pushButtonLbl,SIGNAL(clicked(bool)),this,SLOT(genLbl()));
    connect(ui->pushButtonSite,SIGNAL(clicked(bool)),this,SIGNAL(reqUpdSite()));
    connect(ui->pushButtonCreVar,SIGNAL(clicked(bool)),this,SLOT(createVar()));
    connect(ui->pushButtonDelVar,SIGNAL(clicked(bool)),this,SLOT(deleteVar()));
    connect(ui->comboBoxVar,SIGNAL(currentIndexChanged(int)),this,SLOT(loadVars()));
    connect(ui->plainTextEditDescr,SIGNAL(textChanged()),this,SLOT(varChanged()));
    connect(ui->plainTextEditDescrTu,SIGNAL(textChanged()),this,SLOT(varChanged()));
    connect(ui->lineEditVarZnam,SIGNAL(textChanged(QString)),this,SLOT(varChanged()));
    connect(ui->lineEditProcVar,SIGNAL(textChanged(QString)),this,SLOT(varChanged()));
    connect(ui->comboBoxVarGost,SIGNAL(currentIndexChanged(int)),this,SLOT(varChanged()));
    connect(ui->pushButtonSaveVar,SIGNAL(clicked(bool)),this,SLOT(saveVar()));
    connect(ui->pushButtonCopy,SIGNAL(clicked(bool)),this,SLOT(copyTableData()));
    connect(ui->pushButtonPaste,SIGNAL(clicked(bool)),this,SLOT(pasteTableData()));
    connect(ui->pushButtonUpd,SIGNAL(clicked(bool)),this,SLOT(upd()));
    connect(ui->toolButtonEdtVar,SIGNAL(clicked(bool)),this,SLOT(edtVar()));

    this->upd();
}

FormMarkEl::~FormMarkEl()
{
    savesettings();
    delete ui;
}

void FormMarkEl::loadsettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    ui->splitter->restoreState(settings.value("mark_splitter").toByteArray());
}

void FormMarkEl::savesettings()
{
    QSettings settings("szsm", QApplication::applicationName());
    settings.setValue("mark_splitter", ui->splitter->saveState());
}

void FormMarkEl::setCurrentPix(const QPixmap &pix)
{
    ui->labelPix->setPixmap(pix.scaled(ui->labelPix->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void FormMarkEl::configureVarRels(RestTableModel *model, int id_el, int id_var)
{
    model->setDefaultValue("id_el",id_el);
    model->setDefaultValue("id_var",id_var);
    RestFilter filter = RestFilter(RestFilter::Group::And);
    filter.addRule(model->tableName(),"id_el",RestFilter::Op::Eq,id_el);
    filter.addRule(model->tableName(),"id_var",RestFilter::Op::Eq,id_var);
    model->setFilter(filter);
    model->select();
}

void FormMarkEl::upd()
{
    if (sender()==ui->pushButtonUpd){
        mapPix.clear();

        QVector<RestTableModel*> arrMod;
        arrMod.push_back(modelMark);
        arrMod.push_back(modelAmp);
        arrMod.push_back(modelAmpTu);
        arrMod.push_back(modelChemTu);
        arrMod.push_back(modelMechTu);
        arrMod.push_back(modelPlav);
        arrMod.push_back(modelEan);
        arrMod.push_back(modelProvol);
        arrMod.push_back(modelGost);
        arrMod.push_back(modelIntClass);
        RelModels::instance()->updateRels(arrMod);

        RelModels::instance()->getModel("el_diam")->refresh();
        RelModels::instance()->getModel("vars")->refresh();
        RelModels::instance()->getModel("mark")->refresh();
    }
    modelMark->select();
}


void FormMarkEl::loadVars()
{
    if (mapper->currentIndex()<0){
        modelVar->clear();
        modelGost->clear();
        modelAmp->clear();
        modelAmpTu->clear();
        modelChemTu->clear();
        modelMechTu->clear();
        modelIntClass->clear();
        return;
    }

    int id_el=mapper->modelData(mapper->currentIndex(),"id").toInt();
    int id_var=ui->comboBoxVar->getCurrentData().val.toInt();

    modelVar->setPath("api/elrtr/variants/get/"+QString::number(id_el)+"/"+QString::number(id_var));
    modelVar->select();

    configureVarRels(modelGost,id_el,id_var);
    configureVarRels(modelAmp,id_el,id_var);
    configureVarRels(modelAmpTu,id_el,id_var);
    configureVarRels(modelChemTu,id_el,id_var);
    configureVarRels(modelMechTu,id_el,id_var);
    configureVarRels(modelIntClass,id_el,id_var);
}

void FormMarkEl::loadVarsFinished()
{
    bool ok = modelVar->rowCount();
    blockVar(!ok);
    if (ok){
        ui->lineEditVarZnam->setText(modelVar->getModelData(0,"znam").toString());
        ui->plainTextEditDescr->setPlainText(modelVar->getModelData(0,"descr").toString());
        ui->lineEditProcVar->setText(modelVar->getModelData(0,"proc").toString());
        ui->plainTextEditDescrTu->setPlainText(modelVar->getModelData(0,"descrtu").toString());
        colVal g;
        g.val=modelVar->getModelData(0,"id_gost_type");
        ui->comboBoxVarGost->setCurrentData(g);
    }
    ui->pushButtonSaveVar->setEnabled(false);
}

void FormMarkEl::refreshCont(int index)
{
    int id_el=mapper->modelData(index,"id").toInt();

    modelEan->setDefaultValue("id_el",id_el);
    modelEan->setFilter(RestFilter::rule(modelEan->tableName(),"id_el",RestFilter::Op::Eq,id_el));
    modelEan->select();

    modelPlav->setDefaultValue("id_el",id_el);
    modelPlav->setFilter(RestFilter::rule(modelPlav->tableName(),"id_el",RestFilter::Op::Eq,id_el));
    modelPlav->select();

    modelProvol->setDefaultValue("id_el",id_el);
    modelProvol->setFilter(RestFilter::rule(modelProvol->tableName(),"id_el",RestFilter::Op::Eq,id_el));
    modelProvol->select();

    int id_pix=mapper->modelData(index,"id_pos").toInt();
    QPixmap pix=mapPix.value(id_pix);
    if (!pix.isNull()){
        setCurrentPix(pix);
    } else {
        QNetworkReply *reply = RestConnection::instance()->sendGet(RestConnection::instance()->buildUrl("api/pos-"+QString::number(id_pix)+".png"));
        reply->setProperty("id_pix",id_pix);
        connect(reply,SIGNAL(finished()),this,SLOT(updImgFinished()));
    }

    modelVarList->setPath("api/elrtr/variants/list/"+QString::number(id_el));
    modelVarList->select();

    loadVars();
}

void FormMarkEl::updImgFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) return;

    const auto err = reply->error();
    const QString errStr = reply->errorString();
    reply->deleteLater();

    if (err == QNetworkReply::OperationCanceledError) return;

    const QByteArray data = reply->readAll();
    if (err != QNetworkReply::NoError) {
        RestError::critical(tr("Ошибка"), errStr + "\n" + data);
        return;
    }

    QPixmap pix;
    pix.loadFromData(data);
    if (!pix.isNull()){
        int id_pix = reply->property("id_pix").toInt();
        mapPix.insert(id_pix,pix);
        int index = mapper->currentIndex();
        if (index>=0){
            int current_id_pix = mapper->modelData(index,"id_pos").toInt();
            if (id_pix==current_id_pix){
                setCurrentPix(pix);
            } else {
                pix=mapPix.value(current_id_pix);
                if (!pix.isNull()){
                    setCurrentPix(pix);
                }
            }
        }
    }
}

void FormMarkEl::updVarsList()
{
    QString str = tr("Варианты:");
    for (int i=0; i<modelVarList->rowCount(); i++){
        str+= " " + modelVarList->getModelData(i,"nam").toString() + ";";
    }
    ui->groupBoxVar->setTitle(str);
}

void FormMarkEl::genLbl()
{

}

void FormMarkEl::blockVar(bool b)
{
    ui->pushButtonCreVar->setEnabled(b);
    ui->pushButtonDelVar->setEnabled(!b);
    ui->pushButtonCopy->setEnabled(!b);
    ui->pushButtonPaste->setEnabled(!b);

    ui->lineEditVarZnam->setEnabled(!b);
    ui->plainTextEditDescr->setEnabled(!b);
    ui->lineEditProcVar->setEnabled(!b);
    if (b){
        ui->lineEditVarZnam->clear();
        ui->plainTextEditDescr->clear();
        ui->lineEditProcVar->clear();
        ui->plainTextEditDescrTu->clear();
        colVal g;
        g.val=-1;
        ui->comboBoxVarGost->setCurrentData(g);
    }
}

void FormMarkEl::createVar()
{
    if (mapper->currentIndex()<0){
        return;
    }

    int id_el=mapper->modelData(mapper->currentIndex(),"id").toInt();
    int id_var=ui->comboBoxVar->getCurrentData().val.toInt();
    QByteArray data;
    bool ok = RestConnection::instance()->sendSyncRequest("api/elrtr/variants/post/"+QString::number(id_el)+"/"+QString::number(id_var),"POST","{}",data);
    if (ok) {
        modelVarList->select();
        loadVars();
    }
}

void FormMarkEl::saveVar()
{
    if (mapper->currentIndex() < 0) {
        return;
    }

    int id_el = mapper->modelData(mapper->currentIndex(), "id").toInt();
    int id_var = ui->comboBoxVar->getCurrentData().val.toInt();

    QByteArray data;
    QJsonDocument doc;
    QJsonObject object
        {
          {"znam", ui->lineEditVarZnam->text()},
          {"descr", ui->plainTextEditDescr->toPlainText()},
          {"proc", ui->lineEditProcVar->text()},
          {"descrtu", ui->plainTextEditDescrTu->toPlainText()},
          {"id_gost_type", ui->comboBoxVarGost->getCurrentData().val.toInt()},
    };

    doc.setObject(object);
    QByteArray body = doc.toJson(QJsonDocument::Compact);

    QString url = QString("api/elrtr/variants/put/%1/%2").arg(id_el).arg(id_var);

    bool ok = RestConnection::instance()->sendSyncRequest(url, "PUT", body, data);

    ui->pushButtonSaveVar->setEnabled(!ok);
}

void FormMarkEl::deleteVar()
{
    if (mapper->currentIndex()<0){
        return;
    }
    int n=QMessageBox::question(this,tr("Подтвердите удаление"),tr("Удалить вариант \"")+ui->comboBoxVar->currentText()+"\"",QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
        int id_el=mapper->modelData(mapper->currentIndex(),"id").toInt();
        int id_var=ui->comboBoxVar->getCurrentData().val.toInt();
        QByteArray data;
        bool ok = RestConnection::instance()->sendSyncRequest("api/elrtr/variants/delete/"+QString::number(id_el)+"/"+QString::number(id_var),"DELETE","{}",data);
        if (ok) {
            modelVarList->select();
            loadVars();
        }
    }
}

void FormMarkEl::varChanged()
{
    ui->pushButtonSaveVar->setEnabled(true);
}


void FormMarkEl::copyTableData()
{
    if (mapper->currentIndex()<0){
        return;
    }
    id_el_src = mapper->modelData(mapper->currentIndex(),"id").toInt();
    id_var_src = ui->comboBoxVar->getCurrentData().val.toInt();
}

void FormMarkEl::pasteTableData()
{
    if (mapper->currentIndex()<0 || id_el_src<0 || id_var_src<0){
        return;
    }

    QString src="";

    int rowEl = RelModels::instance()->getModel("mark")->findRowByKey(id_el_src);
    if (rowEl>=0){
        src+=RelModels::instance()->getModel("mark")->getModelData(rowEl).disp;
    }
    int rowVar = RelModels::instance()->getModel("vars")->findRowByKey(id_var_src);
    if (rowVar>=0){
        src+=":"+RelModels::instance()->getModel("vars")->getModelData(rowVar).disp;
    }
    if (!src.isEmpty()) {
        src = " "+tr("из")+" "+src;
    }

    int n=QMessageBox::question(this,tr("Подтвердите действие"),tr("Скопировать параметры варианта")+src+"? "+tr("Это приведет к перезаписи текущих!"),QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
        int id_el=mapper->modelData(mapper->currentIndex(),"id").toInt();
        int id_var=ui->comboBoxVar->getCurrentData().val.toInt();

        QByteArray data;
        QJsonDocument doc;
        QJsonObject object
            {
              {"id_el_src", id_el_src},
              {"id_var_src", id_var_src},
              };

        doc.setObject(object);
        QByteArray body = doc.toJson(QJsonDocument::Compact);

        QString url = QString("api/elrtr/variants/copy/%1/%2").arg(id_el).arg(id_var);

        bool ok = RestConnection::instance()->sendSyncRequest(url, "POST", body, data);
        if (ok) {
            modelVarList->select();
            loadVars();
        }
    }
}

void FormMarkEl::edtVar()
{
    RestTableDialog d("vars");
    d.model->select();
    d.setWindowTitle(tr("Редактирование названий вариантов"));
    if (d.exec()==QDialog::Accepted){
        RelModels::instance()->getModel("vars")->refresh();
        QVariantList pks = d.currentPk();
        if (pks.size()==1){
            colVal c;
            c.val= pks.at(0).toInt();
            ui->comboBoxVar->setCurrentData(c);
        }
    } else {
        RelModels::instance()->getModel("vars")->refresh();
    }
}

CustomDelegate::CustomDelegate(QObject *parent) : RestItemDelegate(parent)
{

}

void CustomDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const auto *restTableModel = qobject_cast<const RestTableModel *>(index.model());
    if (!restTableModel) {
        return RestItemDelegate::setEditorData(editor, index);
    }

    if (index.column() == restTableModel->columnIndex("pr2")) {
        QLineEdit *line = qobject_cast<QLineEdit *>(editor);
        if (line) {
            QString pr = index.model()->data(index, Qt::EditRole).toString();
            // Qt 5 совместимый вариант split
            QStringList list = pr.split(QLatin1Char(':'));

            if (list.size() == 4) {
                // В Qt 5 нет перегрузки .arg() для 4-х строк.
                // Разбиваем на два вызова, что полностью удовлетворяет clazy:
                // Первый вызов заменяет %1, %2, %3. Второй заменяет %4.
                line->setText(QString("%1±%2°C %3 %4")
                                  .arg(list.at(0), list.at(1), list.at(2))
                                  .arg(list.at(3)));
            } else {
                line->clear();
            }
        }
    } else {
        RestItemDelegate::setEditorData(editor, index);
    }
}

void CustomDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto *restTableModel = qobject_cast<RestTableModel *>(model);
    if (!restTableModel) {
        return RestItemDelegate::setModelData(editor, model, index);
    }

    if (index.column() == restTableModel->columnIndex("pr2")) {
        QLineEdit *line = qobject_cast<QLineEdit *>(editor);
        if (line) {
            QString text = line->text().trimmed();

            QRegularExpression reg(R"(^\s*(\d+)\s*±\s*(\d+)\s*°C\s+(\d+)\s+([^\s:]+)\s*$)");
            QRegularExpressionMatch match = reg.match(text);

            if (match.hasMatch()) {
                // Qt 5 совместимый сборщик строки:
                // Передаем сначала 3 аргумента, затем 4-й
                QString s = QString("%1:%2:%3:%4")
                                .arg(match.captured(1), match.captured(2), match.captured(3))
                                .arg(match.captured(4));
                model->setData(index, s);
            } else {
                model->setData(index, QString());
            }
        }
    } else {
        RestItemDelegate::setModelData(editor, model, index);
    }
}
