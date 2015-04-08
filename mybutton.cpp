#include "mybutton.h"
#include <QDebug>
#include <QRect>
#include <QPainter>
#include <QCursor>
#include <QSize>

void MyButton::changeIcon(int i)
{
   qDebug()<<"in changeIcon"<<i;
    switch(i)
    {
    case 0://playing
        {
            tmp = press;
        }
        break;
    case 1://stoping
        {
              tmp = Release;
        }
        break;
    }

    this->update();
}

MyButton::MyButton(QWidget *parent)
    :QPushButton(parent)

{
    press = new QImage();
    tmp= new QImage();
    Release = new QImage();
    press->load(":/img/size.png");
    this->setFixedSize(press->width(),press->height() );



    this->update();


}

MyButton::~MyButton()
{

}

void MyButton::setImag(QString s)
{
   press->load(s);
   qDebug()<<s;
}

void MyButton::setReleaseImag(QString s)
{
    Release->load(s);
    tmp = Release;
}

void MyButton::paintEvent(QPaintEvent *)
{
//qDebug()<<"in paint";
QPainter paint(this);
QRect fram(0,0,this->width(),this->height());
QRect imgfram(0,0,tmp->width(),tmp->height());
paint.drawImage(fram, (*tmp), imgfram);

}

void MyButton::mousePressEvent(QMouseEvent *e)
{

    if( e->button() == Qt::LeftButton && tmp == Release)
        {
            tmp = press;
        }
    else
        {
              tmp = Release;
        }

    emit stateChange();
    this->update();
    qDebug()<<"in mousePressEvent";
}

void MyButton::mouseReleaseEvent(QMouseEvent *)
{
//    if( e->button() == Qt::LeftButton )
//    {
//        QImage *tmp = press;
//        press = Release;
//        Release = press;
//    }
     this->update();
    this->clicked();
}

