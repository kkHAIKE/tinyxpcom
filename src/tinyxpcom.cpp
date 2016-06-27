#ifndef nsXPCOM_h__
#include "nsXPCOM.h"
#endif

#ifndef nsCOMPtr_h__
#include "nsCOMPtr.h"
#endif

#include "nsIGenericFactory.h"
#include "nsIServiceManager.h"

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

#include <prlock.h>
#include <prlink.h>
#include <prio.h>

class Module;
class Component;
static std::map<std::string, Module*> g_sonameMap;
static std::map<std::string, Component*> g_cidMap;
static std::map<std::string, Component*> g_contractMap;
static Module* g_lastModule;

class Component
{
public:
    const nsModuleComponentInfo *info;

    Module* module;
    std::string cid;
    std::string contractID;
    PRInt32 isSet;

    Component():info(nsnull), module(nsnull), isSet(0) {}
};

class Module
{
public:
    const nsModuleInfo *info;

    std::string soname;
    PRInt64 modifyTime;
    std::vector<Component*> components;
    PRLock *lock;
    bool isLoad;
    bool isInit;

    Module():info(nsnull), isLoad(false), isInit(false) {
        lock = PR_NewLock();
    }

    ~Module() {
        freeComponents();
        PR_DestroyLock (lock);
    }

    void freeComponents() {
        for (size_t i = 0; i < components.size (); ++i) {
            Component* component = components[i];

            g_cidMap.erase (component->cid);
            g_contractMap.erase (component->contractID);
            delete component;
        }
        components.clear ();
    }

    void addComponent(Component* component) {
        component->module = this;
        components.push_back (component);
    }

    nsresult load() {
        std::string fpath = "components/" + soname;
        PRLibrary* lib = PR_LoadLibrary (fpath.c_str ());
        if (lib == nsnull) {
            return NS_ERROR_FAILURE;
        }

        nsGetModuleProc proc = (nsGetModuleProc)PR_FindFunctionSymbol(lib, "NSGetModule");
        if (proc) {
            proc (nsnull, nsnull, nsnull);
            return NS_OK;
        }
        PR_UnloadLibrary (lib);
        return NS_ERROR_FAILURE;
    }

    nsresult staticLoad() {
        g_lastModule = this;
        nsresult rv = load();
        if (NS_SUCCEEDED (rv)) {
            isLoad = true;
        }
        g_lastModule = nsnull;
        return rv;
    }

    nsresult dynamicLoad() {
        // 先检测有没有载入
        nsresult rv = NS_OK;
        if (!isLoad) {
            PR_Lock (lock);
            if (!isLoad) {
                rv = load();
            }
            PR_Unlock (lock);
        }
        if (NS_FAILED (rv)) return rv;

        // 检测有没有初始化
        if (!isInit) {
            PR_Lock (lock);
            if (!isInit) {
                rv = info->mCtor (nsnull);
            }
            PR_Unlock (lock);
        }
        return rv;
    }
};

void loadDat(std::ifstream &fp)
{
    std::string tmpline;
    int step = 0;
    while (std::getline (fp, tmpline)) {
        if (tmpline.empty ()) {
            step = 0;
            continue;
        }

        if (step == 1) {
            size_t p = tmpline.rfind (',');

            Module *module = new Module();
            module->soname = tmpline.substr (4, p - 4);
            sscanf (tmpline.substr (p + 1).c_str (), "%ld", &module->modifyTime);

            g_sonameMap[module->soname] = module;
        }
        else if (step == 2) {
            size_t p = tmpline.find (',');
            size_t q = tmpline.rfind (':');

            Component *component = new Component();
            component->cid = tmpline.substr (0, p);
            std::string soname = tmpline.substr (q + 1);

            std::map<std::string, Module*>::iterator it = g_sonameMap.find (soname);
            if (it == g_sonameMap.end ()) {
                delete component;
            } else {
                g_cidMap[component->cid] = component;
                it->second->addComponent (component);
            }
        }
        else if (step == 3) {
            size_t p = tmpline.rfind (',');

            std::string cid = tmpline.substr (p + 1);
            std::map<std::string, Component*>::iterator it = g_cidMap.find (cid);
            if (it != g_cidMap.end ()) {
                it->second->contractID = tmpline.substr (0, p);
                g_contractMap[it->second->contractID] = it->second;
            }
        }
        else if (tmpline == "[COMPONENTS]") {
            step = 1;
        }
        else if (tmpline == "[CLASSIDS]") {
            step = 2;
        }
        else if (tmpline == "[CONTRACTIDS]") {
            step = 3;
        }
    }
}
void saveDat(std::ofstream &fp)
{
    fp << "Generated File. Do not edit." << std::endl << std::endl;
    fp << "[HEADER]" << std::endl;
    fp << "Version,0,5" << std::endl << std::endl;

    fp << "[COMPONENTS]" << std::endl;
    for (std::map<std::string, Module*>::iterator it = g_sonameMap.begin (); it != g_sonameMap.end (); ++it) {
        fp << "rel:" << it->first << "," << it->second->modifyTime << std::endl;
    }

    fp << std::endl << "[CLASSIDS]" << std::endl;
    for (std::map<std::string, Component*>::iterator it = g_cidMap.begin (); it != g_cidMap.end (); ++it) {
        fp << it->first << ",,application/x-mozilla-native,,rel:" << it->second->module->soname << std::endl;
    }

    fp << std::endl << "[CONTRACTIDS]" << std::endl;
    for (std::map<std::string, Component*>::iterator it = g_contractMap.begin (); it != g_contractMap.end (); ++it) {
        fp << it->first << "," << it->second->cid << std::endl;
    }

    fp << std::endl << "[CATEGORIES]";
}

