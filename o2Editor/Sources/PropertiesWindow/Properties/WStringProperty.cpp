#include "WStringProperty.h"

#include "SceneWindow/SceneEditScreen.h"
#include "UI/EditBox.h"
#include "UI/UIManager.h"

namespace Editor
{
	WStringProperty::WStringProperty(UIWidget* widget /*= nullptr*/)
	{
		if (widget)
			mEditBox = dynamic_cast<UIEditBox*>(widget);
		else
			mEditBox = o2UI.CreateWidget<UIEditBox>("singleline");

		mEditBox->layout.minHeight = 10;
		mEditBox->onChangeCompleted = Function<void(const WString&)>(this, &WStringProperty::OnEdited);
	}

	WStringProperty::~WStringProperty()
	{
		delete mEditBox;
	}

	void WStringProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, const WString& value) { *((Property<WString>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<WString>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, const WString& value) { *((WString*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((WString*)(ptr)); };
		}

		mValuesPointers = targets;

		Update();
	}

	void WStringProperty::Update()
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
			mEditBox->text = "--";
			mCommonValue = "";
		}
		else mEditBox->text = mCommonValue;
	}

	UIWidget* WStringProperty::GetWidget() const
	{
		return mEditBox;
	}

	WString WStringProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool WStringProperty::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	void WStringProperty::SetValue(const WString& value)
	{
		mCommonValue = value;
		mValuesDifferent = false;
		mEditBox->text = value;

		o2EditorSceneScreen.OnSceneChanged();
	}

	void WStringProperty::SetUnknownValue()
	{
		mCommonValue = "";
		mValuesDifferent = true;
		mEditBox->text = "--";

		o2EditorSceneScreen.OnSceneChanged();
	}

	const Type* WStringProperty::GetFieldType() const
	{
		return &TypeOf(WString);
	}

	void WStringProperty::OnEdited(const WString& data)
	{
		if (mValuesDifferent && data == "--")
			return;

		mCommonValue = data;
		mValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();

		o2EditorSceneScreen.OnSceneChanged();
	}
}
 
CLASS_META(Editor::WStringProperty)
{
	BASE_CLASS(Editor::IPropertyField);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mValuesDifferent);
	PROTECTED_FIELD(mEditBox);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(WString, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(void, SetValue, const WString&);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PROTECTED_FUNCTION(void, OnEdited, const WString&);
}
END_META;
 