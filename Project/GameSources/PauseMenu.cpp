#include"stdafx.h"
#include"Scene.h"
#include"PauseMenu.h"

namespace basecross
{
	void PauseMenu::OnCreate()
	{
		auto& app = App::GetApp();
		auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		app->RegisterTexture(L"Pause", path + L"Pause.png"); // 画像ファイルを読み込んでアセットとして登録する
		app->RegisterTexture(L"Cursor", path + L"SelectCursor.png"); // 画像ファイルを読み込んでアセットとして登録する
		app->RegisterTexture(L"ToTitle", path + L"PB_ToTitle.png"); // 画像ファイルを読み込んでアセットとして登録する

		auto sWidth = app->GetGameWidth() * 2;
		auto sHeight = app->GetGameHeight() * 2;

		//画面の明るさフィルター
		auto ScreenFilter = ObjectFactory::Create<SpriteGeneric>(GetStage());
		ScreenFilter->SetSize(sWidth, sHeight, Vec2(0.5f, 0.5f));
		ScreenFilter->SetPos(Vec2(0, 0));
		ScreenFilter->SetTexture(L"Pause");
		ScreenFilter->SetColor(Col4(0.0f, 0.0f, 0.0f, 0.55f));
		m_menuText.push_back(ScreenFilter);

		//ポーズ
		auto PauseText = ObjectFactory::Create<SpriteGeneric>(GetStage());
		PauseText->SetSize(400.0f, 200.0f, Vec2(0.5f, 0.5f));
		PauseText->SetPos(Vec2(0, 200));
		PauseText->SetTexture(L"Pause");
		PauseText->SetColor(Col4(1, 1, 1, 1));
		m_menuText.push_back(PauseText);

		//選択中のボタンに出るカーソル
		auto Cursor = ObjectFactory::Create<SpriteGeneric>(GetStage());
		Cursor->SetSize(300.0f,150.0f,Vec2(0.5f,0.5f));
		Cursor->SetPos(Vec2(0, 0));
		Cursor->SetTexture(L"Cursor");
		Cursor->SetColor(Col4(1, 1, 1, 1));
		m_menuText.push_back(Cursor);

		//タイトルボタン
		auto titleButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		titleButton->SetSize(300.0f, 150.0f, Vec2(0.5f, 0.5f));
		titleButton->SetPos(Vec2(0, 0));
		titleButton->SetTexture(L"ToTitle");
		titleButton->SetColor(Col4(1, 1, 1, 1));
		m_menuButton.push_back(titleButton);
	}

	void PauseMenu::OnUpdate()
	{
		SelectMenu();
	}

	void PauseMenu::OnDraw()
	{
		for (auto obj : m_menuText)
		{
			obj->OnDraw();
		}
		for (auto obj : m_menuButton)
		{
			obj->OnDraw();
		}
	}

	void PauseMenu::SelectMenu()
	{
		auto& app = App::GetApp();
		auto scene = app->GetScene<Scene>();
		auto device = app->GetInputDevice();
		auto pad = device.GetControlerVec()[0];

		std::wstringstream wss(L"");
		wss << m_selectMenu;

		float stickY = pad.fThumbLY;
		float threshold = 0.25f;

		if (!m_lock)
		{
			if (stickY > threshold || pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				m_selectMenu = (m_selectMenu == 1) ? m_menuNum : m_selectMenu - 1;
				m_lock = true;
			}
			else if (stickY < -threshold || pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				m_selectMenu = (m_selectMenu % m_menuNum) + 1;
				m_lock = true;
			}
		}
		else if (std::abs(stickY) < 0.25f)
		{
			m_lock = false;
		}

		if (pad.wPressedButtons & XINPUT_GAMEPAD_A)
		{
			switch (m_selectMenu)
			{
			case 1:

				break;
			case 2:
				PostEvent(0.1f, GetThis<PauseMenu>(), scene, L"ToTitleStage");
				break;
			}
		}
		scene->SetDebugString(wss.str());
	}
}