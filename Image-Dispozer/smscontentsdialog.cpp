#include "smscontentsdialog.h"
#include "ui_smscontentsdialog.h"

smsContentsDialog::smsContentsDialog(bool *startSmsEnable, bool *statSmsEnable, bool *alarmSmsEnable, bool *protoSmsEnable,
                                     QString *startSmsText, QString *statSmsText, QString *alarmSmsText, QString *protoSmsText, bool *changed, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::smsContentsDialog)
{
    ui->setupUi(this);

    startSmsEnable_p = startSmsEnable;
    statSmsEnable_p = statSmsEnable;
    alarmSmsEnable_p = alarmSmsEnable;
    protoSmsEnable_p = protoSmsEnable;

    startSmsText_p = startSmsText;
    statSmsText_p = statSmsText;
    alarmSmsText_p = alarmSmsText;
    protoSmsText_p = protoSmsText;

    changed_t = changed;

    ui->startSmsEdit->setText(*startSmsText_p);
    ui->statSmsEdit->setText(*statSmsText_p);
    ui->alarmSmsEdit->setText(*alarmSmsText_p);
    ui->protoSmsEdit->setText(*protoSmsText_p);

    ui->startSmsEnabled->setChecked(*startSmsEnable_p);
    ui->statSmsEnabled->setChecked(*statSmsEnable_p);
    ui->alarmSmsEnabled->setChecked(*alarmSmsEnable_p);
    ui->protoSmsEnabled->setChecked(*protoSmsEnable_p);

    connect(ui->confirmationButton, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

smsContentsDialog::~smsContentsDialog()
{
    delete ui;
}

void smsContentsDialog::accepted()
{
    int minimum_characters = 5;

    // check if all messages contain some text
    if(ui->startSmsEdit->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, tr("Missing message"), tr("Start SMS message is missing or is empty."), QMessageBox::Ok);
        return;
    }
    if(ui->statSmsEdit->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, tr("Missing message"), tr("Status SMS message is missing or is empty."), QMessageBox::Ok);
        return;
    }
    if(ui->alarmSmsEdit->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, tr("Missing message"), tr("Alarm SMS message is missing or is empty."), QMessageBox::Ok);
        return;
    }
    if(ui->protoSmsEdit->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, tr("Missing message"), tr("Protocol SMS message is missing or is empty."), QMessageBox::Ok);
        return;
    }

    // check if the messages contain at least minimum number of characters
    if(ui->startSmsEdit->toPlainText().length()<minimum_characters)
    {
        QMessageBox::warning(this, tr("Short message"), tr("Start SMS message must contain at least %1 characters.").arg(minimum_characters), QMessageBox::Ok);
        return;
    }
    if(ui->statSmsEdit->toPlainText().length()<minimum_characters)
    {
        QMessageBox::warning(this, tr("Short message"), tr("Status SMS message must contain at least %1 characters.").arg(minimum_characters), QMessageBox::Ok);
        return;
    }
    if(ui->alarmSmsEdit->toPlainText().length()<minimum_characters)
    {
        QMessageBox::warning(this, tr("Short message"), tr("Alarm SMS message must contain at least %1 characters.").arg(minimum_characters), QMessageBox::Ok);
        return;
    }
    if(ui->protoSmsEdit->toPlainText().length()<minimum_characters)
    {
        QMessageBox::warning(this, tr("Short message"), tr("Protocol SMS message must contain at least %1 characters.").arg(minimum_characters), QMessageBox::Ok);
        return;
    }

    // check if informative sms texts are ended up with ':'
    QRegExp regular_exp("([\\w|\\s]+:$)");

    if(regular_exp.indexIn(ui->statSmsEdit->toPlainText(), 0)!=0)
    {
        QMessageBox::warning(this, tr("Missing ending character"), tr("Status SMS message must be ended with \":\""), QMessageBox::Ok);
        return;
    }
    else
    {
        // check if read text is equal to the text in text edit box - if there is only one ':' these string lengths should be equal
        if(regular_exp.cap(1).length()!=ui->statSmsEdit->toPlainText().length())
        {
            QMessageBox::warning(this, tr("Not allowed character"), tr("You can use only standard word characters and spaces within the status SMS text."), QMessageBox::Ok);
            return;
        }
    }
    if(regular_exp.indexIn(ui->alarmSmsEdit->toPlainText(), 0)!=0)
    {
        QMessageBox::warning(this, tr("Missing ending character"), tr("Alarm SMS message must be ended with \":\""), QMessageBox::Ok);
        return;
    }
    else
    {
        // check if read text is equal to the text in text edit box - if there is only one ':' these string lengths should be equal
        if(regular_exp.cap(1).length()!=ui->alarmSmsEdit->toPlainText().length())
        {
            QMessageBox::warning(this, tr("Not allowed character"), tr("You can use only standard word characters and spaces within the alarm SMS text."), QMessageBox::Ok);
            return;
        }
    }
    if(regular_exp.indexIn(ui->protoSmsEdit->toPlainText(), 0)!=0)
    {
        QMessageBox::warning(this, tr("Missing ending character"), tr("Protocol SMS message must be ended with \":\""), QMessageBox::Ok);
        return;
    }
    else
    {
        // check if read text is equal to the text in text edit box - if there is only one ':' these string lengths should be equal
        if(regular_exp.cap(1).length()!=ui->protoSmsEdit->toPlainText().length())
        {
            QMessageBox::warning(this, tr("Not allowed character"), tr("You can use only standard word characters and spaces within the protocol SMS text."), QMessageBox::Ok);
            return;
        }
    }

    // now all texts seem to be OK, lets save them
    startSmsText_p->clear();
    startSmsText_p->append(ui->startSmsEdit->toPlainText());
    statSmsText_p->clear();
    statSmsText_p->append(ui->statSmsEdit->toPlainText());
    alarmSmsText_p->clear();
    alarmSmsText_p->append(ui->alarmSmsEdit->toPlainText());
    protoSmsText_p->clear();
    protoSmsText_p->append(ui->protoSmsEdit->toPlainText());


    // save enabled/disabled state for messages
    *startSmsEnable_p = ui->startSmsEnabled->isChecked();
    *statSmsEnable_p = ui->statSmsEnabled->isChecked();
    *alarmSmsEnable_p = ui->alarmSmsEnabled->isChecked();
    *protoSmsEnable_p = ui->protoSmsEnabled->isChecked();

    *changed_t = true;

    this->close();
}
