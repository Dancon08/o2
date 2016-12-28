#include "ActorProperty.h"

#include "Animation/AnimatedFloat.h"
#include "Animation/AnimatedVector.h"
#include "Animation/Animation.h"
#include "Application/Application.h"
#include "Assets/ActorAsset.h"
#include "Assets/Assets.h"
#include "AssetsWindow/AssetsIconsScroll.h"
#include "AssetsWindow/AssetsWindow.h"
#include "Render/Sprite.h"
#include "Scene/Actor.h"
#include "SceneWindow/SceneEditScreen.h"
#include "TreeWindow/ActorsTree.h"
#include "TreeWindow/TreeWindow.h"
#include "UI/Button.h"
#include "UI/UIManager.h"
#include "UI/Widget.h"

namespace Editor
{
	ActorProperty::ActorProperty(UIWidget* widget /*= nullptr*/)
	{
		if (!widget)
			widget = o2UI.CreateWidget<UIWidget>("actorProperty");

		mBox = widget;
		mNameText = widget->GetLayerDrawable<Text>("caption");
		mBox->onDraw += [&]() { OnDrawn(); };
		mBox->SetFocusable(true);
	}

	ActorProperty::~ActorProperty()
	{
		delete mBox;
	}

	void ActorProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, Actor* value) { *((Property<Actor*>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<Actor*>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, Actor* value) { *((Actor**)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((Actor**)(ptr)); };
		}

		mValuesPointers = targets;

		Update();
	}

	void ActorProperty::Update()
	{
		if (mValuesPointers.IsEmpty())
			return;

		mCommonValue = mGetFunc(mValuesPointers[0]);
		mValuesDifferent = false;

		for (int i = 1; i < mValuesPointers.Count(); i++)
		{
			if (mCommonValue != mGetFunc(mValuesPointers[i]))
			{
				mValuesDifferent = true;
				break;
			}
		}

		if (mValuesDifferent)
		{
			mNameText->text = "--";
			mBox->layer["caption"]->transparency = 1.0f;
		}
		else
		{
			if (!mCommonValue)
			{
				mNameText->text = "Null:Actor";
				mBox->layer["caption"]->transparency = 0.5f;
			}
			else
			{
				mNameText->text = mCommonValue->GetName();
				mBox->layer["caption"]->transparency = 1.0f;
			}
		}
	}

	UIWidget* ActorProperty::GetWidget() const
	{
		return mBox;
	}

	Actor* ActorProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool ActorProperty::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	const Type* ActorProperty::GetFieldType() const
	{
		return &TypeOf(Actor*);
	}

	void ActorProperty::SetValue(Actor* value)
	{
		mCommonValue = value;
		mValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();

		Update();

		o2EditorSceneScreen.OnSceneChanged();
	}

	void ActorProperty::SetUnknownValue()
	{
		mCommonValue = nullptr;
		mValuesDifferent = true;

		mNameText->text = "--";
		mBox->layer["caption"]->transparency = 1.0f;

		o2EditorSceneScreen.OnSceneChanged();
	}

	bool ActorProperty::IsUnderPoint(const Vec2F& point)
	{
		return mBox->IsUnderPoint(point);
	}

	void ActorProperty::OnCursorEnter(const Input::Cursor& cursor)
	{
		mBox->SetState("hover", true);
	}

	void ActorProperty::OnCursorExit(const Input::Cursor& cursor)
	{
		mBox->SetState("hover", false);
	}

	void ActorProperty::OnCursorPressed(const Input::Cursor& cursor)
	{
		o2UI.FocusWidget(mBox);

		if (mCommonValue)
		{
			if (mCommonValue->IsAsset())
			{				
				o2EditorAssets.ShowAssetIcon(o2Assets.GetAssetPath(mCommonValue->GetAssetId()));
			}
			else if (mCommonValue->IsOnScene())
			{
				o2EditorTree.HightlightActorsTreeNode(mCommonValue);
				//o2EditorSceneScreen.SelectActor(mCommonValue, false);
			}
		}
	}

	void ActorProperty::OnKeyPressed(const Input::Key& key)
	{
		if (mBox->IsFocused() && key == VK_DELETE || key == VK_BACK)
			SetValue(nullptr);
	}

	void ActorProperty::OnDropped(ISelectableDragableObjectsGroup* group)
	{
		if (auto* actorsTree = dynamic_cast<UIActorsTree*>(group))
			OnDroppedFromActorsTree(actorsTree);
		else if (auto* assetsScroll = dynamic_cast<UIAssetsIconsScrollArea*>(group))
			OnDroppedFromAssetsScroll(assetsScroll);
	}

	void ActorProperty::OnDragEnter(ISelectableDragableObjectsGroup* group)
	{
		if (auto* actorsTree = dynamic_cast<UIActorsTree*>(group))
			OnDragEnterFromActorsTree(actorsTree);
		else if (auto* assetsScroll = dynamic_cast<UIAssetsIconsScrollArea*>(group))
			OnDragEnterFromAssetsScroll(assetsScroll);
	}

	void ActorProperty::OnDragExit(ISelectableDragableObjectsGroup* group)
	{
		if (auto* actorsTree = dynamic_cast<UIActorsTree*>(group))
			OnDragExitFromActorsTree(actorsTree);
		else if (auto* assetsScroll = dynamic_cast<UIAssetsIconsScrollArea*>(group))
			OnDragExitFromAssetsScroll(assetsScroll);
	}

	void ActorProperty::OnDroppedFromActorsTree(UIActorsTree* actorsTree)
	{
		if (actorsTree->GetSelectedActors().Count() > 1)
			return;

		SetValue(actorsTree->GetSelectedActors()[0]);

		o2Application.SetCursor(CursorType::Arrow);
		mBox->Focus();
	}

	void ActorProperty::OnDragEnterFromActorsTree(UIActorsTree* actorsTree)
	{
		if (actorsTree->GetSelectedActors().Count() > 1)
			return;

		o2Application.SetCursor(CursorType::Hand);
		mBox->SetState("focused", true);
	}

	void ActorProperty::OnDragExitFromActorsTree(UIActorsTree* actorsTree)
	{
		if (actorsTree->GetSelectedActors().Count() > 1)
			return;

		o2Application.SetCursor(CursorType::Arrow);
		mBox->SetState("focused", false);
	}

	void ActorProperty::OnDroppedFromAssetsScroll(UIAssetsIconsScrollArea* assetsIconsScroll)
	{
		if (assetsIconsScroll->GetSelectedAssets().Count() > 1)
			return;

		auto lastSelectedAsset = assetsIconsScroll->GetSelectedAssets().Last();
		if (lastSelectedAsset.mType != TypeOf(ActorAsset).ID())
			return;

		SetValue(o2Scene.GetAssetActorByID(lastSelectedAsset.mId));

		o2Application.SetCursor(CursorType::Arrow);
		mBox->Focus();
	}

	void ActorProperty::OnDragEnterFromAssetsScroll(UIAssetsIconsScrollArea* assetsIconsScroll)
	{
		if (assetsIconsScroll->GetSelectedAssets().Count() > 1)
			return;

		auto lastSelectedAsset = assetsIconsScroll->GetSelectedAssets().Last();
		if (lastSelectedAsset.mType != TypeOf(ActorAsset).ID())
			return;

		o2Application.SetCursor(CursorType::Hand);
		mBox->SetState("focused", true);
	}

	void ActorProperty::OnDragExitFromAssetsScroll(UIAssetsIconsScrollArea* assetsIconsScroll)
	{
		if (assetsIconsScroll->GetSelectedAssets().Count() > 1)
			return;

		o2Application.SetCursor(CursorType::Arrow);
		mBox->SetState("focused", false);
	}

}
 
