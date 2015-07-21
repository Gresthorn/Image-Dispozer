#ifndef EXPORTSELECTORDIALOG_H
#define EXPORTSELECTORDIALOG_H

#include <QDialog>

struct dataToExport {
    bool deviceExpEnable;
    bool imagesExpEnable;
    bool soundsExpEnable;
    bool contactExpEnable;
    bool smsExpEnable;
    bool apnExpEnable;
};

namespace Ui {
class exportSelectorDialog;
}

class exportSelectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit exportSelectorDialog(struct dataToExport * dataToExp, QWidget *parent = 0);
    ~exportSelectorDialog();

private:
    Ui::exportSelectorDialog *ui;

    dataToExport * data_to_export;

public slots:
    void accepted(void);
};

#endif // EXPORTSELECTORDIALOG_H
