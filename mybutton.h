#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QAbstractButton>
#include <QMouseEvent>
#include <QString>
#include <QPaintEvent>
#include <QPushButton>
#include <phonon>


class MyButton : public QPushButton
{

Q_OBJECT

public :
    signals:
        void stateChange();




public:
    MyButton(QWidget *parent=0);
    ~MyButton();

    void setImag(QString s);
    void setReleaseImag(QString s);
    void changeIcon(int);


protected:
   void	 paintEvent ( QPaintEvent *  ) ;
   void  mousePressEvent(QMouseEvent *);
   void  mouseReleaseEvent(QMouseEvent *);

private:
  QImage *press;
  QImage *Release;
  QImage *tmp;




};

#endif // MYBUTTON_H
