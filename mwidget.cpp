#include "mwidget.h"
#include "ui_mwidget.h"
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTextCodec>
#include <QStringList> //存储分割后的字符串
#include <QRegExp> //正则表达式

#define  PLAY 0
#define  STOP 1



MWidget::MWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MWidget)
{
    ui->setupUi(this);


    setWindowTitle("effectice english listenner");
    this->setFixedSize(this->width(),this->height());

    //初始化按键
    initIBtn();

    //构造媒体图
    createMediaGraph();

    //创建列表
    musicList = new MList(this);
    //创建解析器
    createAnalyze(); //代码实现要求在媒体图创建之后创建

    source_List = new  QList<Phonon::MediaSource>  ;
    source_List->clear();

//    musicList->show();

    bar = new QMenuBar(this);

    QMenu *open = bar->addMenu("open");
    open->addAction(tr("打开文件"),this,SLOT(openFile_slot()) );
    open->addAction(tr("音乐列表"),this,SLOT(showMusicList_slot()) );
    bar->show();

    //绑定信号和槽
    connectSig_Slots();

    seek = new Phonon::SeekSlider(mediatObject,this);
    seek->setMaximumSize(480,30);
    seek->setMinimumSize(seek->maximumSize());
    seek->show();
    seek->move(ui->hSlider_music->pos());

    repeatPlayFlag = false;
    seek_Stop = -1;
    seek_start = -1;
    ui->hSlider_music->hide();

    ui->verticalSlider->setMaximumSize(30,170);
    ui->verticalSlider->setMinimumSize( ui->verticalSlider->maximumSize());
    quietFlage = 0; //初始化静音标志

//    emit mediatObject->setTickInterval(20000);
    lrc = new MLrc(this);

    lrc->setText(tr("卡洛斯的房间") );
    lrc->update();
     lrc->show();
    lrcMap = new QMap<qint64,QString>();

}

MWidget::~MWidget()
{
    delete ui;
}

void MWidget::play()
{
    mediatObject->play();
    ui->EditMusicName->setText(mediatObject->currentSource().fileName());
}


//slots

void MWidget::updateTimeLable_slot(qint64 currTime)
{

    qint64 totalTime = mediatObject->totalTime();
    QTime total( (totalTime/60000)/60
                 ,(totalTime/60000)%60
                 ,(totalTime/1000)%60
               );
    QTime current( (currTime/60000)/60
                   ,(currTime/60000)%60
                   ,(currTime/1000)%60
                 );
    QString s = current.toString("hh:mm:ss") + "/" + total.toString("hh:mm:ss");



    ui->labelTime->setText(s);

    if(!lrcMap->isEmpty())
    {
        qreal previousTime = 0;
        qreal laterTime = 0;

        foreach (qreal time, lrcMap->keys())
        {
            if(time <= currTime)
            {
                previousTime = time;
            }
            else
            {
                laterTime = time;
                break;
            }
        }//foreach



         QString currLrc =   lrcMap->value(previousTime);
        if(currLrc.length()<=1)
            currLrc = tr("完了。。。");

       if(currLrc != lrc->text())
       {
            lrc->setText(currLrc);
            qint64 interval = previousTime - laterTime;
            lrc->showLrcMask(interval);
            lrc->update();
       }


    }//if

    //重复播放停止位置
    if(  currTime >= seek_Stop && seek_Stop >= seek_start && repeatPlayFlag == true)
        mediatObject->seek(seek_start);
}




