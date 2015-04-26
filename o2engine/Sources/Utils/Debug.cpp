#include "Debug.h"

#include <cstdarg>
#include "Utils/Log/LogStream.h"
#include "Utils/Log/FileLogStream.h"
#include "Utils/Log/ConsoleLogStream.h"

namespace o2
{
	CREATE_SINGLETON(Debug);

	Debug::Debug()
	{
		FileLogStream* fileLogStream = new FileLogStream("", "log.txt");
		mLogStream = new ConsoleLogStream("");
		fileLogStream->BindStream(mLogStream);
	}

	Debug::~Debug()
	{
		delete mLogStream->GetParentStream();
	}

	void Debug::Log(const char* format, ...)
	{
		va_list vlist;
		va_start(vlist, format);

		mInstance->mLogStream->OutStr(String::Format(format, vlist));

		va_end(vlist);
	}

	void Debug::Log(const String& out)
	{
		mInstance->mLogStream->OutStr(out);
	}

	void Debug::LogWarning(const char* format, ...)
	{
		va_list vlist;
		va_start(vlist, format);

		mInstance->mLogStream->WarningStr(String::Format(format, vlist));

		va_end(vlist);
	}

	void Debug::LogWarning(const String& out)
	{
		mInstance->mLogStream->WarningStr(out);
	}

	void Debug::LogError(const char* format, ...)
	{
		va_list vlist;
		va_start(vlist, format);

		mInstance->mLogStream->ErrorStr(String::Format(format, vlist));

		va_end(vlist);
	}

	void Debug::LogError(const String& out)
	{
		mInstance->mLogStream->ErrorStr(out);
	}

	LogStream* Debug::GetLog()
	{
		return mInstance->mLogStream;
	}
}