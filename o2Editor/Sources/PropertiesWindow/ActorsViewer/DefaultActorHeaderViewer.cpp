#include "DefaultActorHeaderViewer.h"

#include "PropertiesWindow/Properties/AssetProperty.h"
#include "PropertiesWindow/Properties/BooleanProperty.h"
#include "PropertiesWindow/Properties/LayerProperty.h"
#include "PropertiesWindow/Properties/StringProperty.h"
#include "PropertiesWindow/Properties/TagProperty.h"
#include "UI/Button.h"
#include "UI/EditBox.h"
#include "UI/Image.h"
#include "UI/Toggle.h"
#include "UI/Widget.h"

namespace Editor
{
	DefaultActorHeaderViewer::DefaultActorHeaderViewer()
	{
		mDataView = mnew UIWidget();
		mDataView->layout.minHeight = 60;

		mEnableProperty = mnew BooleanProperty(o2UI.CreateWidget<UIToggle>("actorHeadEnable"));
		mEnableProperty->GetWidget()->layout = UIWidgetLayout::Based(BaseCorner::LeftTop, Vec2F(20, 20), Vec2F(-4, 5));
		mDataView->AddChild(mEnableProperty->GetWidget());

		mNameProperty = mnew StringProperty(o2UI.CreateWidget<UIEditBox>("actorHeadName"));
		mNameProperty->GetWidget()->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 16, 13, 17, -3);
		mDataView->AddChild(mNameProperty->GetWidget());

		mLockProperty = mnew BooleanProperty(o2UI.CreateWidget<UIToggle>("actorHeadLock"));
		mLockProperty->GetWidget()->layout = UIWidgetLayout::Based(BaseCorner::RightTop, Vec2F(20, 20), Vec2F(6, 4));
		mDataView->AddChild(mLockProperty->GetWidget());

		auto linkImg = o2UI.CreateImage("ui/UI2_prefab_link_big.png");
		linkImg->layout = UIWidgetLayout::Based(BaseCorner::LeftTop, Vec2F(20, 20), Vec2F(-4, -15));
		mDataView->AddChild(linkImg);

		mPrototypeProperty = mnew AssetProperty<ActorAsset>(o2UI.CreateWidget<UIWidget>("actorHeadAssetProperty"));
		mPrototypeProperty->GetWidget()->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 16, 33, 17, 17);
		mDataView->AddChild(mPrototypeProperty->GetWidget());

		mPrototypeApplyBtn = o2UI.CreateWidget<UIButton>("acceptPrototype");
		mPrototypeApplyBtn->layout = UIWidgetLayout::Based(BaseCorner::RightTop, Vec2F(25, 25), Vec2F(-34, -24));
		mDataView->AddChild(mPrototypeApplyBtn);

		mPrototypeRevertBtn = o2UI.CreateWidget<UIButton>("revertPrototype");
		mPrototypeRevertBtn->layout = UIWidgetLayout::Based(BaseCorner::RightTop, Vec2F(25, 25), Vec2F(-14, -24));
		mDataView->AddChild(mPrototypeRevertBtn);

		mPrototypeBreakBtn = o2UI.CreateWidget<UIButton>("breakPrototype");
		mPrototypeBreakBtn->layout = UIWidgetLayout::Based(BaseCorner::RightTop, Vec2F(25, 25), Vec2F(6, -24));
		mDataView->AddChild(mPrototypeBreakBtn);
	}

	DefaultActorHeaderViewer::~DefaultActorHeaderViewer()
	{
		delete mEnableProperty;
		delete mDataView;
	}

	void DefaultActorHeaderViewer::SetTargetActors(const Vector<Actor*>& actors)
	{
		Vector<void*> enableTargets = actors.Select<void*>([](Actor* x) { return &x->enabled; });
		mEnableProperty->Setup(enableTargets, true);

		Vector<void*> nameTargets = actors.Select<void*>([](Actor* x) { return &x->name; });
		mNameProperty->Setup(nameTargets, true);

		Vector<void*> lockTargets = actors.Select<void*>([](Actor* x) { return &x->locked; });
		mLockProperty->Setup(lockTargets, true);

// 		Vector<void*> prototypeTargets = actors.Select<void*>([](Actor* x) { return &x->locked; });
// 		mPrototypeProperty->Setup(lockTargets, true);
	}

	UIWidget* DefaultActorHeaderViewer::GetWidget() const
	{
		return mDataView;
	}
}
 
CLASS_META(Editor::DefaultActorHeaderViewer)
{
	BASE_CLASS(Editor::IActorHeaderViewer);

	PUBLIC_FIELD(mDataView);
	PUBLIC_FIELD(mEnableProperty);
	PUBLIC_FIELD(mNameProperty);
	PUBLIC_FIELD(mLockProperty);
	PUBLIC_FIELD(mPrototypeProperty);
	PUBLIC_FIELD(mPrototypeApplyBtn);
	PUBLIC_FIELD(mPrototypeRevertBtn);
	PUBLIC_FIELD(mPrototypeBreakBtn);
	PUBLIC_FIELD(mTagsProperty);
	PUBLIC_FIELD(mLayerProperty);

	PUBLIC_FUNCTION(void, SetTargetActors, const Vector<Actor*>&);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
}
END_META;
 