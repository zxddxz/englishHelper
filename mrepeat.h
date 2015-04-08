#ifndef MREPEAT_H
#define MREPEAT_H
#include <QScrollBar>
#include <QPaintEvent>
#include <QMouseEvent>

class Mrepeat : public QScrollBar
{
    Q_OBJECT

public:
    Mrepeat(QWidget * parent = 0 );
    ~Mrepeat();
signals:
    void repeatMove();
protected:

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

};

#endif // MREPEAT_H
