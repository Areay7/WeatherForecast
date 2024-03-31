#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QContextMenuEvent>
#include <QMenu>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QMessageBox>

#include "weatherdata.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "weathertool.h"

#include <QPainter>


#define INCREMENT 3         // 温度每升高或降低1度，y轴的增量
#define POINT_RADIUS 3      // 曲线描点的大小
#define TEXT_OFFSET_X 6
#define TEXT_OFFSET_Y 8

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private:
    void onReplied(QNetworkReply* reply);

protected:
    // 重写父类方法
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    // 获取天气数据、解析Json、更新UI
    void getWeatherInfo(QString cityCode);

    void parseJson(QByteArray &byteArray);
    void updateUI();

    // 重写父类的eventfilter方法
    bool eventFilter(QObject* watched, QEvent* event);

    // 绘制高低温曲线
    void paintHighCurve();
    void paintLowCurve();

private slots:
    void on_btnSearch_clicked();

private:
    QMenu *mExitMenu;   // 右键退出的菜单
    QAction *mExitAct;  // 退出行为 - 菜单项
    QPoint mOffset;     // 窗口移动时，鼠标与窗口左上角的偏移

    // Http 请求
    QNetworkAccessManager *mNetAccessManager;

    // 今天和将来6天的天气
    Today mToday;
    Day mDay[6];

    // 星期和日期
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDateList;

    // 天气和天气图标
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;

    // 天气污染指数
    QList<QLabel*> mAqiList;

    // 风力和风向
    QList<QLabel*> mFxList;
    QList<QLabel*> mFlList;

    QMap<QString, QString> mTypeMap;

};
#endif // MAINWINDOW_H
