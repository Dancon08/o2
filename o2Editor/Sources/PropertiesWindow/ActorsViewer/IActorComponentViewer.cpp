#include "stdafx.h"
#include "IActorComponentViewer.h"

#include "Core/EditorScope.h"
#include "Core/Properties/Properties.h"
#include "Core/UI/SpoilerWithHead.h"
#include "Scene/UI/UIManager.h"
#include "Scene/UI/Widget.h"
#include "Scene/UI/Widgets/Button.h"
#include "Scene/UI/Widgets/Image.h"

namespace Editor
{
	IActorComponentViewer::IActorComponentViewer()
	{
		PushEditorScopeOnStack scope;

		mSpoiler = o2UI.CreateWidget<SpoilerWithHead>();

		mSpoiler->expandHeight = false;
		mSpoiler->expandWidth = true;
		mSpoiler->fitByChildren = true;
		mSpoiler->borderBottom = 5;
		mSpoiler->borderRight = 5;
		mSpoiler->SetCaption("COmponent");
		mSpoiler->GetIcon()->SetImageName("ui/UI4_component_icon.png");
		mSpoiler->GetIcon()->layout->center -= Vec2F(2, 0);
		mSpoiler->GetIcon()->GetImage()->SetColor(Color4(235, 255, 253));

		mRemoveButton = o2UI.CreateButton("", THIS_FUNC(RemoveTargetComponents), "close");
		*mRemoveButton->layout = WidgetLayout::Based(BaseCorner::RightTop, Vec2F(20, 20), Vec2F(1, 0));
		mSpoiler->AddInternalWidget(mRemoveButton);

		mSpoiler->SetExpanded(true);
	}

	IActorComponentViewer::~IActorComponentViewer()
	{
		if (mSpoiler)
			delete mSpoiler;
	}

	void IActorComponentViewer::SetTargetComponents(const Vector<Component*>& components)
	{
		mTargetComponents = components;
		mBuiltWithHidden = o2EditorProperties.IsPrivateFieldsVisible();

		if (!components.IsEmpty())
		{
			String caption = components[0]->GetName();
			if (caption.IsEmpty())
				caption = o2EditorProperties.MakeSmartFieldName(GetComponentType()->GetName());

			mSpoiler->SetCaption(caption);
			mSpoiler->GetIcon()->SetImageName(components[0]->GetIcon());
		}
	}

	Widget* IActorComponentViewer::GetWidget() const
	{
		return mSpoiler;
	}

	void IActorComponentViewer::Expand()
	{
		mSpoiler->Expand();
	}

	void IActorComponentViewer::Collapse()
	{
		mSpoiler->Collapse();
	}

	void IActorComponentViewer::Refresh()
	{}

	void IActorComponentViewer::Rebuild()
	{}

	bool IActorComponentViewer::IsBuiltWithEmpty() const
	{
		return mBuiltWithHidden;
	}

	void IActorComponentViewer::RemoveTargetComponents()
	{
		for (auto comp : mTargetComponents)
			delete comp;

		mTargetComponents.Clear();

	}

}

DECLARE_CLASS(Editor::IActorComponentViewer);
