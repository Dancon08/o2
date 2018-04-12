#include "stdafx.h"
#include "AssetsWindow.h"

#include "Application/Application.h"
#include "Assets/ActorAsset.h"
#include "Assets/AnimationAsset.h"
#include "Assets/Assets.h"
#include "Assets/AssetsTree.h"
#include "Assets/DataAsset.h"
#include "Assets/FolderAsset.h"
#include "Assets/ImageAsset.h"
#include "AssetsWindow/AssetsIconsScroll.h"
#include "AssetsWindow/FoldersTree.h"
#include "AssetsWindow/UIAssetIcon.h"
#include "Core/EditorConfig.h"
#include "UI/Button.h"
#include "UI/EditBox.h"
#include "UI/GridLayout.h"
#include "UI/Label.h"
#include "UI/Tree.h"
#include "UI/UIManager.h"
#include "UI/WidgetLayer.h"
#include "UI/WidgetLayout.h"
#include "Utils/System/Clipboard.h"
#include "Utils/Delegates.h"
#include "Utils/FileSystem/FileSystem.h"

DECLARE_SINGLETON(Editor::AssetsWindow);

namespace Editor
{
	AssetsWindow::AssetsWindow()
	{
		if (Application::IsReady())
			InitializeWindow();
	}

	AssetsWindow::~AssetsWindow()
	{}

	void AssetsWindow::InitializeWindow()
	{
		o2Assets.onAssetsRebuilded += THIS_FUNC(OnAssetsRebuilded);

		mWindow->caption = "Assets";
		mWindow->name = "assets window";
		mWindow->SetIcon(mnew Sprite("ui/UI2_folder_icon.png"));
		mWindow->SetIconLayout(Layout::Based(BaseCorner::LeftTop, Vec2F(20, 20), Vec2F(0, 1)));
		mWindow->SetViewLayout(Layout::BothStretch(-2, 0, 0, 18));
		mWindow->SetClippingLayout(Layout::BothStretch(-1, 0, 0, 18));

		InitializeUpPanel();
		InitializeDownPanel();
		InitializeFoldersTree();
		InitializeFoldersTreeVisibleState();
		InitializeFoldersTreeSeparator();

		OpenFolder("");
	}

	void AssetsWindow::InitializeFoldersTreeSeparator()
	{
		auto separatorLayer = mFoldersTree->FindLayer("separator");

		mSeparatorHandle.isUnderPoint = [=](const Vec2F& point) {
			RectF rt = separatorLayer->drawable->GetRect();
			rt.left -= 2; rt.right += 2;
			return rt.IsInside(point);
		};

		mSeparatorCoef = o2EditorConfig.GetProjectUserData()["layout/assetsWindow/separator_coef"];

		if (mSeparatorCoef < FLT_EPSILON)
			mSeparatorCoef = 0.5f;

		mFoldersTree->layout->anchorRight = mSeparatorCoef;
		mAssetsGridScroll->layout->anchorLeft = mSeparatorCoef;

		mSeparatorHandle.onMoved = [&](const Input::Cursor& cursor) {
			float anchorDelta = cursor.delta.x / mWindow->layout->width;
			mFoldersTree->layout->anchorRight += anchorDelta;
			mAssetsGridScroll->layout->anchorLeft += anchorDelta;

			auto& userData = o2EditorConfig.GetProjectUserData();
			userData["layout/assetsWindow/separator_coef"].SetValue(mFoldersTree->layout->GetAnchorRight());
		};

		mAssetsGridScroll->onDraw += [&]() { mSeparatorHandle.OnDrawn(); };

		mSeparatorHandle.cursorType = CursorType::SizeWE;
	}

	void AssetsWindow::InitializeFoldersTreeVisibleState()
	{
		mFoldersTreeShowCoef = 1.0f;
		mFoldersTreeVisible = true;

		mFoldersTreeShowAnim.SetTarget(this);
		*mFoldersTreeShowAnim.AddAnimationValue<float>("mFoldersTreeShowCoef") =
			AnimatedValue<float>::EaseInOut(0, 1, 0.2f);

		mFoldersTreeShowAnim.onUpdate = [&](float dt) {
			mFoldersTree->layout->anchorRight = mSeparatorCoef*mFoldersTreeShowCoef;
			mAssetsGridScroll->layout->anchorLeft = mSeparatorCoef*mFoldersTreeShowCoef;

			float disableCoef = 0.2f;
			if (mFoldersTreeShowCoef < disableCoef && mFoldersTree->IsEnabled())
				mFoldersTree->Hide();

			if (mFoldersTreeShowCoef >= disableCoef && !mFoldersTree->IsEnabled())
				mFoldersTree->Show();
		};
	}

