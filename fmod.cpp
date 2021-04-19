/*

Created Date: Apr 05 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

#define FMOD_DEFAULT_IN_GOLDSRC		(FMOD_LOOP_OFF | FMOD_3D | FMOD_3D_WORLDRELATIVE | FMOD_3D_LINEARROLLOFF)
#define FMOD_MAX_CHANNEL_GOLDSRC	4092	// just... max it out, according to user manual. Reserve an additional channel for all other 2D first-personal SFX.

FMOD::System* gFModSystem = nullptr;
FMOD::Channel* g_phLocal2DChannel = nullptr;

double g_flClientTime = 0;
double g_flClientTimeDelta = 0;

static std::unordered_map<std::string, FMOD::Sound*> g_mapSoundPrecache;
static char szBuffer1[256], szBuffer2[1024];

namespace gFMODChannelManager
{
	struct channel_info_s
	{
		FMOD::Channel* m_pChannel;
		double m_flNextAvailable;
	};
	std::array<channel_info_s, FMOD_MAX_CHANNEL_GOLDSRC> m_Channels;

	FMOD::Channel** Allocate(float flCooldown, fmod_channel_info_t* infoReturned)
	{
		size_t iMin = 0;
		double flMinTime = 99999999.0;

		for (size_t i = 0; i < FMOD_MAX_CHANNEL_GOLDSRC; i++)
		{
			if (m_Channels[i].m_flNextAvailable < g_flClientTime)
			{
				iMin = i;
				break;
			}

			// record the time by the way.
			if (m_Channels[i].m_flNextAvailable < flMinTime)
			{
				iMin = i;
				flMinTime = m_Channels[i].m_flNextAvailable;
			}
		}

		if (infoReturned)	// If additional info is required.
		{
			infoReturned->m_uIndex = iMin;
			infoReturned->m_ppChannel = &m_Channels[iMin].m_pChannel;
		}

		// just occuiped a furthest unused channel.
		m_Channels[iMin].m_flNextAvailable = g_flClientTime + flCooldown;
		return &m_Channels[iMin].m_pChannel;
	}

	FMOD::Channel** PermanentAllocate(size_t* piIndex)
	{
		size_t iMin = 0, i = 0;
		double flMinTime = 99999999.0;

		for (i = 0; i < FMOD_MAX_CHANNEL_GOLDSRC; i++)
		{
			if (m_Channels[i].m_flNextAvailable < g_flClientTime)
			{
				iMin = i;
				break;
			}

			// record the time by the way.
			if (m_Channels[i].m_flNextAvailable < flMinTime)
			{
				iMin = i;
				flMinTime = m_Channels[i].m_flNextAvailable;
			}
		}

		// just occuiped a furthest unused channel.
		m_Channels[iMin].m_flNextAvailable = g_flClientTime + 31622400.0;

		if (piIndex)
			*piIndex = iMin;

		return &m_Channels[iMin].m_pChannel;
	}

	void PermanentAllocate(fmod_channel_info_t* info)
	{
		info->m_ppChannel = PermanentAllocate(&info->m_uIndex);
	}

	void Free(size_t index)
	{
		m_Channels[index].m_flNextAvailable = g_flClientTime;
		m_Channels[index].m_pChannel->stop();
	}

	void Free(fmod_channel_info_t* info)
	{
		if (!info->m_uIndex || !info->m_ppChannel)	// Already freed?
			return;

		Free(info->m_uIndex);

		info->m_ppChannel = nullptr;
		info->m_uIndex = 0U;
	}
}

#ifdef _DEBUG
void Test_FMOD(void)
{
	PlaySound("weapons/xm1014-2.wav");
}

void CountChannels(void)
{
	int iResult = 0;

	for (auto& info : gFMODChannelManager::m_Channels)
	{
		if (info.m_flNextAvailable < g_flClientTime)
		{
			iResult++;
		}
	}

	std::string text = "Avaliable channels: " + std::to_string(iResult) + "\n";
	gEngfuncs.pfnConsolePrint(text.c_str());
}
#endif

void Sound_Init()
{
#ifdef _DEBUG
	gEngfuncs.pfnAddCommand("testfmod", &Test_FMOD);
	gEngfuncs.pfnAddCommand("countchannels", &CountChannels);
#endif

	/*
		Create a System object and initialize.
	*/
	FMOD::System_Create(&gFModSystem);
	gFModSystem->init(FMOD_MAX_CHANNEL_GOLDSRC + 1, FMOD_INIT_NORMAL, nullptr);	// plus an additional 2D first-personal SFX channel.
	Q_memset(&gFMODChannelManager::m_Channels, NULL, sizeof(gFMODChannelManager::m_Channels));

	/*
		Set the distance units. (meters/feet etc).
	*/
	gFModSystem->set3DSettings(1.0, SND_DISTANCEFACTOR, 1.0f);
}

