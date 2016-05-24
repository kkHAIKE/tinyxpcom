#ifndef nsComponentManagerUtils_h__
#define nsComponentManagerUtils_h__

#ifndef nscore_h__
#include "nscore.h"
#endif

#ifndef nsCOMPtr_h__
#include "nsCOMPtr.h"
#endif

class NS_COM_GLUE nsCreateInstanceByContractID : public nsCOMPtr_helper
{
public:
    nsCreateInstanceByContractID( const char* aContractID, nsISupports* aOuter, nsresult* aErrorPtr )
        : mContractID(aContractID),
          mOuter(aOuter),
          mErrorPtr(aErrorPtr)
    {
        // nothing else to do here
    }
    
    virtual nsresult NS_FASTCALL operator()( const nsIID&, void** ) const;
    
private:
    const char*   mContractID;
    nsISupports*  mOuter;
    nsresult*     mErrorPtr;
};


inline
const nsCreateInstanceByContractID
do_CreateInstance( const char* aContractID, nsresult* error = 0 )
{
    return nsCreateInstanceByContractID(aContractID, 0, error);
}

#endif

