#pragma once

#include "Events/CursorEventsListener.h"
#include "Utils/CursorEventsArea.h"
#include "Utils/Delegates.h"
#include "Utils/Math/Curve.h"
#include "Utils/Singleton.h"

using namespace o2;

namespace o2
{
	class UIWindow;
}

namespace Editor
{
	class UICurveEditor;

	// --------------------
	// Curve editing dialog
	// --------------------
	class CurveEditorDlg: public Singleton<CurveEditorDlg>, public CursorEventsListener
	{
	public:
		// Default constructor
		CurveEditorDlg();

		// Destructor
		~CurveEditorDlg();

		// Shows editor dialog
		static void Show(Function<void()> onChanged);

		// Adds editing curve with color. If color is default it will be randomized
		static void AddEditingCurve(Curve* curve, const Color4& color = Color4::Green());

		// Removed curve from editing
		static void RemoveEditingCurve(Curve* curve);

		// Removes all editing curves
		static void RemoveAllEditingCurves();

		// Adds curves range with color. It can't be edited, just a solid color between curves
		static void AddCurvesRange(Curve* curveA, Curve* curveB, const Color4& color = Color4::Green());

		// Removes curve range
		static void RemoveCurvesRange(Curve* curveA, Curve* curveB);

	protected:
		Function<void()> mOnChangedCallback;

		UIWindow*      mWindow;
		UICurveEditor* mEditorWidget;

	protected:
		// Initialize controls
		void InitializeControls();

		// Calls when cursor was pressed outside, closed dialog
		void OnCursorPressedOutside();
	};
	 
}
 