/*

Created Date: Mar 30 2021

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

	FMOD::Channel** Allocate(float flCooldown = 3.5f)
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

			// record the time on the way.
			if (m_Channels[i].m_flNextAvailable < flMinTime)
			{
				iMin = i;
				flMinTime = m_Channels[i].m_flNextAvailable;
			}

			if (m_Channels[i].m_flNextAvailable > g_flClientTime)
				continue;

			m_Channels[i].m_flNextAvailable = g_flClientTime + flCooldown;	// since only gun fire SFX supported by FMOD... it quite enough.
			return &m_Channels[i].m_pChannel;
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

			// record the time on the way.
			if (m_Channels[i].m_flNextAvailable < flMinTime)
			{
				iMin = i;
				flMinTime = m_Channels[i].m_flNextAvailable;
			}

			if (m_Channels[i].m_flNextAvailable > g_flClientTime)
				continue;

			m_Channels[i].m_flNextAvailable = g_flClientTime + 99999.0;

			if (piIndex)
				*piIndex = i;

			return &m_Channels[i].m_pChannel;
		}

		// just occuiped a furthest unused channel.
		m_Channels[iMin].m_flNextAvailable = g_flClientTime + 99999.0;

		if (piIndex)
			*piIndex = iMin;

		return &m_Channels[iMin].m_pChannel;
	}

	void Free(size_t index)
	{
		m_Channels[index].m_flNextAvailable = g_flClientTime;
		m_Channels[index].m_pChannel->stop();
	}
}

#ifdef _DEBUG
void Test_FMOD(void)
{
	PlaySound("weapons/xm1014-2.wav");
}
#endif

void Sound_Init()
{
#ifdef _DEBUG
	gEngfuncs.pfnAddCommand("testfmod", &Test_FMOD);
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

void PlaySound(const char* szSound, int iPitch)
{
	Q_snprintf(szBuffer1, ARRAYSIZE(szBuffer1) - 1U, "sound/%s", szSound);
	g_pInterface->FileSystem->GetLocalPath(szBuffer1, szBuffer2, ARRAYSIZE(szBuffer2) - 1U);

	if (g_mapSoundPrecache.find(szBuffer2) == g_mapSoundPrecache.end())
	{
		// precache no found, let's do it.
		gFModSystem->createSound(szBuffer2, FMOD_2D | FMOD_LOOP_OFF, 0, &g_mapSoundPrecache[szBuffer2]);
	}

	gFModSystem->playSound(g_mapSoundPrecache[szBuffer2], nullptr, false, &g_phLocal2DChannel);
	g_phLocal2DChannel->setPitch(float(iPitch) / 100.0f);
}

void Play3DSound(const char* szSound, float flMinDist, float flMaxDist, const Vector& vecOrigin, int iPitch)
{
	Q_snprintf(szBuffer1, ARRAYSIZE(szBuffer1) - 1U, "sound/%s", szSound);
	g_pInterface->FileSystem->GetLocalPath(szBuffer1, szBuffer2, ARRAYSIZE(szBuffer2) - 1U);

	FMOD_TIMEUNIT iLength = 3500;
	FMOD_VECTOR	pos = VecConverts(vecOrigin, true);

	if (g_mapSoundPrecache.find(szBuffer2) == g_mapSoundPrecache.end())
	{
		// precache no found, let's do it.
		gFModSystem->createSound(szBuffer2, FMOD_DEFAULT_IN_GOLDSRC, 0, &g_mapSoundPrecache[szBuffer2]);
	}

	g_mapSoundPrecache[szBuffer2]->set3DMinMaxDistance(flMinDist / SND_DISTANCEFACTOR, flMaxDist / SND_DISTANCEFACTOR);
	g_mapSoundPrecache[szBuffer2]->getLength(&iLength, FMOD_TIMEUNIT_MS);

	auto ppChannel = gFMODChannelManager::Allocate(float(iLength) / 1000.0f);
	gFModSystem->playSound(g_mapSoundPrecache[szBuffer2], nullptr, true, ppChannel);
	(*ppChannel)->set3DAttributes(&pos, &g_fmodvecZero);
	(*ppChannel)->setPitch(float(iPitch) / 100.0f);	// original formula for most CS weapons: 94 + gEngfuncs.pfnRandomLong(0, 0xf)
	(*ppChannel)->setPaused(false);
}

void Sound_Think(double flDeltaTime)
{
	Vector vecFwd, vecRight, vecUp;
	gEngfuncs.pfnAngleVectors(g_vecViewAngle, vecFwd, vecRight, vecUp);

	FMOD_VECTOR pos = VecConverts(g_vecViewOrigin, true);
	FMOD_VECTOR vel = VecConverts(g_vecPlayerVelocity, true);
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