/*
* 状态会随着mediatObject 变化及函数的调用自动改变 无须直接改变状态
*/
void MWidget::stateChanged_slot()
{
    qDebug()<<"in stateChanged "<<mediatObject->state() ;
    if(mediatObject->errorType() == Phonon::FatalError)
    {
        QMessageBox::warning(this,QObject::tr("致命错误"),mediatObject->errorString());
        ui->btnPlay->changeIcon(STOP);
        return;
    }
    else if( mediatObject->state() == Phonon::ErrorState )
    {
         QMessageBox::warning(this,QObject::tr("播放错误"),mediatObject->errorString());
         ui->btnPlay->changeIcon(STOP);
         return;
    }


    if( mediatObject->state() == Phonon::LoadingState && !source_List->isEmpty() )
    {
        ui->btnPlay->setCursor(Qt::WaitCursor);
        ui->btnPlay->changeIcon(STOP);
    }
    else if(source_List->isEmpty())
    {
        QMessageBox::warning(this,QObject::tr("播放错误"),tr("播放列表为空") );
          ui->btnPlay->changeIcon(STOP);
    }
    else if(mediatObject->state() == Phonon::PlayingState)
    {

//        mediatObject->stop();
       mediatObject->pause();
       qDebug()<<" mediatObject_Analyse  state"<<mediatObject_Analyse->state();
       qDebug()<<" mediatObject  state"<<mediatObject->state();
       qDebug()<<"title::"  <<mediatObject->metaData("TITLE");
       qDebug()<<"title::"  <<mediatObject->metaData("ARTIST");
       qDebug()<<"title::"  <<mediatObject->currentTime();
       qDebug()<<"title::"  <<mediatObject->totalTime();
       qDebug()<<"source:"<<mediatObject_Analyse->currentSource().fileName();
       qDebug()<<"title::"  <<mediatObject_Analyse->metaData("TITLE");
       qDebug()<<"title::"  <<mediatObject_Analyse->metaData("ARTIST");
       qDebug()<<"title::"  <<mediatObject_Analyse->currentTime();
       qDebug()<<"title::"  <<mediatObject_Analyse->totalTime();
    }
    else if(mediatObject->state() == Phonon::PausedState
            || mediatObject->state() == Phonon::StoppedState
            )
    {
            play();
//             mediatObject->play();

    }
}

void MWidget::stateChanged_slot(Phonon::State newState, Phonon::State oldState)
{
qDebug()<<"in state "<<mediatObject->state() ;
    if(newState == Phonon::ErrorState )
    {
        if(mediatObject->errorType() == Phonon::FatalError)
        {
            QMessageBox::warning(this,tr("致命错误"),mediatObject->errorString());
            return;
        }
        else
        {
             QMessageBox::warning(this,tr("播放错误"),mediatObject->errorString());
             return;
        }
    }
    else if(newState == Phonon::PlayingState)
    {
        ui->btnPlay->changeIcon(PLAY);

    }
    else if(newState == Phonon::PausedState
            || newState == Phonon::StoppedState
            || newState == Phonon::LoadingState
            )
    {
         ui->btnPlay->changeIcon(STOP);
    }


}



void MWidget::openFile_slot()
{
    qDebug()<<"openFile_slot";
    QStringList list = QFileDialog::getOpenFileNames(this,tr("打开音乐文件")
                            ,QDesktopServices::storageLocation(QDesktopServices::MusicLocation));\

    if(list.isEmpty())
        return;

    //资源列表大小
    currentAddIndex = source_List->size();

     Phonon::MediaSource   source;
    //添加资源到资源列表
    foreach (QString  i, list)
    {
        source =  Phonon::MediaSource(i);

            qDebug()<<"in ";
            source_List->append(source);


    }




    //setCurrentSource自动改变状态stateChanged() 信号发送  此时可以解析资源 analyzeSource_slot()
    //设置从第一个新加入的资源开始解析
    if( !source_List->empty())
       {

          mediatObject_Analyse->setCurrentSource(source_List->at(currentAddIndex));

           mediatObject_Analyse->play();

            qDebug()<<"state"<<mediatObject_Analyse->state();

       }





//       analyzeSource();
}




