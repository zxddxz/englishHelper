#include "mlrc.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QMenu>
#include <QDebug>


MLrc::MLrc(QWidget *parent)
    :QLabel(parent)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //设置透明背景
    setAttribute(Qt::WA_TranslucentBackground);
    setText("zxd Player");

    setFixedSize(800,60);

    //初始化歌词效果所需变量
    initPaint();


}

MLrc::~MLrc()
{

}

void MLrc::paintEvent(QPaintEvent *)
{
       qDebug()<<"paintEvent";
    QPainter painter(this);
    painter.setFont(font);

    //底层文字
    painter.setPen(QColor(0,0,0,200));
    painter.drawText(1,1,800,60,Qt::AlignLeft,text() );
//     painter.drawText(1,1,800,60,Qt::AlignLeft,"fkaljdfljlsdfj");
    //渐变文字
    painter.setPen(QPen(QBrush(linearGadient) ,0)  );
    painter.drawText(0,0,800,60,Qt::AlignLeft,text());
//      painter.drawText(0,0,800,60,Qt::AlignLeft,"asdfsdfasdfsdfsdfsdf");
    //设置遮罩
    painter.setPen(QPen(QBrush(maskLinearGradient) ,0) );
    painter.drawText(0,0,lrcMaskWidth,60,Qt::AlignLeft,text());
//    painter.drawText(0,0,800,60,Qt::AlignLeft,"uwioejflasdgjgjoiis");

}

void MLrc::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        offset = e->globalPos() - frameGeometry().topLeft();
        qDebug()<<"mousePressEvent"<<offset;
    }
}

void MLrc::mouseMoveEvent(QMouseEvent *e)
{
     if(e->buttons() == Qt::LeftButton)
     {
          qDebug()<<"mouseMoveEvent"<<offset;
         move(e->globalPos()-offset);

     }
}

//右键菜单
void MLrc::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu(this);
    menu.addAction(tr("关闭歌词"),this,SLOT(hide()));
    menu.exec(e->globalPos());

}


void MLrc::showLrcMask(qint64 intervalTime)
{
    //更新遮罩的间隔时间
    qreal count = intervalTime / 30;

    //遮罩增加宽度
    lrcMaskWidthInterval = 3;
//    qDebug()<<"lenght"<<text().length()<<lrcMaskWidthInterval<<intervalTime<<count;
    lrcMaskWidth = 0;
    timer->start(40);
}

void MLrc::stoplrMask()
{
    timer->stop();
    lrcMaskWidth = 0;
    update();

}





void MLrc::initPaint()
{
    //歌词的线性填充
    linearGadient.setStart(0,10);
    linearGadient.setFinalStop(0,40);
    linearGadient.setColorAt(0.1, QColor(14,179,255));
    linearGadient.setColorAt(0.5, QColor(114,32,255));
    linearGadient.setColorAt(0.1, QColor(14,179,255));

    //遮罩的线性渐变填充
    maskLinearGradient.setStart(0,10);
    maskLinearGradient.setFinalStop(0,40);
    maskLinearGradient.setColorAt(0.1, QColor(222,54,4));
    maskLinearGradient.setColorAt(0.5, QColor(255,72,16));
    maskLinearGradient.setColorAt(0.9, QColor(222,54,4));

    //设置字体
   font.setFamily("Times New Roman");
   font.setBold(true);
   font.setPointSize(30);

   //设置定时器
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()) );
    lrcMaskWidth = 0;
    lrcMaskWidthInterval = 0;
}

void MLrc::timeout()
{
    lrcMaskWidth += lrcMaskWidthInterval;
    this->update();
}

