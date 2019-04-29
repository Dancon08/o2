#pragma once

#include "AnimationWindow/Timeline.h"
#include "AnimationWindow/TrackControls/ITrackControl.h"
#include "Scene/UI/Widget.h"
#include "Utils/Editor/DragHandle.h"

using namespace o2;

namespace Editor
{
	// -------------------------------------------------
	// Animation control track for key frames animations
	// Creates handles for each keys and updates them
	// -------------------------------------------------
	template<typename AnimatedValueType>
	class KeyFramesTrackControl: public ITrackControl
	{
	public:
		// Default constructor
		KeyFramesTrackControl();

		// Copy-constructor
		KeyFramesTrackControl(const KeyFramesTrackControl& other);

		// Destructor
		~KeyFramesTrackControl();


		// Copy-operator
		KeyFramesTrackControl& operator=(const KeyFramesTrackControl& other);


		// Draws handles with scissor rect
		void Draw() override;


		// Sets animated value, updates and creates key handles
		void SetAnimatedValue(IAnimatedValue* animatedValue) override;

		// Returns animated value
		AnimatedValueType* GetAnimatedValue() const;


		// Sets timeline for calculating handles positions
		void SetTimeline(AnimationTimeline* timeline) override;


		// Updates handles position on timeline
		void UpdateHandles();


		// Returns key handles list
		KeyHandlesVec GetKeyHandles() const override;

		// Returns key handle position
		float GetKeyPosition(int idx) const override;

		SERIALIZABLE(KeyFramesTrackControl<AnimatedValueType>);

	private:
		KeyHandlesVec      mHandles;                 // List of handles, each for keys
		AnimatedValueType* mAnimatedValue = nullptr; // Editing animated value
		AnimationTimeline* mTimeline = nullptr;      // Timeline used for calculating handles positions

	private:
		void InitializeHandles();

		WidgetDragHandle* CreateHandle();

		void ChangeHandleIndex(int oldIndex, int newIndex);
	};

	template<typename AnimatedValueType>
	KeyFramesTrackControl<AnimatedValueType>::KeyFramesTrackControl():
		ITrackControl()
	{}

	template<typename AnimatedValueType>
	KeyFramesTrackControl<AnimatedValueType>::KeyFramesTrackControl(const KeyFramesTrackControl& other) :
		ITrackControl(other)
	{}

	template<typename AnimatedValueType>
	KeyFramesTrackControl<AnimatedValueType>::~KeyFramesTrackControl()
	{
		for (auto handle : mHandles)
			delete handle;
	}

	template<typename AnimatedValueType>
	KeyFramesTrackControl<AnimatedValueType>& KeyFramesTrackControl<AnimatedValueType>::operator=(const KeyFramesTrackControl& other)
	{
		Widget::operator=(other);
		return *this;
	}

	template<typename AnimatedValueType>
	void KeyFramesTrackControl<AnimatedValueType>::Draw()
	{
		DrawDebugFrame();

		if (!mResEnabledInHierarchy)
			return;

		OnDrawn();

		o2Render.EnableScissorTest(mTimeline->layout->GetWorldRect());

		for (auto child : mDrawingChildren)
			child->Draw();

		o2Render.DisableScissorTest();

		DrawDebugFrame();
	}

	template<typename AnimatedValueType>
	void KeyFramesTrackControl<AnimatedValueType>::SetAnimatedValue(IAnimatedValue* animatedValue)
	{
		if (mAnimatedValue)
			mAnimatedValue->onKeysChanged -= THIS_FUNC(UpdateHandles);

		mAnimatedValue = dynamic_cast<AnimatedValueType*>(animatedValue);

		if (mAnimatedValue)
			mAnimatedValue->onKeysChanged += THIS_FUNC(UpdateHandles);

		InitializeHandles();
	}

	template<typename AnimatedValueType>
	AnimatedValueType* KeyFramesTrackControl<AnimatedValueType>::GetAnimatedValue() const
	{
		return mAnimatedValue;
	}

	template<typename AnimatedValueType>
	void KeyFramesTrackControl<AnimatedValueType>::SetTimeline(AnimationTimeline* timeline)
	{
		mTimeline = timeline;
	}

	template<typename AnimatedValueType>
	ITrackControl::KeyHandlesVec KeyFramesTrackControl<AnimatedValueType>::GetKeyHandles() const
	{
		return mHandles;
	}

	template<typename AnimatedValueType>
	float KeyFramesTrackControl<AnimatedValueType>::GetKeyPosition(int idx) const
	{
		return mHandles[idx]->handle->GetPosition().x;
	}

