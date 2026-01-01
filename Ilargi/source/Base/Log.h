#pragma once

namespace Ilargi
{
	class Log
	{
	public:
		/*
		* @brief Sets the application name for the logs.
		* @param aClientName The application name.
		*/
		static void SetClientName(std::string aClientName) { sClientName = aClientName; }

		/*
		* @brief Engine log for debug. Writes in white color on the console.
		* @param aStr The string that will be written in the console.
		*/
		static void CoreTrace(std::string_view aStr);
		
		/*
		* @brief Engine log for information. Writes in green color on the console.
		* @param aStr The string that will be written on the console.
		*/
		static void CoreInfo(std::string_view aStr);

		/*
		* @brief Engine log for warnings. Writes in yellow color on the console.
		* @param aStr The string that will be written on the console.
		*/
		static void CoreWarn(std::string_view aStr);

		/*
		* @brief Engine log for errors. Writes in red color on the console.
		* @param aStr The string that will be written on the console.
		*/
		static void CoreError(std::string_view aStr);

		/*
		* @brief Application log for debug. Writes in white color on the console.
		* @param aStr The string that will be written in the console.
		*/
		static void Trace(std::string_view aStr);

		/*
		* @brief Application log for information. Writes in green color on the console.
		* @param aStr The string that will be written on the console.
		*/
		static void Info(std::string_view aStr);

		/*
		* @brief Application log for warnings. Writes in yellow color on the console.
		* @param aStr The string that will be written on the console.
		*/
		static void Warn(std::string_view aStr);

		/*
		* @brief Application log for errors. Writes in red color on the console.
		* @param aStr The string that will be written on the console.
		*/
		static void Error(std::string_view aStr);

	private:
		static std::string sClientName; // The application name.
	};
}

// Engine log
#define	ILG_CORE_TRACE(...)			::Ilargi::Log::CoreTrace(std::format(__VA_ARGS__))
#define	ILG_CORE_INFO(...)			::Ilargi::Log::CoreInfo(std::format(__VA_ARGS__))
#define	ILG_CORE_WARN(...)			::Ilargi::Log::CoreWarn(std::format(__VA_ARGS__))
#define	ILG_CORE_ERROR(...)			::Ilargi::Log::CoreError(std::format(__VA_ARGS__))

// Client log
#define	ILG_TRACE(...)			::Ilargi::Log::Trace(std::format(__VA_ARGS__))
#define	ILG_INFO(...)			::Ilargi::Log::Info(std::format(__VA_ARGS__))
#define	ILG_WARN(...)			::Ilargi::Log::Warn(std::format(__VA_ARGS__))
#define	ILG_ERROR(...)			::Ilargi::Log::Error(std::format(__VA_ARGS__))