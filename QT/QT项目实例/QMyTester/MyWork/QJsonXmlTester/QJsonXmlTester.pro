QT += core gui
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../PublicClass/JsonHelper.cpp \
    ../PublicClass/XmlHelper.cpp \
    ../PublicClass/public.cpp \
    QJsonXmlTester.cpp \
    main.cpp

HEADERS += \
    ../PublicClass/JsonHelper.h \
    ../PublicClass/XmlHelper.h \
    ../PublicClass/public.h \
    QJsonXmlTester.h

FORMS += \
    QJsonXmlTester.ui

#添加头文件目录
INCLUDEPATH +=$$PWD/../PublicClass
#添加库目录、库文件
LIBS += -L$$PWD/lib

#定义构建目录
build_dir = $$PWD/../../MyProduct
#配置构建目录
DESTDIR = $$build_dir/bin
OBJECTS_DIR = $$build_dir/$$TARGET/obj
MOC_DIR = $$build_dir/$$TARGET/moc
RCC_DIR = $$build_dir/$$TARGET/rcc
UI_HEADERS_DIR = $$build_dir/$$TARGET/ui_headers

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
