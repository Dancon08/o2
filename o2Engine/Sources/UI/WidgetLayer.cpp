#include "WidgetLayer.h"

#include "UI/Widget.h"

namespace o2
{
	UIWidgetLayer::UIWidgetLayer():
		mDepth(0.0f), name((String)Math::Random<UInt>(0, UINT_MAX)), 
		interactableLayout(Vec2F(), Vec2F(1.0f, 1.0f), Vec2F(), Vec2F()), drawable(nullptr)
	{
		InitializeProperties();
	}

	UIWidgetLayer::UIWidgetLayer(const UIWidgetLayer& other):
		mDepth(other.mDepth), name(other.name), layout(other.layout), mTransparency(other.mTransparency), 
		mResTransparency(1.0f), interactableLayout(other.interactableLayout), mParent(nullptr), mOwnerWidget(nullptr),
		drawable(nullptr)
	{
		if (other.drawable)
			drawable = other.drawable->Clone();

		for (auto child : other.mChilds)
			AddChild(child->Clone());

		InitializeProperties();
	}

	UIWidgetLayer::~UIWidgetLayer()
	{
		delete drawable;
	}

	UIWidgetLayer& UIWidgetLayer::operator=(const UIWidgetLayer& other)
	{
		delete drawable;
		for (auto child : mChilds)
			delete child;

		mChilds.Clear();
		drawable = nullptr;

		mDepth = other.mDepth;
		name = other.name;

		if (other.drawable)
			drawable = other.drawable->Clone();

		for (auto child : other.mChilds)
			AddChild(child->Clone());

		SetTransparency(other.mTransparency);

		if (mOwnerWidget)
			mOwnerWidget->UpdateLayersDrawingSequence();

		return *this;
	}

	void UIWidgetLayer::Draw()
	{
		drawable->Draw();
	}

	void UIWidgetLayer::Update(float dt)
	{
	} 
	
	UIWidgetLayer* UIWidgetLayer::AddChild(UIWidgetLayer* node)
	{
		if (node->GetParent())
			node->GetParent()->RemoveChild(node, false);

		node->mParent = this;

		mChilds.Add(node);

		OnChildAdded(node);

		return node;
	}
	
	bool UIWidgetLayer::RemoveChild(UIWidgetLayer* node, bool release /*= true*/)
	{
		node->mParent = nullptr;

		if (!mChilds.Remove(node))
			return false;

		if (release && node)
			delete node;

		return true;
	}
	
	void UIWidgetLayer::RemoveAllChilds()
	{
		for (auto child : mChilds)
			if (child)
				delete child;

		mChilds.Clear();
	}
	
	void UIWidgetLayer::SetParent(UIWidgetLayer* parent)
	{
		if (parent)
		{
			parent->AddChild(this);
		}
		else
		{
			if (mParent)
				mParent->RemoveChild(this, false);

			mParent = nullptr;
		}
	}
	
	UIWidgetLayer* UIWidgetLayer::GetParent() const
	{
		return mParent;
	}
	
	UIWidgetLayer::ChildsVec& UIWidgetLayer::GetChilds()
	{
		return mChilds;
	}
	
	const UIWidgetLayer::ChildsVec& o2::UIWidgetLayer::GetChilds() const
	{
		return mChilds;
	}

	UIWidgetLayer* UIWidgetLayer::AddChildLayer(const String& name, IRectDrawable* drawable,
													const Layout& layout /*= Layout::Both()*/, float depth /*= 0.0f*/)
	{
		if (Math::Equals(depth, 0.0f))
			depth = (float)mOwnerWidget->mDrawingLayers.Count();

		UIWidgetLayer* layer = mnew UIWidgetLayer();
		layer->depth = depth;
		layer->name = name;
		layer->drawable = drawable;
		layer->layout = layout;

		return AddChild(layer);
	}

