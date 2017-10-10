#pragma once

#include "Core/UIManager/BasicUIManager.h"

using namespace o2;

namespace Editor
{
	class EditorUIManagerBuilder: public BasicUIManagerBuilder
	{
	public:
		void RebuildExpandButton();
		void RebuildDockableWndStyle();
		void RebuildPlayStopButtonStyle();
		void RebuildPauseButtonStyle();
		void RebuildStepButtonStyle();
		void RebuildRoundDropDown();
		void RebuildArrowToggle();
		void RebuildBrushToggle();
		void RebuildMoveToggle();
		void RebuildRotateToggle();
		void RebuildScaleToggle();
		void RebuildFrameToggle();
		void RebuildSearchButton();
		void RebuildListTreeToggle();
		void RebuildRevertBtn();
		void RebuildActorsTreeNodeEnableToggle();
		void RebuildActorsTreeLockToggle();
		void RebuildActorsTreeLinkBtn();
		void RebuildActorsTree();
		void RebuildPanelDownButton();
		void RebuildTrashDownPanelButton();
		void RebuildMessagesDownPanelToggle();
		void RebuildWarningsDownPanelToggle();
		void RebuildErrorsDownPanelToggle();
		void RebuildFilterMenuButton();
		void RebuildTreeMenuButton();
		void RebuildFoldersTree();
		void RebuildRegularAssetIcon();
		void RebuildFolderAssetIcon();
		void RebuildPrototypeAssetIcon();
		void RebuildPrefabPreviewAssetIcon();
		void RebuildImagePreviewAssetIcon();
		void RebuildTextAssetIcon();
		void RebuildAnimationAssetIcon();
		void RebuildAssetsGridScroll();
		void RebuildLinkBtn();
		void RebuildSinglelineEditBoxWithArrows();
		void RebuildSinglelineEditboxProperty();
		void RebuildEditorDropdown();
		void RebuildActorPropety();
		void RebuildColorPropety();
		void RebuildAssetPropety();
		void RebuildComponentProperty();
		void RebuildVector2Property();
		void RebuildRedEditBoxStyle();
		void RebuildGreenEditBoxStyle();
		void RebuildColoredVector2Property();
		void RebuildRectProperty();
		void RebuildNewRectProperty();
		void RebuildActorHeadEnableToggle();
		void RebuildActorHeadName();
		void RebuildActorHeadLockToggle();
		void RebuildActorHeadActorAssetProperty();
		void RebuildActorHeadTagsProperty();
		void RebuildActorHeadLayerProperty();
		void RebuildAcceptPrototypeBtn();
		void RebuildRevertPrototypeBtn();
		void RebuildBreakPrototypeBtn();
		void RebuildComponentOptionsBtn();
		void RebuildComponentSaveBtn();
		void RebuildHorWideScrollbar();
		void RebuildVerWideScrollbar();
		void RebuildHorWideProgressbar();
		void RebuildVerWideProgressbar();
		void RebuildBooleanProperty();

		void RebuildEditorUIManager();

		IOBJECT(EditorUIManagerBuilder);
	};
}