	void AssetsWindow::InitializeFoldersTree()
	{
		mFoldersTree = mnew UIAssetsFoldersTree();
		*mFoldersTree->layout = UIWidgetLayout(0.0f, 1.0f, 0.5f, 0.0f, 0.0f, -18.0f, 0.0f, 18.0f);

		auto separatorLayer = mFoldersTree->AddLayer("separator", mnew Sprite("ui/UI_Ver_separator.png"),
													 Layout::VerStretch(HorAlign::Right, -2, 0, 5, 0));
		mFoldersTree->UpdateView();
		mWindow->AddChild(mFoldersTree);

		// assets scroll & grid
		mAssetsGridScroll = o2UI.CreateWidget<UIAssetsIconsScrollArea>();
		*mAssetsGridScroll->layout = UIWidgetLayout(0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -18.0f, 0.0f, 18.0f);
		mWindow->AddChild(mAssetsGridScroll);

		mAssetsGridScroll->onAssetsSelected = [&](const Vector<String>& assetsPaths) {
			if (!assetsPaths.IsEmpty())
				mSelectedAssetPathLabel->text = assetsPaths.Last();
			else
				mSelectedAssetPathLabel->text = mAssetsGridScroll->GetViewingPath();
		};
	}

	void AssetsWindow::InitializeDownPanel()
	{
		UIWidget* downPanel = mnew UIWidget();
		downPanel->AddLayer("back", mnew Sprite("ui/UI2_small_panel_down_back.png"),
							Layout::BothStretch(-4, -5, -4, -5));
		*downPanel->layout = UIWidgetLayout::HorStretch(VerAlign::Bottom, 0, 0, 20, 0);

		auto showTreeBtn = o2UI.CreateWidget<UIButton>("menu tree");
		*showTreeBtn->layout = UIWidgetLayout::Based(BaseCorner::Left, Vec2F(20, 20), Vec2F(0, 0));
		showTreeBtn->onClick += THIS_FUNC(OnShowTreePressed);
		downPanel->AddChild(showTreeBtn);

		mSelectedAssetPathLabel = o2UI.CreateWidget<UILabel>();
		*mSelectedAssetPathLabel->layout = UIWidgetLayout::BothStretch(20, 0, 0, 0);
		mSelectedAssetPathLabel->text = "Assets/Folder/Image.png";
		mSelectedAssetPathLabel->horOverflow = UILabel::HorOverflow::Dots;
		mSelectedAssetPathLabel->horAlign = HorAlign::Left;
		downPanel->AddChild(mSelectedAssetPathLabel);

		mWindow->AddChild(downPanel);
	}

	void AssetsWindow::InitializeUpPanel()
	{
		UIWidget* upPanel = mnew UIWidget();
		upPanel->name = "up panel";
		*upPanel->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 0, 0, 20, 0);
		upPanel->AddLayer("back", mnew Sprite("ui/UI_square_field.png"), Layout::BothStretch(-4, -4, -5, -5));

		UIButton* searchButton = o2UI.CreateWidget<UIButton>("search");
		*searchButton->layout = UIWidgetLayout::Based(BaseCorner::Left, Vec2F(20, 20), Vec2F(-1, 1));
		upPanel->AddChild(searchButton);

		mFilterButton = o2UI.CreateWidget<UIButton>("menu filter");
		*mFilterButton->layout = UIWidgetLayout::Based(BaseCorner::Right, Vec2F(20, 20), Vec2F(0, 1));
		mFilterButton->onClick += THIS_FUNC(OnMenuFilterPressed);
		upPanel->AddChild(mFilterButton);

		mSearchEditBox = o2UI.CreateWidget<UIEditBox>("backless");
		*mSearchEditBox->layout = UIWidgetLayout::BothStretch(19, 2, 21, -2);
		mSearchEditBox->onChanged += THIS_FUNC(OnSearchEdited);
		upPanel->AddChild(mSearchEditBox);

