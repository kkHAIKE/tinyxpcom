#ifndef nsXPCOM_h__
#define nsXPCOM_h__

//#ifdef MOZILLA_INTERNAL_API
//# define NS_InitXPCOM2               NS_InitXPCOM2_P
//#endif

#include "nscore.h"

#ifdef __cplusplus
#define DECL_CLASS(c) class c
#else
#define DECL_CLASS(c) typedef struct c c
#endif

DECL_CLASS(nsISupports);
DECL_CLASS(nsIModule);
DECL_CLASS(nsIComponentManager);
DECL_CLASS(nsIServiceManager);
DECL_CLASS(nsIFile);
DECL_CLASS(nsIDirectoryServiceProvider);

typedef nsresult (PR_CALLBACK *nsGetModuleProc)(nsIComponentManager *aCompMgr,
                                                nsIFile* location,
                                                nsIModule** return_cobj);

XPCOM_API(nsresult)
NS_InitXPCOM2(nsIServiceManager* *result,
              nsIFile* binDirectory,
              nsIDirectoryServiceProvider* appFileLocationProvider);

XPCOM_API(nsresult)
NS_ShutdownXPCOM(nsIServiceManager* servMgr);

#endif
