#ifndef nsXPCOM_h__
#include "nsXPCOM.h"
#endif

#ifndef nsCOMPtr_h__
#include "nsCOMPtr.h"
#endif

#include "nsComponentManagerUtils.h"

nsresult
CallGetService(const nsCID &aCID, const nsIID &aIID, void **aResult);
nsresult
CallGetService(const char *aContractID, const nsIID &aIID, void **aResult);
nsresult
CallCreateInstance(const nsCID &aCID, nsISupports *aDelegate,
                   const nsIID &aIID, void **aResult);
nsresult
CallCreateInstance(const char *aContractID, nsISupports *aDelegate,
                   const nsIID &aIID, void **aResult);

nsresult
nsCreateInstanceByContractID::operator()( const nsIID& aIID, void** aInstancePtr ) const
{
    nsresult status = CallCreateInstance(mContractID, mOuter, aIID, aInstancePtr);
    if (NS_FAILED(status))
        *aInstancePtr = 0;
    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}

nsresult
nsGetServiceByCID::operator()( const nsIID& aIID, void** aInstancePtr ) const
{
    nsresult status = CallGetService(mCID, aIID, aInstancePtr);
    if ( NS_FAILED(status) )
        *aInstancePtr = 0;

    return status;
}

nsresult
nsGetServiceByCIDWithError::operator()( const nsIID& aIID, void** aInstancePtr ) const
{
    nsresult status = CallGetService(mCID, aIID, aInstancePtr);
    if ( NS_FAILED(status) )
        *aInstancePtr = 0;

    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}

nsresult
nsGetServiceByContractID::operator()( const nsIID& aIID, void** aInstancePtr ) const
{
    nsresult status = CallGetService(mContractID, aIID, aInstancePtr);
    if ( NS_FAILED(status) )
        *aInstancePtr = 0;

    return status;
}

nsresult
nsGetServiceByContractIDWithError::operator()( const nsIID& aIID, void** aInstancePtr ) const
{
    nsresult status = CallGetService(mContractID, aIID, aInstancePtr);
    if ( NS_FAILED(status) )
        *aInstancePtr = 0;

    if ( mErrorPtr )
        *mErrorPtr = status;
    return status;
}
