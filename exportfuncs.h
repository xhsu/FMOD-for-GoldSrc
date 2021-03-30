/*

Created Date: Mar 29 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

// From Initialize()
extern cl_enginefunc_t gEngfuncs;

// From view.cpp
extern Vector g_vecViewAngle;
extern Vector g_vecViewOrigin;

// From weapons.cpp
extern Vector g_vecPlayerVelocity;

int Initialize(struct cl_enginefuncs_s* pEnginefuncs, int iVersion);
void V_CalcRefdef(struct ref_params_s* pparams);
void HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);
void HUD_Frame(double flDeltaTime);
void HUD_StudioEvent(const mstudioevent_s* pEvent, const cl_entity_s* pEntity);
