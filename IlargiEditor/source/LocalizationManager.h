#pragma once

namespace Ilargi
{
	class LocalizationManager
	{
	public:
		static bool LoadLanguage(const std::string& aLanguage);
		static const std::string& Get(const std::string& aKey);

	private:
		static std::unordered_map<std::string, std::string> mStrings;
		static std::string mCurrentLanguage;
	};
}

#define LOC(aKey) LocalizationManager::Get(aKey).c_str()

enum class Texts
{
	// FILE MENU
	FILE = 0,
	NEW_SCENE = 1,
	OPEN_SCENE = 2,
	SAVE_SCENE = 3,
	SAVE_SCENE_AS = 4,
	EXIT = 5,

	// EDIT MENU
	EDIT = 6,
	UNDO = 7,
	REDO = 8,
	COPY = 9,
	PASTE = 10,
	DELETE = 11,
	DUPLICATE = 12,

	// LANGUAGES
	LOCALIZATION = 13,
	ENGLISH = 14,
	SPANISH = 15
};