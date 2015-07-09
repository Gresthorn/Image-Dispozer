#ifndef SMSCONTENTSDIALOG_H
#define SMSCONTENTSDIALOG_H

#include <QDialog>
#include <QString>
#include <QMessageBox>
#include <QRegExp>
#include <QDebug>

namespace Ui {
class smsContentsDialog;
}

class smsContentsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit smsContentsDialog(bool *startSmsEnable, bool *statSmsEnable, bool *alarmSmsEnable, bool *protoSmsEnable,
                               QString *startSmsText, QString *statSmsText, QString *alarmSmsText, QString *protoSmsText,
                               bool * changed, QWidget *parent = 0);
    ~smsContentsDialog();

private:
    Ui::smsContentsDialog *ui;


    bool *startSmsEnable_p;
    bool *statSmsEnable_p;
    bool *alarmSmsEnable_p;
    bool *protoSmsEnable_p;
    QString *startSmsText_p;
    QString *statSmsText_p;
    QString *alarmSmsText_p;
    QString *protoSmsText_p;

    bool * changed_t;

public slots:
    void accepted(void);
};

#endif // SMSCONTENTSDIALOG_H
