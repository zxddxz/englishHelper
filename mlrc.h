#ifndef MLRC_H
#define MLRC_H
#include <QLabel>
#include <QWidget>
#include <QPaintEvent>
#include <QTimer> //定时器
#include <QMouseEvent>
#include <QContextMenuEvent>
/*
* 背景透明
* 覆盖绘图事件 使用渐变填充
* 使用定时器绘制动态遮罩
*/

class MLrc : public QLabel
{
Q_OBJECT

public:
    MLrc(QWidget *parent );
    ~MLrc();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *);

public:
    void showLrcMask(qint64 intervalTime);
    void stoplrMask();
    void initPaint();


public slots:
    void timeout();

private:
    QLinearGradient linearGadient;
    QLinearGradient maskLinearGradient;
    QFont           font;
    QTimer          *timer;

    qreal lrcMaskWidth;
    //每次歌词遮罩增加的宽度
    qreal lrcMaskWidthInterval;
    QPoint offset;






};

#endif // MLRC_H
