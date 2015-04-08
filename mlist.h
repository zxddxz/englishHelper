#ifndef MLIST_H
#define MLIST_H
#include <QTableWidget> //基类
#include <QStringList>  //列表标题
#include <QContextMenuEvent>  // receive widget context menu events.
#include <QCloseEvent>


class MList : public QTableWidget
{

Q_OBJECT

public:
    MList( QWidget * parent = 0);
    ~MList();


public:
signals:
         void cleanList_sig();

public slots:
    void cleanList_slot();


//覆盖函数
protected:
    void contextMenuEvent(QContextMenuEvent *);
    void closeEvent(QCloseEvent *);


};

#endif // MLIST_H
