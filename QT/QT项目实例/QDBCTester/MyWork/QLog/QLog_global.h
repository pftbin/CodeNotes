#ifndef QLOG_GLOBAL_H
#define QLOG_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QLOG_LIBRARY)
#define QLOG_EXPORT Q_DECL_EXPORT
#else
#define QLOG_EXPORT Q_DECL_IMPORT
#endif

#endif // QLOG_GLOBAL_H
