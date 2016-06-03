#include "CustomDropDown.h"

#include "Render/Render.h"
#include "UIManager.h"

namespace o2
{
	UICustomDropDown::UICustomDropDown():
		UIWidget(), DrawableCursorEventsListener(this)
	{
		mItemsList = mnew UICustomList();
		mItemsList->mParent = this;
		mItemsList->onSelectedItem += [&](auto x) { OnItemSelected(); };
		mItemsList->SetMultiselectionAvailable(false);

		InitializeProperties();
	}

	UICustomDropDown::UICustomDropDown(const UICustomDropDown& other):
		UIWidget(other), DrawableCursorEventsListener(this), mClipLayout(other.mClipLayout), mMaxListItems(other.mMaxListItems)
	{
		mItemsList = other.mItemsList->Clone();
		mItemsList->mParent = this;
		mItemsList->Hide(true);
		mItemsList->onSelectedItem += [&](auto x) { OnItemSelected(); };
		mItemsList->SetMultiselectionAvailable(false);

		RetargetStatesAnimations();
		InitializeProperties();
	}

	UICustomDropDown::~UICustomDropDown()
	{
		delete mItemsList;
	}

	UICustomDropDown& UICustomDropDown::operator=(const UICustomDropDown& other)
	{
		delete mItemsList;

		UIWidget::operator=(other);

		mItemsList = other.mItemsList->Clone();
		mItemsList->mParent = this;
		mItemsList->Hide(true);
		mItemsList->onSelectedItem += [&](auto x) { OnItemSelected(); };
		mItemsList->SetMultiselectionAvailable(false);

		mClipLayout = other.mClipLayout;
		mMaxListItems = other.mMaxListItems;

		RetargetStatesAnimations();
		return *this;
	}

	void UICustomDropDown::Update(float dt)
	{
		if (mFullyDisabled)
			return;

		UIWidget::Update(dt);
		mItemsList->Update(dt);

		if ((o2Input.IsCursorPressed() || Math::Abs(o2Input.GetMouseWheelDelta()) > 0.1f) &&
			!mItemsList->layout.mAbsoluteRect.IsInside(o2Input.GetCursorPos()))
		{
			Collapse();
		}
	}

	void UICustomDropDown::Draw()
	{
		if (mFullyDisabled)
			return;

		UIWidget::Draw();

		o2UI.DrawWidgetAtTop(mItemsList);

		UIWidget* selectedItem = mItemsList->GetItem(mItemsList->GetSelectedItemPos());
		if (selectedItem)
		{
			o2Render.EnableScissorTest(mAbsoluteClip);
			selectedItem->ForceDraw(mAbsoluteClip, mResTransparency);
			o2Render.DisableScissorTest();
		}

		if (UI_DEBUG || o2Input.IsKeyDown(VK_F1))
			DrawDebugFrame();
	}

	void UICustomDropDown::Expand()
	{
		float itemHeight = mItemsList->GetItemSample()->layout.height;
		int itemsVisible = Math::Min(mMaxListItems, mItemsList->GetItemsCount());
		RectF listViewArea = mItemsList->mAbsoluteViewArea;
		RectF listAbsRect = mItemsList->layout.mAbsoluteRect;
		RectF border(listViewArea.left - listAbsRect.left, listViewArea.bottom - listAbsRect.bottom,
					 listAbsRect.right - listViewArea.right, listAbsRect.top - listViewArea.top);
		mItemsList->layout.height = itemHeight*(float)itemsVisible + border.bottom + border.top;

		auto openedState = state["opened"];
		if (openedState)
			*openedState = true;

		mItemsList->SetVisible(true);
		UpdateLayout();
	}

	void UICustomDropDown::Collapse()
	{
		auto openedState = state["opened"];
		if (openedState)
			*openedState = false;

		mItemsList->SetVisible(false);
	}

	bool UICustomDropDown::IsExpanded() const
	{
		return mItemsList->IsVisible();
	}

	void UICustomDropDown::SetItemSample(UIWidget* sample)
	{
		mItemsList->SetItemSample(sample);
	}

	UIWidget* UICustomDropDown::GetItemSample() const
	{
		return mItemsList->GetItemSample();
	}

	UIVerticalLayout* UICustomDropDown::GetLayout() const
	{
		return mItemsList->GetLayout();
	}

	UIWidget* UICustomDropDown::AddItem()
	{
		return mItemsList->AddItem();
	}

