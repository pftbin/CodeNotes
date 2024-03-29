#ifndef QDBC_GLOBAL_H
#define QDBC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QDBC_LIBRARY)
#  define QDBC_EXPORT Q_DECL_EXPORT
#else
#  define QDBC_EXPORT Q_DECL_IMPORT
#endif

#endif // QDBC_GLOBAL_H
