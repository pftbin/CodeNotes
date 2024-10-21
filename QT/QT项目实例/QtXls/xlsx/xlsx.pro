QT += core gui gui-private

TEMPLATE = lib
DEFINES += QT_BUILD_XLSX_LIB
TARGET = QtXlsx

QMAKE_DOCS = $$PWD/doc/qtxlsx.qdocconf

CONFIG += c++17 plugin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 将函数返回类型不匹配视为错误,避免Release版本崩溃
unix {
    QMAKE_CXXFLAGS += -Werror=return-type
}

# 获取Qt版本号
QT_VERSION = $$QT_VERSION
greaterThan(QT_VERSION, 6.6.3): QMAKE_CXXFLAGS += -Wno-unused-parameter

#定义构建目录
build_dir = $$PWD/../bin
#配置构建目录
DESTDIR = $$build_dir/bin
OBJECTS_DIR = $$build_dir/$$TARGET/obj
MOC_DIR = $$build_dir/$$TARGET/moc
RCC_DIR = $$build_dir/$$TARGET/rcc
win32 {
#Realse版本生成pdb
QMAKE_CFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /MAP
QMAKE_LFLAGS_RELEASE += /debug /opt:ref
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += $$PWD/xlsxdocpropscore_p.h \
    $$PWD/xlsxdocpropsapp_p.h \
    $$PWD/xlsxrelationships_p.h \
    $$PWD/xlsxutility_p.h \
    $$PWD/xlsxsharedstrings_p.h \
    $$PWD/xlsxcontenttypes_p.h \
    $$PWD/xlsxtheme_p.h \
    $$PWD/xlsxformat.h \
    $$PWD/xlsxworkbook.h \
    $$PWD/xlsxstyles_p.h \
    $$PWD/xlsxabstractsheet.h \
    $$PWD/xlsxabstractsheet_p.h \
    $$PWD/xlsxworksheet.h \
    $$PWD/xlsxworksheet_p.h \
    $$PWD/xlsxchartsheet.h \
    $$PWD/xlsxchartsheet_p.h \
    $$PWD/xlsxzipwriter_p.h \
    $$PWD/xlsxworkbook_p.h \
    $$PWD/xlsxformat_p.h \
    $$PWD/xlsxglobal.h \
    $$PWD/xlsxdrawing_p.h \
    $$PWD/xlsxzipreader_p.h \
    $$PWD/xlsxdocument.h \
    $$PWD/xlsxdocument_p.h \
    $$PWD/xlsxcell.h \
    $$PWD/xlsxcell_p.h \
    $$PWD/xlsxdatavalidation.h \
    $$PWD/xlsxdatavalidation_p.h \
    $$PWD/xlsxcellreference.h \
    $$PWD/xlsxcellrange.h \
    $$PWD/xlsxrichstring_p.h \
    $$PWD/xlsxrichstring.h \
    $$PWD/xlsxconditionalformatting.h \
    $$PWD/xlsxconditionalformatting_p.h \
    $$PWD/xlsxcolor_p.h \
    $$PWD/xlsxnumformatparser_p.h \
    $$PWD/xlsxdrawinganchor_p.h \
    $$PWD/xlsxmediafile_p.h \
    $$PWD/xlsxabstractooxmlfile.h \
    $$PWD/xlsxabstractooxmlfile_p.h \
    $$PWD/xlsxchart.h \
    $$PWD/xlsxchart_p.h \
    $$PWD/xlsxsimpleooxmlfile_p.h \
    $$PWD/xlsxcellformula.h \
    $$PWD/xlsxcellformula_p.h \
    xlsx.h

SOURCES += $$PWD/xlsxdocpropscore.cpp \
    $$PWD/xlsxdocpropsapp.cpp \
    $$PWD/xlsxrelationships.cpp \
    $$PWD/xlsxutility.cpp \
    $$PWD/xlsxsharedstrings.cpp \
    $$PWD/xlsxcontenttypes.cpp \
    $$PWD/xlsxtheme.cpp \
    $$PWD/xlsxformat.cpp \
    $$PWD/xlsxstyles.cpp \
    $$PWD/xlsxworkbook.cpp \
    $$PWD/xlsxabstractsheet.cpp \
    $$PWD/xlsxworksheet.cpp \
    $$PWD/xlsxchartsheet.cpp \
    $$PWD/xlsxzipwriter.cpp \
    $$PWD/xlsxdrawing.cpp \
    $$PWD/xlsxzipreader.cpp \
    $$PWD/xlsxdocument.cpp \
    $$PWD/xlsxcell.cpp \
    $$PWD/xlsxdatavalidation.cpp \
    $$PWD/xlsxcellreference.cpp \
    $$PWD/xlsxcellrange.cpp \
    $$PWD/xlsxrichstring.cpp \
    $$PWD/xlsxconditionalformatting.cpp \
    $$PWD/xlsxcolor.cpp \
    $$PWD/xlsxnumformatparser.cpp \
    $$PWD/xlsxdrawinganchor.cpp \
    $$PWD/xlsxmediafile.cpp \
    $$PWD/xlsxabstractooxmlfile.cpp \
    $$PWD/xlsxchart.cpp \
    $$PWD/xlsxsimpleooxmlfile.cpp \
    $$PWD/xlsxcellformula.cpp \
    xlsx.cpp



#Define this macro if you want to run tests, so more AIPs will get exported.
CONFIG(debug, debug|release):DEFINES += XLSX_TEST

QMAKE_TARGET_COMPANY = "Debao Zhang"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2013-2014 Debao Zhang <hello@debao.me>"
QMAKE_TARGET_DESCRIPTION = ".Xlsx file wirter for Qt5"