	UIWidget* UICustomDropDown::AddItem(int position)
	{
		return mItemsList->AddItem(position);
	}

	void UICustomDropDown::RemoveItem(UIWidget* item)
	{
		mItemsList->RemoveItem(item);
	}

	void UICustomDropDown::RemoveItem(int position)
	{
		mItemsList->RemoveItem(position);
	}

	void UICustomDropDown::MoveItem(int position, int newPosition)
	{
		mItemsList->MoveItem(position, newPosition);
	}

	void UICustomDropDown::MoveItem(UIWidget* item, int newPosition)
	{
		mItemsList->MoveItem(item, newPosition);
	}

	int UICustomDropDown::GetItemPosition(UIWidget* item)
	{
		return mItemsList->GetItemPosition(item);
	}

	UIWidget* UICustomDropDown::GetItem(int position) const
	{
		return mItemsList->GetItem(position);
	}

	void UICustomDropDown::RemoveAllItems()
	{
		mItemsList->RemoveAllItems();
	}

	void UICustomDropDown::SortItems(const Function<bool(UIWidget*, UIWidget*)>& sortFunc)
	{
		mItemsList->SortItems(sortFunc);
	}

	int UICustomDropDown::GetItemsCount() const
	{
		return mItemsList->GetItemsCount();
	}

	void UICustomDropDown::SelectItem(UIWidget* item)
	{
		mItemsList->SelectItem(item);
	}

	void UICustomDropDown::SelectItemAt(int position)
	{
		mItemsList->SelectItemAt(position);
	}

	UIWidget* UICustomDropDown::GetSelectedItem() const
	{
		return mItemsList->GetSelectedItem();
	}

	int UICustomDropDown::GetSelectedItemPosition() const
	{
		return mItemsList->GetSelectedItemPos();
	}

	UICustomList* UICustomDropDown::GetListView() const
	{
		return mItemsList;
	}

	void UICustomDropDown::SetMaxListSizeInItems(int itemsCount)
	{
		mMaxListItems = itemsCount;
	}

	void UICustomDropDown::SetClippingLayout(const Layout& layout)
	{
		mClipLayout = layout;
		UpdateLayout();
	}

	Layout UICustomDropDown::GetClippingLayout()
	{
		return mClipLayout;
	}

	void UICustomDropDown::OnCursorPressed(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = true;
	}

	void UICustomDropDown::OnCursorReleased(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = false;

		if (UIWidget::IsUnderPoint(cursor.position))
		{
			if (IsExpanded())
				Collapse();
			else
				Expand();
		}
	}

	void UICustomDropDown::OnCursorPressBreak(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = false;
	}

	void UICustomDropDown::OnCursorEnter(const Input::Cursor& cursor)
	{
		auto selectState = state["select"];
		if (selectState)
			*selectState = true;
	}

	void UICustomDropDown::OnCursorExit(const Input::Cursor& cursor)
	{
		auto selectState = state["select"];
		if (selectState)
			*selectState = false;
	}

	void UICustomDropDown::OnVisibleChanged()
	{
		interactable = mResVisible;
	}

	void UICustomDropDown::UpdateLayout(bool forcible /*= false*/)
	{
		if (layout.mDrivenByParent && !forcible)
		{
			if (mParent)
				mParent->UpdateLayout();

			return;
		}

		RecalculateAbsRect();
		UpdateLayersLayouts();

		mChildsAbsRect = layout.mAbsoluteRect;
		mAbsoluteClip = mClipLayout.Calculate(layout.mAbsoluteRect);

		for (auto child : mChilds)
			child->UpdateLayout();

		mItemsList->UpdateLayout();
	}

	void UICustomDropDown::OnItemSelected()
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = false;

		Collapse();
		onSelectedPos(mItemsList->GetSelectedItemPos());
		onSelectedItem(mItemsList->GetSelectedItem());

		OnSelectionChanged();
	}

	void UICustomDropDown::OnSelectionChanged()
	{}

	void UICustomDropDown::InitializeProperties()
	{
		INITIALIZE_PROPERTY(UICustomDropDown, selectedItem, SelectItem, GetSelectedItem);
		INITIALIZE_PROPERTY(UICustomDropDown, selectedItemPos, SelectItemAt, GetSelectedItemPosition);
		INITIALIZE_ACCESSOR(UICustomDropDown, item, GetItem);
		INITIALIZE_GETTER(UICustomDropDown, itemsCount, GetItemsCount);
	}
}