EXPORT_XPCOM_API(nsresult)
NS_InitXPCOM2(nsIServiceManager* *,
                              nsIFile*,
                              nsIDirectoryServiceProvider*)
{
    // 入口三个参数都不管
    // 先载入dat文件
    bool save = false;
    std::ifstream fp ("components/compreg.dat");
    if (fp.is_open ()) {
        loadDat (fp);
        fp.close ();

        // 检测所有的文件时间
        for (std::map<std::string, Module*>::iterator it = g_sonameMap.begin (); it != g_sonameMap.end (); ++it) {
            std::string fpath = "components/" + it->first;
            PRFileInfo64 info;

            PRStatus status = PR_GetFileInfo64(fpath.c_str (), &info);
            if (status != PR_SUCCESS) {
                // 其实可以删除这个入口，现到最终load处理
            } else {
                PRInt64 usecPerMsec, ftime;
                LL_I2L(usecPerMsec, PR_USEC_PER_MSEC);
                LL_DIV(ftime, info.modifyTime, usecPerMsec);
                if (ftime != it->second->modifyTime) {
                    // 重载
                    it->second->freeComponents ();
                    if (NS_SUCCEEDED (it->second->staticLoad ())) {
                        save = true;
                    }
                }
            }
        }
    } else {
        // 扫描
        PRDir* fdir = PR_OpenDir ("components");
        if (fdir != nsnull) {
            PRDirEntry* entry;
            while (entry = PR_ReadDir(fdir, PR_SKIP_BOTH), entry != nsnull) {
                const char *p = strrchr (entry->name, '.');
                if (p && (strcmp (p, ".so") == 0 || strcmp (p, ".dll") == 0)) {
                    std::string fpath = "components/";
                    fpath += entry->name;
                    PRFileInfo64 info;
                    PR_GetFileInfo64(fpath.c_str (), &info);

                    Module* module = new Module();
                    module->soname = entry->name;
                    PRInt64 usecPerMsec;
                    LL_I2L(usecPerMsec, PR_USEC_PER_MSEC);
                    LL_DIV(module->modifyTime, info.modifyTime, usecPerMsec);

                    if (NS_SUCCEEDED (module->staticLoad ())) {
                        save = true;
                        g_sonameMap[module->soname] = module;
                    } else {
                        delete module;
                    }
                }
            }

            PR_CloseDir (fdir);
        }
    }

    if (save) {
        // 采用移动方式
        pthread_t tid = pthread_self();
        char fpath[255];
        sprintf (fpath, "components/compreg.dat.%lu", tid);
        std::ofstream fp (fpath);
        if (fp.is_open ()) {
            saveDat (fp);
            fp.close ();

            PR_Rename (fpath, "components/compreg.dat");
        }
    }

    return NS_OK;
}

EXPORT_XPCOM_API(nsresult)
NS_ShutdownXPCOM(nsIServiceManager* servMgr)
{
    return NS_OK;
}

NS_COM_GLUE nsresult
NS_NewGenericModule2(nsModuleInfo const *info, nsIModule* *)
{
    if (g_lastModule) {
        // 静态载入
        g_lastModule->info = info;
        for (PRUint32 i = 0; i < info->mCount; ++i) {
            Component* component = new Component();
            component->info = info->mComponents + i;
            component->contractID = component->info->mContractID;

            char cid[NSID_LENGTH];
            component->info->mCID.ToProvidedString (cid);
            component->cid = cid;

            g_cidMap[component->cid] = component;
            g_contractMap[component->contractID] = component;
            g_lastModule->addComponent (component);
        }
    } else {
        // 动态载入
        Module* module = nsnull;
        for (PRUint32 i = 0; i < info->mCount; ++i) {
            std::map<std::string, Component*>::iterator it = g_contractMap.find (info->mComponents[i].mContractID);
            if (it != g_contractMap.end ()) {
                Component* component = it->second;

                if (module == nsnull) {
                    module = component->module;
                    module->info = info;
                }

                component->info = info->mComponents + i;
            }
        }
    }

    return NS_OK;
}

///////////////////////////////////////

nsresult
CallGetService(const nsCID &aCID, const nsIID &aIID, void **aResult)
{
    return NS_ERROR_FAILURE;
}
nsresult
CallGetService(const char *aContractID, const nsIID &aIID, void **aResult)
{
    return NS_ERROR_FAILURE;
}
nsresult
CallCreateInstance(const nsCID &aCID, nsISupports *aDelegate,
                   const nsIID &aIID, void **aResult)
{
    return NS_ERROR_FAILURE;
}
nsresult
CallCreateInstance(const char *aContractID, nsISupports *aDelegate,
                   const nsIID &aIID, void **aResult)
{
    std::map<std::string, Component*>::iterator it = g_contractMap.find (aContractID);
    if (it == g_contractMap.end ()) {
        return NS_ERROR_SERVICE_NOT_FOUND;
    }

    if (PR_ATOMIC_ADD(&it->second->isSet, 0) == 0) {
        // 没有载入完全动态载入
        nsresult rv = it->second->module->dynamicLoad ();
        if (NS_FAILED(rv)) return rv;

        PR_ATOMIC_SET(&it->second->isSet, 1);
    }

    if (it->second->info == nsnull) {
        return NS_ERROR_FACTORY_NOT_REGISTERED;
    }

    return it->second->info->mConstructor(aDelegate, aIID, aResult);
}
