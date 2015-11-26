#include "EventSystem.h"

#include "Events/ApplicationEventsListener.h"
#include "Events/CursorEventsListener.h"
#include "Events/DragEventsListener.h"
#include "Events/KeyboardEventsListener.h"
#include "Render/Render.h"
#include "Utils/Debug.h"

namespace o2
{
	DECLARE_SINGLETON(EventSystem);

	EventSystem::EventSystem()
	{}

	EventSystem::~EventSystem()
	{}

	void EventSystem::Update(float dt)
	{
		mCursorListeners.Sort([](const Ptr<CursorEventsListener>& a, const Ptr<CursorEventsListener>& b) { return a->Depth() > b->Depth(); });
		mDragListeners.Sort([](const Ptr<DragEventsListener>& a, const Ptr<DragEventsListener>& b) { return a->Depth() > b->Depth(); });

		mLastUnderCursorListeners = mUnderCursorListeners;
		mUnderCursorListeners.Clear();

		for (const Input::Cursor& cursor : o2Input.GetCursors())
			ProcessCursorTracing(cursor);

		ProcessCursorExit();
		ProcessCursorEnter();
		ProcessScrolling();

		for (const Input::Cursor& cursor : o2Input.GetCursors())
		{
			if (cursor.mPressedTime < FLT_EPSILON && cursor.mPressed)
				ProcessCursorPressed(cursor);
			else
				ProcessCursorDown(cursor);
		}

		for (const Input::Cursor& cursor : o2Input.GetReleasedCursors())
		{
			ProcessCursorReleased(cursor);
			mPressedListeners.Remove(cursor.mId);
		}

		for (const Input::Key& key : o2Input.GetPressedKeys())
		{
			if (key.mKey == -1)
				ProcessRBPressed();
			else if (key.mKey == -2)
				ProcessMBPressed();
			else
				ProcessKeyPressed(key);
		}

		for (const Input::Key& key : o2Input.GetDownKeys())
		{
			if (key.mKey == -1)
				ProcessRBDown();
			else if (key.mKey == -2)
				ProcessMBDown();
			else
				ProcessKeyDown(key);
		}

		for (const Input::Key& key : o2Input.GetReleasedKeys())
		{
			if (key.mKey == -1)
				ProcessRBReleased();
			else if (key.mKey == -2)
				ProcessMBReleased();
			else
				ProcessKeyReleased(key);
		}
	}

