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

// From xash3d
#define SND_VOLUME			(1<<0)	// a scaled byte
#define SND_ATTENUATION		(1<<1)	// a byte
#define SND_LARGE_INDEX		(1<<2)	// a send sound as short
#define SND_PITCH			(1<<3)	// a byte
#define SND_SENTENCE		(1<<4)	// set if sound num is actually a sentence num
#define SND_STOP			(1<<5)	// stop the sound
#define SND_CHANGE_VOL		(1<<6)	// change sound vol
#define SND_CHANGE_PITCH	(1<<7)	// change sound pitch
#define SND_SPAWNING		(1<<8)	// we're spawning, used in some cases for ambients (not sent across network)

#define S_StartStaticSound_SIG	"\x55\x8B\xEC\x83\xEC\x44\x53\x56\x57\x8B\x7D\x10\x85\xFF\xC7\x45\xFC\x00\x00\x00\x00"
#define S_StartDynamicSound_SIG	"\x55\x8B\xEC\x83\xEC\x48\xA1\x2A\x2A\x2A\x2A\x53\x56\x57\x85\xC0"
#define Cache_Check_SIG			"\x55\x8B\xEC\x57\x8B\x7D\x08\x8B\x07\x85\xC0\x75\x2A\x5F\x5D\xC3"

typedef struct cache_user_s
{
	void*	data;	// What the fuck, Valve?

} cache_user_t;

#define MAX_QPATH 64

typedef struct sfx_s
{
	char			name[MAX_QPATH];
	cache_user_t	cache;	// Feel funny, HUH?
	int				servercount;

} sfx_t;

typedef struct sfxcache_s
{
	int 	length;
	int 	loopstart;
	int 	speed;
	int 	width;
	int 	stereo;
	byte	data[1];		// variable sized

} sfxcache_t;

using EntitySoundChannel = std::unordered_map<int, fmod_channel_info_t>;
using EntitySoundMap = std::unordered_map<int, EntitySoundChannel>;
extern EntitySoundMap g_mapEntitySound;

using PositionSoundMap = std::map<Vector, fmod_channel_info_t>;	// Fuck the unordered_map, it requires a hash function.
extern PositionSoundMap g_mapPositionSounds;

void Sound_InstallHook();
void S_StartStaticSound(int entnum, int entchannel, sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t* sfxin, Vector& origin, float fvol, float attenuation, int flags, int pitch);
