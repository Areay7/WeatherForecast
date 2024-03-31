#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint); // 设置窗口无边框
    setWindowIcon(QIcon(":/res/icon.png"));
    setFixedSize(width(), height());


    // 构建右键菜单
    mExitMenu  = new QMenu(this);
    mExitAct = new QAction();

    mExitAct->setText("退出");
    mExitAct->setIcon(QIcon(":/res/close.png"));

    mExitMenu->addAction(mExitAct);

    connect(mExitAct, &QAction::triggered, this, [=]{
        qApp->exit(0);
    });

    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplied);

    // UI 初始化
    // 将控件添加到控件数组
    // 星期和日期
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;
    mDateList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;

    // 天气和天气图标
    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;

    // 天气指数
    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;

    // 风向和风力
    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;

    //天气对应图标
    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/res/type/BaoYu.png");
    mTypeMap.insert("暴雨到暴雪",":/res/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到大暴雪",":/res/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪",":/res/type/DaDaoBaoXue.png");
    mTypeMap.insert("大到暴雨",":/res/type/DaDaoBaoYu.png");
    mTypeMap.insert("大雪",":/res/type/DaXue.png");
    mTypeMap.insert("大雨",":/res/type/DaYu.png");
    mTypeMap.insert("冻雨",":/res/type/DongYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("浮尘",":/res/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/res/type/QiangShaChenBao.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("雾",":/res/type/Wu.png");
    mTypeMap.insert("小到中雨",":/res/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小到中雪",":/res/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");
    mTypeMap.insert("雪",":/res/type/Xue.png");
    mTypeMap.insert("扬沙",":/res/type/YangSha.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨",":/res/type/Yu.png");
    mTypeMap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");
    mTypeMap.insert("阵雪",":/res/type/ZhenXue.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");
    mTypeMap.insert("中雪",":/res/type/ZhongXue.png");

    // 直接在构造中， 请求天气数据
    // "101010100"为北京城市编码   101030100  天津  101300401
    //    getWeatherInfo("101030100");
    getWeatherInfo("天津");

    // 5. 给标签添加事件过滤器
    // 参数指定为this，也就是当前窗口对象 MainWindow
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

// 重写父类虚函数
// 父类中默认的实现  是忽略右键菜单事件
// 重写之后，就可以处理右键菜单
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    // 弹出右键菜单
    mExitMenu->exec(QCursor::pos());

    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mOffset =  event->globalPos() - this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

void MainWindow::getWeatherInfo(QString cityName)
{
    QString cityCode = WeatherTool::getCityCode(cityName);
    if(cityCode.isEmpty())
    {
        QMessageBox::warning(this, "天气", "请检查输入是否正确！", QMessageBox::Ok);
        return;
    }

    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    mNetAccessManager->get(QNetworkRequest(url));
}

void MainWindow::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &err);
    if(err.error != QJsonParseError::NoError)
    {
        return;
    }

    QJsonObject rootObj = doc.object();
    qDebug() << rootObj.value("message").toString();

    // 1.解析日期和城市
    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();

    // 2.解析 yesterday
    QJsonObject objData = rootObj.value("data").toObject();

    QJsonObject objYesterday = objData.value("yesterday").toObject();
    mDay[0].week = objYesterday.value("week").toString();
    mDay[0].date = objYesterday.value("ymd").toString();

    mDay[0].type = objYesterday.value("type").toString();

    QString s;
    s = objYesterday.value("high").toString().split(" ").at(1);     // 18度
    s = s.left(s.length() - 1);                                     // 18
    mDay[0].high = s.toInt();

    s = objYesterday.value("low").toString().split(" ").at(1);     // 18度
    s = s.left(s.length() - 1);                                    // 18
    mDay[0].low = s.toInt();

    // 风向 和 风力
    mDay[0].fx = objYesterday.value("fx").toString();
    mDay[0].fl = objYesterday.value("fl").toString();

    // 污染指数
    mDay[0].aqi = objYesterday.value("aqi").toDouble();

    // 3.解析 forecast 中 5 天的数据
    QJsonArray forecastArr = objData.value("forecast").toArray();

    for(int i=0; i<5; i++)
    {
        QJsonObject objforecast = forecastArr[i].toObject();
        mDay[i+1].week  = objforecast.value("week").toString();
        mDay[i+1].date  = objforecast.value("ymd").toString();
        // 天气类型
        mDay[i+1].type  = objforecast.value("type").toString();
        // 高温低温
        QString s;
        s = objforecast.value("high").toString().split(" ").at(1);     // 18度
        s = s.left(s.length() - 1);                                   // 18
        mDay[i+1].high = s.toInt();

        s = objforecast.value("low").toString().split(" ").at(1);     // 18度
        s = s.left(s.length() - 1);                                  // 18
        mDay[i+1].low = s.toInt();

        // 风向风力
        mDay[i+1].fx = objforecast.value("fx").toString();
        mDay[i+1].fl = objforecast.value("fl").toString();

        // 污染指数
        mDay[i+1].aqi = objforecast.value("aqi").toDouble();
    }

    // 4.解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();

    mToday.wendu = objData.value("wendu").toString().toInt();
    mToday.shidu = objData.value("shidu").toString();
    mToday.pm25 = objData.value("pm25").toInt();
    mToday.quality = objData.value("quality").toString();

    // 5.forecast 中第一个数组元素，也是今天的数据
    mToday.type = mDay[1].type;
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    // 6.更新 UI
    // 6.1 显示文本和图标
    updateUI();

    // 6.2 绘制温度曲线
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
}

