QT -= gui
QT += sql

TEMPLATE = lib
DEFINES += QDBC_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#定义构建目录
build_dir = $$PWD/../../MyProduct
#配置构建目录
DESTDIR = $$build_dir/bin
OBJECTS_DIR = $$build_dir/$$TARGET/obj
MOC_DIR = $$build_dir/$$TARGET/moc


SOURCES += \
    DBC.cpp \
    QDBC.cpp \
    QDBObject.cpp \
    QLogger.cpp

HEADERS += \
    DBC.h \
    IDBC.h \
    QDBC_global.h \
    QDBC.h \
    QDBObject.h \
    QLogger.h




# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
