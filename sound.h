/*

Created Date: Mar 30 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

// channels
#define CHAN_AUTO				0
#define CHAN_WEAPON				1
#define CHAN_VOICE				2
#define CHAN_ITEM				3
#define CHAN_BODY				4
#define CHAN_STREAM				5   // allocate stream channel from the static or dynamic area
#define CHAN_STATIC				6   // allocate channel from the static area
#define CHAN_NETWORKVOICE_BASE	7   // voice data coming across the network
#define CHAN_NETWORKVOICE_END	500 // network voice data reserves slots (CHAN_NETWORKVOICE_BASE through CHAN_NETWORKVOICE_END).
#define CHAN_BOT				501 // channel used for bot chatter.

#define SND_STOP				BIT(5)	// duplicated in protocol.h stop sound
#define SND_CHANGE_VOL			BIT(6)	// duplicated in protocol.h change sound vol
#define SND_CHANGE_PITCH		BIT(7)	// duplicated in protocol.h change sound pitch
#define SND_SPAWNING			BIT(8)	// duplicated in protocol.h we're spawing, used in some cases for ambients

#define S_StartStaticSound_SIG	"\x55\x8B\xEC\x83\xEC\x44\x53\x56\x57\x8B\x7D\x10\x85\xFF\xC7\x45\xFC\x00\x00\x00\x00"
#define S_StartDynamicSound_SIG	"\x55\x8B\xEC\x83\xEC\x48\xA1\x2A\x2A\x2A\x2A\x53\x56\x57\x85\xC0"

using EntitySoundChannel = std::unordered_map<int, FMOD::Channel**>;
using EntitySoundMap = std::unordered_map<int, EntitySoundChannel>;

extern EntitySoundMap g_mapEntitySound;

void Sound_InstallHook();
void S_StartStaticSound(int entnum, int entchannel, struct sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch);
void S_StartDynamicSound(int entnum, int entchannel, struct sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch);