CLASS_META(Editor::ActorProperty)
{
	BASE_CLASS(Editor::IPropertyField);
	BASE_CLASS(o2::KeyboardEventsListener);
	BASE_CLASS(o2::DragDropArea);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mValuesDifferent);
	PROTECTED_FIELD(mBox);
	PROTECTED_FIELD(mNameText);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(Actor*, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PUBLIC_FUNCTION(void, SetValue, Actor*);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PUBLIC_FUNCTION(bool, IsUnderPoint, const Vec2F&);
	PROTECTED_FUNCTION(void, OnCursorEnter, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorExit, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorPressed, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnKeyPressed, const Input::Key&);
	PROTECTED_FUNCTION(void, OnDropped, ISelectableDragableObjectsGroup*);
	PROTECTED_FUNCTION(void, OnDragEnter, ISelectableDragableObjectsGroup*);
	PROTECTED_FUNCTION(void, OnDragExit, ISelectableDragableObjectsGroup*);
	PROTECTED_FUNCTION(void, OnDroppedFromActorsTree, UIActorsTree*);
	PROTECTED_FUNCTION(void, OnDragEnterFromActorsTree, UIActorsTree*);
	PROTECTED_FUNCTION(void, OnDragExitFromActorsTree, UIActorsTree*);
	PROTECTED_FUNCTION(void, OnDroppedFromAssetsScroll, UIAssetsIconsScrollArea*);
	PROTECTED_FUNCTION(void, OnDragEnterFromAssetsScroll, UIAssetsIconsScrollArea*);
	PROTECTED_FUNCTION(void, OnDragExitFromAssetsScroll, UIAssetsIconsScrollArea*);
}
END_META;
 