FMOD::Sound* PrecacheSound(std::string szSample, FMOD_MODE iMode)
{
	// Add the "sound/" at the start.
	if (strncmp(szSample.c_str(), "sound/", 6U))
	{
		Q_snprintf(szBuffer1, ARRAYSIZE(szBuffer1) - 1U, "sound/%s", szSample.c_str());
		g_pInterface->FileSystem->GetLocalPath(szBuffer1, szBuffer2, ARRAYSIZE(szBuffer2) - 1U);
	}
	else
		g_pInterface->FileSystem->GetLocalPath(szSample.c_str(), szBuffer2, ARRAYSIZE(szBuffer2) - 1U);

	if (g_mapSoundPrecache.find(szBuffer2) == g_mapSoundPrecache.end())
	{
		// precache no found, let's do it.
		gFModSystem->createSound(szBuffer2, iMode, 0, &g_mapSoundPrecache[szBuffer2]);
	}
	else
	{
		// Switch to current mode. This is because that it could be played under a different mode.
		g_mapSoundPrecache[szBuffer2]->setMode(iMode);
	}

	return g_mapSoundPrecache[szBuffer2];
}

void PlaySound(const char* szSound, float flVolume, int iPitch)
{
	FMOD_TIMEUNIT iLength = 3500;
	auto pSound = PrecacheSound(szSound, FMOD_2D | FMOD_LOOP_OFF);
	pSound->getLength(&iLength, FMOD_TIMEUNIT_MS);

	auto ppChannel = gFMODChannelManager::Allocate(float(iLength) / 1000.0f);
	gFModSystem->playSound(pSound, nullptr, false, ppChannel);
	(*ppChannel)->setVolume(flVolume);
	(*ppChannel)->setVolumeRamp(false);
	(*ppChannel)->setPitch(float(iPitch) / 100.0f);
	(*ppChannel)->setPaused(false);
}

void Play3DSound(const char* szSound, float flMinDist, float flMaxDist, const Vector& vecOrigin, int iPitch)
{
	FMOD_TIMEUNIT iLength = 3500;
	FMOD_VECTOR	pos = VecConverts(vecOrigin, true);
	auto pSound = PrecacheSound(szSound, FMOD_DEFAULT_IN_GOLDSRC);

	pSound->set3DMinMaxDistance(flMinDist / SND_DISTANCEFACTOR, flMaxDist / SND_DISTANCEFACTOR);
	pSound->getLength(&iLength, FMOD_TIMEUNIT_MS);

	auto ppChannel = gFMODChannelManager::Allocate(float(iLength) / 1000.0f);
	gFModSystem->playSound(pSound, nullptr, true, ppChannel);	// Have to activate the channel first. Otherwise it will be a nullptr.
	(*ppChannel)->set3DAttributes(&pos, &g_fmodvecZero);
	(*ppChannel)->setPitch(float(iPitch) / 100.0f);	// original formula for most CS weapons: 94 + gEngfuncs.pfnRandomLong(0, 0xf)
	(*ppChannel)->setPaused(false);
}

