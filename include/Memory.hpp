/*
	* SayuLib::Memory
	* Functions for scanning memory
	* Requires Pattern16

	* Copyright (c) 2026 Sayuri ('Umgak')

	* This program is free software; licensed under the MIT license.
	* You should have received a copy of the license along with this program.
	* If not, see <https://opensource.org/licenses/MIT>.
*/

#pragma once
#ifndef SAYU_LIB_MEMORY_H
#define SAYU_LIB_MEMORY_H
#include <string>
#include <cstddef>

namespace SayuLib::Memory {
	// Use Pattern16 to scan memory (by default .text section) for an AOB
	void* aobScanModule(const std::string& aob, const char* const module = nullptr, const char* const section = ".text");
	void* aobScanModule(const std::string& aob, const ptrdiff_t offset, const char* const module = nullptr, const char* const section = ".text");
	
	// Extract a base pointer from a RIP-relative instruction
	// by default, 64 bit mov is the target, but this can be overriden with instructionOffset and opcodeOffset
	void* aobScanBase(const std::string& aob, const ptrdiff_t opcodeOffset = 3, const ptrdiff_t instructionOffset = 7, const char* const module = nullptr, const char* const section = ".text");

	// Extract a base pointer from a RIP-relative instruction
	// standalone, called by aobScanBase
	void* extractBase(void* address, const ptrdiff_t opcodeOffset = 3, const ptrdiff_t instructionOffset = 7);
}
#endif