void MainWindow::updateUI()
{
    // 1.更新日期和城市
    ui->lblDate->setText(QDateTime::fromString(mToday.date, "yyyyMMdd").toString("yyyy/MM/dd") + " " + mDay[1].week);
    ui->lblCity->setText(mToday.city);

    // 2.更新今天
    ui->lblTypeIcon->setPixmap(mTypeMap[mToday.type]);
    ui->lblTemp->setText(QString::number(mToday.wendu) + "°");
    ui->lblType->setText(mToday.type);
    ui->lblLowHigh->setText(QString::number(mToday.low) + " ~ " + QString::number(mToday.high) + "°C");

    ui->lblGanMao->setText("感冒指数： " + mToday.ganmao);
    ui->lblWindFl->setText(mToday.fl);
    ui->lblWindFx->setText(mToday.fx);

    ui->lblPM25->setText(QString::number(mToday.pm25));

    ui->lblShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);

    // 3.更新六天
    for(int i=0; i<6; i++)
    {
        // 3.1 更新日期和时间
        mWeekList[i]->setText("周" + mDay[i].week.right(1));
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");

        QStringList ymdList = mDay[i].date.split("-");
        mDateList[i]->setText(ymdList[1] + "/" + ymdList[2]);

        // 3.2 更新天气类型
        mTypeList[i]->setText(mDay[i].type);
        mTypeIconList[i]->setPixmap(mTypeMap[mDay[i].type]);

        // 3.3 更新空气质量
        if(mDay[i].aqi > 0 && mDay[i].aqi <= 50)
        {
            mAqiList[i]->setText("优");
            mAqiList[i]->setStyleSheet("background-color: rgb(121,184,0);");
        }else if(mDay[i].aqi > 50 && mDay[i].aqi <= 100)
        {
            mAqiList[i]->setText("良");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,187,23);");
        }else if(mDay[i].aqi > 100 && mDay[i].aqi <= 150)
        {
            mAqiList[i]->setText("轻度");
            mAqiList[i]->setStyleSheet("background-color: rgb(255,87,97);");
        }else if(mDay[i].aqi > 150 && mDay[i].aqi <= 200)
        {
            mAqiList[i]->setText("中度");
            mAqiList[i]->setStyleSheet("background-color: rgb(235,17,27);");
        }else if(mDay[i].aqi > 200 && mDay[i].aqi <= 250)
        {
            mAqiList[i]->setText("重度");
            mAqiList[i]->setStyleSheet("background-color: rgb(170,0,0);");
        }else
        {
            mAqiList[i]->setText("严重");
            mAqiList[i]->setStyleSheet("background-color: rgb(110,0,0);");
        }
        // 3.4 更新风力、风向
        mFxList[i]->setText(mDay[i].fx);
        mFlList[i]->setText(mDay[i].fl);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint)
    {
        paintHighCurve();
    }
    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint)
    {
        paintLowCurve();
    }
    return QWidget::eventFilter(watched, event);
}

