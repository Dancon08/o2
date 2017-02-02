#include "UITestScreen.h"

#include "Assets/ImageAsset.h"
#include "BasicUIStyle.h"
#include "Render/Render.h"
#include "TestApplication.h"
#include "UI/Button.h"
#include "UI/ContextMenu.h"
#include "UI/DropDown.h"
#include "UI/EditBox.h"
#include "UI/HorizontalLayout.h"
#include "UI/HorizontalProgress.h"
#include "UI/Label.h"
#include "UI/List.h"
#include "UI/Toggle.h"
#include "UI/Tree.h"
#include "UI/UIManager.h"
#include "UI/VerticalLayout.h"
#include "UI/VerticalProgress.h"
#include "UI/WidgetLayout.h"
#include "UI/Window.h"
#include "Utils/CommonTypes.h"

UITestScreen::UITestScreen(TestApplication* application):
	ITestScreen(application)
{}

UITestScreen::~UITestScreen()
{}

#undef CreateWindow

void UITestScreen::Load()
{
	o2::BasicUIStyleBuilder uiBuilder;
	uiBuilder.RebuildBasicUIStyle();

	mBackground.LoadFromImage("ui/UI_Background.png");
	mBackground.size = (o2::Vec2I)o2Render.resolution + o2::Vec2I(30, 30);
    mBackground.size = o2::Vec2F(100, 100);
	o2Application.onResizingEvent += [&]() { mBackground.size = (o2::Vec2I)o2Render.resolution + o2::Vec2I(30, 30); };

    //return;
    
	auto window = o2UI.CreateWindow("UI elements samples");
	o2UI.AddWidget(window);
	window->layout.size = o2::Vec2F(300, 300);
	window->name = "UI elements window";

	auto verLayout = o2UI.CreateVerLayout();
	verLayout->baseCorner = o2::BaseCorner::Top;
	verLayout->expandHeight = false;
	verLayout->fitByChildren = true;
	verLayout->border = o2::RectF(5, 5, 5, 5);
	verLayout->spacing = 10;
	verLayout->layout = o2::UIWidgetLayout::BothStretch();
	window->AddChild(verLayout);

	//button
	auto buttonContainer = o2UI.CreateVerLayout();
	buttonContainer->layout.height = 50;
	auto button = o2UI.CreateButton("Click me!");
	buttonContainer->AddChild(o2UI.CreateLabel("Button"));
	buttonContainer->AddChild(button);
	verLayout->AddChild(buttonContainer);

	auto buttonContext = o2UI.CreateWidget<o2::UIContextMenu>();
	for (int i = 0; i < 15; i++)
	{
		o2::WString itemName = o2::WString::Format("Item #%i", i + 1);
		buttonContext->AddItem(o2::UIContextMenu::Item(itemName, [=]() { o2Debug.Log("Pressed " + itemName); }));
	}

	buttonContext->AddItem(
		o2::UIContextMenu::Item("Icon item", []() { o2Debug.Log("Pressed icon"); },
							    mnew o2::ImageAsset("ui/UI_search_regular.png")));

	o2::UIContextMenu::Item itm("Sub items", {
		o2::UIContextMenu::Item("Sub 1", []() { o2Debug.Log("Pressed sub 1"); }),
		o2::UIContextMenu::Item("Sub 2",{ o2::UIContextMenu::Item("Sub 1", []() { o2Debug.Log("Pressed sub 1"); }),
							o2::UIContextMenu::Item("Sub 2", []() { o2Debug.Log("Pressed sub 2"); }),
							o2::UIContextMenu::Item("Sub 3", []() { o2Debug.Log("Pressed sub 3"); }) }),
							o2::UIContextMenu::Item("Sub 3", []() { o2Debug.Log("Pressed sub 3"); })
	});

	buttonContext->AddItem(itm);
	buttonContext->AddItem(itm);

	button->AddChild(buttonContext);
	button->onClick = [=]() { buttonContext->Show(o2Input.GetCursorPos()); };

	//check box
	auto checkBoxContainer = o2UI.CreateVerLayout();
	checkBoxContainer->layout.height = 50;
	checkBoxContainer->AddChild(o2UI.CreateLabel("Check box"));
	checkBoxContainer->AddChild(o2UI.CreateToggle("Toggle me!"));
	verLayout->AddChild(checkBoxContainer);

	//edit box
	auto editBoxContainer = o2UI.CreateVerLayout();
	editBoxContainer->fitByChildren = true;
	editBoxContainer->AddChild(o2UI.CreateLabel("Edit box"));
	editBoxContainer->AddChild(o2UI.CreateEditBox());
	verLayout->AddChild(editBoxContainer);

	// hor progress
	auto horProgressContainer = o2UI.CreateVerLayout();
	horProgressContainer->fitByChildren = true;
	horProgressContainer->AddChild(o2UI.CreateLabel("Horizontal progress"));
	horProgressContainer->AddChild(o2UI.CreateHorProgress());
	verLayout->AddChild(horProgressContainer);

	// ver progress
	auto verProgressContainer = o2UI.CreateVerLayout();
	verProgressContainer->fitByChildren = true;
	verProgressContainer->expandHeight = false;
	verProgressContainer->AddChild(o2UI.CreateLabel("Vertical progress"));
	auto verProgress = o2UI.CreateVerProgress();
	verProgress->layout.height = 100;
	verProgressContainer->AddChild(verProgress);
	verLayout->AddChild(verProgressContainer);

	// hor scroll bar
	auto horScrollContainer = o2UI.CreateVerLayout();
	horScrollContainer->fitByChildren = true;
	horScrollContainer->AddChild(o2UI.CreateLabel("Horizontal scroll bar"));
	horScrollContainer->AddChild(o2UI.CreateHorScrollBar());
	verLayout->AddChild(horScrollContainer);

	// ver scroll bar
	auto verScrollContainer = o2UI.CreateVerLayout();
	verScrollContainer->fitByChildren = true;
	verScrollContainer->expandHeight = false;
	verScrollContainer->AddChild(o2UI.CreateLabel("Vertical scroll bar"));
	auto verScroll = o2UI.CreateVerScrollBar();
	verScroll->layout.height = 100;
	verScrollContainer->AddChild(verScroll);
	verLayout->AddChild(verScrollContainer);

	// list
	auto listContainer = o2UI.CreateVerLayout();
	listContainer->fitByChildren = true;
	listContainer->expandHeight = false;
	listContainer->AddChild(o2UI.CreateLabel("List"));

	auto list = o2UI.CreateList();
	list->name = "test list";
	list->layout.height = 100;
	for (int i = 0; i < 10; i++)
		list->AddItem(o2::String::Format("List item #%i", i + 1));

	listContainer->AddChild(list);
	verLayout->AddChild(listContainer);

	// drop down
	auto dropdownContainer = o2UI.CreateVerLayout();
	dropdownContainer->fitByChildren = true;
	dropdownContainer->AddChild(o2UI.CreateLabel("Drop down"));

	auto dropdown = o2UI.CreateDropdown();
	for (int i = 0; i < 10; i++)
		dropdown->AddItem(o2::String::Format("Drop down item #%i", i + 1));
	dropdown->SetMaxListSizeInItems(5);

	dropdownContainer->AddChild(dropdown);
	verLayout->AddChild(dropdownContainer);

	// label test window
	auto labelTestWindow = o2UI.CreateWindow("Label test");
	auto testLabel = o2UI.CreateLabel("Label text multi line Label text multi line\nLabel text multi line\nLabel text multi lineLabel text multi line\nLabel text multi line");
	labelTestWindow->AddChild(testLabel);
	testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0);

	auto labelTestEdit = o2UI.CreateEditBox();
	labelTestEdit->layout = o2::UIWidgetLayout::BothStretch(0, 100, 0, 0);
	labelTestEdit->layout.anchorTop = 0;
	labelTestEdit->layout.offsetTop = 195;
	labelTestWindow->AddChild(labelTestEdit);
	labelTestEdit->onChanged += [=](const o2::WString& text) { testLabel->text = text; };

	auto testButtonslayout = o2UI.CreateVerLayout();
	auto alignHorTestButtonsLayout = o2UI.CreateHorLayout();
	auto alignVerTestButtonsLayout = o2UI.CreateHorLayout();
	auto overflowHorTestButtonsLayout = o2UI.CreateHorLayout();
	auto overflowVerTestButtonsLayout = o2UI.CreateHorLayout();

	testButtonslayout->layout = o2::UIWidgetLayout::BothStretch();
	testButtonslayout->layout.anchorTop = 0;
	testButtonslayout->layout.offsetTop = 95;
	testButtonslayout->AddChild(alignHorTestButtonsLayout);
	testButtonslayout->AddChild(alignVerTestButtonsLayout);
	testButtonslayout->AddChild(overflowHorTestButtonsLayout);
	testButtonslayout->AddChild(overflowVerTestButtonsLayout);

	testButtonslayout->spacing = 5;

	alignHorTestButtonsLayout->AddChild(o2UI.CreateButton("|<-", [=]() { testLabel->horAlign = o2::HorAlign::Left; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	alignHorTestButtonsLayout->AddChild(o2UI.CreateButton("->|<-", [=]() { testLabel->horAlign = o2::HorAlign::Middle; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));

	alignHorTestButtonsLayout->AddChild(o2UI.CreateButton("->|", [=]() {
		testLabel->horAlign = o2::HorAlign::Right;
		testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0);
	}));

	alignHorTestButtonsLayout->AddChild(o2UI.CreateButton("---", [=]() { testLabel->horAlign = o2::HorAlign::Both; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	alignHorTestButtonsLayout->spacing = 5;

	alignVerTestButtonsLayout->AddChild(o2UI.CreateButton("/\\", [=]() {
		testLabel->verAlign = o2::VerAlign::Top;
		testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0);
	}));

	alignVerTestButtonsLayout->AddChild(o2UI.CreateButton("---", [=]() { testLabel->verAlign = o2::VerAlign::Middle; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	alignVerTestButtonsLayout->AddChild(o2UI.CreateButton("\\/", [=]() { testLabel->verAlign = o2::VerAlign::Bottom; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	alignVerTestButtonsLayout->AddChild(o2UI.CreateButton("-\n-\n-", [=]() { testLabel->verAlign = o2::VerAlign::Both; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	alignVerTestButtonsLayout->spacing = 5;

	overflowHorTestButtonsLayout->AddChild(o2UI.CreateButton("None", [=]() { testLabel->horOverflow = o2::UILabel::HorOverflow::None; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	overflowHorTestButtonsLayout->AddChild(o2UI.CreateButton("Cut", [=]() { testLabel->horOverflow = o2::UILabel::HorOverflow::Cut; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	overflowHorTestButtonsLayout->AddChild(o2UI.CreateButton("Dots", [=]() { testLabel->horOverflow = o2::UILabel::HorOverflow::Dots; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	overflowHorTestButtonsLayout->AddChild(o2UI.CreateButton("Expand", [=]() { testLabel->horOverflow = o2::UILabel::HorOverflow::Expand; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	overflowHorTestButtonsLayout->AddChild(o2UI.CreateButton("Wrap", [=]() { testLabel->horOverflow = o2::UILabel::HorOverflow::Wrap; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	overflowHorTestButtonsLayout->spacing = 5;

	overflowVerTestButtonsLayout->AddChild(o2UI.CreateButton("None", [=]() { testLabel->verOverflow = o2::UILabel::VerOverflow::None; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	overflowVerTestButtonsLayout->AddChild(o2UI.CreateButton("Cut", [=]() { testLabel->verOverflow = o2::UILabel::VerOverflow::Cut; testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0); }));
	overflowVerTestButtonsLayout->AddChild(o2UI.CreateButton("Expand", [=]() {
		testLabel->verOverflow = o2::UILabel::VerOverflow::Expand;
		testLabel->layout = o2::UIWidgetLayout::BothStretch(0, 200, 0, 0);
	}));
	overflowVerTestButtonsLayout->spacing = 5;

	labelTestWindow->AddChild(testButtonslayout);
	o2UI.AddWidget(labelTestWindow);
	labelTestWindow->layout.size = o2::Vec2F(300, 300);

	auto treeWnd = o2UI.AddWindow("Tree");
	treeWnd->layout.size = o2::Vec2F(300, 300);
}

void UITestScreen::Unload()
{
	o2UI.RemoveAllWidgets();
}

void UITestScreen::Update(float dt)
{
	if (o2Input.IsKeyPressed('D'))
		o2Debug.Log("debug");

	if (o2Input.IsKeyPressed(VK_ESCAPE))
		mApplication->GoToScreen("MainTestScreen");
}

void UITestScreen::Draw()
{
	mBackground.Draw();
    //o2Render.DrawRectFrame(o2::Vec2F(-100, -100), o2::Vec2F(100, 100));
}

o2::String UITestScreen::GetId() const
{
	return "UITestScreen";
}
