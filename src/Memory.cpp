/*
	* SayuLib::Memory
	* Functions for scanning memory
	* Requires Pattern16

	* Copyright (c) 2026 Sayuri ('Umgak')

	* This program is free software; licensed under the MIT license.
	* You should have received a copy of the license along with this program.
	* If not, see <https://opensource.org/licenses/MIT>.
*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <unordered_map>
#include <utility>
#include <vector>
#include "Memory.hpp"
#include "Hash.hpp"

#include "Pattern16/include/Pattern16.h"

namespace SayuLib::Memory {
	struct memRegionCache {
		struct memoryRegion {
			void* start = 0;
			size_t size = 0;
		};
		// cache for memory regions
		static inline std::unordered_map<HMODULE,
			std::unordered_map<uint64_t, std::vector<memoryRegion>>> _data;

		static void cacheModule(const HMODULE hModule) 
		{
			uintptr_t base = reinterpret_cast<uintptr_t>(hModule);
			auto* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
			auto* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dosHeader->e_lfanew);
			auto* section = IMAGE_FIRST_SECTION(ntHeader);

			for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++) {
				char nameBuf[IMAGE_SIZEOF_SHORT_NAME + 1]{};
				memcpy(nameBuf, section[i].Name, IMAGE_SIZEOF_SHORT_NAME);
				uint64_t sHash = Hash::hash_64(nameBuf);

				auto start = reinterpret_cast<void*>(base + section[i].VirtualAddress);
				auto size = static_cast<size_t>(section[i].Misc.VirtualSize);
				_data[hModule][sHash].emplace_back(start, size);
			}
		}

		static const std::vector<memoryRegion>& Get(const char* const module, const char* const section) 
		{
			static const std::vector<memoryRegion> nullRegion{};
			HMODULE hModule = GetModuleHandleA(module);
			if (!hModule) {
				return nullRegion;
			}
			auto moduleIt = _data.find(hModule);
			if (moduleIt == _data.end()) {
				cacheModule(hModule);
				moduleIt = _data.find(hModule);
			}
			auto& sectionMap = moduleIt->second;
			auto sectionIt = sectionMap.find(Hash::hash_64(section));
			if (sectionIt == sectionMap.end()) {
				return nullRegion;
			}
			return sectionIt->second;
		}
	};

	void* aobScanModule(const std::string& aob, const char* const module, const char* const section) 
	{
		const auto& regions = memRegionCache::Get(module, section);
		if (regions.empty()) return nullptr;
		for (const auto& region : regions)
		{
			void* address = Pattern16::scan(region.start, region.size, aob);
			if (address) return address;
		}
		return nullptr;
	}

	void* aobScanModule(const std::string& aob, const ptrdiff_t offset, const char* const module, const char* const section) 
	{
		void* address = aobScanModule(aob, module, section);
		if (address == nullptr) return address;
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(address) + offset);
	}

	void* aobScanBase(const std::string& aob, const ptrdiff_t opcodeOffset, const ptrdiff_t instructionOffset, const char* const module, const char* const section)
	{
		void* instruction = aobScanModule(aob, module, section);
		if (instruction == nullptr) return instruction;
		return extractBase(instruction, opcodeOffset, instructionOffset);
	}

	void* extractBase(void* instruction, const ptrdiff_t opcodeOffset, const ptrdiff_t instructionOffset)
	{
		uintptr_t address = reinterpret_cast<uintptr_t>(instruction);
		return reinterpret_cast<void*>(address + instructionOffset + *reinterpret_cast<int32_t*>(address + opcodeOffset));
	}
}