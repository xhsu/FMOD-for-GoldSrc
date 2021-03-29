/*

Created Date: Mar 29 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

extern cl_enginefunc_t gEngfuncs;

int Initialize(struct cl_enginefuncs_s* pEnginefuncs, int iVersion);
int HUD_Redraw(float time, int intermission);