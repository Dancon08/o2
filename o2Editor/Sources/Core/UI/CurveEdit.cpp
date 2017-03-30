#include "CurveEdit.h"

#include "Application/Application.h"
#include "Render/Render.h"
#include "Render/Sprite.h"
#include "UI/HorizontalScrollBar.h"
#include "UI/VerticalScrollBar.h"

namespace Editor
{
	UICurveEditor::UICurveEditor()
	{
		mReady = false;

		mHorScrollbar = mnew UIHorizontalScrollBar();
		mHorScrollbar->layout = UIWidgetLayout::HorStretch(VerAlign::Bottom, 0, 0, 20);
		mHorScrollbar->SetParent(this);
		mChilds.Remove(mHorScrollbar);

		mVerScrollbar = mnew UIVerticalScrollBar();
		mVerScrollbar->layout = UIWidgetLayout::VerStretch(HorAlign::Right, 0, 0, 20);
		mVerScrollbar->SetParent(this);
		mChilds.Remove(mVerScrollbar);

		mSelectionSprite = mnew Sprite();
		mTextFont = FontRef("stdFont.ttf");
		mText = mnew Text(mTextFont);

		mReady = true;
	}

	UICurveEditor::UICurveEditor(const UICurveEditor& other):
		UIScrollView(other), mHorScrollbar(other.mHorScrollbar->Clone()), mVerScrollbar(other.mVerScrollbar->Clone()),
		mSelectionSprite(other.mSelectionSprite->Clone()), mTextFont(other.mTextFont)
	{
		mReady = false;

		mHorScrollbar->SetParent(this);
		mChilds.Remove(mHorScrollbar);

		mVerScrollbar->SetParent(this);
		mChilds.Remove(mVerScrollbar);

		mText = mnew Text(mTextFont);

		RetargetStatesAnimations();

		mReady = true;
	}

	UICurveEditor::~UICurveEditor()
	{
		delete mHorScrollbar;
		delete mVerScrollbar;
		delete mSelectionSprite;
		delete mText;
	}

	Editor::UICurveEditor& UICurveEditor::operator=(const UICurveEditor& other)
	{
		UIScrollView::operator=(other);

		mReady = false;

		delete mHorScrollbar;
		delete mVerScrollbar;
		delete mSelectionSprite;
		delete mText;

		mHorScrollbar = other.mHorScrollbar->Clone();
		mHorScrollbar->SetParent(this);
		mChilds.Remove(mHorScrollbar);

		mVerScrollbar = other.mVerScrollbar->Clone();
		mVerScrollbar->SetParent(this);
		mChilds.Remove(mVerScrollbar);

		mSelectionSprite = other.mSelectionSprite->Clone();
		mTextFont        = other.mTextFont;
		mText            = mnew Text(mTextFont);

		RetargetStatesAnimations();

		mReady = true;

		return *this;
	}

	void UICurveEditor::Draw()
	{
		UIScrollView::Draw();

		if (!mReady)
			return;

		mHorScrollbar->Draw();
		mVerScrollbar->Draw();
	}

	void UICurveEditor::Update(float dt)
	{
		UIScrollView::Update(dt);

		if (!mReady)
			return;

		mHorScrollbar->Update(dt);
		mVerScrollbar->Update(dt);
	}

	void UICurveEditor::AddEditingCurve(Curve* curve, const Color4& color /*= Color4(-1, -1, -1, -1)*/)
	{
		CurveInfo* info = mnew CurveInfo();
		info->curve = curve;
		info->viewScale = Vec2F();

		if (color == Color4(-1, -1, -1, -1))
		{
			if (mCurves.IsEmpty())
				info->color = Color4::Green();
			else
				info->color = Color4::SomeColor(mCurves.Count());
		}
		else info->color = color;


	}

	void UICurveEditor::RemoveEditingCurve(Curve* curve)
	{
		for (auto info : mCurves)
		{
			if (info->curve == curve)
			{
				for (auto handle : info->handles)
				{
					mAllHandles.Remove(handle);
					mSelectedHandles.Remove(handle);
				}

				delete info;
				mCurves.Remove(info);
				break;
			}
		}
	}

