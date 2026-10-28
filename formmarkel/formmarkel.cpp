#include "formmarkel.h"
#include "ui_formmarkel.h"

FormMarkEl::FormMarkEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMarkEl)
{
    ui->setupUi(this);
    loadsettings();

    ui->lineEditPr->setInputMask(QString("099±90°C 009 xxxxx"));
    /*ui->dateEdit->setDate(QDate::currentDate());

    ui->comboBoxVarGost->setModel(Rels::instance()->relGostType->model());

    colVal defDim;
    defDim.val=4;
    if (!Rels::instance()->relDiam->model()->isInital()){
        Rels::instance()->relDiam->refreshModel();
    }
    ui->comboBoxDiam->setModel(Rels::instance()->relDiam->model());
    ui->comboBoxDiam->setCurrentData(defDim);
    ui->comboBoxDiam->setEditable(false);

    modelProvol = new DbTableModel("el_provol",this);
    modelProvol->addColumn("id_el","id_el");
    modelProvol->addColumn("id_prov",tr("Проволока"),Rels::instance()->relProvol);

    ui->tableViewProvol->setModel(modelProvol);
    ui->tableViewProvol->setColumnHidden(0,true);
    ui->tableViewProvol->setColumnWidth(1,200);

    colVal defVar;
    defVar.val=1;
    if (!Rels::instance()->relVar->isInital()){
        Rels::instance()->relVar->refreshModel();
    }
    ui->comboBoxVar->setModel(Rels::instance()->relVar->model());
    ui->comboBoxVar->setCurrentData(defVar);
    ui->comboBoxVar->setEditable(false);

    modelEan = new DbTableModel("ean_el",this);
    modelEan->addColumn("id_el","id_el");
    modelEan->addColumn("id_diam",tr("Диам."),Rels::instance()->relDiam);
    modelEan->addColumn("id_pack",tr("Упаковка (ед., групп.)"),Rels::instance()->relPack);
    modelEan->addColumn("ean_ed",tr("Штрих код (ед.)"),Rels::instance()->relEanEd);
    modelEan->addColumn("ean_group",tr("Штрих код (гр.)"),Rels::instance()->relEanGr);
    modelEan->setSort("diam.sdim");
    ui->tableViewPack->setModel(modelEan);
    ui->tableViewPack->setColumnHidden(0,true);
    ui->tableViewPack->setColumnWidth(1,60);
    ui->tableViewPack->setColumnWidth(2,190);
    ui->tableViewPack->setColumnWidth(3,125);
    ui->tableViewPack->setColumnWidth(4,125);

    DbDelegate *delegate=qobject_cast<DbDelegate *>(ui->tableViewPack->itemDelegate());
    if (delegate){
        connect(delegate,SIGNAL(createEdt(QModelIndex)),this,SLOT(updEanRel(QModelIndex)));
    }

    modelGost = new DbTableModel("gost_el",this);
    modelGost->addColumn("id_el",tr("Марка"));
    modelGost->addColumn("id_var",tr("Вариант"));
    modelGost->addColumn("dat",tr("Дата нач."));
    modelGost->addColumn("id_gost",tr("Документ"),Rels::instance()->relGost);
    modelGost->addColumn("dat_end",tr("Дата кон."));
    modelGost->setSort("gost_el.dat, gost_new.nam");
    modelGost->setDefaultValue(4,QVariant());
    ui->tableViewGost->setModel(modelGost);
    ui->tableViewGost->setColumnHidden(0,true);
    ui->tableViewGost->setColumnHidden(1,true);
    ui->tableViewGost->setColumnWidth(2,80);
    ui->tableViewGost->setColumnWidth(3,200);
    ui->tableViewGost->setColumnWidth(4,80);

    modelAmp = new DbTableModel("amp",this);
    modelAmp->addColumn("id","id");
    modelAmp->addColumn("id_el","id_el");
    modelAmp->addColumn("id_var","id_var");
    modelAmp->addColumn("id_diam",QString::fromUtf8("Ф, мм"),Rels::instance()->relDiam);
    modelAmp->addColumn("bot",QString::fromUtf8("Нижнее"));
    modelAmp->addColumn("vert",QString::fromUtf8("Вертикаль."));
    modelAmp->addColumn("ceil",QString::fromUtf8("Потолоч."));
    modelAmp->setSort("diam.sdim");

    modelAmpTu = new DbTableModel("amp_tu",this);
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

    ui->tableViewAmp->setModel(modelAmp);
    ui->tableViewAmp->setColumnHidden(0,true);
    ui->tableViewAmp->setColumnHidden(1,true);
    ui->tableViewAmp->setColumnHidden(2,true);
    ui->tableViewAmp->setColumnWidth(3,70);
    ui->tableViewAmp->setColumnWidth(4,90);
    ui->tableViewAmp->setColumnWidth(5,90);
    ui->tableViewAmp->setColumnWidth(6,90);

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

    modelPlav = new DbTableModel("el_plav",this);
    modelPlav->addColumn("id_el","id_el");
    modelPlav->addColumn("id_plav",QString::fromUtf8("Параметр"),Rels::instance()->relPlav);
    modelPlav->addColumn("value",QString::fromUtf8("Значение"));
    modelPlav->setDecimals(2,2);

    ui->tableViewPlav->setModel(modelPlav);
    ui->tableViewPlav->setColumnHidden(0,true);
    ui->tableViewPlav->setColumnWidth(1,250);
    ui->tableViewPlav->setColumnWidth(2,80);

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
    /*modelMark->addColumn("id","id");
    modelMark->addColumn("marka",QString::fromUtf8("Марка"));
    modelMark->addColumn("marka_sert",QString::fromUtf8("Название для сертификата"));
    modelMark->addColumn("id_grp",QString::fromUtf8("Группа"),Rels::instance()->relGrp);
    modelMark->addColumn("id_vid",QString::fromUtf8("Вид"),Rels::instance()->relVid);
    modelMark->addColumn("id_gost",QString::fromUtf8("Проволока"),Rels::instance()->relProvol);
    modelMark->addColumn("id_pic",QString::fromUtf8("Положение при сварке"),Rels::instance()->relPol);
    modelMark->addColumn("id_gost_type",QString::fromUtf8("Тип по ГОСТ"),Rels::instance()->relGostType);
    modelMark->addColumn("id_iso_type",QString::fromUtf8("Тип по ISO"),Rels::instance()->relIso);
    modelMark->addColumn("id_aws_type",QString::fromUtf8("Тип по AWS"),Rels::instance()->relAws);
    modelMark->addColumn("id_denominator",QString::fromUtf8("Знаменатель"),Rels::instance()->relZnam);
    modelMark->addColumn("id_purpose",QString::fromUtf8("Буквенное обозначение"),Rels::instance()->relBukv);
    modelMark->addColumn("descr",QString::fromUtf8("Описание"));
    modelMark->addColumn("vl",QString::fromUtf8("Допустимое содержание влаги"));
    modelMark->addColumn("pr2",QString::fromUtf8("Режим повторной прокалки"));
    modelMark->addColumn("id_u",QString::fromUtf8("Индекс сортировки"));
    modelMark->addColumn("katalog",QString::fromUtf8("Каталог"));
    modelMark->addColumn("descr_spec",QString::fromUtf8("Особые свойства"));
    modelMark->addColumn("descr_feature",QString::fromUtf8("Особенности сварки"));
    modelMark->setSort("elrtr.marka");
    modelMark->setFilter("elrtr.id<>0");
    modelMark->select();*/
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
    mapper->addMapping(ui->lineEditInd,"id_u");
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

    /*connect(mapper,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshCont(int)));
    connect(modelMark,SIGNAL(sigUpd()),Rels::instance()->relElMark,SLOT(refreshModel()));
    connect(ui->cmdLbl,SIGNAL(clicked(bool)),this,SLOT(gelLbl()));
    connect(ui->cmdExt,SIGNAL(clicked(bool)),this,SIGNAL(reqUpdSite()));
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
    connect(ui->toolButtonEdtVar,SIGNAL(clicked(bool)),this,SLOT(edtVar()));*/

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

int FormMarkEl::id_el()
{
    QModelIndex ind=ui->tableViewMark->model()->index(mapper->currentIndex(),0);
    return ui->tableViewMark->model()->data(ind,Qt::EditRole).toInt();
}

int FormMarkEl::id_diam()
{
    return ui->comboBoxDiam->getCurrentData().val.toInt();
}

int FormMarkEl::id_var()
{
    return ui->comboBoxVar->getCurrentData().val.toInt();
}

void FormMarkEl::updEanRel(QModelIndex index)
{
    /*if (index.column()==3){
        Rels::instance()->relEanEd->refreshModel();
    } else if (index.column()==4){
        Rels::instance()->relEanGr->refreshModel();
    }*/
}

void FormMarkEl::loadVars()
{
    /*if (sender()==ui->comboBoxVar && ui->comboBoxVar->currentIndex()<0){
        return;
    }
    int id_e=id_el();
    int id_v=id_var();
    bool ok=false;
    QSqlQuery query;
    query.prepare("select znam, descr, proc, descrtu, id_gost_type from el_var where id_el = :id_el and id_var=:id_var");
    query.bindValue(":id_el",id_e);
    query.bindValue(":id_var",id_v);
    if (query.exec()){
        if (query.size()>0){
            query.next();
            ok=true;
            ui->lineEditVarZnam->setText(query.value(0).toString());
            ui->plainTextEditDescr->setPlainText(query.value(1).toString());
            ui->lineEditProcVar->setText(query.value(2).toString());
            ui->plainTextEditDescrTu->setPlainText(query.value(3).toString());
            colVal g;
            g.val=query.value(4).toInt();
            ui->comboBoxVarGost->setCurrentData(g);
        }
    } else {
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
    ui->pushButtonSaveVar->setEnabled(false);

    modelAmp->setDefaultValue(1,id_e);
    modelAmp->setDefaultValue(2,id_v);
    modelAmp->setFilter("amp.id_el = "+QString::number(id_e)+" and amp.id_var = "+QString::number(id_v));
    modelAmp->select();

    modelAmpTu->setDefaultValue(1,id_e);
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

    modelGost->setDefaultValue(0,id_e);
    modelGost->setDefaultValue(1,id_v);
    modelGost->setFilter("gost_el.id_el = "+QString::number(id_e)+" and gost_el.id_var = "+QString::number(id_v));
    modelGost->select();

    modelIntClass->setDefaultValue(0,id_e);
    modelIntClass->setDefaultValue(1,id_v);
    modelIntClass->setFilter("el_int_class.id_el = "+QString::number(id_e)+" and el_int_class.id_var = "+QString::number(id_v));
    modelIntClass->select();

    blockVar(!ok);*/
}

void FormMarkEl::refreshCont(int index)
{
    /*QModelIndex ind=ui->tableViewMark->model()->index(index,0);
    int id_el=ui->tableViewMark->model()->data(ind,Qt::EditRole).toInt();

    modelEan->setDefaultValue(0,id_el);
    modelEan->setFilter("ean_el.id_el = "+QString::number(id_el));
    modelEan->select();

    modelPlav->setDefaultValue(0,id_el);
    modelPlav->setFilter("el_plav.id_el = "+QString::number(id_el));
    modelPlav->select();

    modelProvol->setDefaultValue(0,id_el);
    modelProvol->setFilter("el_provol.id_el = "+QString::number(id_el));
    modelProvol->select();

    updImg();
    updVarsList();
    loadVars();*/
}

void FormMarkEl::updImg()
{
    /*QModelIndex ind=ui->tableViewMark->model()->index(mapper->currentIndex(),6);
    int id_pix=ui->tableViewMark->model()->data(ind,Qt::EditRole).toInt();
    QPixmap pix;
    pix.loadFromData(Rels::instance()->mapPolPix.value(id_pix));
    if (pix.isNull()){
        ui->labelPix->setPixmap(pix);
    } else {
        ui->labelPix->setPixmap(pix.scaled(ui->labelPix->size(),Qt::KeepAspectRatio));
    }*/
}

void FormMarkEl::updVarsList()
{
    /*QString str = tr("Варианты:");
    QModelIndex ind=ui->tableViewMark->model()->index(mapper->currentIndex(),0);
    int id_el=ui->tableViewMark->model()->data(ind,Qt::EditRole).toInt();
    QSqlQuery query;
    query.prepare("select ev2.nam  from el_var ev "
                  "inner join elrtr_vars ev2 on ev2.id = ev.id_var "
                  "where id_el = :id_el order by ev.id_var");
    query.bindValue(":id_el",id_el);
    if (query.exec()){
        while (query.next()){
            str+=" "+query.value(0).toString()+";";
        }
    } else {
        QMessageBox::critical(this,tr("Ошибка"),query.lastError().text(),QMessageBox::Cancel);
    }
    ui->groupBoxVar->setTitle(str);*/
}

void FormMarkEl::gelLbl()
{

}

void FormMarkEl::blockVar(bool b)
{
    ui->pushButtonCreVar->setEnabled(b);
    ui->pushButtonDelVar->setEnabled(!b);
    ui->pushButtonCopy->setEnabled(!b && (modelAmp->isEmpty() || modelChemTu->isEmpty() || modelMechTu->isEmpty()|| modelGost->isEmpty()) && id_var()!=1);

    ui->lineEditVarZnam->setEnabled(!b);
    ui->plainTextEditDescr->setEnabled(!b);
    ui->lineEditProcVar->setEnabled(!b);
    if (b){
        ui->lineEditVarZnam->clear();
        ui->plainTextEditDescr->clear();
        ui->lineEditProcVar->clear();
        ui->plainTextEditDescrTu->clear();
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

void FormMarkEl::upd()
{
    modelMark->select();
    /*Rels::instance()->refreshPolPix();
    modelMark->refreshRelsModel();
    Rels::instance()->relPlav->refreshModel();
    modelEan->refreshRelsModel();
    modelChemTu->refreshRelsModel();
    modelMechTu->refreshRelsModel();
    Rels::instance()->relVar->refreshModel();
    Rels::instance()->relPol->refreshModel();*/
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
    if (index.column()==14){
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
        RestItemDelegate::setEditorData(editor,index);
    }
}

void CustomDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column()==14){
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
        RestItemDelegate::setModelData(editor,model,index);
    }
}
