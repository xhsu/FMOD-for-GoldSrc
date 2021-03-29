/*

Created Date: Mar 29 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

cl_exportfuncs_t gExportfuncs;
mh_interface_t* g_pInterface;
metahook_api_t* g_pMetaHookAPI;
mh_enginesave_t* g_pMetaSave;

DWORD g_dwEngineBase = 0;
DWORD g_dwEngineSize = 0;
DWORD g_dwEngineBuildnum = 0;

void IPlugins::Init(metahook_api_t* pAPI, mh_interface_t* pInterface, mh_enginesave_t* pSave)
{
	g_pInterface = pInterface;
	g_pMetaHookAPI = pAPI;
	g_pMetaSave = pSave;
}

void IPlugins::Shutdown(void)
{
}

void IPlugins::LoadEngine(void)
{
	g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();
	g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
	g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();
}

void IPlugins::LoadClient(cl_exportfuncs_t* pExportFunc)
{
	// client.dll initialize.
	Q_memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));

	// Hooks
	pExportFunc->Initialize = &Initialize;
	pExportFunc->HUD_Redraw = &HUD_Redraw;
}

void IPlugins::ExitGame(int iResult)
{
}

EXPOSE_SINGLE_INTERFACE(IPlugins, IPlugins, METAHOOK_PLUGIN_API_VERSION);
