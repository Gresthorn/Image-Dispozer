#ifndef CONTACTDATADIALOG_H
#define CONTACTDATADIALOG_H

#include <QDialog>
#include <QString>
#include <QMessageBox>

#include <QDebug>

namespace Ui {
class contactDataDialog;
}

class contactDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit contactDataDialog(QString * start, QString * stat, QString * alarm, QString * proto,
                               QString *callNum1, QString *callNum2, QString *callNum3, QString *callNum4,
                               bool * changed, QWidget *parent = 0);
    ~contactDataDialog();

private:
    Ui::contactDataDialog *ui;

    QString * start_sms_number;
    QString * stat_sms_number;
    QString * alarm_sms_number;
    QString * proto_sms_number;
    QString * call_number_1;
    QString * call_number_2;
    QString * call_number_3;
    QString * call_number_4;

    bool * changed_t;

public slots:
    void accepted(void);
};

#endif // CONTACTDATADIALOG_H
