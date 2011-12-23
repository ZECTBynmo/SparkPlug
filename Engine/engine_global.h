#ifndef ENGINE_GLOBAL_H
#define ENGINE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef ENGINE_LIB
# define ENGINE_EXPORT Q_DECL_EXPORT
#else
# define ENGINE_EXPORT Q_DECL_IMPORT
#endif

#endif // ENGINE_GLOBAL_H
