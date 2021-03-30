/*

Created Date: Mar 30 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define S_StartStaticSound_SIG	"\x55\x8B\xEC\x83\xEC\x44\x53\x56\x57\x8B\x7D\x10\x85\xFF\xC7\x45\xFC\x00\x00\x00\x00"
#define S_StartDynamicSound_SIG	"\x55\x8B\xEC\x83\xEC\x48\xA1\x2A\x2A\x2A\x2A\x53\x56\x57\x85\xC0"

#define FMOD_DEFAULT_IN_GOLDSRC		(FMOD_LOOP_OFF | FMOD_3D | FMOD_3D_WORLDRELATIVE | FMOD_3D_LINEARROLLOFF)
#define FMOD_MAX_CHANNEL_GOLDSRC	4092	// just... max it out, according to user manual. Reserve an additional channel for all other 2D first-personal SFX.

extern FMOD::System* gFModSystem;
extern FMOD::Channel* g_phLocal2DChannel;

extern double g_flClientTime;
extern double g_flClientTimeDelta;

constexpr FMOD_VECTOR	g_fmodvecZero = { 0.0f, 0.0f, 0.0f };
constexpr float			SND_DISTANCEFACTOR = 39.3701f;          // Units per meter. I.e feet would = 3.28. centimeters would = 100. inch would 39.3701f.

inline FMOD_VECTOR VecConverts(const Vector& v, bool bScale = false)
{
	// this function has some problem.
	// you cannot place a calculating formula in the first parameter.

	if (bScale)
		return FMOD_VECTOR({ v.x / SND_DISTANCEFACTOR, v.z / SND_DISTANCEFACTOR, v.y / SND_DISTANCEFACTOR });	// FMOD has a inversed Y/Z axis.

	return FMOD_VECTOR({ v.x, v.z, v.y });
}

void Sound_Init();
void Sound_Think(double flDeltaTime);
void Sound_Exit();

void PlaySound(const char* szSound, int iPitch = 100);
void Play3DSound(const char* szSound, float flMinDist, float flMaxDist, const Vector& vecOrigin, int iPitch = 100);
