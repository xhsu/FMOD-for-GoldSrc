#ifndef _METAHOOK_H
#define _METAHOOK_H

#include <windows.h>
#include <stdio.h>

typedef int (*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);

typedef struct hook_s hook_t;

#define VIDEOMODE_SOFTWARE 0
#define VIDEOMODE_OPENGL 1
#define VIDEOMODE_D3D 2

typedef struct metahook_api_s
{
	BOOL (*UnHook)(hook_t *pHook);
	hook_t *(*InlineHook)(void *pOldFuncAddr, void *pNewFuncAddr, void *&pCallBackFuncAddr);
	hook_t *(*VFTHook)(void *pClass, int iTableIndex, int iFuncIndex, void *pNewFuncAddr, void *&pCallBackFuncAddr);
	hook_t *(*IATHook)(HMODULE hModule, const char *pszModuleName, const char *pszFuncName, void *pNewFuncAddr, void *&pCallBackFuncAddr);
	void *(*GetClassFuncAddr)(...);
	DWORD (*GetModuleBase)(HMODULE hModule);
	DWORD (*GetModuleSize)(HMODULE hModule);
	HMODULE (*GetEngineModule)(void);
	DWORD (*GetEngineBase)(void);
	DWORD (*GetEngineSize)(void);
	void *(*SearchPattern)(void *pStartSearch, DWORD dwSearchLen, const char *pPattern, DWORD dwPatternLen);
	void (*WriteDWORD)(void *pAddress, DWORD dwValue);
	DWORD (*ReadDWORD)(void *pAddress);
	DWORD (*WriteMemory)(void *pAddress, BYTE *pData, DWORD dwDataSize);
	DWORD (*ReadMemory)(void *pAddress, BYTE *pData, DWORD dwDataSize);
	DWORD (*GetVideoMode)(int *width, int *height, int *bpp, bool *windowed);
	DWORD (*GetEngineBuildnum)(void);
	CreateInterfaceFn (*GetEngineFactory)(void);
	DWORD (*GetNextCallAddr)(void *pAddress, DWORD dwCount);
}
metahook_api_t;

#include "cdll_export.h"

typedef struct mh_enginesave_s
{
	cl_exportfuncs_t *pExportFuncs;
	cl_enginefunc_t *pEngineFuncs;
}
mh_enginesave_t;

#include <../Interface/IFileSystem.h>
#include <../Interface/ICommandLine.h>
#include <../Interface/IRegistry.h>

typedef struct mh_interface_s
{
	ICommandLine *CommandLine;
	IFileSystem *FileSystem;
	IRegistry *Registry;
}
mh_interface_t;

#include <../Interface/IPlugins.h>

extern mh_interface_t *g_pInterface;
extern cl_enginefunc_t gEngfuncs;
extern metahook_api_t *g_pMetaHookAPI;
extern mh_enginesave_t *g_pMetaSave;
extern DWORD g_dwEngineBase;
extern DWORD g_dwEngineSize;
extern DWORD g_dwEngineBuildnum;
#endif
