/*

Created Date: Mar 29 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#include <array>
#include <list>
#include <unordered_map>

#include "public/basetypes.h"
#include "engine/maintypes.h"
#include "engine/archtypes.h"
#include "public/strtools.h"
#include "dlls/vector.h"

// interface
#include "public/interface.h"
#include "Interface/IFileSystem.h"

// core of metahook (version 0.4)
#include "metahook.h"

// hook client export funcs
#include "exportfuncs.h"

// for view.cpp hooking.
#include "ref_params.h"

// for HUD_PostRunCmd() hooking.
#include "entity_state.h"

// for HUD_StudioEvent() hooking.
#include "studio_event.h"

// Of course you need access to entity.
#include "cl_entity.h"

// FMOD: What this plugin for.
#include "FMOD/fmod.hpp"
#include "fmod.h"
#include "sound.h"