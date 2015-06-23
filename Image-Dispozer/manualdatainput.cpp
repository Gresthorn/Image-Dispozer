#include "manualdatainput.h"

manualDataInput::manualDataInput(QString title, const QStringList & names, QList<int> * values, const QList<int> &intervals, bool * ok, QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    // if number of elements in lists does not match there is no way to know what default value to set or what text to display
    if(names.count()!=values->count())
    {
        QMessageBox::critical(this, tr("Error"), tr("The number of values does not match number of identifiers.\n"
                                                    "Dialog could not be created. Contact developer for more information"),
                              QMessageBox::Ok);

        this->close();
        return;
    }

    ok_d = ok;
    *ok_d = false;

    values_origin = values;

    // this layout will hold items in separate rows
    main_layout = new QVBoxLayout;

    int it = 0;
    // iterate over all values and create a rows for them
    Q_FOREACH(int val, *values)
    {
        QWidget * w = new QWidget(this);
        QHBoxLayout * h_l = new QHBoxLayout;

        QSpinBox * s_b = new QSpinBox(w);
        // set interval (min, max), default increment step is 1
        s_b->setMinimum(intervals.at(it*3));
        s_b->setMaximum(intervals.at(it*3+1));
        s_b->setSingleStep(intervals.at(it*3+2));

        s_b->setValue(val);
        QLabel * l = new QLabel(names.at(it));
        l->setAlignment(Qt::AlignRight);


        h_l->addWidget(l);
        h_l->addWidget(s_b);

        hItemSpinBoxList.append(s_b);
        hItemLabelList.append(l);
        hItemLayoutList.append(h_l);

        w->setLayout(h_l);

        main_layout->addWidget(w);

        it++;
    }

    // append buttons to the end
    accept = new QPushButton(tr("OK"), this);
    reject = new QPushButton(tr("Cancel"), this);
    QWidget * w = new QWidget(this);
    QHBoxLayout * h_l = new QHBoxLayout;
    h_l->addWidget(accept);
    h_l->addWidget(reject);
    w->setLayout(h_l);
    main_layout->addWidget(w);

    this->setLayout(main_layout);

    this->setWindowTitle(title);

    // for title to be at least a little seen
    this->setMinimumSize(150, 10);

    connect(accept, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(reject, SIGNAL(clicked()), this, SLOT(rejected()));

    // display window and make size fixed, so user will not be able to resize it again
    this->show();
    this->setFixedSize(this->size());
}

manualDataInput::~manualDataInput()
{  
    // delete all objects and free memory

    while(!hItemLayoutList.isEmpty())
    {
        delete hItemLayoutList.first();
        hItemLayoutList.removeFirst();
    }

    while(!hItemSpinBoxList.isEmpty())
    {
        delete hItemSpinBoxList.first();
        hItemSpinBoxList.removeFirst();
    }

    while(!hItemLabelList.isEmpty())
    {
        delete hItemLabelList.first();
        hItemLabelList.removeFirst();
    }

    delete main_layout;

    delete accept;
    delete reject;
}

void manualDataInput::accepted()
{
    *ok_d = true;
    int it = 0;
    // iterate over all values and create a rows for them
    Q_FOREACH(QSpinBox * spin_boxes, hItemSpinBoxList)
    {
        (*values_origin)[it] = spin_boxes->value();
        it++;
    }

    this->close();
}

void manualDataInput::rejected()
{
    // ok_d is already set to false in constructor
    this->close();
}

