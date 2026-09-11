#pragma once
#ifndef NLA_VERSION_HPP
#define NLA_VERSION_HPP

#define NLA_NAME "No Lockpick Activate"
#define NLA_SHORT_NAME "NoLockpickActivate"

#define MAKE_STR_HELPER(a_str) #a_str
#define MAKE_STR(a_str) MAKE_STR_HELPER(a_str)
#define MAKE_DLL_VER_EX(major, minor, patch, sub) ((((major) & 0xFF) << 24) | (((minor) & 0xFF) << 16) | (((patch) & 0xFF) << 8) | ((sub) & 0xFF))
#define MAKE_DLL_VER(major, minor, patch)				  MAKE_DLL_VER_EX(major, minor, patch, 0)

#define NLA_VERSION_MAJOR	4
#define NLA_VERSION_MINOR	0
#define NLA_VERSION_PATCH	0
#define NLA_VERSION_HOTFIX	0

#define NLA_VERSION_VERSTRING		MAKE_STR(NLA_VERSION_MAJOR) "." MAKE_STR(NLA_VERSION_MINOR) "." MAKE_STR(NLA_VERSION_PATCH)
#define NLA_VERSION_VERSTRING_EX	MAKE_STR(NLA_VERSION_MAJOR) "." MAKE_STR(NLA_VERSION_MINOR) "." MAKE_STR(NLA_VERSION_PATCH) "." MAKE_STR(NLA_VERSION_HOTFIX)
#define NLA_DLL_VERSION				MAKE_DLL_VER(NLA_VERSION_MAJOR, NLA_VERSION_MINOR, NLA_VERSION_PATCH)

#endif