		mWindow->AddChild(upPanel);
	}

	void AssetsWindow::Update(float dt)
	{
		IEditorWindow::Update(dt);
		mFoldersTreeShowAnim.Update(dt);
	}

	void AssetsWindow::SelectAsset(UID id)
	{
		String assetPath = o2Assets.GetAssetPath(id);
		String folder = o2FileSystem.GetParentPath(assetPath);

		if (GetOpenedFolderPath() != folder)
			OpenFolder(folder);

		mAssetsGridScroll->SelectAsset(id);
	}

	void AssetsWindow::SelectAsset(const String& path)
	{
		UID id = o2Assets.GetAssetId(path);
		String folder = o2FileSystem.GetParentPath(path);

		if (GetOpenedFolderPath() != folder)
			OpenFolder(folder);

		mAssetsGridScroll->SelectAsset(id);
	}

	void AssetsWindow::SelectAsset(const Vector<UID>& ids)
	{
		for (auto id : ids)
			SelectAsset(id);
	}

	void AssetsWindow::SelectAssets(const Vector<String>& paths)
	{
		for (auto path : paths)
			SelectAsset(path);
	}

	void AssetsWindow::OpenAsset(UID id)
	{
		OpenAsset(o2Assets.GetAssetPath(id));
	}

	void AssetsWindow::OpenAsset(const String& path)
	{
		String fullPath = o2FileSystem.GetParentPath(o2Application.GetBinPath() + "/" + o2Assets.GetAssetsPath() + path);
		ShellExecute(NULL, "explore", fullPath, NULL, NULL, SW_SHOWNORMAL);
	}

	void AssetsWindow::OpenAndEditAsset(UID id)
	{
		OpenAndEditAsset(o2Assets.GetAssetPath(id));
	}

	void AssetsWindow::OpenAndEditAsset(const String& path)
	{
		String fullPath = o2Application.GetBinPath() + "/" + o2Assets.GetAssetsPath() + path;
		ShellExecute(NULL, "edit", fullPath, NULL, NULL, SW_SHOWNORMAL);
	}

	void AssetsWindow::DeselectAssets()
	{
		mAssetsGridScroll->DeselectAllAssets();
	}

	Vector<AssetInfo> AssetsWindow::GetSelectedAssets() const
	{
		return mAssetsGridScroll->GetSelectedAssets();
	}

	String AssetsWindow::GetOpenedFolderPath() const
	{
		return mAssetsGridScroll->GetViewingPath();
	}

	void AssetsWindow::OpenFolder(const String& path)
	{
		mAssetsGridScroll->SetViewingPath(path);
		mFoldersTree->SelectAndExpandFolder(path);
		mSelectedAssetPathLabel->text = path;
	}

	void AssetsWindow::ShowAssetIcon(UID id)
	{
		String assetPath = o2Assets.GetAssetPath(id);
		String folder = o2FileSystem.GetParentPath(assetPath);

		if (GetOpenedFolderPath() != folder)
			OpenFolder(folder);

		mAssetsGridScroll->HightlightAsset(id);
	}

	void AssetsWindow::ShowAssetIcon(const String& path)
	{
		UID assetId = o2Assets.GetAssetId(path);
		String folder = o2FileSystem.GetParentPath(path);

		if (GetOpenedFolderPath() != folder)
			OpenFolder(folder);

		mAssetsGridScroll->HightlightAsset(assetId);
	}

	void AssetsWindow::CopyAssets(const Vector<String>& assetsPaths)
	{
		mCuttingAssets.Clear();
		mAssetsGridScroll->UpdateCuttingAssets();

		Vector<WString> paths;
		for (auto& path : assetsPaths)
			paths.Add(o2Application.GetBinPath() + "/" + o2Assets.GetAssetsPath() + path);

		Clipboard::CopyFiles(paths);
	}

	void AssetsWindow::CopyAssetFolder(const String& src, const String& dst)
	{
		o2FileSystem.FolderCreate(dst);

		FolderInfo info = o2FileSystem.GetFolderInfo(src);
		info.ClampPathNames();

		for (auto& file : info.mFiles)
		{
			if (!file.mPath.EndsWith(".meta"))
				o2FileSystem.FileCopy(src + "/" + file.mPath, dst + "/" + file.mPath);
		}

		for (auto& folder : info.mFolders)
			CopyAssetFolder(src + "/" + folder.mPath, dst + "/" + folder.mPath);
	}

	void AssetsWindow::CutAssets(const Vector<String>& assetsPaths)
	{
		mCuttingAssets.Clear();

		Vector<WString> paths;
		for (auto& path : assetsPaths)
		{
			String fullPath = o2Application.GetBinPath() + "/" + o2Assets.GetAssetsPath() + path;
			mCuttingAssets.Add(Pair<UID, String>(o2Assets.GetAssetId(path), fullPath));
			paths.Add(fullPath);
		}

		Clipboard::CopyFiles(paths);

		mAssetsGridScroll->UpdateCuttingAssets();
	}

	void AssetsWindow::PasteAssets(const String& targetPath)
	{
		Vector<WString> paths = Clipboard::GetCopyFiles();
		for (auto path : paths)
		{
			String fileName = o2FileSystem.GetPathWithoutDirectories(path);
			bool isFolder = o2FileSystem.IsFolderExist(path);

			String copyFileName = o2Application.GetBinPath() + "/" + o2Assets.GetAssetsPath() + targetPath + "/" + fileName;
			String extension = o2FileSystem.GetFileExtension(fileName);
			String fileNameWithoutExt = o2FileSystem.GetFileNameWithoutExtension(fileName);
			bool endsAsCopy = fileNameWithoutExt.EndsWith("copy");
			int i = 0;
			while (o2FileSystem.IsFileExist(copyFileName))
			{
				copyFileName = o2Application.GetBinPath() + "/" + o2Assets.GetAssetsPath() + targetPath + "/" +
					fileNameWithoutExt;

				if (!endsAsCopy)
					copyFileName += " copy";

				if (i > 0)
					copyFileName += (String)(i + 1) + "." + extension;

				if (!isFolder)
					copyFileName += "." + extension;

				i++;
			}

			if (mCuttingAssets.ContainsPred([&](auto x) { return x.second == path; }))
			{
				o2FileSystem.FileMove(path, copyFileName);
				o2FileSystem.FileMove(path + ".meta", copyFileName + ".meta");
			}
			else
			{
				if (!isFolder)
					o2FileSystem.FileCopy(path, copyFileName);
				else
					CopyAssetFolder(path, copyFileName);
			}
		}

		mCuttingAssets.Clear();
		mAssetsGridScroll->UpdateCuttingAssets();

		o2Assets.RebuildAssets();
	}

	void AssetsWindow::DeleteAssets(const Vector<String>& assetsPaths)
	{
		mCuttingAssets.Clear();
		mAssetsGridScroll->UpdateCuttingAssets();

		for (auto& path : assetsPaths)
			o2Assets.RemoveAsset(path, false);

		o2Assets.RebuildAssets();
	}

	void AssetsWindow::ImportAssets(const String& targetPath)
	{

	}

	void AssetsWindow::CreateFolderAsset(const String& targetPath)
	{
		FolderAssetRef folderAsset = FolderAssetRef::CreateAsset();
		folderAsset->Save(o2Assets.MakeUniqueAssetName(targetPath + "/New folder"));

		o2Assets.RebuildAssets();
	}

	void AssetsWindow::CreatePrefabAsset(const String& targetPath)
	{
		ActorAssetRef folderAsset = ActorAssetRef::CreateAsset();
		folderAsset->Save(o2Assets.MakeUniqueAssetName(targetPath + "/New prototye.proto"));

		o2Assets.RebuildAssets();
	}

	void AssetsWindow::CreateScriptAsset(const String& targetPath)
	{

	}

	void AssetsWindow::CreateAnimationAsset(const String& targetPath)
	{
		AnimationAssetRef folderAsset = AnimationAssetRef::CreateAsset();
		folderAsset->Save(o2Assets.MakeUniqueAssetName(targetPath + "/New animation.anim"));

		o2Assets.RebuildAssets();
	}

	Sprite* AssetsWindow::GetAssetIconSprite(const AssetRef& asset)
	{
		const Type& type = asset->GetType();

		if (type == TypeOf(ImageAsset))
			return mnew Sprite(asset->GetPath());
		else if (type == TypeOf(ActorAsset))
			return mnew Sprite("ui/UI2_actor_icon.png");
		else if (type == TypeOf(FolderAsset))
			return mnew Sprite("ui/UI2_big_folder_icon.png");
		else if (type == TypeOf(DataAsset))
			return mnew Sprite("ui/UI2_big_text_file_icon.png");
		else if (type == TypeOf(AnimationAsset))
			return mnew Sprite("ui/UI2_anim_file_icon.png"); 

		return mnew Sprite("ui/UI2_big_file_icon.png"); 
	}

	void AssetsWindow::OnSearchEdited(const WString& search)
	{}

	void AssetsWindow::OnMenuFilterPressed()
	{}

	void AssetsWindow::OnShowTreePressed()
	{
		mFoldersTreeVisible = !mFoldersTreeVisible;

		if (mFoldersTreeVisible)
			mFoldersTreeShowAnim.PlayForward();
		else
			mFoldersTreeShowAnim.PlayBack();
	}

	void AssetsWindow::OnAssetsRebuilded(const Vector<UID>& changedAssets)
	{
		mFoldersTree->UpdateView();
		mAssetsGridScroll->UpdateAssetsPath();
	}
}

DECLARE_CLASS(Editor::AssetsWindow);
