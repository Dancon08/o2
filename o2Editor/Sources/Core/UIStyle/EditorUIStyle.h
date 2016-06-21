#pragma once

#include "Core/UIStyle/BasicUIStyle.h"

using namespace o2;

namespace Editor
{
	class EditorUIStyleBuilder: public BasicUIStyleBuilder
	{
	public:
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
		void RebuildPrefabAssetIcon();
		void RebuildPrefabPreviewAssetIcon();
		void RebuildImagePreviewAssetIcon();
		void RebuildTextAssetIcon();
		void RebuildAnimationAssetIcon();
		void RebuildAssetsGridScroll();
		void RebuildLinkBtn();
		void RebuildEditorDropdown();

		void RebuildEditorUIStyle();

		IOBJECT(EditorUIStyleBuilder);
	};
}