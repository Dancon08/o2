#include "stdafx.h"
#include "IPropertyField.h"

#include "Scene/UI/UIManager.h"
#include "Scene/UI/Widget.h"
#include "Scene/UI/WidgetLayout.h"
#include "Scene/UI/WidgetState.h"
#include "Scene/UI/Widgets/Button.h"
#include "Scene/UI/Widgets/Label.h"
#include "SceneWindow/SceneEditScreen.h"

namespace Editor
{
	IPropertyField::IPropertyField()
	{
		mRevertBtn = FindChildByType<Button>("revert");
		if (mRevertBtn)
			mRevertBtn->onClick = THIS_FUNC(Revert);

		mCaption = FindChildByType<Label>("caption");
	}

	IPropertyField::IPropertyField(const IPropertyField& other) :
		HorizontalLayout(other)
	{}

	IPropertyField& IPropertyField::operator=(const IPropertyField& other)
	{
		HorizontalLayout::operator=(other);
		return *this;
	}

	void IPropertyField::UpdateChildren(float dt)
	{
		auto prevChildrenRect = mChildrenWorldRect;

		if (mRemoveBtn)
			mChildrenWorldRect.right -= 20;

		HorizontalLayout::UpdateChildren(dt);
		mChildrenWorldRect = prevChildrenRect;
	}

	void IPropertyField::SetValueAndPrototypeProxy(const TargetsVec& targets)
	{
		mValuesProxies = targets;
		Refresh();
	}

	void IPropertyField::SetValueProxy(const Vector<IAbstractValueProxy*>& targets)
	{
		auto protoTargets = targets.Select<Pair<IAbstractValueProxy*, IAbstractValueProxy*>>(
			[](IAbstractValueProxy* x) { return Pair<IAbstractValueProxy*, IAbstractValueProxy*>(x, nullptr); });

		SetValueAndPrototypeProxy(protoTargets);
	}

	void IPropertyField::SetCaption(const WString& text)
	{
		if (mCaption)
			mCaption->text = text;

		auto captionWidget = dynamic_cast<Label*>(FindChild("propertyName"));
		if (captionWidget)
			captionWidget->text = text;
		else
		{
			auto captionLayer = GetLayerDrawable<Text>("caption");
			if (captionLayer)
				captionLayer->text = text;
		}
	}

	WString IPropertyField::GetCaption() const
	{
		if (mCaption)
			return mCaption->text;

		auto captionWidget = dynamic_cast<Label*>(FindChild("propertyName"));
		if (captionWidget)
			return captionWidget->text;
		else
		{
			auto captionLayer = GetLayerDrawable<Text>("caption");
			if (captionLayer)
				return captionLayer->text;
		}

		return "";
	}

	Button* IPropertyField::AddRemoveButton()
	{
		mRemoveBtn = o2UI.CreateWidget<Button>("remove small");
		*mRemoveBtn->layout = WidgetLayout::Based(BaseCorner::RightTop, Vec2F(20, 20), Vec2F(20, 0));
		AddInternalWidget(mRemoveBtn);

		return mRemoveBtn;
	}

	bool IPropertyField::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	void IPropertyField::SetValuePath(const String& path)
	{
		mValuesPath = path;
	}

	const String& IPropertyField::GetValuePath() const
	{
		return mValuesPath;
	}

	void IPropertyField::SetCaptionLabel(Label* label)
	{
		mCaption = label;
	}

	Label* IPropertyField::GetCaptionLabel() const
	{
		return mCaption;
	}

	void IPropertyField::SetRevertable(bool revertable)
	{
		mRevertable = revertable;
		CheckRevertableState();
	}

	bool IPropertyField::IsRevertable() const
	{
		return mRevertable;
	}

	void IPropertyField::SpecializeFieldInfo(const FieldInfo* fieldInfo)
	{
		SpecializeType(fieldInfo->GetType());
	}

	void IPropertyField::UpdateChildrenTransforms()
	{
		auto prevChildrenRect = mChildrenWorldRect;

		if (mRemoveBtn)
			mChildrenWorldRect.right -= 20;

		HorizontalLayout::UpdateChildrenTransforms();
		mChildrenWorldRect = prevChildrenRect;
	}

	void IPropertyField::CheckValueChangeCompleted()
	{
		Vector<DataNode> valuesData;
		StoreValues(valuesData);

		if (mBeforeChangeValues != valuesData)
			onChangeCompleted(mValuesPath, mBeforeChangeValues, valuesData);
	}

	bool IPropertyField::IsValueRevertable() const
	{
		return false;
	}

	void IPropertyField::CheckRevertableState()
	{
		if (auto revertState = state["revert"])
			*revertState = IsValueRevertable();
	}

	void IPropertyField::OnValueChanged()
	{
		CheckRevertableState();
		onChanged(this);
		o2EditorSceneScreen.OnSceneChanged();
	}

	void IPropertyField::BeginUserChanging()
	{
		StoreValues(mBeforeChangeValues);
	}

	void IPropertyField::EndUserChanging()
	{
		CheckValueChangeCompleted();
	}
}

DECLARE_CLASS(Editor::IPropertyField);