	void EventSystem::OnApplicationStarted()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationStarted();
	}

	void EventSystem::OnApplicationClosing()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationClosing();
	}

	void EventSystem::OnApplicationActivated()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationActivated();
	}

	void EventSystem::OnApplicationDeactivated()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationDeactivated();
	}

	void EventSystem::OnApplicationSized()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationSized();
	}

	void EventSystem::ProcessCursorTracing(const Input::Cursor& cursor)
	{
		for (auto listener : mCursorListeners)
		{
			if (!listener->mInteractable || IsListenerClipped(listener->Depth(), cursor.mPosition) || 
				!listener->IsUnderPoint(cursor.mPosition))
				continue;

			auto drag = dynamic_cast<DragEventsListener*>(listener.Get());
			if (drag && drag->IsDragging())
				continue;

			mUnderCursorListeners.Add(cursor.mId, listener);

			return;
		}
	}

	void EventSystem::ProcessCursorEnter()
	{
		for (auto underCursor : mUnderCursorListeners)
		{
			if (!underCursor.Value()->mInteractable)
				continue;

			if (!(mLastUnderCursorListeners.ContainsKey(underCursor.Key()) &&
				  mLastUnderCursorListeners[underCursor.Key()] == underCursor.Value()))
			{
				underCursor.Value()->OnCursorEnter(*o2Input.GetCursor(underCursor.Key()));
			}
		}
	}

	void EventSystem::ProcessCursorExit()
	{
		for (auto lastUnderCursor : mLastUnderCursorListeners)
		{
			if (!(mUnderCursorListeners.ContainsKey(lastUnderCursor.Key()) &&
				  mUnderCursorListeners[lastUnderCursor.Key()] == lastUnderCursor.Value()))
			{
				lastUnderCursor.Value()->OnCursorExit(*o2Input.GetCursor(lastUnderCursor.Key()));
			}
		}
	}

	Ptr<DragEventsListener> EventSystem::GetDragListenerUnderCursor(CursorId cursorId) const
	{
		Vec2F cursorPos = o2Input.GetCursorPos(cursorId);

		for (auto listener : mDragListeners)
		{
			if (listener->IsDragging())
				continue;

			if (!listener->mInteractable || IsListenerClipped(listener->Depth(), cursorPos) || 
				!listener->IsUnderPoint(cursorPos))
				continue;

			return listener;

			break;
		}

		return nullptr;
	}

	Ptr<CursorEventsListener> EventSystem::GetCursorListenerUnderCursor(CursorId cursorId) const
	{
		if (mUnderCursorListeners.ContainsKey(cursorId))
			return mUnderCursorListeners[cursorId];

		return nullptr;
	}

	EventSystem::CursEventsListenersVec EventSystem::GetAllCursorListenersUnderCursor(CursorId cursorId) const
	{
		CursEventsListenersVec res;
		Vec2F cursorPos = o2Input.GetCursorPos(cursorId);
		for (auto listener : mCursorListeners)
		{
			if (!listener->mInteractable || IsListenerClipped(listener->Depth(), cursorPos) || 
				!listener->IsUnderPoint(cursorPos))
				continue;

			res.Add(listener);
		}

		return res;
	}

	void EventSystem::BreakCursorEvent()
	{
		for (auto kv : mPressedListeners)
		{
			kv.Value()->OnCursorPressBreak(*o2Input.GetCursor(kv.Key()));
			kv.Value()->mIsPressed = false;
		}

		mPressedListeners.Clear();
	}

	void EventSystem::ProcessCursorPressed(const Input::Cursor& cursor)
	{
		if (!mUnderCursorListeners.ContainsKey(cursor.mId))
			return;

		auto listener = mUnderCursorListeners[cursor.mId];

		if (!listener->mInteractable)
			return;

		mPressedListeners.Add(cursor.mId, listener);

		listener->OnCursorPressed(cursor);
		listener->mIsPressed = true;
	}

	void EventSystem::ProcessCursorDown(const Input::Cursor& cursor)
	{
		if (mPressedListeners.ContainsKey(cursor.mId))
			mPressedListeners[cursor.mId]->OnCursorStillDown(cursor);

		if (cursor.mDelta.Length() > FLT_EPSILON)
		{
			if (mUnderCursorListeners.ContainsKey(cursor.mId))
				mUnderCursorListeners[cursor.mId]->OnCursorMoved(cursor);
		}
	}

	void EventSystem::ProcessCursorReleased(const Input::Cursor& cursor)
	{
		if (mPressedListeners.ContainsKey(cursor.mId))
		{
			mPressedListeners[cursor.mId]->OnCursorReleased(cursor);
			mPressedListeners[cursor.mId]->mIsPressed = false;
			mPressedListeners.Remove(cursor.mId);
		}
	}

	void EventSystem::ProcessRBPressed()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (!mUnderCursorListeners.ContainsKey(cursor.mId))
			return;

		auto listener = mUnderCursorListeners[cursor.mId];

		mRightButtonPressedListener = listener;

		listener->OnCursorRightMousePressed(cursor);
		listener->mIsRightMousePressed = true;
	}

	void EventSystem::ProcessRBDown()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (mRightButtonPressedListener)
			mRightButtonPressedListener->OnCursorRightMouseStayDown(cursor);
	}

	void EventSystem::ProcessRBReleased()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (mRightButtonPressedListener)
		{
			mRightButtonPressedListener->OnCursorRightMouseReleased(cursor);
			mRightButtonPressedListener->mIsRightMousePressed = false;
		}
	}

	void EventSystem::ProcessMBPressed()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (!mUnderCursorListeners.ContainsKey(cursor.mId))
			return;

		auto listener = mUnderCursorListeners[cursor.mId];

		mMiddleButtonPressedListener = listener;

		listener->OnCursorMiddleMousePressed(cursor);
		listener->mIsMiddleMousePressed = true;
	}

	void EventSystem::ProcessMBDown()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (mMiddleButtonPressedListener)
			mMiddleButtonPressedListener->OnCursorMiddleMouseStayDown(cursor);
	}

	void EventSystem::ProcessMBReleased()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (mMiddleButtonPressedListener)
		{
			mMiddleButtonPressedListener->OnCursorMiddleMouseReleased(cursor);
			mMiddleButtonPressedListener->mIsMiddleMousePressed = false;
		}
	}

	void EventSystem::ProcessScrolling()
	{
		float scroll = o2Input.GetMouseWheelDelta();
		if (!Math::Equals(scroll, 0.0f))
		{
			for (auto kv : mUnderCursorListeners)
				kv.Value()->OnScrolled(scroll);
		}
	}

	void EventSystem::ProcessKeyPressed(const Input::Key& key)
	{
		for (auto listener : mKeyboardListeners)
			listener->OnKeyPressed(key);
	}

	void EventSystem::ProcessKeyDown(const Input::Key& key)
	{
		for (auto listener : mKeyboardListeners)
			listener->OnKeyStayDown(key);
	}

	void EventSystem::ProcessKeyReleased(const Input::Key& key)
	{
		for (auto listener : mKeyboardListeners)
			listener->OnKeyReleased(key);
	}

	bool EventSystem::IsListenerClipped(float depth, const Vec2F& cursorPos) const
	{
		bool clipped = false;
		for (auto scissorInfo : o2Render.GetScissorInfos())
		{
			if (depth > scissorInfo.mBeginDepth && depth <= scissorInfo.mEndDepth &&
				!scissorInfo.mScissorRect.IsInside(cursorPos))
			{
				return true;
			}
		}

		return false;
	}

	void EventSystem::RegCursorListener(CursorEventsListener* listener)
	{
		if (!IsSingletonInitialzed())
			return;

		mInstance->mCursorListeners.Add(listener);
	}

	void EventSystem::UnregCursorListener(CursorEventsListener* listener)
	{
		mInstance->mCursorListeners.Remove(listener);
		mInstance->mPressedListeners.RemoveAll([&](auto x) { return x.Value() == listener; });

		if (mInstance->mRightButtonPressedListener == listener)
			mInstance->mRightButtonPressedListener = nullptr;

		if (mInstance->mMiddleButtonPressedListener == listener)
			mInstance->mMiddleButtonPressedListener = nullptr;

		mInstance->mUnderCursorListeners.RemoveAll([&](auto x) { return x.Value() == listener; });
		mInstance->mLastUnderCursorListeners.RemoveAll([&](auto x) { return x.Value() == listener; });
	}

	void EventSystem::RegDragListener(DragEventsListener* listener)
	{
		if (!IsSingletonInitialzed())
			return;

		mInstance->mDragListeners.Add(listener);
	}

	void EventSystem::UnregDragListener(DragEventsListener* listener)
	{
		mInstance->mDragListeners.Remove(listener);
	}

	void EventSystem::RegKeyboardListener(KeyboardEventsListener* listener)
	{
		if (!IsSingletonInitialzed())
			return;

		mInstance->mKeyboardListeners.Add(listener);
	}

	void EventSystem::UnregKeyboardListener(KeyboardEventsListener* listener)
	{
		mInstance->mKeyboardListeners.Remove(listener);
	}

	void EventSystem::RegApplicationListener(ApplicationEventsListener* listener)
	{
		if (!IsSingletonInitialzed())
			return;

		mInstance->mApplicationListeners.Add(listener);
	}

	void EventSystem::UnregApplicationListener(ApplicationEventsListener* listener)
	{
		mInstance->mApplicationListeners.Remove(listener);
	}
}