#include "IActorTransformViewer.h"

#include "Scene/Actor.h"
#include "UI/Widget.h"

namespace Editor
{
	IActorTransformViewer::IActorTransformViewer()
	{
		mDataView = mnew UIWidget();
		mDataView->name = "actor head";
	}

	IActorTransformViewer::~IActorTransformViewer()
	{
		delete mDataView;
	}

	UIWidget* IActorTransformViewer::GetWidget() const
	{
		return mDataView;
	}

	void IActorTransformViewer::Expand()
	{

	}

	void IActorTransformViewer::Collapse()
	{

	}
}
 
CLASS_META(Editor::IActorTransformViewer)
{
	BASE_CLASS(o2::IObject);

	PROTECTED_FIELD(mDataView);

	PUBLIC_FUNCTION(void, SetTargetActors, const Vector<Actor*>&);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(void, Expand);
	PUBLIC_FUNCTION(void, Collapse);
}
END_META;
 