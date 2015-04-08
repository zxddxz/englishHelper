#include "mrepeat.h"
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QDebug>
#include <QBrush>
Mrepeat::Mrepeat(QWidget *parent)
    :QScrollBar(parent)
{

}

Mrepeat::~Mrepeat()
{

}



void Mrepeat::mousePressEvent(QMouseEvent *e)
{
    emit repeatMove();
    QScrollBar::mousePressEvent(e);
}

void Mrepeat::mouseMoveEvent(QMouseEvent *e)
{
    qDebug()<<e->pos();
   emit repeatMove();
   QScrollBar::mouseMoveEvent(e);
}

