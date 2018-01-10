#include "stdafx.h"
#include "Toggle.h"

#include "Render/Sprite.h"
#include "Render/Text.h"
#include "UI/EditBox.h"
#include "UI/UIManager.h"
#include "UI/WidgetLayer.h"
#include "UI/WidgetLayout.h"
#include "UI/WidgetState.h"

namespace o2
{
	UIToggle::UIToggle():
		UIWidget(), DrawableCursorEventsListener(this)
	{
		InitializeProperties();
	}

	UIToggle::UIToggle(const UIToggle& other):
		UIWidget(other), DrawableCursorEventsListener(this), mToggleGroup(nullptr)
	{
		mCaptionText = GetLayerDrawable<Text>("caption");
		mBackLayer = FindLayer("back");

		RetargetStatesAnimations();

		mValue = !other.mValue;
		SetValue(other.mValue);

		if (other.IsValueUnknown())
			SetValueUnknown();

		InitializeProperties();
	}

	UIToggle& UIToggle::operator=(const UIToggle& other)
	{
		CopyData(other);
		return *this;
	}

	UIToggle::~UIToggle()
	{
		if (mToggleGroup && mToggleGroup->mOwner == this)
			delete mToggleGroup;
	}

	void UIToggle::Update(float dt)
	{
		UIWidget::Update(dt);

		if (mFullyDisabled || mIsClipped)
			return;

		if (mToggleGroup && mToggleGroup->mPressed && mToggleGroup->mPressedValue != mValue &&
			(mToggleGroup->mType == UIToggleGroup::Type::VerOneClick || mToggleGroup->mType == UIToggleGroup::Type::HorOneClick))
		{
			Vec2F cursor = o2Input.GetCursorPos();
			bool underPoint = false;

			if (mToggleGroup->mType == UIToggleGroup::Type::VerOneClick)
				underPoint = cursor.y > layout->worldBottom && cursor.y < layout->worldTop;
			else
				underPoint = cursor.x > layout->worldLeft && cursor.x < layout->worldRight;
		
			if (underPoint)
			{
				SetValue(!mValue);
				onClick();
				onToggleByUser(mValue);
				mToggleGroup->mToggled.Add(this);
			}
		}
	}

	void UIToggle::SetCaption(const WString& text)
	{
		if (mCaptionText)
			mCaptionText->SetText(text);
	}

	WString UIToggle::GetCaption() const
	{
		if (mCaptionText)
			return mCaptionText->GetText();

		return WString();
	}

	void UIToggle::SetValue(bool value)
	{
		if (mValue == value && !mValueUnknown)
			return;

		mValue = value;
		mValueUnknown = false;

		auto valueState = state["value"];
		if (valueState)
			*valueState = mValue;

		auto unknownState = state["unknown"];
		if (unknownState)
			*unknownState = false;

		onToggle(mValue);

		if (mToggleGroup)
			mToggleGroup->OnToggled(this);
	}

	void UIToggle::SetValueUnknown()
	{
		mValueUnknown = true;

		auto unknownState = state["unknown"];
		if (unknownState)
			*unknownState = true;

		auto valueState = state["value"];
		if (valueState)
			*valueState = false;
	}

	bool UIToggle::IsValueUnknown() const
	{
		return mValueUnknown;
	}

	bool UIToggle::GetValue() const
	{
		return mValue;
	}

	bool UIToggle::IsFocusable() const
	{
		return true;
	}

	void UIToggle::SetToggleGroup(UIToggleGroup* toggleGroup)
	{
		if (mToggleGroup == toggleGroup)
			return;

		if (mToggleGroup)
		{
			if (mToggleGroup->mOwner == this)
			{
				if (mToggleGroup->mToggles.Count() == 1)
				{
					mToggleGroup->mToggles.Clear();
					delete mToggleGroup;
				}
				else
				{
					mToggleGroup->mToggles.Remove(this);
					mToggleGroup->mOwner = mToggleGroup->mToggles[0];
				}
			}
			else mToggleGroup->mToggles.Remove(this);
		}

		mToggleGroup = toggleGroup;
		mToggleGroup->mToggles.Add(this);

		if (!mToggleGroup->mOwner)
		{
			mToggleGroup->mOwner = this;

			if (mToggleGroup->mType == UIToggleGroup::Type::OnlySingleTrue)
				SetValue(true);
		}
	}

	UIToggleGroup* UIToggle::GetToggleGroup() const
	{
		return mToggleGroup;
	}

	void UIToggle::OnCursorPressed(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = true;

		o2UI.FocusWidget(this);

		if (mToggleGroup)
		{
			SetValue(!mValue);
			onClick();

			mToggleGroup->mPressed = true;
			mToggleGroup->mPressedValue = mValue;
			mToggleGroup->mToggled.Clear();
			mToggleGroup->mToggled.Add(this);
			mToggleGroup->onPressed(mValue);
		}
	}

