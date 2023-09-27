#include "ilargipch.h"

// Main headers
#include "Log.h"

// Define colors for the console
#define DEFAULT "\x1B[0m"
#define INFO "\x1B[32m"
#define WARN "\x1B[33m"
#define ERROR "\x1B[31m"

namespace Ilargi
{
	std::string Log::clientName = "";

	void Log::CoreTrace(std::string_view str)
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &time);

		std::cout << std::format("[{0}:{1}:{2}] Ilargi: {3}", tt.tm_hour, tt.tm_min, tt.tm_sec, str) << std::endl;
	}
	
	void Log::CoreInfo(std::string_view str)
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &time);

		std::cout << INFO << std::format("[{0}:{1}:{2}] Ilargi: {3}", tt.tm_hour, tt.tm_min, tt.tm_sec, str) << DEFAULT << std::endl;
	}
	
	void Log::CoreWarn(std::string_view str)
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &time);

		std::cout << WARN << std::format("[{0}:{1}:{2}] Ilargi: {3}", tt.tm_hour, tt.tm_min, tt.tm_sec, str) << DEFAULT << std::endl;
	}
	
	void Log::CoreError(std::string_view str)
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &time);

		std::cout << ERROR << std::format("[{0}:{1}:{2}] Ilargi: {3}", tt.tm_hour, tt.tm_min, tt.tm_sec, str) << DEFAULT << std::endl;
	}
	
	void Log::Trace(std::string_view str)
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &time);

		std::cout << std::format("[{0}:{1}:{2}] {3}: {4}", tt.tm_hour, tt.tm_min, tt.tm_sec, clientName, str) << std::endl;
	}
	
	void Log::Info(std::string_view str)
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &time);

		std::cout << INFO << std::format("[{0}:{1}:{2}] {3}: {4}", tt.tm_hour, tt.tm_min, tt.tm_sec, clientName, str) << DEFAULT << std::endl;
	}
	
	void Log::Warn(std::string_view str)
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &time);

		std::cout << WARN << std::format("[{0}:{1}:{2}] {3}: {4}", tt.tm_hour, tt.tm_min, tt.tm_sec, clientName, str) << DEFAULT << std::endl;
	}
	
	void Log::Error(std::string_view str)
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &time);

		std::cout << ERROR << std::format("[{0}:{1}:{2}] {3}: {4}", tt.tm_hour, tt.tm_min, tt.tm_sec, clientName, str) << DEFAULT << std::endl;
	}
}