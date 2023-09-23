#pragma once

namespace Ilargi
{
	class Log
	{
	public:
		static void SetClientName(std::string appName) { clientName = appName; }

		static void CoreTrace(std::string_view str);
		static void CoreInfo(std::string_view str);
		static void CoreWarn(std::string_view str);
		static void CoreError(std::string_view str);

		static void Trace(std::string_view str);
		static void Info(std::string_view str);
		static void Warn(std::string_view str);
		static void Error(std::string_view str);

	private:
		static std::string clientName;
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