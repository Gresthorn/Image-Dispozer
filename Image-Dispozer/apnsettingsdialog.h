#ifndef APNSETTINGSDIALOG_H
#define APNSETTINGSDIALOG_H

#include <QDialog>
#include <QRegExp>
#include <QMessageBox>

enum address_mode {
    IP_ADDR,
    TEXT
};

namespace Ui {
class apnSettingsDialog;
}

class apnSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit apnSettingsDialog(QString * ap_name, QString * login, QString * password, bool * request_password, bool * use_proxy,
                                address_mode * addr_mode, QString * address, QString * DNS1, QString * DNS2, bool * changed, QWidget *parent = 0);
    ~apnSettingsDialog();

private:
    Ui::apnSettingsDialog *ui;

    QString * ap_name_t;
    QString * login_t;
    QString * password_t;
    bool * request_password_t;
    bool * use_proxy_t;
    address_mode * addr_mode_t;
    QString * address_t;
    QString * DNS1_t;
    QString * DNS2_t;

    bool * changed_t;

public slots:
    void accepted(void);
};

#endif // APNSETTINGSDIALOG_H
