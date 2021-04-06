/*

Created Date: Mar 30 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

void (*g_pfnS_StartStaticSound)(int entnum, int entchannel, sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch) = nullptr;
void (*g_pfnS_StartDynamicSound)(int entnum, int entchannel, sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch) = nullptr;
void *(*g_pfnCache_Check)(cache_user_t* c) = nullptr;

EntitySoundMap g_mapEntitySound;
PositionSoundMap g_mapPositionSounds;

void Sound_InstallHook()
{
	*(void**)&g_pfnS_StartDynamicSound = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, S_StartDynamicSound_SIG, sizeof(S_StartDynamicSound_SIG) - 1U);
	if (g_pfnS_StartDynamicSound)
		g_pMetaHookAPI->InlineHook(g_pfnS_StartDynamicSound, S_StartDynamicSound, (void*&)g_pfnS_StartDynamicSound);
	else
		Sys_Error("Function \"S_StartDynamicSound\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);

	*(void**)&g_pfnS_StartStaticSound = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, S_StartStaticSound_SIG, sizeof(S_StartStaticSound_SIG) - 1U);
	if (g_pfnS_StartStaticSound)
		g_pMetaHookAPI->InlineHook(g_pfnS_StartStaticSound, S_StartStaticSound, (void*&)g_pfnS_StartStaticSound);
	else
		Sys_Error("Function \"S_StartStaticSound\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);

	*(void**)&g_pfnCache_Check = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, Cache_Check_SIG, sizeof(Cache_Check_SIG) - 1U);
	if (!g_pfnCache_Check)
		Sys_Error("Function \"Cache_Check\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);
}

bool Sound_IsLoopedByWav(sfx_t* pSFXin)
{
	sfxcache_t* sc = (sfxcache_t *)g_pfnCache_Check(&pSFXin->cache);

	return sc->loopstart >= 0;
}

void S_StartStaticSound(int iEntity, int iChannel, sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch)
{
	cl_entity_t* pEntity = gEngfuncs.GetEntityByIndex(iEntity);
	bool bDummyEntity = (!pEntity || !pEntity->model);

	if (Sound_IsLoopedByWav(pSFXin))
		gEngfuncs.pfnRandomLong(0, 1);

	if (bDummyEntity || !strstr(pSFXin->name, "weapons"))
		g_pfnS_StartStaticSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);
	//else if (gEngfuncs.pEfxAPI->EV_IsLocal(iEntity - 1))
	//{
	//	PlaySound(pSFXin->name);
	//}
	else
	{
		FMOD::Sound* pSound = PrecacheSound(pSFXin->name, FMOD_DEFAULT_IN_GOLDSRC);
		FMOD_VECTOR	pos = VecConverts(vecOrigin, true);

		pSound->set3DMinMaxDistance(0.1f / SND_DISTANCEFACTOR, (972.85404f / fmax(0.001f, flAttenuation)) / SND_DISTANCEFACTOR);

		if (!g_mapEntitySound[iEntity][iChannel].m_ppChannel)
			gFMODChannelManager::PermanentAllocate(&g_mapEntitySound[iEntity][iChannel]);

		auto ppChannel = g_mapEntitySound[iEntity][iChannel].m_ppChannel;
		gFModSystem->playSound(pSound, nullptr, true, ppChannel);	// Have to activate the channel first. Otherwise it will be a nullptr.
		(*ppChannel)->set3DAttributes(&pos, &g_fmodvecZero);
		(*ppChannel)->setVolume(flVolume);
		(*ppChannel)->setVolumeRamp(false);
		(*ppChannel)->setPitch(float(iPitch) / 100.0f);	// original formula for most CS weapons: 94 + gEngfuncs.pfnRandomLong(0, 0xf)
		(*ppChannel)->setPaused(false);	// Keep this as the final step.
	}

	//std::string szSample = std::string((char*)pSFXin);
	//cl_entity_t* pEntity = gEngfuncs.GetEntityByIndex(iEntity);
	//FMOD::Channel** ppChannel = nullptr;
	//FMOD_VECTOR pos = g_fmodvecZero;
	//fmod_channel_info_t* pChannelInfo = nullptr;

	//// No eneity or entity model? You must be a dummy sounder.
	//if (!pEntity || !pEntity->model)
	//{
	//	if (!g_mapPositionSounds[vecOrigin].m_ppChannel)	// No found.
	//	{
	//		gFMODChannelManager::PermanentAllocate(&g_mapPositionSounds[vecOrigin]);
	//	}

	//	ppChannel = g_mapPositionSounds[vecOrigin].m_ppChannel;
	//	pChannelInfo = &g_mapPositionSounds[vecOrigin];
	//	pos = VecConverts(vecOrigin, true);
	//}

	//else if (!g_mapEntitySound[iEntity][iChannel].m_ppChannel)
	//{
	//	ppChannel = g_mapEntitySound[iEntity][iChannel].m_ppChannel = gFMODChannelManager::PermanentAllocate(&g_mapEntitySound[iEntity][iChannel].m_uIndex);
	//	pChannelInfo = &g_mapEntitySound[iEntity][iChannel];
	//	pos = VecConverts(pEntity->origin, true);
	//}
	//else
	//{
	//	ppChannel = g_mapEntitySound[iEntity][iChannel].m_ppChannel;
	//	pChannelInfo = &g_mapEntitySound[iEntity][iChannel];
	//	pos = VecConverts(pEntity->origin, true);
	//}

	//if (bitsFlags & SND_STOP)
	//{
	//	(*ppChannel)->stop();

	//	gFMODChannelManager::Free(pChannelInfo);	// The sound is over. Free it.
	//}
	//else if (bitsFlags & SND_CHANGE_PITCH)
	//{
	//	(*ppChannel)->setPitch(float(iPitch) / 100.0f);
	//}
	//else if (bitsFlags & SND_CHANGE_VOL)
	//{
	//	(*ppChannel)->setVolume(flVolume);
	//	(*ppChannel)->setVolumeRamp(true);
	//}
	///*
	//else if (bitsFlags & SND_SPAWNING)
	//{
	//	// LUNA: What to do with this?????
	//}
	//*/
	//else
	//{
	//	auto pSound = PrecacheSound(szSample, FMOD_DEFAULT_IN_GOLDSRC);
	//	pSound->set3DMinMaxDistance(1.0f / SND_DISTANCEFACTOR, 1000.0f / SND_DISTANCEFACTOR);	// TODO

	//	gFModSystem->playSound(pSound, nullptr, true, ppChannel);	// Have to activate the channel first. Otherwise it will be a nullptr.
	//	(*ppChannel)->set3DAttributes(&pos, &g_fmodvecZero);
	//	(*ppChannel)->setVolume(flVolume);
	//	(*ppChannel)->setVolumeRamp(false);
	//	(*ppChannel)->setPitch(float(iPitch) / 100.0f);	// original formula for most CS weapons: 94 + gEngfuncs.pfnRandomLong(0, 0xf)
	//	(*ppChannel)->setPaused(false);	// Keep this as the final step.
	//}
}

