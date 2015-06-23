#ifndef MANUALDATAINPUT_H
#define MANUALDATAINPUT_H

#include <QWidget>
#include <QDialog>
#include <QList>
#include <QLabel>
#include <QStringList>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <QDebug>

class manualDataInput : public QDialog
{
    Q_OBJECT

public:
    manualDataInput(QString title, const QStringList &names, QList<int> * values, const QList<int> &intervals, bool * ok, QWidget * parent = NULL, Qt::WindowFlags f = 0);
    ~manualDataInput();

private:
    QVBoxLayout * main_layout;

    bool * ok_d;

    QList<int> * values_origin;

    QList<QHBoxLayout * > hItemLayoutList;
    QList<QSpinBox * > hItemSpinBoxList;
    QList<QLabel * > hItemLabelList;

    QPushButton * accept;
    QPushButton * reject;

public slots:
    void accepted(void);
    void rejected(void);
};

#endif // MANUALDATAINPUT_H
