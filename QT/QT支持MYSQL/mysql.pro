TEMPLATE = lib
TARGET = qsqlmysql
CONFIG += plugin
QT += sql-private
INCLUDEPATH += "MySQL安装路径/include"
LIBS += -L"MySQL安装路径/lib" -lmysqlclient
SOURCES += main.cpp qsql_mysql.cpp qsql_mysql_p.h
HEADERS += qsql_mysql_p.h