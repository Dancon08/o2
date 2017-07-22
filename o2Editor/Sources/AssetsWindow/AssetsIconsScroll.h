#pragma once

#include "Assets/ActorAsset.h"
#include "Assets/AssetInfo.h"
#include "Assets/ImageAsset.h"
#include "Events/DrawableCursorEventsListener.h"
#include "Events/KeyboardEventsListener.h"
#include "UI/ScrollArea.h"
#include "Utils/Containers/Pair.h"
#include "Utils/DragAndDrop.h"

using namespace o2;

namespace o2
{
	class Actor;
	class Sprite;
	class UIContextMenu;
	class UIGridLayout;
}

namespace Editor
{
	class ActorProperty;
	class ComponentProperty;
	class IAssetProperty;
	class UIActorsTree;
	class UIAssetIcon;

	// ------------------------
	// Assets icons scroll area
	// ------------------------
	class UIAssetsIconsScrollArea: public UIScrollArea, public DragDropArea, public KeyboardEventsListener, 
		public ISelectableDragableObjectsGroup
	{
	public:
		Function<void(const Vector<String>&)> onAssetsSelected; // Assets selected event

		// Default constructor
		UIAssetsIconsScrollArea();

		// Copy-constructor
		UIAssetsIconsScrollArea(const UIAssetsIconsScrollArea& other);

		// Destructor
		~UIAssetsIconsScrollArea();

		// Copy-operator
		UIAssetsIconsScrollArea& operator=(const UIAssetsIconsScrollArea& other);

		// Draws widget
		void Draw();

		// Updates widget
		void Update(float dt);

		// Sets current assets viewing path
		void SetViewingPath(const String& path);

		// Returns viewing path
		String GetViewingPath() const;

		// Updates assets path contents
		void UpdateAssetsPath();

		// Returns is this widget can be selected
		bool IsFocusable() const;

		// Hightlights asset
		void HightlightAsset(UID id);

		// Selects asset
		void SelectAsset(UID id, bool scroll = true);

		// Deselects all assets
		void DeselectAllAssets();

		// Returns selected assets infos
		Vector<AssetInfo> GetSelectedAssets() const;

		// Return asset icon under point
		UIAssetIcon* GetIconUnderPoint(const Vec2F& point) const;

		// Returns node hightlight drawable
		Sprite* GetHightlightDrawable() const;

		// Sets hightlight animation
		void SetHightlightAnimation(const Animation& animation);

		// Sets hightlight layout
		void SetHightlightLayout(const Layout& layout);

		// Returns selecting rectangle drawable
		Sprite* GetSelectingDrawable() const;

		// Returns true if point is in this object
		bool IsUnderPoint(const Vec2F& point);

		SERIALIZABLE(UIAssetsIconsScrollArea);

	protected:
		typedef Vector<UIAssetIcon*> AssetsIconsVec;
		typedef Vector<Sprite*> SpritesVec;
		typedef Dictionary<String, AssetsIconsVec> IconArrsDict;
		typedef Vector<Pair<UID, String>> AssetIdPathVec;
		typedef Vector<Actor*> ActorsVec;
		typedef Vector<AssetRef*> AssetsVec;

		const Vec2F        mAssetIconSize = Vec2F(50, 60);

		String             mCurrentPath = "_";          // Current viewing path

		UIGridLayout*      mGrid = nullptr;             // Assets icons grid
		UIContextMenu*     mContextMenu = nullptr;      // Assets Context menu

		AssetsIconsVec     mSelectedAssetsIcons;        // Selected assets icons
		AssetsVec          mSelectedPreloadedAssets;    // Preloaded selected assets

		UIAssetIcon*       mHightlightIcon = nullptr;  // Current hightlighting asset icon
		Animation          mHightlightAnim;             // Icon hightlight animation @SERIALIZABLE
		Sprite*            mHightlightSprite = nullptr; // Icon hightlight sprite @SERIALIZABLE
		Layout             mHightlightLayout;           // Icon hightlight sprite layout @SERIALIZABLE

		IconArrsDict       mIconsPool;                  // Assets icons pool

		Sprite*            mSelectionSprite = nullptr;  // Icons selection rectangle sprite @SERIALIZABLE
		bool               mSelecting = false;          // Is selecting icons 
		Vec2F              mPressedPoint;               // Pressed point
		AssetsIconsVec     mCurrentSelectingIcons;      // Selecting icons at current selection

		bool               mIsDraggingIcons = false;    // Is dragging icons
		bool               mDragEnded = false;          // Is dragging ended
		UIAssetIcon*       mDragIcon = nullptr;         // Dragging icon
		Vec2F              mDragOffset;                 // Dragging offset from cursor to icon center
		ActorsVec          mInstSceneDragActors;        // Instantiated actors when dragging asset above scene

		AssetIdPathVec     mCuttingAssets;              // Current cutted assets
						   
		bool               mNeedRebuildAssets = false;  // Is assets needs to rebuild

		bool               mChangePropertiesTargetsFromThis = false;

	protected:
		// It is called when assets selection was changed
		void OnAssetsSelected();

		// Updates layout
		void UpdateLayout(bool forcible = false, bool withChildren = true);

		// Updates cutting assets
		void UpdateCuttingAssets();

		// It is called when widget was selected
		void OnFocused();

		// It is called when widget was deselected
		void OnUnfocused();

		// It is called when cursor pressed on this
		void OnCursorPressed(const Input::Cursor& cursor);

		// It is called when cursor released (only when cursor pressed this at previous time)
		void OnCursorReleased(const Input::Cursor& cursor);

		// It is called when cursor pressing was broken (when scrolled scroll area or some other)
		void OnCursorPressBreak(const Input::Cursor& cursor);

		// It is called when cursor stay down during frame
		void OnCursorStillDown(const Input::Cursor& cursor);

		// Begins selecting icons
		void BeginSelecting();

		// Updates selection frame
		void UpdateSelection(const Input::Cursor& cursor);

		// Completes selecting
		void CompleteSelecting();

		// Registers actors creation undo action
		void RegActorsCreationAction();

		// It is called when cursor moved on this (or moved outside when this was pressed)
		void OnCursorMoved(const Input::Cursor& cursor);

		// It is called when right mouse button was released (only when right mouse button pressed this at previous time)
		void OnCursorRightMouseReleased(const Input::Cursor& cursor);

		// It is called when key was released
		void OnKeyReleased(const Input::Key& key);

		// Initializes assets context menu
		void InitializeContext();

		// Prepares icons pools
		void PrepareIconsPools();

		// Returns asset icon from pool
		UIAssetIcon* GetAssetIconFromPool(const String& style);

		// Frees icon to pool
		void FreeAssetIconToPool(UIAssetIcon* icon);

		// Returns image asset icon for asset
		UIAssetIcon* GetImageAssetIcon(const AssetInfo& asset);

		// Updates assets grid size
		void UpdateAssetsGridSize();

		// It is called when asset icon double clicked
		void OnIconDblClicked(UIAssetIcon* icon);

		// It is called when context copy pressed
		void OnContextCopyPressed();

		// It is called when context cut pressed
		void OnContextCutPressed();

		// It is called when context paste pressed
		void OnContextPastePressed();

		// It is called when context delete pressed
		void OnContextDeletePressed();

		// It is called when context open pressed
		void OnContextOpenPressed();

		// It is called when context show in explorer pressed
		void OnContextShowInExplorerPressed();

		// It is called when context import pressed
		void OnContextImportPressed();

		// It is called when context create folder pressed
		void OnContextCreateFolderPressed();

		// It is called when context create prefab pressed
		void OnContextCreatePrefabPressed();

		// It is called when context create script pressed
		void OnContextCreateScriptPressed();

		// It is called when context create animation pressed
		void OnContextCreateAnimationPressed();

		// Instantiates dragging assets
		void InstantiateDraggingAssets();

		// Removes and clears instantiated dragging assets
		void ClearInstantiatedDraggingAssets();

		// Instentiates actor from asset info
		Actor* InstantiateAsset(const AssetInfo& assetInfo);

		// Dummy asset instantiate function from asset
		template<typename _type>
		Actor* InstantiateAsset(const _type& asset);

		// Instantiate actor from image asset
		Actor* InstantiateAsset(const ImageAssetRef& asset);

		// Instantiate actor from actor asset
		Actor* InstantiateAsset(const ActorAssetRef& asset);

		// Checks preloaded assets for saving
		void CheckPreloadedAssetsSaving();
		
// ISelectableDragableObjectsGroup implementation

		// Returns selected objects in group
		SelectDragObjectsVec GetSelectedDragObjects() const;

		// Returns all objects in group 
		SelectDragObjectsVec GetAllObjects() const;

		// Selects object
		void Select(SelectableDragableObject* object);

		// Selects object
		void Select(SelectableDragableObject* object, bool sendOnSelectionChanged);

		// Deselects object
		void Deselect(SelectableDragableObject* object);

		// Adds selectable object to group
		void AddSelectableObject(SelectableDragableObject* object);

		// Removes selectable object from group
		void RemoveSelectableObject(SelectableDragableObject* object);

		// It is called when selectable draggable object was released
		void OnSelectableObjectCursorReleased(SelectableDragableObject* object, const Input::Cursor& cursor);

		// It is called when selectable object was began to drag
		void OnSelectableObjectBeganDragging(SelectableDragableObject* object);

// DragDropArea implementation

		// Begins dragging selected items
		void BeginDragging(UIAssetIcon* icon);

		// Ends dragging items
		void EndDragging(bool droppedToThis = false);

		// Updates dragging graphics
		void UpdateDraggingGraphics();

		// It is called when some drag listeners was entered to this area
		void OnDragEnter(ISelectableDragableObjectsGroup* group);

		// It is called when some drag listeners was dragged above this area
		void OnDraggedAbove(ISelectableDragableObjectsGroup* group);

		// It is called when some drag listeners was exited from this area
		void OnDragExit(ISelectableDragableObjectsGroup* group);

		// It is called when some selectable listeners was dropped to this
		void OnDropped(ISelectableDragableObjectsGroup* group);

		// It is called when dropped dragged assets icons selected and started dragging from this
		void OnDroppedFromThis();

		// It is called when dropped dragged actors tree nodes selected and started dragging from actors tree
		void OnDroppedFromActorsTree(UIActorsTree* actorsTree);

		friend class AssetsWindow;
		friend class SceneEditScreen;
		friend class UIActorsTree;
		friend class UIAssetIcon;
	};

	template<typename _type>
	Actor* UIAssetsIconsScrollArea::InstantiateAsset(const _type& asset)
	{
		return nullptr;
	}
}
