/*

Created Date: Mar 30 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

void (*g_pfnS_StartStaticSound)(int entnum, int entchannel, struct sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch) = nullptr;
void (*g_pfnS_StartDynamicSound)(int entnum, int entchannel, struct sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch) = nullptr;

EntitySoundMap g_mapEntitySound;

void Sound_InstallHook()
{
	*(void**)&g_pfnS_StartDynamicSound = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, S_StartDynamicSound_SIG, sizeof(S_StartDynamicSound_SIG) - 1);
	if (g_pfnS_StartDynamicSound)
		g_pMetaHookAPI->InlineHook(g_pfnS_StartDynamicSound, S_StartDynamicSound, (void*&)g_pfnS_StartDynamicSound);

	*(void**)&g_pfnS_StartStaticSound = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, S_StartStaticSound_SIG, sizeof(S_StartStaticSound_SIG) - 1);
	if (g_pfnS_StartStaticSound)
		g_pMetaHookAPI->InlineHook(g_pfnS_StartStaticSound, S_StartStaticSound, (void*&)g_pfnS_StartStaticSound);
}

void S_StartStaticSound(int iEntity, int iChannel, struct sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch)
{
	std::string str = std::string((char*)pSFXin);
	g_pfnS_StartStaticSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);

	if (!g_mapEntitySound[iEntity][iChannel])
		g_mapEntitySound[iEntity][iChannel] = gFMODChannelManager::PermanentAllocate(nullptr);
}

void S_StartDynamicSound(int iEntity, int iChannel, struct sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch)
{
	std::string str = std::string((char*)pSFXin);
	g_pfnS_StartDynamicSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);
}