void MWidget::analyzeSource(Phonon::State newState, Phonon::State oldState)
{
    qDebug()<<"analyzeSource  mediatObject_Analyse  state"<<mediatObject_Analyse->state() ;
    qDebug()<<"newState"<<newState <<"oldState"<<oldState;
    if(mediatObject_Analyse->errorType() ==Phonon::ErrorState )
    {
        QMessageBox::warning(this,QObject::tr("打开文件错误"),mediatObject_Analyse->errorString());
        //获取下一个源
        while( !source_List->empty()
               && source_List->takeLast() == mediatObject_Analyse->currentSource())
        { ; }
        return;
    }

    if( /* mediatObject_Analyse->state()  != Phonon::PausedState */
       newState != Phonon::PlayingState )
        { return; }


qDebug()<<"source:"<<mediatObject_Analyse->currentSource().fileName();
qDebug()<<"title::"  <<mediatObject_Analyse->metaData("TITLE");
qDebug()<<"title::"  <<mediatObject_Analyse->metaData("ARTIST");
qDebug()<<"title::"  <<mediatObject_Analyse->currentTime();
qDebug()<<"title::"  <<mediatObject_Analyse->totalTime();
    //获取媒体信息
//    metaData_Map = new  QMap<QString,QString> ( mediatObject_Analyse->metaData() );
     QMap<QString,QString>    metaData  =  ( mediatObject_Analyse->metaData() );
     metaData_Map = &metaData;
    //如果titile为空则用文件名
    QString title = metaData_Map->value("TITLE");
    if(title == "")
    {
        QString  s = mediatObject_Analyse->currentSource().fileName();
        title = QFileInfo(s).baseName();
    }

    QTableWidgetItem *titleItem =  new  QTableWidgetItem(title);
    titleItem->setFlags(titleItem->flags()^Qt::ItemIsEditable);  //设置为不可编辑

    QTableWidgetItem *artItem =  new  QTableWidgetItem(metaData_Map->value("ARTIST") );
    artItem->setFlags(artItem->flags()^Qt::ItemIsEditable);


    qint64 totalTime = mediatObject_Analyse->totalTime();
    QTime total( (totalTime/60000)/60
                 ,(totalTime/60000)%60
                 ,(totalTime/1000)%60
               );
    QTableWidgetItem *totalItem =  new  QTableWidgetItem( total.toString("hh:mm:ss") );
    totalItem->setFlags(totalItem->flags()^Qt::ItemIsEditable);

qDebug()<<"analyzeSource_slot   title"<<title<< metaData_Map->value("ARTIST") << mediatObject_Analyse->totalTime();


qDebug()<<"analyzeSource_slot   title"<<mediatObject_Analyse->metaData("TITLE")<< mediatObject_Analyse->metaData("ARTIST") << mediatObject_Analyse->totalTime();

//mediatObject_Analyse->pause();
 mediatObject_Analyse->stop();
    //插入列表
    int row = musicList->rowCount();
    qDebug()<<"row"<<row;
    musicList->insertRow(row);
    musicList->setItem(row,0,titleItem);
    musicList->setItem(row,1,artItem);
    musicList->setItem(row,2,totalItem);

    //如果还有资源设置下一个为当前资源 状态变化后会重调此函数
     int index = source_List->indexOf(  mediatObject_Analyse->currentSource() ) + 1 ;

     if(  index  < source_List->size() )
     {
        mediatObject_Analyse->setCurrentSource( source_List->at(index) );
        mediatObject_Analyse->play();
//        analyzeSource(); //递归调自身分析 并加入列表
     }
     else  //解析完成后 列表显示完成，处理列表焦点
     {
        if(musicList->selectedItems().isEmpty()) //默认播放刚解析过的列表的第一个
        {
qDebug()<<"in isEmpty";
            if( mediatObject->state() == Phonon::PausedState
               || mediatObject->state() == Phonon::StoppedState )
            {
                mediatObject->setCurrentSource(source_List->at(currentAddIndex));
//                mediatObject->play();
                play();
                 lrc->stoplrMask();
                 resolveLrc(source_List->at(currentAddIndex).fileName());
            }
//            else if(mediatObject->state() == Phonon::PlayingState ) //如果正在播放则继续向下
//            {
//                int curr = source_List->indexOf(mediatObject->currentSource());
//                musicList->selectRow(curr);
//            }
            int curr = source_List->indexOf(mediatObject->currentSource());
            musicList->selectRow(curr);
        }//如果有歌曲已选无须处理
//        else
//        {

//             mediatObject->stop();
//             mediatObject->setCurrentSource(
//                         source_List->at( musicList->selectedItems().at(0)->row())  );

//             mediatObject->play();
//             int curr = source_List->indexOf(mediatObject->currentSource());
//             musicList->selectRow(curr);
//        }

     }


}//analyzeSource()



