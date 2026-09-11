/*
	* main.cpp
	* Entrypoint and log setup for the mod

	* Copyright (c) 2026 Sayuri ('Umgak')

	* This program is free software; licensed under the MIT license.
	* You should have received a copy of the license along with this program.
	* If not, see <https://opensource.org/licenses/MIT>.
*/
#include "NLA_Version.hpp"				// Internal consts for NLA versions
#include "skse64_common/skse_version.h"	// RUNTIME_VERSION
#include "skse64/PluginAPI.h"			// SKSEPluginVersionData, PluginInfo
#include <stdio.h>						// snprintf
#include <ShlObj.h>						// CSIDL_MYDOCUMENTS
#include "NoLockpickActivate.hpp"

static void initgLog()
{
	char path[FILENAME_MAX] = { '\0' };
	snprintf(path, sizeof(path), "%s%s.log", R"(\My Games\Skyrim Special Edition\SKSE\)", NLA_SHORT_NAME);
	gLog.OpenRelative(CSIDL_MYDOCUMENTS, path);
	gLog.SetPrintLevel(IDebugLog::kLevel_Error);
	gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
	_MESSAGE("[MESSAGE] %s v%s", NLA_NAME, NLA_VERSION_VERSTRING);
}

extern "C" {
	__declspec(dllexport) SKSEPluginVersionData SKSEPlugin_Version = {
		SKSEPluginVersionData::kVersion,
		NLA_DLL_VERSION,
		NLA_NAME,
		"Sayu",
		"umgak@discord", // "this is not for showing to users" yes ian but it's included in my source code AND the output DLL
		SKSEPluginVersionData::kVersionIndependentEx_NoStructUse,
		SKSEPluginVersionData::kVersionIndependent_Signatures,
		0,
		0,
	};

	__declspec(dllexport) bool SKSEPlugin_Query(const SKSEInterface* skse, PluginInfo* info)
	{
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = NLA_NAME;
		info->version = NLA_DLL_VERSION;
		initgLog();
		if (skse->isEditor)	{
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!");
			return false;
		}
		return true;
	}

	__declspec(dllexport) bool SKSEPlugin_Load(const SKSEInterface* skse)
	{
		PluginHandle pluginHandle = skse->GetPluginHandle();
		// logging is not set up in SKSEPlugin_Query on AE, since it uses SKSEPluginVersionData instead
		if (skse->runtimeVersion >= RUNTIME_VERSION_1_6_317) {
			initgLog();
		}
		if (!NoLockpickActivate::init()) {
			_FATALERROR("[FATAL ERROR] Something went wrong when installing code hook. This is fatal. Skipping remainder of init process.");
			return false;
		}
		_MESSAGE("[MESSAGE] %s loaded successfully.", NLA_NAME);
		return true;
	}
};