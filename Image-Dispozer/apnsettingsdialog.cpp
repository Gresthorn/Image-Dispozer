#include "apnsettingsdialog.h"
#include "ui_apnsettingsdialog.h"

apnSettingsDialog::apnSettingsDialog(QString *ap_name, QString *login, QString *password, bool *request_password, bool *use_proxy, address_mode *addr_mode, QString *address, QString *DNS1, QString *DNS2, bool *changed, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::apnSettingsDialog)
{
    ui->setupUi(this);

    ui->apnEdit->setText(*ap_name);
    ui->loginEdit->setText(*login);
    ui->passwordEdit->setText(*password);
    ui->requestPassCheckBox->setChecked(*request_password);
    ui->useProxyCheckBox->setChecked(*use_proxy);
    ui->addressModeComboBox->setCurrentIndex(*addr_mode);
    ui->addressEdit->setText(*address);
    ui->dns1Edit->setText(*DNS1);
    ui->dns2Edit->setText(*DNS2);


    ap_name_t = ap_name;
    login_t = login;
    password_t = password;
    request_password_t = request_password;
    use_proxy_t = use_proxy;
    addr_mode_t = addr_mode;
    address_t = address;
    DNS1_t = DNS1;
    DNS2_t = DNS2;

    changed_t = changed;

    connect(ui->acceptButton, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

apnSettingsDialog::~apnSettingsDialog()
{
    delete ui;
}

void apnSettingsDialog::accepted()
{
    // check ip address
    QRegExp t_ip("(^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
                     "(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)");
    QRegExp t_url("^(https?\\:\\/\\/)?([\\da-z\\.-]+)\\.([a-z\\.]{2,6})([\\/\\w\\.-]*)*\\/?$");
    QRegExp t_url2("^(https?\\:\\/\\/)?((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))(\\/[\\/\\w\\.-]*)*\\/?$");

    // if something goes wrong, put info here
    QString message;


    if(!ui->addressEdit->text().isEmpty())
    {
        if(ui->addressModeComboBox->currentIndex()==TEXT)
        {
            if(t_url.indexIn(ui->addressEdit->text(), 0)!=0) message.append(tr("\"Address\" seems not to be correct.\n"));
        }
        else if(ui->addressModeComboBox->currentIndex()==IP_ADDR)
        {
            if(t_url2.indexIn(ui->addressEdit->text(), 0)!=0) message.append(tr("\"Address\" seems not to be correct.\n"));
        }
    }
    if(!ui->dns1Edit->text().isEmpty())
    {
        if(t_ip.indexIn(ui->dns1Edit->text(), 0)!=0) message.append(tr("\"DNS 1\" address is incorrect.\n"));
    }
    if(!ui->dns2Edit->text().isEmpty())
    {
        if(t_ip.indexIn(ui->dns2Edit->text(), 0)!=0) message.append(tr("\"DNS 2\" address is incorrect.\n"));
    }

    if(!message.isEmpty())
    {
        QMessageBox::warning(this, tr("Incorrect input"), message, QMessageBox::Ok);
        return;
    }


    ap_name_t->clear();
    ap_name_t->append(ui->apnEdit->text());
    login_t->clear();
    login_t->append(ui->loginEdit->text());
    password_t->clear();
    password_t->append(ui->passwordEdit->text());
    (*request_password_t) = ui->requestPassCheckBox->isChecked();
    (*use_proxy_t) = ui->useProxyCheckBox->isChecked();

    switch(ui->addressModeComboBox->currentIndex())
    {
        case 0:
            (*addr_mode_t) = IP_ADDR;
            break;
        case 1:
            (*addr_mode_t) = TEXT;
            break;
        default:
            (*addr_mode_t) = IP_ADDR;
            break;
    }


    address_t->clear();
    address_t->append(ui->addressEdit->text());
    DNS1_t->clear();
    DNS1_t->append(ui->dns1Edit->text());
    DNS2_t->clear();
    DNS2_t->append(ui->dns2Edit->text());

    *changed_t = true;

    this->close();
}