	void UICurveEditor::RemoveAllEditingCurves()
	{
		auto curves = mCurves;
		for (auto info : curves)
			RemoveEditingCurve(info->curve);
	}

	void UICurveEditor::AddCurvesRange(Curve* curveA, Curve* curveB, const Color4& color /*= Color4(-1, -1, -1, -1)*/)
	{

	}

	void UICurveEditor::RemoveCurvesRange(Curve* curveA, Curve* curveB)
	{

	}

	void UICurveEditor::UpdateLayout(bool forcible /*= false*/, bool withChildren /*= true*/)
	{
		UIScrollView::UpdateLayout(forcible, withChildren);

		if (!mReady)
			return;

		mHorScrollbar->UpdateLayout();
		mVerScrollbar->UpdateLayout();
	}

	void UICurveEditor::SetSelectionSpriteImage(const ImageAssetRef& image)
	{
		mSelectionSprite->LoadFromImage(image);
	}

	void UICurveEditor::SetTextFont(const FontRef& font)
	{
		mTextFont = font;
		mText->SetFont(mTextFont);
	}

	void UICurveEditor::SetHorScrollbar(UIHorizontalScrollBar* scrollbar)
	{
		delete mHorScrollbar;

		mHorScrollbar = scrollbar;
		AddChild(mHorScrollbar);
		mChilds.Remove(mHorScrollbar);

		UpdateLayout();
	}

	void UICurveEditor::SetVerScrollbar(UIVerticalScrollBar* scrollbar)
	{
		delete mVerScrollbar;

		mVerScrollbar = scrollbar;
		AddChild(mVerScrollbar);
		mChilds.Remove(mVerScrollbar);

		UpdateLayout();
	}

	void UICurveEditor::RedrawContent()
	{
		DrawGrid();
	}

	UICurveEditor::CurveInfo::CurveInfo()
	{

	}

	UICurveEditor::CurveInfo::~CurveInfo()
	{

	}

	void UICurveEditor::CurveInfo::UpdateApproximatedPoints()
	{

	}

	UICurveEditor::RangeInfo::RangeInfo()
	{

	}

	UICurveEditor::RangeInfo::~RangeInfo()
	{

	}

	void UICurveEditor::RangeInfo::UpdateMesh()
	{

	}

}

CLASS_META(Editor::UICurveEditor)
{
	BASE_CLASS(Editor::UIScrollView);

	PROTECTED_FIELD(mHorScrollbar).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mVerScrollbar).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mCurves);
	PROTECTED_FIELD(mRanges);
	PROTECTED_FIELD(mAllHandles);
	PROTECTED_FIELD(mSelectedHandles);
	PROTECTED_FIELD(mSelectionSprite).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mTextFont).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mText);

	PUBLIC_FUNCTION(void, Draw);
	PUBLIC_FUNCTION(void, Update, float);
	PUBLIC_FUNCTION(void, AddEditingCurve, Curve*, const Color4&);
	PUBLIC_FUNCTION(void, RemoveEditingCurve, Curve*);
	PUBLIC_FUNCTION(void, RemoveAllEditingCurves);
	PUBLIC_FUNCTION(void, AddCurvesRange, Curve*, Curve*, const Color4&);
	PUBLIC_FUNCTION(void, RemoveCurvesRange, Curve*, Curve*);
	PUBLIC_FUNCTION(void, SetSelectionSpriteImage, const ImageAssetRef&);
	PUBLIC_FUNCTION(void, SetTextFont, const FontRef&);
	PUBLIC_FUNCTION(void, SetHorScrollbar, UIHorizontalScrollBar*);
	PUBLIC_FUNCTION(void, SetVerScrollbar, UIVerticalScrollBar*);
	PUBLIC_FUNCTION(void, UpdateLayout, bool, bool);
	PROTECTED_FUNCTION(void, RedrawContent);
}
END_META;
 