void Sound_Think(double flDeltaTime)
{
	/*
		Iterate through all entity-based sound and find the dead ones.
	*/
	FMOD_VECTOR pos = g_fmodvecZero;
	FMOD_VECTOR vel = g_fmodvecZero;

	for (auto& rgChannelSetPair : g_mapEntitySound)
	{
		cl_entity_t* pEntity = gEngfuncs.GetEntityByIndex(rgChannelSetPair.first);
		bool bShouldRemove = (!pEntity || !pEntity->model);	// Del its channel if the entity gets removed.

		for (auto& ppChannelPair : rgChannelSetPair.second)
		{
			auto& ppChannel = ppChannelPair.second.m_ppChannel;

			if (!ppChannel || !ppChannelPair.second.m_uIndex)	// Already freed.
				continue;	

			FMOD::Sound* pSound = nullptr;
			(*ppChannel)->getCurrentSound(&pSound);	// Free the channel if there were no sound playing.

			if (bShouldRemove || !pSound)
			{
				gFMODChannelManager::Free(&ppChannelPair.second);	// Just free the channel if it is entity gets removed.
			}
			else	// Otherwise, update the entity info for the the sound.
			{
				pos = VecConverts(pEntity->origin, true);
				vel = VecConverts(pEntity->curstate.velocity, true);

				(*ppChannel)->set3DAttributes(&pos, &vel);	// Sync the position with this entity.
			}
		}
	}

	for (auto& channelPair : g_mapPositionSounds)
	{
		auto& ppChannel = channelPair.second.m_ppChannel;

		if (!ppChannel)	// Already freed.
			continue;

		FMOD::Sound* pSound = nullptr;
		(*ppChannel)->getCurrentSound(&pSound);

		if (!pSound)	// Free channel if there were no sound playing.
		{
			gFMODChannelManager::Free(&channelPair.second);
		}
	}

	for (auto& staticChannel : g_lstStaticSounds)
	{
		pos = VecConverts(staticChannel.m_pEntity->origin, true);
		vel = VecConverts(staticChannel.m_pEntity->curstate.velocity, true);

		(*staticChannel.m_ppChannel)->set3DAttributes(&pos, &vel);	// Sync the position with this entity.

		// We don't need to manually free anything in this channel. They will be freed automatically.
	}

	auto iter = g_lstStaticSounds.begin();
	while (iter != g_lstStaticSounds.end())
	{
		if (!iter->m_ppChannel || !iter->m_uIndex)	// Looping static sound gets stopped.
		{
			iter = g_lstStaticSounds.erase(iter);
		}
		else
		{
			pos = VecConverts(iter->m_pEntity->origin, true);
			vel = VecConverts(iter->m_pEntity->curstate.velocity, true);

			(*iter->m_ppChannel)->set3DAttributes(&pos, &vel);

			iter++;
		}
	}

	/*
		Update the listener. (i.e., the player.)
	*/
	Vector vecFwd, vecRight, vecUp;
	gEngfuncs.pfnAngleVectors(g_vecViewAngle, vecFwd, vecRight, vecUp);

	pos = VecConverts(g_vecViewOrigin, true);
	vel = VecConverts(g_vecPlayerVelocity, true);
	FMOD_VECTOR forward = VecConverts(vecFwd);
	FMOD_VECTOR up = VecConverts(vecUp);

	// this velocity stuff is only for making Doppler effect.
	// [reference] https://en.wikipedia.org/wiki/Doppler_effect
	gFModSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
	gFModSystem->update();
}

void Sound_Exit()
{
	// only sound precache needs to release. channels don't.
	for (auto& elem : g_mapSoundPrecache)
	{
		elem.second->release();
	}

	g_mapSoundPrecache.clear();

	gFModSystem->close();
	gFModSystem->release();
}
