/*

Created Date: Mar 29 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

cl_enginefunc_t gEngfuncs;

// From view.cpp
Vector g_vecViewAngle;
Vector g_vecViewOrigin;

// From weapons.cpp
Vector g_vecPlayerVelocity;

int Initialize(struct cl_enginefuncs_s* pEnginefuncs, int iVersion)
{
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(gEngfuncs));

	auto bResult = gExportfuncs.Initialize(pEnginefuncs, iVersion);

	// only doing this if the client loaded successfully.
	if (bResult)
	{
		Sound_Init();	// Wait until gEngfuncs ready.
	}

	return bResult;
}

void V_CalcRefdef(struct ref_params_s* pparams)
{
	gExportfuncs.V_CalcRefdef(pparams);

	// Post: Copy some view data.
	g_vecViewAngle = pparams->viewangles;
	g_vecViewOrigin = pparams->vieworg;
}

void HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	gExportfuncs.HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);

	g_vecPlayerVelocity = from->client.velocity;
}

void HUD_Frame(double flDeltaTime)
{
	g_flClientTimeDelta = flDeltaTime;
	g_flClientTime += flDeltaTime;

	Sound_Think(flDeltaTime);
}

void HUD_StudioEvent(const mstudioevent_s* pEvent, const cl_entity_s* pEntity)
{
	if (pEvent->event != 5004)
		return gExportfuncs.HUD_StudioEvent(pEvent, pEntity);

	// Client side sound
	PlaySound(pEvent->options);
}