	UIWidgetLayer* UIWidgetLayer::GetChild(const String& path)
	{
		int delPos = path.Find("/");
		WString pathPart = path.SubStr(0, delPos);

		if (pathPart == "..")
		{
			if (mParent)
			{
				if (delPos == -1)
					return mParent;
				else
					return mParent->GetChild(path.SubStr(delPos + 1));
			}

			return nullptr;
		}

		for (auto child : mChilds)
		{
			if (child->name == pathPart)
			{
				if (delPos == -1)
					return child;
				else
					return child->GetChild(path.SubStr(delPos + 1));
			}
		}

		return nullptr;
	}

	LayersVec UIWidgetLayer::GetAllChilds() const
	{
		LayersVec res = mChilds;
		for (auto child : mChilds)
		{
			res.Add(child->GetAllChilds());
		}

		return res;
	}

	void UIWidgetLayer::SetDepth(float depth)
	{
		mDepth = depth;
		if (mOwnerWidget)
			mOwnerWidget->UpdateLayersDrawingSequence();
	}

	float UIWidgetLayer::GetDepth() const
	{
		return mDepth;
	}

	void UIWidgetLayer::SetTransparency(float transparency)
	{
		mTransparency = transparency;
		UpdateResTransparency();
	}

	float UIWidgetLayer::GetTransparency()
	{
		return mTransparency;
	}

	bool UIWidgetLayer::IsUnderPoint(const Vec2F& point)
	{
		return mInteractableArea.IsInside(point);
	}
	
	const o2::RectF& UIWidgetLayer::GetRect() const
	{
		return mAbsolutePosition;
	}

	void UIWidgetLayer::OnDeserialized(const DataNode& node)
	{
		for (auto child : mChilds)
			child->mParent = this;
	}

	void UIWidgetLayer::SetOwnerWidget(UIWidget* owner)
	{
		mOwnerWidget = owner;

		for (auto child : mChilds)
			child->SetOwnerWidget(owner);

		UpdateResTransparency();
	}

	void UIWidgetLayer::OnChildAdded(UIWidgetLayer* child)
	{
		child->SetOwnerWidget(mOwnerWidget);

		if (mOwnerWidget)
		{
			mOwnerWidget->OnLayerAdded(child);
			mOwnerWidget->UpdateLayersDrawingSequence();
		}
	}

	void UIWidgetLayer::UpdateLayout()
	{
		if (mParent)
			mAbsolutePosition = layout.Calculate(mParent->mAbsolutePosition);
		else
			mAbsolutePosition = layout.Calculate(mOwnerWidget->layout.mAbsoluteRect);

		mInteractableArea = interactableLayout.Calculate(mAbsolutePosition);

		if (drawable)
		{
			drawable->SetRect(mAbsolutePosition);
		}

		for (auto child : mChilds)
			child->UpdateLayout();
	}

	void UIWidgetLayer::UpdateResTransparency()
	{
		if (mParent)
			mResTransparency = transparency*mParent->mResTransparency;
		else
			mResTransparency = transparency*mOwnerWidget->mResTransparency;

		if (drawable)
			drawable->SetTransparency(mResTransparency);

		for (auto child : mChilds)
			child->UpdateResTransparency();
	}

	Dictionary<String, UIWidgetLayer*> UIWidgetLayer::GetAllChildLayers()
	{
		Dictionary<String, UIWidgetLayer*> res;
		for (auto layer : mChilds)
			res.Add(layer->name, layer);

		return res;
	}

	void UIWidgetLayer::InitializeProperties()
	{
		INITIALIZE_PROPERTY(UIWidgetLayer, depth, SetDepth, GetDepth);
		INITIALIZE_PROPERTY(UIWidgetLayer, transparency, SetTransparency, GetTransparency);
		INITIALIZE_ACCESSOR(UIWidgetLayer, child, GetChild);

		child.SetAllAccessFunc(this, &UIWidgetLayer::GetAllChildLayers);
	}

}