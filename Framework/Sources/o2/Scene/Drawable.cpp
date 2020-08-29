#include "o2/stdafx.h"
#include "Drawable.h"

#include "o2/Scene/Actor.h"
#include "o2/Scene/Scene.h"
#include "o2/Scene/SceneLayer.h"

namespace o2
{
	SceneDrawable::SceneDrawable()
	{}

	SceneDrawable::SceneDrawable(const SceneDrawable& other):
		mDrawingDepth(other.mDrawingDepth), drawDepth(this)
	{}

	SceneDrawable::~SceneDrawable()
	{
		if (mLayer)
			mLayer->UnregisterDrawable(this);
	}

	SceneDrawable& SceneDrawable::operator=(const SceneDrawable& other)
	{
		mDrawingDepth = other.mDrawingDepth;

		if (mLayer)
			mLayer->OnDrawableDepthChanged(this);

		return *this;
	}

	void SceneDrawable::Draw()
	{
		OnDrawn();
	}

	void SceneDrawable::SetDrawingDepth(float depth)
	{
		mDrawingDepth = depth;

		if (mLayer)
			mLayer->OnDrawableDepthChanged(this);
	}

	float SceneDrawable::GetDrawingDepth() const
	{
		return mDrawingDepth;
	}

	void SceneDrawable::SetLastOnCurrentDepth()
	{
		if (mLayer && mIsOnScene)
			mLayer->SetLastByDepth(this);
	}

	void SceneDrawable::SetLayer(SceneLayer* newLayer)
	{
		if (newLayer == mLayer)
			return;

		if (mLayer && mIsOnScene)
			mLayer->UnregisterDrawable(this);

		mLayer = newLayer;

		if (mLayer && mIsOnScene)
			mLayer->RegisterDrawable(this);
	}

	void SceneDrawable::OnRemoveFromScene()
	{
		if (mLayer)
			mLayer->UnregisterDrawable(this);

		mIsOnScene = false;
	}

	void SceneDrawable::OnAddToScene()
	{
		if (mLayer)
			mLayer->RegisterDrawable(this);

		mIsOnScene = true;
	}

#if IS_EDITOR
	SceneEditableObject* SceneDrawable::GetEditableOwner()
	{
		return nullptr;
	}

	void SceneDrawable::OnDrawn()
	{
		if (auto obj = GetEditableOwner())
			o2Scene.OnObjectDrawn(obj);

		IDrawable::OnDrawn();
	}
#endif

}

DECLARE_CLASS(o2::SceneDrawable);
