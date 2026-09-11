/*
	* Settings header
	* Loads and returns the settings

	* Copyright (c) 2026 Sayuri ('Umgak')

	* This program is free software; licensed under the MIT license.
	* You should have received a copy of the license along with this program.
	* If not, see <https://opensource.org/licenses/MIT>.
*/
#pragma once
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "NLA_Version.hpp"
#include <string>
#include <format>
#include <map>
#include "inicpp/inicpp.hpp"
#include "skse64/GameForms.h"

class Settings {
public:
	typedef std::map<bool, std::map<FormType, bool>> modSettingsMap;

	typedef struct modSettings {
		IDebugLog::LogLevel logLevel = IDebugLog::kLevel_DebugMessage;
		modSettingsMap settings = {
			{false,	{{kFormType_Container, false}, {kFormType_Door, true}}}, // defaults for isKey = false
			{true,	{{kFormType_Container, false}, {kFormType_Door, true}}},	// defaults for isKey = true
		};
	} modSettings;

	static const modSettings readConfig()
	{
		try {
			std::string modConfigPath = std::format(R"(Data\SKSE\Plugins\{}.ini)", NLA_SHORT_NAME);
			inicpp::IniManager _ini(modConfigPath);
			modSettingsMap loadedSettings;
			loadedSettings[false][kFormType_Container] 	= _ini["ActivateWithLockpick"]["CONT"];
			loadedSettings[false][kFormType_Door] 		= _ini["ActivateWithLockpick"]["DOOR"];
			loadedSettings[true][kFormType_Container]  	= _ini["ActivateWithKey"]["CONT"];
			loadedSettings[true][kFormType_Door] 		= _ini["ActivateWithKey"]["DOOR"];
			return modSettings{
				.logLevel = static_cast<IDebugLog::LogLevel>(_ini[NLA_SHORT_NAME].toInt("LogLevel")),
				.settings = loadedSettings
			};
		}
		catch (...) {
			_WARNING("[WARNING] Failed to parse config. Falling back to default behavior.");
			return modSettings{};
		}
	}
};
#endif