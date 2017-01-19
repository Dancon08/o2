#include "ConsoleLogStream.h"

#include <iostream>

#ifdef WINDOWS
#include <io.h>
#endif

// TODO: Can't include <wincon.h> - compiler throws many stupid errors from GDI

namespace o2
{
	ConsoleLogStream::ConsoleLogStream():
		LogStream()
	{
		InitConsole();
	}

	ConsoleLogStream::ConsoleLogStream(const WString& id):
		LogStream(id)
	{
		InitConsole();
	}

	ConsoleLogStream::~ConsoleLogStream()
	{
		//FreeConsole();
	}

	void ConsoleLogStream::OutStrEx(const WString& str)
	{
		puts(((String)str).Data());
		//puts("\n");
	}

	void ConsoleLogStream::InitConsole()
	{
		/*if (AllocConsole())
		{
			int hCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
			*stdout = *(::_fdopen(hCrt, "w"));
			::setvbuf(stdout, NULL, _IONBF, 0);
			*stderr = *(::_fdopen(hCrt, "w"));
			::setvbuf(stderr, NULL, _IONBF, 0);
		}*/
	}
}
