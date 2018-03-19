#pragma once

#include "UIDockableWindow.h"
#include "Utils/Serializable.h"

using namespace o2;

namespace Editor
{
	// -----------------------------
	// Basic editor window interface
	// -----------------------------
	class IEditorWindow: public IObject
	{
	public:
		// Default constructor
		IEditorWindow();

		// Copy-constructor
		IEditorWindow(const IEditorWindow& other);

		// Virtual destructor
		virtual ~IEditorWindow();

		// Sets visibility
		virtual void SetVisible(bool visible);

		// Updates window logic
		virtual void Update(float dt);

		// Draws window stuff
		virtual void Draw();

		// Returns is window is visible
		bool IsVisible();

		// Shows window
		void Show();

		// Hides window
		void Hide();

		IOBJECT(IEditorWindow);

	protected:
		UIDockableWindow* mWindow = nullptr; // Dockable UI window 

	protected:
		// It is called after that all windows was created
		virtual void PostInitializeWindow() {}

		friend class WindowsManager;
		friend class WindowsLayout;
	};
}

CLASS_BASES_META(Editor::IEditorWindow)
{
	BASE_CLASS(o2::IObject);
}
END_META;
CLASS_FIELDS_META(Editor::IEditorWindow)
{
	PROTECTED_FIELD(mWindow);
}
END_META;
CLASS_METHODS_META(Editor::IEditorWindow)
{

	PUBLIC_FUNCTION(void, SetVisible, bool);
	PUBLIC_FUNCTION(void, Update, float);
	PUBLIC_FUNCTION(void, Draw);
	PUBLIC_FUNCTION(bool, IsVisible);
	PUBLIC_FUNCTION(void, Show);
	PUBLIC_FUNCTION(void, Hide);
	PROTECTED_FUNCTION(void, PostInitializeWindow);
}
END_META;
