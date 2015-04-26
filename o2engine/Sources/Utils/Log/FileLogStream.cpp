#include "FileLogStream.h"

#include <fstream>

namespace o2
{
	FileLogStream::FileLogStream(const String& fileName):
		LogStream(), mFilename(fileName)
	{
		std::fstream ofs(mFilename, std::ios::out);
		if (ofs) 
			ofs.close();
	}

	FileLogStream::FileLogStream(const String& id, const String& fileName):
		LogStream(id), mFilename(fileName)
	{
		std::fstream ofs(mFilename, std::ios::out);
		if (ofs)
			ofs.close();
	}

	FileLogStream::~FileLogStream()
	{
	}

	void FileLogStream::OutStrEx(const String& str)
	{
		std::fstream ofs(mFilename, std::ios::app);
		if (ofs)
		{
			ofs << str << std::endl;
			ofs.close();
		}
	}
}