void S_StartDynamicSound(int iEntity, int iChannel, sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch)
{
	cl_entity_t* pEntity = gEngfuncs.GetEntityByIndex(iEntity);
	bool bDummyEntity = (!pEntity || !pEntity->model);

	if (Sound_IsLoopedByWav(pSFXin))
		gEngfuncs.pfnRandomLong(0, 1);

	if (bDummyEntity || !strstr((const char*)pSFXin, "weapons"))
		g_pfnS_StartDynamicSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);
	else
	{
		FMOD::Sound* pSound = PrecacheSound((const char*)pSFXin, FMOD_DEFAULT_IN_GOLDSRC);
		FMOD_VECTOR	pos = VecConverts(vecOrigin, true);

		pSound->set3DMinMaxDistance(0.1f / SND_DISTANCEFACTOR, (972.85404f / fmax(0.001f, flAttenuation)) / SND_DISTANCEFACTOR);

		if (!g_mapEntitySound[iEntity][iChannel].m_ppChannel)
			gFMODChannelManager::PermanentAllocate(&g_mapEntitySound[iEntity][iChannel]);

		auto ppChannel = g_mapEntitySound[iEntity][iChannel].m_ppChannel;
		gFModSystem->playSound(pSound, nullptr, true, ppChannel);	// Have to activate the channel first. Otherwise it will be a nullptr.
		(*ppChannel)->set3DAttributes(&pos, &g_fmodvecZero);
		(*ppChannel)->setVolume(flVolume);
		(*ppChannel)->setVolumeRamp(false);
		(*ppChannel)->setPitch(float(iPitch) / 100.0f);	// original formula for most CS weapons: 94 + gEngfuncs.pfnRandomLong(0, 0xf)
		(*ppChannel)->setPaused(false);	// Keep this as the final step.
	}
}
