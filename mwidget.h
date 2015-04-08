#ifndef MWIDGET_H
#define MWIDGET_H

#include <QWidget>
#include <QString>
#include <QPixmap>
#include <phonon>
#include <phonon/mediasource.h>
#include <QTimer>   //提供计时器对象
#include <QTime>    //提供时间函数
#include <QButtonGroup>
#include <QList>
#include <QMap>
#include <QFileDialog>
#include <QMenuBar>
#include <QAction>
#include "mlist.h"
#include "mlrc.h"
#include <QMap>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPaintEvent>
#include "mrepeat.h"


namespace Ui {
class MWidget;
}

class MWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MWidget(QWidget *parent = 0);
    ~MWidget();
    void play();

public slots:
     void updateTimeLable_slot(qint64 currTime);    //更新时间 设置hslider_music
     void stateChanged_slot();  //按扭控制
     void stateChanged_slot(Phonon::State newState,Phonon::State oldState); //由播放状态自动控制
     void openFile_slot();      //打开文件后处理  mediatObject_Analyse 播放状态会变
     void aboutToFinish_slot();  //当播放快结束时媒体对象会发送aboutToFinish信号 或是按下下一首歌
     void musicListClicked_slot(int row); //处理播放列表点击
     void cleanSource_List_slot();
     void showMusicList_slot();
     void analyzeSource(Phonon::State, Phonon::State oldState);      //分析 并加入列表
     void getVolume_fromOutput(qreal); //output 音量变化时触发设置垂直slide
     void speedUp();


public:
    Ui::MWidget *ui;

protected:
     void paintEvent(QPaintEvent *);


private slots:
    void on_verticalSlider_sliderMoved(int position);

    void on_verticalSlider_valueChanged(int value);

    void on_btnSound_clicked();

    void seekPos(int);
    void seekStop(int);

//    void on_hScrollBarStart_sliderMoved(int position);

    void on_ckBoxRepeat_clicked(bool checked);

private:
    void createMediaGraph();   //构造媒体图
    void connectSig_Slots();   //链接槽和信号
    void createAnalyze();
    void initIBtn();           //初始化按键
    void resolveLrc(const QString &sourceFileName);

    Phonon::MediaObject  *mediatObject;
    Phonon::AudioOutput * outPut;
    QTimer               *timer;
    QButtonGroup         *btnGroup;
    MList                *musicList; //播放列表的窗口
    Phonon::MediaObject  *mediatObject_Analyse;
    Phonon::AudioOutput  *outPut_Analyze;
    QList<Phonon::MediaSource>      *source_List;  //当前可用资源列表
    QMap<QString,QString>           *metaData_Map;
    int                  currentAddIndex ; //刚添加的列表的第一个资源的索引
    QFileDialog          *openFile;
    QMenuBar             *bar;

    MLrc                 *lrc;
    QMap<qint64,QString> *lrcMap;

    Phonon::SeekSlider   *seek;
    Phonon::VolumeSlider *volume;
    qreal                oldVolume;
    int                  quietFlage;

    qreal                seek_start;
    qreal                seek_Stop;
    bool                 repeatPlayFlag;


};

#endif // MWIDGET_H
