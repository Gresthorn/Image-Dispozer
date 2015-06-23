#ifndef INFOLABEL_H
#define INFOLABEL_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include <QCursor>
#include <QApplication>

class infoLabel : public QLabel
{

    Q_OBJECT

public:
    infoLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~infoLabel();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

signals:
    void triggered(void);
};

#endif // INFOLABEL_H
