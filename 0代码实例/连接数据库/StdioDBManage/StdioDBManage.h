

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Fri Jul 22 13:38:08 2022
 */
/* Compiler settings for .\StdioDBManage.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __StdioDBManage_h__
#define __StdioDBManage_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IStdioDB_FWD_DEFINED__
#define __IStdioDB_FWD_DEFINED__
typedef interface IStdioDB IStdioDB;
#endif 	/* __IStdioDB_FWD_DEFINED__ */


#ifndef __StdioDB_FWD_DEFINED__
#define __StdioDB_FWD_DEFINED__

#ifdef __cplusplus
typedef class StdioDB StdioDB;
#else
typedef struct StdioDB StdioDB;
#endif /* __cplusplus */

#endif 	/* __StdioDB_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_StdioDBManage_0000_0000 */
/* [local] */ 

typedef /* [v1_enum][uuid] */  DECLSPEC_UUID("3E7F6883-A445-4f2d-AACA-92A68A416A83") 
enum STD_DATABASE_TYPE
    {	DB_SQLSERVER	= 1,
	DB_ORACLE	= 2,
	DB_SYBASE	= 3,
	DB_DB2	= 4,
	DB_ACCESS	= 5,
	DB_MYSQL	= 6
    } 	STD_DATABASE_TYPE;



extern RPC_IF_HANDLE __MIDL_itf_StdioDBManage_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_StdioDBManage_0000_0000_v0_0_s_ifspec;


#ifndef __STDIODBMANAGELib_LIBRARY_DEFINED__
#define __STDIODBMANAGELib_LIBRARY_DEFINED__

/* library STDIODBMANAGELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_STDIODBMANAGELib;

#ifndef __IStdioDB_INTERFACE_DEFINED__
#define __IStdioDB_INTERFACE_DEFINED__

/* interface IStdioDB */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IStdioDB;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F09B6EFC-F0BA-4A9A-920F-E2C4CD9FA30F")
    IStdioDB : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRecordset( 
            /* [in] */ BSTR bstrSQL,
            /* [retval][out] */ /* external definition not present */ _Recordset **ppRs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExecuteCommand( 
            /* [in] */ BSTR bstrCmd,
            /* [retval][out] */ unsigned long *pulAffectCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateRecordset( 
            /* [in] */ /* external definition not present */ _Recordset *pRs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeviateTransaction( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ManualCommit( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDatabaseType( 
            /* [retval][out] */ long *plDatabaseType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNextID( 
            /* [in] */ BSTR bstrTableName,
            /* [retval][out] */ long *plNextID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRecordsetWithLOB( 
            /* [in] */ BSTR bstrSql,
            /* [retval][out] */ /* external definition not present */ _Recordset **ppRs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateRecordsetWithLOB( 
            /* [in] */ /* external definition not present */ _Recordset *pRs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LockItemData( 
            /* [in] */ BSTR bstrDataID,
            /* [in] */ long lLockType,
            /* [in] */ long lLoginInfoID,
            /* [in] */ long lUserID,
            /* [retval][out] */ long *plLockFlag) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckDBStatus( 
            /* [retval][out] */ long *plDBStatus) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStdioDBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IStdioDB * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IStdioDB * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IStdioDB * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IStdioDB * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IStdioDB * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IStdioDB * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IStdioDB * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRecordset )( 
            IStdioDB * This,
            /* [in] */ BSTR bstrSQL,
            /* [retval][out] */ /* external definition not present */ _Recordset **ppRs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExecuteCommand )( 
            IStdioDB * This,
            /* [in] */ BSTR bstrCmd,
            /* [retval][out] */ unsigned long *pulAffectCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateRecordset )( 
            IStdioDB * This,
            /* [in] */ /* external definition not present */ _Recordset *pRs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeviateTransaction )( 
            IStdioDB * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ManualCommit )( 
            IStdioDB * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDatabaseType )( 
            IStdioDB * This,
            /* [retval][out] */ long *plDatabaseType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNextID )( 
            IStdioDB * This,
            /* [in] */ BSTR bstrTableName,
            /* [retval][out] */ long *plNextID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRecordsetWithLOB )( 
            IStdioDB * This,
            /* [in] */ BSTR bstrSql,
            /* [retval][out] */ /* external definition not present */ _Recordset **ppRs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateRecordsetWithLOB )( 
            IStdioDB * This,
            /* [in] */ /* external definition not present */ _Recordset *pRs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LockItemData )( 
            IStdioDB * This,
            /* [in] */ BSTR bstrDataID,
            /* [in] */ long lLockType,
            /* [in] */ long lLoginInfoID,
            /* [in] */ long lUserID,
            /* [retval][out] */ long *plLockFlag);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckDBStatus )( 
            IStdioDB * This,
            /* [retval][out] */ long *plDBStatus);
        
        END_INTERFACE
    } IStdioDBVtbl;

    interface IStdioDB
    {
        CONST_VTBL struct IStdioDBVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStdioDB_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IStdioDB_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IStdioDB_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IStdioDB_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IStdioDB_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IStdioDB_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IStdioDB_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IStdioDB_GetRecordset(This,bstrSQL,ppRs)	\
    ( (This)->lpVtbl -> GetRecordset(This,bstrSQL,ppRs) ) 

#define IStdioDB_ExecuteCommand(This,bstrCmd,pulAffectCount)	\
    ( (This)->lpVtbl -> ExecuteCommand(This,bstrCmd,pulAffectCount) ) 

#define IStdioDB_UpdateRecordset(This,pRs)	\
    ( (This)->lpVtbl -> UpdateRecordset(This,pRs) ) 

#define IStdioDB_DeviateTransaction(This)	\
    ( (This)->lpVtbl -> DeviateTransaction(This) ) 

#define IStdioDB_ManualCommit(This)	\
    ( (This)->lpVtbl -> ManualCommit(This) ) 

#define IStdioDB_GetDatabaseType(This,plDatabaseType)	\
    ( (This)->lpVtbl -> GetDatabaseType(This,plDatabaseType) ) 

#define IStdioDB_GetNextID(This,bstrTableName,plNextID)	\
    ( (This)->lpVtbl -> GetNextID(This,bstrTableName,plNextID) ) 

#define IStdioDB_GetRecordsetWithLOB(This,bstrSql,ppRs)	\
    ( (This)->lpVtbl -> GetRecordsetWithLOB(This,bstrSql,ppRs) ) 

#define IStdioDB_UpdateRecordsetWithLOB(This,pRs)	\
    ( (This)->lpVtbl -> UpdateRecordsetWithLOB(This,pRs) ) 

#define IStdioDB_LockItemData(This,bstrDataID,lLockType,lLoginInfoID,lUserID,plLockFlag)	\
    ( (This)->lpVtbl -> LockItemData(This,bstrDataID,lLockType,lLoginInfoID,lUserID,plLockFlag) ) 

#define IStdioDB_CheckDBStatus(This,plDBStatus)	\
    ( (This)->lpVtbl -> CheckDBStatus(This,plDBStatus) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IStdioDB_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_StdioDB;

#ifdef __cplusplus

class DECLSPEC_UUID("DD4349CC-788B-4aed-B3A3-4F3F278EECEC")
StdioDB;
#endif
#endif /* __STDIODBMANAGELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


