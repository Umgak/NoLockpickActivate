/*
	* NoLockpickActivate.cpp
	* NoLockpickActivate main code

	* Copyright (c) 2026 Sayuri ('Umgak')

	* This program is free software; licensed under the MIT license.
	* You should have received a copy of the license along with this program.
	* If not, see <https://opensource.org/licenses/MIT>.
*/
#include "NoLockpickActivate.hpp"
#include "Memory.hpp"
#include "Settings.hpp"
#define XBYAK_NO_OP_NAMES
#include "skse64/xbyak/xbyak.h"
#include "skse64/GameReferences.h"
#include "skse64_common/BranchTrampoline.h"

namespace NoLockpickActivate {
	Settings::modSettings settings;

	typedef bool (*_activate)(TESObjectREFR* object, TESObjectREFR* activator, uint8_t unk, TESBoundObject* boundObject, int32_t count, bool defaultProcessingOnly);
	_activate Activate = nullptr; // storage for TESObjectREFR::Activate

	// mimic the same parameters as TESObjectREFR::Activate, to ensure that parameters get forwarded properly
	static bool _noLockpickActivate(TESObjectREFR* object, TESObjectREFR* activator, uint8_t unk, TESBoundObject* boundObject, int32_t count, bool defaultProcessingOnly, bool isKey)
	{
		FormType ft = static_cast<FormType>(object->baseForm->GetFormType());
		std::string ts;
		if (ft == kFormType_Container) {
			ts = "CONT";
		} else if (ft == kFormType_Door) {
			ts = "DOOR";
		}
		auto btoc = [](bool b) {return (b) ? "true" : "false";};
		bool doActivate = settings.settings[isKey][ft];
		_DMESSAGE("[DEBUG] Entrypoint hit. formType: %s, isKey: %s, doActivate: %s", ts, btoc(isKey), btoc(doActivate));
		if (doActivate) {
			// forward args to Activate call
			return Activate(object, activator, unk, boundObject, count, defaultProcessingOnly);
		}
		return false;
	}

	static bool _installEntrypoint(uintptr_t entrypoint, bool isKey)
	{
		struct nla_entrypoint : Xbyak::CodeGenerator {
			nla_entrypoint(uintptr_t entrypoint, bool isKey, void* buf) : Xbyak::CodeGenerator(256, buf)
			{
				/* 
				at this entrypoint:
					rcx = TESObjectREFR* object
					rdx = TESObjectREFR* activator
					r8 = uint8_t unk
					r9 = TESBoundObject* boundObject
					[rsp+0x20] = uint32_t count
					[rsp+0x28] = bool defaultProcessingOnly
				need to shift stack values to compensate for extra variable isKey:
					[rsp+0x20] = int32_t count
					[rsp+0x28] = bool defaultProcessingOnly
					[rsp+0x30] = bool isKey
				*/
				Xbyak::Label returnLabel;
				// expand stack shadow space to fit extra variable, keeping 16 byte alignment
				sub(rsp, 0x40);
				// grab old args and move them to new stack positions
				mov(rax, qword[rsp + 0x40 + 0x20]); // int32_t count
				mov(qword[rsp + 0x20], rax);
				mov(rax, qword[rsp + 0x40 + 0x28]); // bool defaultProcessingOnly
				mov(qword[rsp + 0x28], rax);
				// push isKey onto the stack
				mov(qword[rsp + 0x30], (isKey) ? 0x1 : 0x0);
				// call into _noLockpickActivate
				mov(rax, reinterpret_cast<uintptr_t>(_noLockpickActivate));
				call(rax);
				// restore old stack pointer
				add(rsp, 0x40);
				// all logic is handled in _noLockpickActivate, just jump back
				jmp(ptr[rip + returnLabel]);
				L(returnLabel);
				dq(entrypoint + 0x5);
			}
		};
		if (!g_branchTrampoline.Create(1024)) {
			_FATALERROR("[FATAL ERROR] Failed to create branch trampoline.");
			return false;
		}
		if (!g_localTrampoline.Create(1024)) {
			_FATALERROR("[FATAL ERROR] Failed to create codegen buffer.");
			return false;
		}
		try {
			void* codeBuf = g_localTrampoline.StartAlloc();
			if (!codeBuf) return false;
			nla_entrypoint code(entrypoint, isKey, codeBuf);
			g_localTrampoline.EndAlloc(code.getCurr());
			g_branchTrampoline.Write5Branch(entrypoint, reinterpret_cast<uintptr_t>(code.getCode()));
			return true;
		} catch (std::exception &e) {
			_FATALERROR("%s", e.what());
			return false;
		}
	}
	
	bool init()
	{
		settings = Settings::readConfig();
		gLog.SetLogLevel(settings.logLevel);
		uintptr_t unlockWithPickEntrypoint = NULL;
		uintptr_t unlockCONTWithKeyEntrypoint = NULL;
		uintptr_t unlockDOORWithKeyEntrypoint = NULL;

		struct hook {
			const char* name;
			const std::string aobString;
			ptrdiff_t offset;
			uintptr_t& address;
			bool isKey = false;
		};

		hook unlockWithPick = {
			"unlockWithPickEntrypoint",
			"e8 ?? ?? ?? ?? 48 8b 05 ?? ?? ?? ?? 80 78 1a 1d",
			0,
			unlockWithPickEntrypoint,
			false
		};

		hook unlockCONTWithKey = {
			"unlockCONTWithKeyEntrypoint",
			"48 8b d7 c7 44 24 20 01 00 00 00 e8 ?? ?? ?? ??",
			0xb,
			unlockCONTWithKeyEntrypoint,
			true
		};

		hook unlockDOORWithKey = {
			"unlockDOORWithKeyEntrypoint",
			"45 33 c9 45 33 c0 48 8b 15 ?? ?? ?? ?? 48 8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ??",
			0x14,
			unlockDOORWithKeyEntrypoint,
			true
		};

		hook* entries[] = {
			&unlockWithPick,
			&unlockCONTWithKey,
			&unlockDOORWithKey,
		};

		for (const auto &entry : entries) {
			entry->address = reinterpret_cast<uintptr_t>(SayuLib::Memory::aobScanModule(entry->aobString, entry->offset));
			if (!entry->address) {
				_FATALERROR("[FATAL ERROR] Failed to resolve signature for %s!", entry->name);
				return false;
			} else {
				_VMESSAGE("[VERBOSE] %s found at address 0x%p", entry->name, entry->address);
			}
		}
		Activate = reinterpret_cast<_activate>(SayuLib::Memory::extractBase(reinterpret_cast<void*>(unlockWithPickEntrypoint), 0x1, 0x5));
		if (!Activate) {
			_FATALERROR("[FATAL ERROR] Failed to find address for TESObjectREFR::Activate!");
			return false;
		} else {
			_VMESSAGE("[VERBOSE] TESObjectREFR::Activate found at address 0x%p", reinterpret_cast<uintptr_t>(Activate));
		}
		for (const auto &entry : entries) {
			if (!_installEntrypoint(entry->address, entry->isKey)) {
				_FATALERROR("[FATAL ERROR] Failed to install hook for %s!", entry->name);
				return false;
			}
		}
		return true;
	}
}