#include "mainwindow.h"
#include <QApplication>
#include "mwidget.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );

//    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );
     QTextCodec::setCodecForCStrings(  QTextCodec::codecForName("GBK") );

//    MainWindow w;
//    w.show();

    MWidget fram;
    fram.show();
    return a.exec();
}
