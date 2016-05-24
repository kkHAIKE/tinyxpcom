#ifndef nsDebug_h___
#define nsDebug_h___

#ifndef nscore_h___
#include "nscore.h"
#endif

#ifndef nsError_h__
#include "nsError.h"
#endif

#define NS_ABORT_IF_FALSE(_expr, _msg) PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_WARN_IF_FALSE(_expr, _msg)  PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_PRECONDITION(expr, str)     PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_ASSERTION(expr, str)        PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_POSTCONDITION(expr, str)    PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_NOTYETIMPLEMENTED(str)      PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_NOTREACHED(str)             PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_ERROR(str)                  PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_WARNING(str)                PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_ABORT()                     PR_BEGIN_MACRO /* nothing */ PR_END_MACRO
#define NS_BREAK()                     PR_BEGIN_MACRO /* nothing */ PR_END_MACRO

#endif
