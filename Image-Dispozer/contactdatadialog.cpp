#include "contactdatadialog.h"
#include "ui_contactdatadialog.h"

contactDataDialog::contactDataDialog(QString * start, QString * stat, QString * alarm, QString * proto,
                                     QString * callNum1, QString * callNum2, QString * callNum3, QString * callNum4, bool *changed,
                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::contactDataDialog)
{
    ui->setupUi(this);

    start_sms_number = start;
    stat_sms_number = stat;
    alarm_sms_number = alarm;
    proto_sms_number = proto;
    call_number_1 = callNum1;
    call_number_2 = callNum2;
    call_number_3 = callNum3;
    call_number_4 = callNum4;

    ui->startSmsEdit->setText(*start_sms_number);
    ui->statusSmsEdit->setText(*stat_sms_number);
    ui->alarmSmsEdit->setText(*alarm_sms_number);
    ui->protocolSmsEdit->setText(*proto_sms_number);

    ui->callNumEdit1->setText(*call_number_1);
    ui->callNumEdit2->setText(*call_number_2);
    ui->callNumEdit3->setText(*call_number_3);
    ui->callNumEdit4->setText(*call_number_4);

    changed_t = changed;

    connect(ui->confirmationButton, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

contactDataDialog::~contactDataDialog()
{
    delete ui;
}

void contactDataDialog::accepted()
{

    // check values before saving
    QRegExp t_number("(^([+]|\\d)(\\d*(\\s|\\d))*\\d$)");

    // if something goes wrong, put info here
    QString message;

    if(!ui->startSmsEdit->text().isEmpty())
    {
        if(t_number.indexIn(ui->startSmsEdit->text(), 0)!=0) message.append(tr("Start number is incorrect.\n"));
    }
    if(!ui->statusSmsEdit->text().isEmpty())
    {
        if(t_number.indexIn(ui->statusSmsEdit->text(), 0)!=0) message.append(tr("Status number is incorrect.\n"));
    }
    if(!ui->alarmSmsEdit->text().isEmpty())
    {
        if(t_number.indexIn(ui->alarmSmsEdit->text(), 0)!=0) message.append(tr("Alarm number is incorrect.\n"));
    }
    if(!ui->protocolSmsEdit->text().isEmpty())
    {
        if(t_number.indexIn(ui->protocolSmsEdit->text(), 0)!=0) message.append(tr("Protocol number is incorrect.\n"));
    }
    if(!ui->callNumEdit1->text().isEmpty())
    {
        if(t_number.indexIn(ui->callNumEdit1->text(), 0)!=0) message.append(tr("Call number 1 is incorrect.\n"));
    }
    if(!ui->callNumEdit2->text().isEmpty())
    {
        if(t_number.indexIn(ui->callNumEdit2->text(), 0)!=0) message.append(tr("Call number 2 is incorrect.\n"));
    }
    if(!ui->callNumEdit3->text().isEmpty())
    {
        if(t_number.indexIn(ui->callNumEdit3->text(), 0)!=0) message.append(tr("Call number 3 is incorrect.\n"));
    }
    if(!ui->callNumEdit4->text().isEmpty())
    {
        if(t_number.indexIn(ui->callNumEdit4->text(), 0)!=0) message.append(tr("Call number 4 is incorrect.\n"));
    }

    // if some errors occured we will abandon closing sequence

    if(!message.isEmpty())
    {
        QMessageBox::warning(this, tr("Some numbers are incorrect"), message, QMessageBox::Ok);
        return;
    }

    // save values
    start_sms_number->clear();
    start_sms_number->append(ui->startSmsEdit->text());
    stat_sms_number->clear();
    stat_sms_number->append(ui->statusSmsEdit->text());
    alarm_sms_number->clear();
    alarm_sms_number->append(ui->alarmSmsEdit->text());
    proto_sms_number->clear();
    proto_sms_number->append(ui->protocolSmsEdit->text());

    call_number_1->clear();
    call_number_1->append(ui->callNumEdit1->text());
    call_number_2->clear();
    call_number_2->append(ui->callNumEdit2->text());
    call_number_3->clear();
    call_number_3->append(ui->callNumEdit3->text());
    call_number_4->clear();
    call_number_4->append(ui->callNumEdit4->text());

    *changed_t = true;

    this->close();
}
