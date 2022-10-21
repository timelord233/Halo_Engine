#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Halo {
	class HALO_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger(){ return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger(){ return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define HL_CORE_TRACE(...)    ::Halo::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HL_CORE_INFO(...)     ::Halo::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HL_CORE_WARN(...)     ::Halo::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HL_CORE_ERROR(...)    ::Halo::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HL_CORE_CRITICAL(...) ::Halo::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define HL_TRACE(...)         ::Halo::Log::GetClientLogger()->trace(__VA_ARGS__)
#define HL_INFO(...)          ::Halo::Log::GetClientLogger()->info(__VA_ARGS__)
#define HL_WARN(...)          ::Halo::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HL_ERROR(...)         ::Halo::Log::GetClientLogger()->error(__VA_ARGS__)
#define HL_CRITICAL(...)      ::Halo::Log::GetClientLogger()->critical(__VA_ARGS__)
