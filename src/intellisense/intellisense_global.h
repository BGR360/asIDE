#ifndef INTELLISENSE_GLOBAL_H
#define INTELLISENSE_GLOBAL_H

#if defined(INTELLISENSE_LIBRARY)
#  define INTELLISENSE_EXPORT Q_DECL_EXPORT
#else
#  define INTELLISENSE_EXPORT Q_DECL_IMPORT
#endif

#endif // INTELLISENSE_GLOBAL_H