void MWidget::getVolume_fromOutput(qreal i)
{
    qDebug()<<"getVolume_fromOutput"<< i;
//    ui->verticalSlider->setValue(i*100);
//      ui->verticalSlider->setSliderPosition(i*100);
    if(outPut->volume() != 0)
      {
        quietFlage = 0;
        ui->btnSound->changeIcon(STOP);
    }
}

void MWidget::speedUp()
{
    //    mediatObject->set
}

void MWidget::paintEvent(QPaintEvent *e)
{
    qDebug()<<"in paint";
    QWidget::paintEvent(e);
       QPainter painter(this);

       QPen     pen( (QColor(0,1,0)) ); //,5,Qt::SolidLine);

       painter.setPen(pen);


       painter.drawLine(ui->hScrollBarStart->x()+35+ui->hScrollBarStart->value()*(ui->hScrollBarStart->width()-66)/100
                        ,ui->hScrollBarStart->y()
                        ,ui->hScrollBarStart->x()+35+ui->hScrollBarStart->value()*(ui->hScrollBarStart->width()-66)/100
                        ,ui->hScrollBarStart->y()-190);


       painter.drawLine(ui->hScrollBarStop->x()+35+ui->hScrollBarStop->value()* (ui->hScrollBarStart->width()-66)/100
                        ,ui->hScrollBarStop->y()
                        ,ui->hScrollBarStop->x()+35+ui->hScrollBarStop->value()* (ui->hScrollBarStart->width()-66)/100
                        ,ui->hScrollBarStop->y()-230);



/*
 *用qreal 存储rate失精度
 * start positon is 35
 * scollbar with 66
  QPoint(36,3)
 value 0

 QPoint(486,4)
    value 99
 */
}


void MWidget::resolveLrc(const QString &sourceFileName)
{
        lrcMap->clear();

        if(sourceFileName.isEmpty())
            return;
    QString fileName = sourceFileName;
    QString lrcFileName = fileName.remove(fileName.right(3)) + "lrc";
qDebug()<<"lrcFileName"<<lrcFileName;
    //打开文件
    QFile file(lrcFileName);
    if( !file.open(QIODevice::ReadOnly))
    {
        lrc->setText(lrcFileName+tr("未找到"));
        return;
    }

    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QString  text = file.readAll();

    file.close();

    //分离时间与歌词  并加入QMap中用于定位歌词
    //按行提取
    QStringList  sList = text.split("\n");
    QString s = sList.at(0).trimmed() ;
   qDebug()<<" sList.at(0)" << QString(s.toUtf8());
    QString line;
    int position = 0;
    QRegExp regExp("\\[\\d{2}\\:\\d{2}\\.\\d{2}\\]");
    foreach (QString tmp, sList)
    {
       line = tmp;
       regExp.setPattern("\\[\\d{2}:\\d{2}\\.\\d{2}\\]");
       tmp.replace(regExp,""); //用空字符串来替换时间
qDebug()<<" tmp" << tmp<<"END!!";
qDebug()<<" line" << line;

        int  position = regExp.indexIn(line,0);
qDebug()<<"position"<<position;
       //计算时间 单位为微妙
       while(position != -1) //-1 为结束
       {

            regExp.setPattern("\\d{2}(?=:)"); //后紧跟 :
            regExp.indexIn(line);
            int minite = regExp.cap(0).toInt();

            regExp.setPattern("\\d{2}(?=\\.)");
             regExp.indexIn(line);
            int second = regExp.cap(0).toInt();

            regExp.setPattern("\\d{2}(?=\\])");
             regExp.indexIn(line);
            int msecond = regExp.cap(0).toInt();

qDebug()<<"minite "<<minite<<"second "<<second<<"msecond"<<msecond;

            qint64 totaltime = minite * 60000 + second * 1000 + msecond * 10; //一个msecond表示10毫秒

            lrcMap->insert(totaltime,tmp);
            position += regExp.matchedLength();
            position = regExp.indexIn(line,position); //从上一个position末尾处  开始定位
//            if(position != -1)
       }//while

    }//foreach

    if(sList.isEmpty())
        lrc->setText(lrcFileName + "文件内容错误" );


}