void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);

    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 1.获取x坐标
    int pointX[6] = {0};
    for(int i=0; i<6; i++)
    {
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }

    // 2.获取y坐标
    int tempSum = 0;
    int tempAverage = 0;
    for(int i=0; i<6; i++)
    {
        tempSum += mDay[i].high;
    }
    tempAverage = tempSum / 6;  // 最高温的平均值

    // 计算y轴坐标
    int pointY[6] = {0};
    int yCenter = ui->lblHighCurve->height() / 2;
    for(int i=0; i<6; i++)
    {
        pointY[i] = yCenter - ((mDay[i].high - tempAverage) * INCREMENT);
    }

    // 3.开始绘制
    // 3.1 初始化画笔
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255, 170, 0));

    painter.setPen(pen);
    painter.setBrush(QColor(255, 170, 0));  // 设置画刷-内部填充颜色


    // 3.2 画点、写文本
    for(int i=0; i<6; i++)
    {
        // 显示点
        painter.drawEllipse(QPoint(pointX[i],pointY[i]), POINT_RADIUS, POINT_RADIUS);
        // 显示文本
        painter.drawText(pointX[i] - TEXT_OFFSET_X, pointY[i] - TEXT_OFFSET_Y, QString::number(mDay[i].high) + "°");
    }

    // 3.3 绘制曲线
    for(int i=0; i<5; i++)
    {
        if(i == 0)
        {
            pen.setStyle(Qt::DotLine);  // 虚线
            painter.setPen(pen);
        }else
        {
            pen.setStyle(Qt::SolidLine);  // 实线
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i], pointX[i+1],pointY[i+1]);
    }
}

void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);

    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 1.获取x坐标
    int pointX[6] = {0};
    for(int i=0; i<6; i++)
    {
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }

    // 2.获取y坐标
    int tempSum = 0;
    int tempAverage = 0;
    for(int i=0; i<6; i++)
    {
        tempSum += mDay[i].low;
    }
    tempAverage = tempSum / 6;  // 最高温的平均值

    // 计算y轴坐标
    int pointY[6] = {0};
    int yCenter = ui->lblLowCurve->height() / 2;
    for(int i=0; i<6; i++)
    {
        pointY[i] = yCenter - ((mDay[i].low - tempAverage) * INCREMENT);
    }

    // 3.开始绘制
    // 3.1 初始化画笔
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(0, 255, 255));      // 设置画笔颜色

    painter.setPen(pen);
    painter.setBrush(QColor(0, 255, 255));  // 设置画刷-内部填充颜色


    // 3.2 画点、写文本
    for(int i=0; i<6; i++)
    {
        // 显示点
        painter.drawEllipse(QPoint(pointX[i],pointY[i]), POINT_RADIUS, POINT_RADIUS);
        // 显示文本
        painter.drawText(pointX[i] - TEXT_OFFSET_X, pointY[i] - TEXT_OFFSET_Y, QString::number(mDay[i].low) + "°");
    }

    // 3.3 绘制曲线
    for(int i=0; i<5; i++)
    {
        if(i == 0)
        {
            pen.setStyle(Qt::DotLine);  // 虚线
            painter.setPen(pen);
        }else
        {
            pen.setStyle(Qt::SolidLine);  // 实线
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i], pointX[i+1],pointY[i+1]);
    }
}

void MainWindow::onReplied(QNetworkReply *reply)
{
//    qDebug() << "succuss!";

    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//    qDebug() << "operation : " << reply->operation();
//    qDebug() << "status_code : " << status_code;
//    qDebug() << "url : " << reply->url();
//    qDebug() << "raw header : " << reply->rawHeaderList();

    if(reply->error() != QNetworkReply::NoError || status_code != 200)
    {
//        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this, "天气", "请求数据失败！", QMessageBox::Ok);
    }else
    {
        QByteArray byteArray =  reply->readAll();
        qDebug() << "read all :" << byteArray.data();
        parseJson(byteArray);
    }
    reply->deleteLater();
}


void MainWindow::on_btnSearch_clicked()
{
    QString cityName = ui->leCity->text();
    getWeatherInfo(cityName);
}

