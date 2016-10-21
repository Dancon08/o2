#include "LayerProperty.h"

#include "Animation/AnimatedFloat.h"
#include "Animation/AnimatedVector.h"
#include "Animation/Animation.h"
#include "Render/Sprite.h"
#include "UI/DropDown.h"
#include "UI/UIManager.h"

namespace Editor
{

	LayerProperty::LayerProperty(UIWidget* widget /*= nullptr*/)
	{
		if (widget)
			mDropDown = dynamic_cast<UIDropDown*>(widget);
		else
			mDropDown = o2UI.CreateWidget<UIDropDown>("editor property");

		mDropDown->onSelectedText = Function<void(const WString&)>(this, &LayerProperty::SelectLayer);
	}

	LayerProperty::~LayerProperty()
	{
		delete mDropDown;
	}

	void LayerProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, Scene::Layer* value) { *((Property<Scene::Layer*>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<Scene::Layer*>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, Scene::Layer* value) { *((Scene::Layer**)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((Scene::Layer**)(ptr)); };
		}

		mValuesPointers = targets;

		auto layers = o2Scene.GetLayers();
		auto dropdownLayers = mDropDown->GetAllItemsText();
		for (auto itemName : dropdownLayers)
		{
			if (!layers.ContainsPred([&](Scene::Layer* x) { return x->name == itemName; }))
				mDropDown->RemoveItem(itemName);
		}

		for (auto layer : layers)
		{
			if (!dropdownLayers.Contains(layer->name))
				mDropDown->AddItem(layer->name);
		}

		Update();
	}

	void LayerProperty::Update()
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

		mUpdatingValue = true;

		if (mValuesDifferent)
		{
			mDropDown->SelectItemAt(-1);
			mDropDown->SetState("undefined", true);
		}
		else
		{
			if (!mCommonValue)
			{
				mDropDown->SelectItemAt(-1);
				mDropDown->SetState("undefined", false);
			}
			else
			{
				mDropDown->value = mCommonValue->name;
				mDropDown->SetState("undefined", false);
			}
		}

		mUpdatingValue = false;
	}

	UIWidget* LayerProperty::GetWidget() const
	{
		return mDropDown;
	}

	Scene::Layer* LayerProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool LayerProperty::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	const Type* LayerProperty::GetFieldType() const
	{
		return &TypeOf(Scene::Layer*);
	}

	void LayerProperty::SelectLayer(const WString& name)
	{
		if (mUpdatingValue)
			return;

		mCommonValue = o2Scene.GetLayer(name);
		mValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		Update();
	}

}
 
CLASS_META(Editor::LayerProperty)
{
	BASE_CLASS(Editor::IPropertyField);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mValuesDifferent);
	PROTECTED_FIELD(mDropDown);
	PROTECTED_FIELD(mUpdatingValue);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(Scene::Layer*, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PROTECTED_FUNCTION(void, SelectLayer, const WString&);
}
END_META;
 