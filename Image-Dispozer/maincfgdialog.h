#ifndef MAINCFGDIALOG_H
#define MAINCFGDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class mainCFGDialog;
}

enum lang {
    ENG = 0,
    SVK = 1,
    CZE = 2,
    PLN = 3,
    HUN = 4,
    GER = 5
};

enum per_units {
    MINS = 0,
    HRS = 1,
    DAYS = 2,
    MNTHS = 3
};

class mainCFGDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mainCFGDialog(int *period, QString * per_units, QString *lan,
                           bool * ack_banel_error, int * volume, bool * changed, QWidget *parent = 0);
    ~mainCFGDialog();

private:
    Ui::mainCFGDialog *ui;

    int * period_p;
    QString * per_units_p;
    QString * lan_p;
    bool * ack_banel_error_p;
    int * volume_p;

    bool * changed_t;

public slots:
    void accepted(void);
};

#endif // MAINCFGDIALOG_H
