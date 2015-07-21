#include "exportselectordialog.h"
#include "ui_exportselectordialog.h"

exportSelectorDialog::exportSelectorDialog(dataToExport *dataToExp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::exportSelectorDialog)
{
    ui->setupUi(this);

    data_to_export = dataToExp;

    ui->expAPNCfgEnable->setChecked(data_to_export->apnExpEnable);
    ui->expContactNumsCfgEnable->setChecked(data_to_export->contactExpEnable);
    ui->expDeviceCfgEnable->setChecked(data_to_export->deviceExpEnable);
    ui->expImagesCfgEnable->setChecked(data_to_export->imagesExpEnable);
    ui->expSMSCfgEnable->setChecked(data_to_export->smsExpEnable);
    ui->expSoundsCfgEnable->setChecked(data_to_export->soundsExpEnable);

    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->acceptButton, SIGNAL(clicked()), this, SLOT(accepted()));
}

exportSelectorDialog::~exportSelectorDialog()
{
    delete ui;
}

void exportSelectorDialog::accepted()
{
    // user clicked OK button - save changes
    data_to_export->apnExpEnable = ui->expAPNCfgEnable->isChecked();
    data_to_export->contactExpEnable = ui->expContactNumsCfgEnable->isChecked();
    data_to_export->deviceExpEnable = ui->expDeviceCfgEnable->isChecked();
    data_to_export->imagesExpEnable = ui->expImagesCfgEnable->isChecked();
    data_to_export->smsExpEnable = ui->expSMSCfgEnable->isChecked();
    data_to_export->soundsExpEnable = ui->expSoundsCfgEnable->isChecked();

    this->close();
}