void MWidget::aboutToFinish_slot()
{
     int curr = source_List->indexOf(mediatObject->currentSource());
    if(curr+1 < source_List->size() ) //播放下一条
    {
        mediatObject->setCurrentSource(source_List->at(curr+1));
//        mediatObject->play();
        play();
         lrc->stoplrMask();
         resolveLrc(source_List->at(curr+1).fileName());
    }

}

void MWidget::musicListClicked_slot(int row)
{

    if(mediatObject->state() == Phonon::PlayingState)
        mediatObject->stop();
    qDebug()<<"musicListClicked_slot  row ";
           qDebug()<<row<<source_List->size() ;
    mediatObject->clearQueue();

    if(row >= source_List->size())
        return;

    mediatObject->setCurrentSource(source_List->at(row));
//    mediatObject->play();
    play();
     lrc->stoplrMask();
     resolveLrc(source_List->at(row).fileName());

}

void MWidget::cleanSource_List_slot()
{
    source_List->clear();
    if(mediatObject->state() == Phonon::PlayingState)
      {
        mediatObject->stop();
        lrc->stoplrMask();
      }
}

void MWidget::showMusicList_slot()
{
    musicList->show();
}


//private

void MWidget::createMediaGraph()
{
  mediatObject = new Phonon::MediaObject(this);
qDebug()<<" createMediaGraph "<<mediatObject->state() ;
  outPut = new Phonon::AudioOutput(Phonon::MusicCategory,this);
qDebug()<<" createMediaGraph "<<mediatObject->state() ;
  Phonon::createPath(mediatObject,outPut);


//  mediatObject->setCurrentSource(Phonon::MediaSource("../EnglisHelper/sound/1.mp3"));
qDebug()<<" createMediaGraph "<<mediatObject->state() ;



}

void MWidget::connectSig_Slots()
{
    connect(ui->btnPlay,SIGNAL(stateChange()),this,SLOT(stateChanged_slot()) );

    connect(mediatObject,SIGNAL(stateChanged(Phonon::State,Phonon::State))
            ,this,SLOT(stateChanged_slot(Phonon::State,Phonon::State)) );



    connect(mediatObject,SIGNAL(aboutToFinish())
            ,this,SLOT(aboutToFinish_slot()) );

    connect(mediatObject_Analyse,SIGNAL(stateChanged(Phonon::State,Phonon::State))
            ,this,SLOT(analyzeSource(Phonon::State,Phonon::State)) );


    connect(musicList,SIGNAL(cellClicked(int,int))
            ,this,SLOT(musicListClicked_slot(int)) );

    connect(musicList,SIGNAL(cleanList_sig())
            ,this,SLOT(cleanSource_List_slot()) );

    connect(outPut,SIGNAL(volumeChanged(qreal))
            ,this,SLOT(getVolume_fromOutput(qreal))  );

    connect(ui->btnNext, SIGNAL(clicked())
            ,this,SLOT(aboutToFinish_slot()) );

    connect(mediatObject,SIGNAL(tick(qint64))
            ,this,SLOT(updateTimeLable_slot(qint64))  );

    connect(ui->hScrollBarStop,SIGNAL(repeatMove()),this,SLOT(update()) );
    connect(ui->hScrollBarStop,SIGNAL(valueChanged(int)),this,SLOT(update()) );
    connect(ui->hScrollBarStop,SIGNAL(valueChanged(int)),this,SLOT(seekStop(int)) );
    connect(ui->hScrollBarStart,SIGNAL(repeatMove()),this,SLOT(update()) );
    connect(ui->hScrollBarStart,SIGNAL(valueChanged(int)),this,SLOT(update()) );
    connect(ui->hScrollBarStart,SIGNAL(valueChanged(int)),this,SLOT(seekPos(int)) );



}

void MWidget::createAnalyze()
{
    mediatObject_Analyse = mediatObject;
    outPut_Analyze = outPut;
    createMediaGraph();
}

