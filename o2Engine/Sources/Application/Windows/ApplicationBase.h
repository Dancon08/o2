#pragma once

#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include "Utils/Math/Vector2.h"
#include "Utils/Types/String.h"

namespace o2
{
	class Application;

	// -------------------------------
	// Windows application base fields
	// -------------------------------
	class ApplicationBase
	{
	protected:
		HWND   mHWnd;            // Window handle
		UInt   mWndStyle;        // Window styles data
		bool   mWindowed;        // True if app in windowed mode, false if in fullscreen mode
		bool   mWindowResizible; // True, if window can be sized by user
		Vec2I  mWindowedSize;    // Size of window
		Vec2I  mWindowedPos;     // Position of window
		String mWndCaption;      // Window caption
		bool   mActive;          // True, if window is active

		HCURSOR mCurrentCursor;

	public:
		// Returns window handle
		HWND GetWindowHandle() const { return mHWnd; }

		friend class Render;
		friend class FileSystem;
	};
}

#endif // PLATFORM_WINDOWS
