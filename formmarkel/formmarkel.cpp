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

    /*modelAmpTu = new DbTableModel("amp_tu",this);
    modelAmpTu->addColumn("id","id");
    modelAmpTu->addColumn("id_el","id_el");
    modelAmpTu->addColumn("id_var","id_var");
    modelAmpTu->addColumn("id_diam",QString::fromUtf8("Ф, мм"),Rels::instance()->relDiam);
    modelAmpTu->addColumn("lay",QString::fromUtf8("Слой шва"));
    modelAmpTu->addColumn("id_polar",QString::fromUtf8("Полярность"),Rels::instance()->relPolar);
    modelAmpTu->addColumn("bot",QString::fromUtf8("Нижнее"));
    modelAmpTu->addColumn("vert",QString::fromUtf8("Вертик."));
    modelAmpTu->addColumn("ceil",QString::fromUtf8("Потолоч."));
    modelAmpTu->setSort("diam.sdim, amp_tu.id");

    ui->tableViewAmpTu->setModel(modelAmpTu);
    ui->tableViewAmpTu->setColumnHidden(0,true);
    ui->tableViewAmpTu->setColumnHidden(1,true);
    ui->tableViewAmpTu->setColumnHidden(2,true);
    ui->tableViewAmpTu->setColumnWidth(3,60);
    ui->tableViewAmpTu->setColumnWidth(4,180);
    ui->tableViewAmpTu->setColumnWidth(5,80);
    ui->tableViewAmpTu->setColumnWidth(6,75);
    ui->tableViewAmpTu->setColumnWidth(7,75);
    ui->tableViewAmpTu->setColumnWidth(8,75);

    modelChemTu = new DbTableModel("chem_tu",this);
    modelChemTu->addColumn("id_el","id_el");
    modelChemTu->addColumn("id_var","id_var");
    modelChemTu->addColumn("id_chem",QString::fromUtf8("Элемент"),Rels::instance()->relChem);
    modelChemTu->addColumn("min",QString::fromUtf8("Минимум, %"));
    modelChemTu->addColumn("max",QString::fromUtf8("Максимум, %"));
    modelChemTu->setSort("chem_tu.id_chem");
    modelChemTu->setDecimals(3,3);
    modelChemTu->setDecimals(4,3);

    ui->tableViewChem->setModel(modelChemTu);
    ui->tableViewChem->setColumnHidden(0,true);
    ui->tableViewChem->setColumnHidden(1,true);
    ui->tableViewChem->setColumnWidth(2,100);
    ui->tableViewChem->setColumnWidth(3,100);
    ui->tableViewChem->setColumnWidth(4,100);

    modelMechTu = new DbTableModel("mech_tu",this);
    modelMechTu->addColumn("id_el","id_el");
    modelMechTu->addColumn("id_var","id_var");
    modelMechTu->addColumn("id_mech",QString::fromUtf8("Параметр"),Rels::instance()->relMech);
    modelMechTu->addColumn("min",QString::fromUtf8("Минимум"));
    modelMechTu->addColumn("max",QString::fromUtf8("Максимум"));
    modelMechTu->setSort("mech_tu.id_mech");
    modelMechTu->setDecimals(3,3);
    modelMechTu->setDecimals(4,3);

    ui->tableViewMech->setModel(modelMechTu);
    ui->tableViewMech->setColumnHidden(0,true);
    ui->tableViewMech->setColumnHidden(1,true);
    ui->tableViewMech->setColumnWidth(2,150);
    ui->tableViewMech->setColumnWidth(3,90);
    ui->tableViewMech->setColumnWidth(4,90);

    modelIntClass = new DbTableModel("el_int_class",this);
    modelIntClass->addColumn("id_el","id_el");
    modelIntClass->addColumn("id_var","id_var");
    modelIntClass->addColumn("id_class",QString::fromUtf8("Классификация"),Rels::instance()->relIntClass);
    modelIntClass->addColumn("val",QString::fromUtf8("Обозначение"));
    ui->tableViewInt->setModel(modelIntClass);
    ui->tableViewInt->setColumnHidden(0,true);
    ui->tableViewInt->setColumnHidden(1,true);
    ui->tableViewInt->setColumnWidth(2,120);
    ui->tableViewInt->setColumnWidth(3,120);*/

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
    connect(ui->pushButtonLbl,SIGNAL(clicked(bool)),this,SLOT(gelLbl()));
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
    }
    modelMark->select();
}


