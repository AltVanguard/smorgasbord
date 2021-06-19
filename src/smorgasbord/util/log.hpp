#ifndef SMORGASBORD_LOG_HPP
#define SMORGASBORD_LOG_HPP

#include <fmt/format.h>

#include <iostream>
#include <exception>
#include <cassert>

#define SMORGASBORD_PRINT_CALLER_IN_RELEASE 1

#define SMORGASBORD_SET_LOG(...) \
	Smorgasbord::Log Smorgasbord::mainLog(__VA_ARGS__)

// If more portability needed: #include <boost/current_function.hpp>
#if defined(BOOST_CURRENT_FUNCTION)
#	define SMORGASBORD_CURRENT_FUNCTION BOOST_CURRENT_FUNCTION
#elif defined(__GNUC__)
#	define SMORGASBORD_CURRENT_FUNCTION __PRETTY_FUNCTION__
#else
#	define SMORGASBORD_CURRENT_FUNCTION ""
#endif

namespace Smorgasbord {

enum class LogAssertLevel
{
	FatalError = 0,
	Error,
	Warning,
	Info
};

class Log
{
public:
	
private:
	std::ostream* stream;
	LogAssertLevel assertLevel;
	
public:
	Log(
		std::ostream& stream = std::cout,
		LogAssertLevel assertLevel = LogAssertLevel::FatalError)
	{
		SetStream(stream);
		SetAssertLevel(assertLevel);
	}
	
	void SetAssertLevel(LogAssertLevel assertLevel)
	{
		this->assertLevel = assertLevel;
	}
	
	// Oneline info
	void O(const std::string& message)
	{
		*this->stream << "INFO " << message << std::endl;
	}
	
	// Info
	void I(const std::string& caller, const std::string& message)
	{
		*this->stream << "INFO ";
		PrintMessage(caller, message);
	}
	
	// Warning
	void W(const std::string& caller, const std::string& message)
	{
		*this->stream << "WARNING ";
		PrintMessage(caller, message);
		LogAssert((int)assertLevel < (int)LogAssertLevel::Warning);
	}
	
	// Error
	inline void E(const std::string& caller, const std::string& message)
	{
		*this->stream << "ERROR ";
		PrintMessage(caller, message);
		LogAssert((int)assertLevel < (int)LogAssertLevel::Error);
	}
	
	// Fatal error (causes program termination)
	inline void F(const std::string& caller, const std::string& message)
	{
		*this->stream << "FATAL ERROR ";
		PrintMessage(caller, message);
		LogAssert(false);
		throw std::runtime_error(message);
		//abort();
	}
	
	inline void SetStream(std::ostream& stream)
	{
		this->stream = &stream;
	}
	
private:
	void PrintMessage(const std::string& caller, const std::string& message)
	{
#if defined(DEBUG) | SMORGASBORD_PRINT_CALLER_IN_RELEASE
		*this->stream <<
			"in \"" << caller << "\":\n  " << message << "\n" << std::endl;
#else
		*this->stream << message << "\n" << std::endl;
#endif
	}
	
	void LogAssert(bool condition)
	{
		if (!condition)
		{
			I("void Smorgasbord::Log::LogAssert()", "Put a breakpoint here!");
			//assert(false)
		}
	}
};

extern Log mainLog;

}

#define LogO(...) Smorgasbord::mainLog.O(fmt::format(__VA_ARGS__))
#define LogI(...) Smorgasbord::mainLog.I( \
	SMORGASBORD_CURRENT_FUNCTION, fmt::format(__VA_ARGS__))
#define LogW(...) Smorgasbord::mainLog.W( \
	SMORGASBORD_CURRENT_FUNCTION, fmt::format(__VA_ARGS__))
#define LogE(...) Smorgasbord::mainLog.E( \
	SMORGASBORD_CURRENT_FUNCTION, fmt::format(__VA_ARGS__))
#define LogF(...) Smorgasbord::mainLog.F( \
	SMORGASBORD_CURRENT_FUNCTION, fmt::format(__VA_ARGS__))
#define AssertF(check, ...) if (!(check)) { Smorgasbord::mainLog.F( \
	SMORGASBORD_CURRENT_FUNCTION, fmt::format(__VA_ARGS__)); }
#define AssertE(check, ...) if (!(check)) { Smorgasbord::mainLog.E( \
	SMORGASBORD_CURRENT_FUNCTION, fmt::format(__VA_ARGS__)); }

#endif // SMORGASBORD_LOG_HPP
