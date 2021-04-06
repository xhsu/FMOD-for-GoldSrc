/*

Created Date: Mar 30 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

void (*g_pfnS_StartStaticSound)(int entnum, int entchannel, struct sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch) = nullptr;
void (*g_pfnS_StartDynamicSound)(int entnum, int entchannel, struct sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch) = nullptr;

EntitySoundMap g_mapEntitySound;
PositionSoundMap g_mapPositionSounds;

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
	std::string szSample = std::string((char*)pSFXin);
	g_pfnS_StartStaticSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);

	auto pEntity = gEngfuncs.GetEntityByIndex(iEntity);
	FMOD::Channel** ppChannel = nullptr;
	FMOD_VECTOR pos = g_fmodvecZero;

	// No eneity or entity model? You must be a dummy sounder.
	if (!pEntity || !pEntity->model)
	{
		if (g_mapPositionSounds.find(vecOrigin) == g_mapPositionSounds.end())	// No found.
		{
			g_mapPositionSounds[vecOrigin].m_ppChannel = gFMODChannelManager::PermanentAllocate(&g_mapPositionSounds[vecOrigin].m_uIndex);
		}

		ppChannel = g_mapPositionSounds[vecOrigin].m_ppChannel;
		pos = VecConverts(vecOrigin, true);
	}

	else if (!g_mapEntitySound[iEntity][iChannel].m_ppChannel)
	{
		ppChannel = g_mapEntitySound[iEntity][iChannel].m_ppChannel = gFMODChannelManager::PermanentAllocate(&g_mapEntitySound[iEntity][iChannel].m_uIndex);
		pos = VecConverts(pEntity->origin, true);
	}
	else
	{
		ppChannel = g_mapEntitySound[iEntity][iChannel].m_ppChannel;
		pos = VecConverts(pEntity->origin, true);
	}

	if (bitsFlags & SND_STOP)
	{
		(*ppChannel)->stop();
	}
	else if (bitsFlags & SND_CHANGE_PITCH)
	{
		(*ppChannel)->setPitch(float(iPitch) / 100.0f);
	}
	else if (bitsFlags & SND_CHANGE_VOL)
	{
		(*ppChannel)->setVolume(flVolume);
		(*ppChannel)->setVolumeRamp(true);
	}
	/*
	else if (bitsFlags & SND_SPAWNING)
	{
		// LUNA: What to do with this?????
	}
	*/
	else
	{
		auto pSound = PrecacheSound(szSample, FMOD_DEFAULT_IN_GOLDSRC);
		pSound->set3DMinMaxDistance(1.0f / SND_DISTANCEFACTOR, 1000.0f / SND_DISTANCEFACTOR);	// TODO

		gFModSystem->playSound(pSound, nullptr, true, ppChannel);	// Have to activate the channel first. Otherwise it will be a nullptr.
		(*ppChannel)->set3DAttributes(&pos, &g_fmodvecZero);
		(*ppChannel)->setVolume(flVolume);
		(*ppChannel)->setVolumeRamp(false);
		(*ppChannel)->setPitch(float(iPitch) / 100.0f);	// original formula for most CS weapons: 94 + gEngfuncs.pfnRandomLong(0, 0xf)
		(*ppChannel)->setPaused(false);	// Keep this as the final step.
	}
}

void S_StartDynamicSound(int iEntity, int iChannel, struct sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch)
{
	std::string str = std::string((char*)pSFXin);
	g_pfnS_StartDynamicSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);
}
