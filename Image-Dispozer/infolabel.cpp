#include "infolabel.h"

infoLabel::infoLabel(QWidget * parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    // so far nothing needed
}

infoLabel::~infoLabel()
{

}

void infoLabel::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);

    this->setStyleSheet(QString("color:darkgreen;"));

    emit triggered();
}

void infoLabel::enterEvent(QEvent *e)
{
    QLabel::enterEvent(e);

    QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));

    this->setStyleSheet(QString("color:red;"));
}

void infoLabel::leaveEvent(QEvent *e)
{
    QLabel::leaveEvent(e);

    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

    this->setStyleSheet(QString("color:black;"));
}

void infoLabel::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);

    this->setStyleSheet(QString("color:red;"));
}

