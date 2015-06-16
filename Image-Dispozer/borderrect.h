#ifndef BORDERRECT_H
#define BORDERRECT_H

#include <QGraphicsRectItem>
#include <QSizeF>

class borderRect : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

    Q_PROPERTY(QSizeF size READ getSize WRITE setSize);

private:
    QSizeF size;

public:
    borderRect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem * parent);
    ~borderRect();

    void setSize(QSizeF n_size);
    QSizeF getSize(void);
};

#endif // BORDERRECT_H