void FormMarkEl::loadVars()
{
    int id_el=mapper->modelData(mapper->currentIndex(),"id").toInt();
    int id_var=ui->comboBoxVar->getCurrentData().val.toInt();

    modelVar->setPath("api/elrtr/variants/get/"+QString::number(id_el)+"/"+QString::number(id_var));
    modelVar->select();

    configureVarRels(modelGost,id_el,id_var);
    configureVarRels(modelAmp,id_el,id_var);

    /*modelAmpTu->setDefaultValue(1,id_e);
    modelAmpTu->setDefaultValue(2,id_v);
    modelAmpTu->setFilter("amp_tu.id_el = "+QString::number(id_e)+" and amp_tu.id_var = "+QString::number(id_v));
    modelAmpTu->select();

    modelChemTu->setDefaultValue(0,id_e);
    modelChemTu->setDefaultValue(1,id_v);
    modelChemTu->setFilter("chem_tu.id_el = "+QString::number(id_e)+" and chem_tu.id_var = "+QString::number(id_v));
    modelChemTu->select();

    modelMechTu->setDefaultValue(0,id_e);
    modelMechTu->setDefaultValue(1,id_v);
    modelMechTu->setFilter("mech_tu.id_el = "+QString::number(id_e)+" and mech_tu.id_var = "+QString::number(id_v));
    modelMechTu->select();

    modelIntClass->setDefaultValue(0,id_e);
    modelIntClass->setDefaultValue(1,id_v);
    modelIntClass->setFilter("el_int_class.id_el = "+QString::number(id_e)+" and el_int_class.id_var = "+QString::number(id_v));
    modelIntClass->select();*/

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

void FormMarkEl::gelLbl()
{

}

void FormMarkEl::blockVar(bool b)
{
    ui->pushButtonCreVar->setEnabled(b);
    ui->pushButtonDelVar->setEnabled(!b);
    ui->pushButtonCopy->setEnabled(!b);

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
    /*QString znam=ui->comboBoxZnam->currentText();

    QSqlQuery query;
    query.prepare("insert into el_var (id_el, id_var, znam, descr, proc, descrtu, id_gost_type) values "
                  "(:id_el, :id_var, :znam, (select e.descr from elrtr as e where e.id = :id_el ), :proc, "
                  "(select descrtu from el_var where id_el = :id_el and id_var=1), (select e.id_gost_type from elrtr as e where e.id = :id_el ))");
    query.bindValue(":id_el",id_el());
    query.bindValue(":id_var",id_var());
    query.bindValue(":znam",znam);
    query.bindValue(":proc",ui->lineEditPr->text());
    if (!query.exec()){
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }

    copyTableData();
    updVarsList();*/
}

void FormMarkEl::saveVar()
{
    /*QString znam=ui->lineEditVarZnam->text();
    QString descr=ui->plainTextEditDescr->toPlainText();
    QString descrTu=ui->plainTextEditDescrTu->toPlainText();

    QSqlQuery query;
    query.prepare("update el_var set znam = :znam, descr = :descr, proc = :proc, descrtu = :descrtu, id_gost_type = :id_type where id_el = :id_el and id_var = :id_var");
    query.bindValue(":id_el",id_el());
    query.bindValue(":id_var",id_var());
    query.bindValue(":znam",znam);
    query.bindValue(":descr",descr);
    query.bindValue(":proc",ui->lineEditProcVar->text());
    query.bindValue(":descrtu",descrTu.isEmpty()? nullptr : descrTu);
    query.bindValue(":id_type",ui->comboBoxVarGost->getCurrentData().val);
    if (query.exec()){
        loadVars();
    } else {
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }*/
}

void FormMarkEl::deleteVar()
{
    /*int n=QMessageBox::question(this,tr("Подтвердите удаление"),tr("Удалить вариант \"")+ui->comboBoxVar->currentText()+"\"",QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes){
        QSqlQuery query;
        query.prepare("delete from el_var where id_el = :id_el and id_var = :id_var");
        query.bindValue(":id_el",id_el());
        query.bindValue(":id_var",id_var());
        if (query.exec()){
            loadVars();
            updVarsList();
        } else {
            QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
        }
    }*/
}

void FormMarkEl::varChanged()
{
    ui->pushButtonSaveVar->setEnabled(true);
}


void FormMarkEl::copyTableData()
{
    /*if (modelAmp->isEmpty()){
        QSqlQuery queryAmp;
        queryAmp.prepare("insert into amp (id_el, id_diam, bot, vert, ceil, id_var) "
                         "(select id_el, id_diam, bot, vert, ceil, :id_var from amp where id_el = :id_el and id_var = 1)");
        queryAmp.bindValue(":id_el",id_el());
        queryAmp.bindValue(":id_var",id_var());
        if (!queryAmp.exec()){
            QMessageBox::critical(this,tr("Ошибка"),queryAmp.lastError().text(),QMessageBox::Cancel);
        }
    }

    if (modelChemTu->isEmpty()){
        QSqlQuery queryChem;
        queryChem.prepare("insert into chem_tu (id_el, id_var, id_chem, min, max) "
                         "(select id_el, :id_var, id_chem, min, max from chem_tu where id_el = :id_el and id_var = 1)");
        queryChem.bindValue(":id_el",id_el());
        queryChem.bindValue(":id_var",id_var());
        if (!queryChem.exec()){
            QMessageBox::critical(this,tr("Ошибка"),queryChem.lastError().text(),QMessageBox::Cancel);
        }
    }

    if (modelMechTu->isEmpty()){
        QSqlQuery queryMech;
        queryMech.prepare("insert into mech_tu (id_el, id_var, id_mech, min, max) "
                         "(select id_el, :id_var, id_mech, min, max from mech_tu where id_el = :id_el and id_var = 1)");
        queryMech.bindValue(":id_el",id_el());
        queryMech.bindValue(":id_var",id_var());
        if (!queryMech.exec()){
            QMessageBox::critical(this,tr("Ошибка"),queryMech.lastError().text(),QMessageBox::Cancel);
        }
    }

    if (modelGost->isEmpty()){
        QSqlQuery queryGost;
        queryGost.prepare("insert into gost_el (id_el, id_var, id_gost, dat, dat_end) "
                         "(select id_el, :id_var, id_gost, dat, dat_end from gost_el where id_el = :id_el and id_var = 1)");
        queryGost.bindValue(":id_el",id_el());
        queryGost.bindValue(":id_var",id_var());
        if (!queryGost.exec()){
            QMessageBox::critical(this,tr("Ошибка"),queryGost.lastError().text(),QMessageBox::Cancel);
        }
    }

    if (modelAmpTu->isEmpty()){
        QSqlQuery queryAmpTu;
        queryAmpTu.prepare("insert into amp_tu (id_el, id_diam, bot, vert, ceil, id_var, lay, id_polar) "
                         "(select id_el, id_diam, bot, vert, ceil, :id_var, lay, id_polar from amp_tu where id_el = :id_el and id_var = 1 order by id)");
        queryAmpTu.bindValue(":id_el",id_el());
        queryAmpTu.bindValue(":id_var",id_var());
        if (!queryAmpTu.exec()){
            QMessageBox::critical(this,tr("Ошибка"),queryAmpTu.lastError().text(),QMessageBox::Cancel);
        }
    }

    loadVars();*/
}

void FormMarkEl::edtVar()
{
    /*DbRelationEditDialog d(Rels::instance()->relVar);
    if (d.exec()==QDialog::Accepted){
        colVal c = d.currentData();
        ui->comboBoxVar->setCurrentData(c);
    }*/
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
    if (index.column()==restTableModel->columnIndex("pr2")){
        QLineEdit *line = qobject_cast<QLineEdit *>(editor);
        if (line){
            QString pr=index.model()->data(index,Qt::EditRole).toString();
            QStringList list=pr.split(":");
            if (list.size()==4){
                line->setText(QString("%1±%2°C %3 %4").arg(list.at(0)).arg(list.at(1)).arg(list.at(2)).arg(list.at(3)));
            } else {
                line->clear();
            }
        }

    } else {
        return RestItemDelegate::setEditorData(editor,index);
    }
}

void CustomDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto *restTableModel = qobject_cast<RestTableModel *>(model);
    if (!restTableModel) {
        return RestItemDelegate::setModelData(editor, model, index);
    }
    if (index.column()==restTableModel->columnIndex("pr2")){
        QLineEdit *line = qobject_cast<QLineEdit *>(editor);
        if (line){
            QString text=line->text();
            QRegularExpression reg("^\\s*(\\d+)\\s*±\\s*(\\d+)\\s*°C\\s+(\\d+)\\s+(\\w+)");
            reg.setPatternOptions(QRegularExpression::UseUnicodePropertiesOption);
            QRegularExpressionMatch match = reg.match(text);
            QString s;
            if (match.hasMatch()){
                for (int i=1; i<=match.lastCapturedIndex(); i++){
                    if(!s.isEmpty()){
                        s+=":";
                    }
                    s+=match.captured(i);
                }
            }
            model->setData(index,s);
        }
    } else {
        return RestItemDelegate::setModelData(editor,model,index);
    }
}
