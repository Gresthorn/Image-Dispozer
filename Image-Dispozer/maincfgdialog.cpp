#include "maincfgdialog.h"
#include "ui_maincfgdialog.h"

mainCFGDialog::mainCFGDialog(int *period, QString *per_units, QString *lan, bool *ack_banel_error, int *volume, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mainCFGDialog)
{
    ui->setupUi(this);

    period_p = period;
    per_units_p = per_units;
    lan_p = lan;
    ack_banel_error_p = ack_banel_error;
    volume_p = volume;

    ui->contactPeriodEdit->setValue(*period_p);

    if((*per_units)=="mins") ui->periodUnitsEdit->setCurrentIndex(MINS);
    else if((*per_units)=="hours") ui->periodUnitsEdit->setCurrentIndex(HRS);
    else if((*per_units)=="days") ui->periodUnitsEdit->setCurrentIndex(DAYS);
    else if((*per_units)=="months") ui->periodUnitsEdit->setCurrentIndex(MNTHS);
    else ui->periodUnitsEdit->setCurrentIndex(MINS);

    if((*lan)=="ENG") ui->languageEdit->setCurrentIndex(ENG);
    else if((*lan)=="SVK") ui->languageEdit->setCurrentIndex(SVK);
    else if((*lan)=="CZE") ui->languageEdit->setCurrentIndex(CZE);
    else if((*lan)=="PLN") ui->languageEdit->setCurrentIndex(PLN);
    else if((*lan)=="HUN") ui->languageEdit->setCurrentIndex(HUN);
    else if((*lan)=="GER") ui->languageEdit->setCurrentIndex(GER);
    else ui->languageEdit->setCurrentIndex(ENG);

    if(*ack_banel_error) ui->ackEdit->setCurrentIndex(0);
    else ui->ackEdit->setCurrentIndex(1);

    if((*volume)==0) ui->gsmVolumeEdit->setCurrentIndex(0);
    else if((*volume)==-6) ui->gsmVolumeEdit->setCurrentIndex(1);
    else if((*volume)==-12) ui->gsmVolumeEdit->setCurrentIndex(2);
    else if((*volume)==-18) ui->gsmVolumeEdit->setCurrentIndex(3);
    else if((*volume)==-24) ui->gsmVolumeEdit->setCurrentIndex(4);
    else ui->gsmVolumeEdit->setCurrentIndex(0);


    connect(ui->confirmationButton, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

mainCFGDialog::~mainCFGDialog()
{
    delete ui;
}

void mainCFGDialog::accepted()
{
    *period_p = ui->contactPeriodEdit->value();

    per_units_p->clear();
    if( ui->periodUnitsEdit->currentIndex()==MINS) per_units_p->append("mins");
    else if( ui->periodUnitsEdit->currentIndex()==HRS) per_units_p->append("hours");
    else if( ui->periodUnitsEdit->currentIndex()==DAYS) per_units_p->append("days");
    else if( ui->periodUnitsEdit->currentIndex()==MNTHS) per_units_p->append("months");
    else per_units_p->append("mins");

    lan_p->clear();
    if(ui->languageEdit->currentIndex()==ENG) lan_p->append("ENG");
    else if(ui->languageEdit->currentIndex()==CZE) lan_p->append("CZE");
    else if(ui->languageEdit->currentIndex()==SVK) lan_p->append("SVK");
    else if(ui->languageEdit->currentIndex()==PLN) lan_p->append("PLN");
    else if(ui->languageEdit->currentIndex()==HUN) lan_p->append("HUN");
    else if(ui->languageEdit->currentIndex()==GER) lan_p->append("GER");
    else lan_p->append("ENG");

    if(ui->ackEdit->currentIndex()==0) *ack_banel_error_p=true;
    else *ack_banel_error_p=false;

    if(ui->gsmVolumeEdit->currentIndex()==0) *volume_p=0;
    else if(ui->gsmVolumeEdit->currentIndex()==1) *volume_p=-6;
    else if(ui->gsmVolumeEdit->currentIndex()==2) *volume_p=-12;
    else if(ui->gsmVolumeEdit->currentIndex()==3) *volume_p=-18;
    else if(ui->gsmVolumeEdit->currentIndex()==4) *volume_p=-24;
    else *volume_p=0;

    this->close();
}
