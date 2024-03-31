QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    weathertool.cpp

HEADERS += \
    mainwindow.h \
    weatherdata.h \
    weathertool.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TARGET = WeatherForecast

# 获取当前 .pro 文件所在的绝对路径
PRO_FILE_PWD = $$PWD

# 将路径作为宏定义传递给编译后的可执行文件
DEFINES += PRO_FILE_PWD=\\\"$$PRO_FILE_PWD\\\"

RESOURCES += \
    res.qrc \
    weather.qrc
