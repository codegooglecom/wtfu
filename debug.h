#ifndef WTF_DEBUG_H
#define WTF_DEBUG_H

#include "config.h"

#ifdef wtfDEBUG_MEMORY_LEAKS
#   ifndef __WXDEBUG__
#       undef wtfDEBUG_MEMORY_LEAKS
#   endif // __WXDEBUG__
#endif // wtfDEBUG_MEMORY_LEAKS

#ifdef __WXDEBUG__
#   ifndef wtfLogMessage
#       define wtfLogMessage wxLogDebug
#   else
#       define wtfLogMessage wxLogMessage
#   endif // wtfLogMessage
#   ifndef wtfLogError
#       define wtfLogError wxLogDebug
#   else
#       define wtfLogError wxLogError
#   endif // wtfLogError
#endif // __WXDEBUG__

#ifdef wtfDEBUG_DISAMBIGUATE
#   define wtfDEBUG_UTIL_H
#endif // wtfDEBUG_DISAMBIGUATE

#endif // WTF_DEBUG_H