	void UIToggle::OnCursorReleased(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = false;

		if (UIWidget::IsUnderPoint(cursor.position) && 
			!(mToggleGroup && (mToggleGroup->mType == UIToggleGroup::Type::VerOneClick || 
							   mToggleGroup->mType == UIToggleGroup::Type::HorOneClick) && 
			  mToggleGroup->mPressed))
		{
			SetValue(!mValue);
			onClick();
			onToggleByUser(mValue);
		}

		if (mToggleGroup && (mToggleGroup->mType == UIToggleGroup::Type::VerOneClick ||
							 mToggleGroup->mType == UIToggleGroup::Type::HorOneClick))
		{
			mToggleGroup->mPressed = false;
			mToggleGroup->onReleased(mValue);
		}
	}

	void UIToggle::OnCursorPressBreak(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = false;

		if (mToggleGroup && (mToggleGroup->mType == UIToggleGroup::Type::VerOneClick ||
							 mToggleGroup->mType == UIToggleGroup::Type::HorOneClick))
		{
			mToggleGroup->mPressed = false;
			mToggleGroup->onReleased(mValue);
		}
	}

	void UIToggle::OnCursorEnter(const Input::Cursor& cursor)
	{
		auto selectState = state["hover"];
		if (selectState)
			*selectState = true;
	}

	void UIToggle::OnCursorExit(const Input::Cursor& cursor)
	{
		auto selectState = state["hover"];
		if (selectState)
			*selectState = false;
	}

	void UIToggle::OnKeyPressed(const Input::Key& key)
	{
		if (mIsFocused && (key.keyCode == VK_SPACE || key.keyCode == VK_RETURN))
		{
			auto pressedState = state["pressed"];
			if (pressedState)
				*pressedState = true;
		}

		bool isFocusedEditBox = o2UI.GetFocusedWidget() && o2UI.GetFocusedWidget()->GetType() == TypeOf(UIEditBox);
		if (shortcut.IsPressed() && !isFocusedEditBox)
		{
			SetValue(!mValue);
			onClick();
			onToggleByUser(mValue);
		}
	}

	void UIToggle::OnKeyReleased(const Input::Key& key)
	{
		if (mIsFocused && (key.keyCode == VK_SPACE || key.keyCode == VK_RETURN))
		{
			auto pressedState = state["pressed"];
			if (pressedState)
				*pressedState = false;

			SetValue(!mValue);
			onClick();
			onToggleByUser(mValue);
		}

	}

	void UIToggle::OnDeserialized(const DataNode& node)
	{
		UIWidget::OnDeserialized(node);

		mCaptionText = GetLayerDrawable<Text>("caption");
		mBackLayer = FindLayer("back");

		RetargetStatesAnimations();
	}

	void UIToggle::CopyData(const Actor& otherActor)
	{
		const UIToggle& other = dynamic_cast<const UIToggle&>(otherActor);

		UIWidget::CopyData(other);

		mCaptionText = GetLayerDrawable<Text>("caption");
		mBackLayer = FindLayer("back");

		mValue = !other.mValue;
		SetValue(other.mValue);

		if (other.IsValueUnknown())
			SetValueUnknown();

		RetargetStatesAnimations();
	}

	void UIToggle::OnLayerAdded(UIWidgetLayer* layer)
	{
		if (layer->name == "caption" && layer->drawable && layer->drawable->GetType() == TypeOf(Text))
			mCaptionText = (Text*)layer->drawable;

		if (layer->name == "back")
			mBackLayer = layer;
	}

	void UIToggle::OnVisibleChanged()
	{
		interactable = mResVisible;
	}

	void UIToggle::InitializeProperties()
	{
		INITIALIZE_PROPERTY(UIToggle, caption, SetCaption, GetCaption);
		INITIALIZE_PROPERTY(UIToggle, value, SetValue, GetValue);
		INITIALIZE_PROPERTY(UIToggle, toggleGroup, SetToggleGroup, GetToggleGroup);
	}

	UIToggleGroup::UIToggleGroup(Type type):
		mType(type), mPressed(false)
	{}

	UIToggleGroup::~UIToggleGroup()
	{
		for (auto toggle : mToggles)
			toggle->mToggleGroup = nullptr;
	}

	void UIToggleGroup::AddToggle(UIToggle* toggle)
	{
		mToggles.Add(toggle);
		toggle->mToggleGroup = this;
		toggle->SetValue(true);
	}

	void UIToggleGroup::RemoveToggle(UIToggle* toggle)
	{
		mToggles.Remove(toggle);
		toggle->mToggleGroup = nullptr;
	}

	const UIToggleGroup::TogglesVec& UIToggleGroup::GetToggles() const
	{
		return mToggles;
	}

	const UIToggleGroup::TogglesVec& UIToggleGroup::GetToggled() const
	{
		return mToggled;
	}

	void UIToggleGroup::OnToggled(UIToggle* toggle)
	{
		if (mType == Type::OnlySingleTrue)
		{
			if (toggle->GetValue())
			{
				for (auto ctoggle : mToggles)
				{
					if (ctoggle == toggle)
						continue;

					ctoggle->SetValue(false);
				}
			}
			else
			{
				if (!mToggles.Any([&](auto x) { return x->GetValue(); }))
					toggle->SetValue(true);
			}
		}
	}
}

DECLARE_CLASS(o2::UIToggle);

ENUM_META_(o2::UIToggleGroup::Type, Type)
{
	ENUM_ENTRY(HorOneClick);
	ENUM_ENTRY(OnlySingleTrue);
	ENUM_ENTRY(VerOneClick);
}
END_ENUM_META;
