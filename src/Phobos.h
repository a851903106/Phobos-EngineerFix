#pragma once
#include <Phobos.version.h>
#include <Windows.h>
#include <string>

class CCINIClass;
class AbstractClass;

constexpr auto NONE_STR = "<none>";
constexpr auto NONE_STR2 = "none";
constexpr auto TOOLTIPS_SECTION = "ToolTips";
constexpr auto SIDEBAR_SECTION = "Sidebar";
constexpr auto GENERAL_SECTION = "General";

class Phobos
{
public:
	//static CCINIClass* OpenConfig(const char*);
	//static void CloseConfig(CCINIClass*&);

	//variables
	static HANDLE hInstance;

	static const size_t readLength = 2048;
	static char readBuffer[readLength];
	static wchar_t wideBuffer[readLength];
	static const char readDelims[4];

	static const char* AppIconPath;
	static const wchar_t* VersionDescription;

	static bool IsLoadingSaveGame;

#ifdef DEBUG
	static bool DetachFromDebugger();
#endif
};
