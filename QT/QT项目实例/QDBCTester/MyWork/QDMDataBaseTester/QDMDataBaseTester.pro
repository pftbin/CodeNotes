QT += core gui
QT += xml
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#定义构建目录
build_dir = $$PWD/../../MyProduct
#配置构建目录
DESTDIR = $$build_dir/bin
OBJECTS_DIR = $$build_dir/$${TARGET}/obj
MOC_DIR = $$build_dir/$${TARGET}/moc

#DBC
INCLUDEPATH += $$PWD/../QDBC
LIBS += -L$$build_dir/bin -lQDBC
QMAKE_LFLAGS += -Wl,-rpath,$$DESTDIR

#LOG
INCLUDEPATH += $$PWD/../QLog
LIBS += -L$$build_dir/bin -lQLog
QMAKE_LFLAGS += -Wl,-rpath,$$DESTDIR


#添加头文件目录
INCLUDEPATH +=$$PWD/../Public_H
INCLUDEPATH +=$$PWD/../Public_H/widget
INCLUDEPATH +=$$PWD/../QDBC


SOURCES += \
    ../Public_H/public.cpp \
    ../Public_H/JsonHelper.cpp \
    ../Public_H/XmlHelper.cpp \
    ../Public_H/widget/QMyTableView.cpp \
    QDMDataBaseTester.cpp \
    main.cpp

HEADERS += \
    ../Public_H/public.h \
    ../Public_H/JsonHelper.h \
    ../Public_H/XmlHelper.h \
    ../Public_H/widget/QMyTableView.h \
    QDMDataBaseTester.h

FORMS += \
    QDMDataBaseTester.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
