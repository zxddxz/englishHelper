#include "mlist.h"
#include <QAbstractItemView>
#include <QMenu>
#include <QTableWidget>

MList::MList(QWidget *parent)
    :QTableWidget(parent)
{
//初始化
    setWindowTitle(tr("播放列表"));
    //设置窗口标志，独立窗口且只有一个关闭按钮
    setWindowFlags(Qt::Window | Qt::WindowTitleHint);
    resize(320,350);

//    setRowCount(10);   //This property holds the number of rows in the table.
    setColumnCount(3);
   QStringList  list ;
    list<<tr("单曲") <<tr("歌手")<<tr("时长");
    setHorizontalHeaderLabels(list);
    //不显示网格
    setShowGrid(false);
    //设置选单行
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选的为一整行


}

MList::~MList()
{

}

void MList::cleanList_slot()
{
    while(rowCount())
        removeRow(0);

    emit cleanList_sig();
}

void MList::contextMenuEvent(QContextMenuEvent *e)
{
       QMenu menu;
       menu.addAction( tr("清空列表"), this,SLOT(cleanList_slot()) );
       //The action's triggered() signal is connected to the receiver's member slot. The
       menu.exec(e->globalPos()); //在那显示
}

void MList::closeEvent(QCloseEvent *e)
{
    if(isVisible())
        hide();

    e->ignore();
}