void MWidget::initIBtn()
{

//    ui->btnDeal->setEnabled(false);
//    ui->btnNext->setEnabled(false);
//    ui->btnPlay->setEnabled(false);
//    ui->btnSpeedDown->setEnabled(false);
//    ui->btnSpeedUp->setEnabled(false);

    ui->btnSound->setFixedSize(40,40);
    ui->btnSound->setImag( QString(":/img/Graphite_Red_Speaker.ico") );


    ui->btnDeal->setImag(QString(":/img/056.png"));
    ui->btnPlay->setImag( QString(":/img/stop.png") );
    ui->btnNext->setImag( QString(":/img/next.png") );
    ui->btnSpeedUp->setImag( QString(":/img/speedUp.png") );
    ui->btnSpeedDown->setImag( QString(":/img/speedDown.png") );

    ui->btnSound->setReleaseImag( QString(":/img/Graphite_Orange_Speaker.ico") );
    ui->btnDeal->setReleaseImag(QString(":/img/055.png"));
    ui->btnPlay->setReleaseImag( QString(":/img/play.png") );
    ui->btnNext->setReleaseImag( QString(":/img/next.png") );
    ui->btnSpeedUp->setReleaseImag( QString(":/img/speedUp.png") );
    ui->btnSpeedDown->setReleaseImag( QString(":/img/speedDown.png") );

}



void MWidget::on_verticalSlider_sliderMoved(int position)
{
    if(outPut != NULL)
    {
     qDebug()<<"sliderMoved"<<position*0.01;
     outPut->setVolume(position*0.01);
     quietFlage = 0;
     ui->btnSound->changeIcon(STOP);
    }

}

void MWidget::on_verticalSlider_valueChanged(int value)
{
    qDebug()<<"valueChanged"<<value;
     outPut->setVolume(value*0.01);
     quietFlage = 0;
     ui->btnSound->changeIcon(STOP);
}

void MWidget::on_btnSound_clicked()
{

    if( !quietFlage)
    {
    oldVolume = outPut->volume();
    outPut->setVolume(0);
    quietFlage = 1;
    }
    else
        outPut->setVolume(oldVolume);
}

void MWidget::seekPos(int i)
{
     seek_start = i*0.01*mediatObject->totalTime();
     QTime total( ((qint64)seek_start/60000)/60
                  ,((qint64)seek_start/60000)%60
                  ,((qint64)seek_start/1000)%60
                );
     ui->labStart->setText(total.toString("hh:mm:ss"));
    if(repeatPlayFlag == true)
    {
     mediatObject->seek( seek_start);
    }
}

void MWidget::seekStop(int i)
{
    qreal k = i*0.01*mediatObject->totalTime();
    QTime total( ((qint64)k/60000)/60
                 ,((qint64)k/60000)%60
                 ,((qint64)k/1000)%60
               );
    ui->labStop->setText(total.toString("hh:mm:ss"));
    if(k > seek_start)
    seek_Stop = k;
    else //位置考前取消循环
    {  repeatPlayFlag = false;
      ui->ckBoxRepeat->setChecked(false);
    }

}

//void MWidget::on_hScrollBarStart_sliderMoved(int position)
//{
//ui->hScrollBarStart
//}

void MWidget::on_ckBoxRepeat_clicked(bool checked)
{
    repeatPlayFlag = checked;
    if( repeatPlayFlag == true)
        mediatObject->seek(seek_start);
}

void MWidget::on_btnSpeedDown_clicked()
{
   if(ui->hScrollBarStart->value()>=0)
   {
       qreal i = ui->hScrollBarStart->value()-ui->spinBox->value();
       if(i<0)
           i=0;
       seek_start =  i;
       ui->hScrollBarStart->setValue(i);
       this->update();
       emit seekPos(i);
   }

}

void MWidget::on_btnSpeedUp_clicked()
{
    if(ui->hScrollBarStart->value()<=100)
    {
        qreal i = ui->hScrollBarStart->value()+ui->spinBox->value();
        if(i>100)
            i=100;
        seek_start =  i;
        ui->hScrollBarStart->setValue(i);
        this->update();
        emit seekPos(i);
    }
}