	template<typename AnimatedValueType>
	void KeyFramesTrackControl<AnimatedValueType>::InitializeHandles()
	{
		Vector<WidgetDragHandle*> handles = mHandles.Select<WidgetDragHandle*>(
			[](const KeyHandle* x) { return x->handle; });

		for (auto keyHandle : mHandles)
			delete keyHandle;

		mHandles.Clear();

		int idx = 0;
		for (auto& key : mAnimatedValue->GetKeys())
		{
			WidgetDragHandle* handle = nullptr;

			if (!handles.IsEmpty())
				handle = handles.PopBack();
			else
				handle = CreateHandle();

			handle->SetPosition(Vec2F(key.position, 0.0f));
			AddChild(handle);

			KeyHandle* keyhandle = mnew KeyHandle(idx++, handle);
			mHandles.Add(keyhandle);

			handle->onChangedPos = [=](const Vec2F& pos) {
				auto key = mAnimatedValue->GetKeys()[keyhandle->keyIdx];

				key.position = pos.x;
				mAnimatedValue->RemoveKeyAt(keyhandle->keyIdx);
				auto newIdx = mAnimatedValue->AddKey(key);

				if (newIdx != keyhandle->keyIdx) {
					ChangeHandleIndex(keyhandle->keyIdx, newIdx);
					keyhandle->keyIdx = newIdx;
				}
			};

		}
	}


	template<typename AnimatedValueType>
	void KeyFramesTrackControl<AnimatedValueType>::ChangeHandleIndex(int oldIndex, int newIndex)
	{
		KeyHandle* editingHandle = mHandles[oldIndex];
		mHandles.RemoveAt(oldIndex);
		mHandles.Insert(editingHandle, newIndex);

		for (int i = 0; i < mHandles.Count(); i++)
			mHandles[i]->keyIdx = i;
	}

	template<typename AnimatedValueType>
	void KeyFramesTrackControl<AnimatedValueType>::UpdateHandles()
	{
		if (!mAnimatedValue)
			return;

		int idx = 0;
		for (auto& key : mAnimatedValue->GetKeys())
		{			
			auto keyHandle = mHandles[idx++];
			keyHandle->handle->SetPosition(Vec2F(key.position, 0.0f));
		}
	}


	template<typename AnimatedValueType>
	WidgetDragHandle* KeyFramesTrackControl<AnimatedValueType>::CreateHandle()
	{
		WidgetDragHandle* handle = mnew WidgetDragHandle(mnew Sprite("ui/UI4_key.png"), 
														 mnew Sprite("ui/UI4_key_hover.png"),
														 mnew Sprite("ui/UI4_key_pressed.png"),
														 mnew Sprite("ui/UI4_selected_key.png"),
														 mnew Sprite("ui/UI4_selected_key_hover.png"),
														 mnew Sprite("ui/UI4_selected_key_pressed.png"));

		handle->cursorType = CursorType::SizeWE;
		handle->SetSpritesSizePivot(Vec2F(7, 1));
		handle->SetSelectionGroup(mTimeline);

		handle->checkPositionFunc = [&](const Vec2F& pos) { 
			float position = pos.x;
			if (position < 0.0f)
				position = 0.0f;

			return Vec2F(position, layout->GetHeight()*0.5f);
		};

		handle->localToWidgetOffsetTransformFunc = [&](const Vec2F& pos) {
			float worldXPos = mTimeline->LocalToWorld(pos.x);
			float localXPos = worldXPos - layout->GetWorldLeft();

			return Vec2F(localXPos, 0);
		};

		handle->widgetOffsetToLocalTransformFunc = [&](const Vec2F& pos) {
			float worldXPos = layout->GetWorldLeft() + pos.x; 
			float localXPos = mTimeline->WorldToLocal(worldXPos);

			return Vec2F(localXPos, 0);
		};

		return handle;
	}
}

META_TEMPLATES(typename AnimatedValueType)
CLASS_BASES_META(Editor::KeyFramesTrackControl<AnimatedValueType>)
{
	BASE_CLASS(Editor::ITrackControl);
}
END_META;
META_TEMPLATES(typename AnimatedValueType)
CLASS_FIELDS_META(Editor::KeyFramesTrackControl<AnimatedValueType>)
{
	PRIVATE_FIELD(mHandles);
	PRIVATE_FIELD(mAnimatedValue);
	PRIVATE_FIELD(mTimeline);
}
END_META;
META_TEMPLATES(typename AnimatedValueType)
CLASS_METHODS_META(Editor::KeyFramesTrackControl<AnimatedValueType>)
{

	PUBLIC_FUNCTION(void, Draw);
	PUBLIC_FUNCTION(void, SetAnimatedValue, IAnimatedValue*);
	PUBLIC_FUNCTION(AnimatedValueType*, GetAnimatedValue);
	PUBLIC_FUNCTION(void, SetTimeline, AnimationTimeline*);
	PUBLIC_FUNCTION(void, UpdateHandles);
	PUBLIC_FUNCTION(KeyHandlesVec, GetKeyHandles);
	PUBLIC_FUNCTION(float, GetKeyPosition, int);
	PRIVATE_FUNCTION(void, InitializeHandles);
	PRIVATE_FUNCTION(WidgetDragHandle*, CreateHandle);
	PRIVATE_FUNCTION(void, ChangeHandleIndex, int, int);
}
END_META;