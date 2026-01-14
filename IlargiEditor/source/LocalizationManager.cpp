#include "ilargipch.h"

#include "LocalizationManager.h"

#include <arduinojson/ArduinoJson-v7.0.4.h>

namespace Ilargi
{
	std::unordered_map<std::string, std::string> LocalizationManager::mStrings;
	std::string LocalizationManager::mCurrentLanguage;

	bool LocalizationManager::LoadLanguage(const std::string& aLanguage)
	{
		std::ifstream file(aLanguage, std::ios::in);

		if (file.is_open())
		{
			mStrings.clear();

			JsonDocument document;
			deserializeJson(document, file);

			for (JsonPair jsonObject : document.as<JsonObject>())
			{
				mStrings[jsonObject.key().c_str()] = jsonObject.value().as<std::string>();
			}
		}

		return file.is_open();
	}

	const std::string& LocalizationManager::Get(const std::string& aKey)
	{
		if (mStrings.find(aKey) != mStrings.end())
			return mStrings[aKey];

		static std::string missing;
		missing = "#" + aKey + "#";
		return missing;
	}
}