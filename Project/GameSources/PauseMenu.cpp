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

		//ポーズ
		auto PauseText = ObjectFactory::Create<SpriteGeneric>(GetStage());
		PauseText->SetSize(400.0f, 200.0f, Vec2(0.5f, 0.5f));
		PauseText->SetPos(Vec3(0, 200, 0));
		PauseText->SetTexture(L"Pause");
		PauseText->SetColor(Col4(1, 1, 1, 1));
		m_menuText.push_back(PauseText);

		//タイトルボタン
		auto titleButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		titleButton->SetSize(200.0f, 100.0f, Vec2(0.5f, 0.5f));
		titleButton->SetPos(Vec3(0, 0, 0));
		titleButton->SetTexture(L"Pause");
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

		float stickY = pad.fThumbLY;
		float threshold = 0.25f;

		if (!m_lock)
		{
			if (stickY > threshold)
			{
				m_selectMenu = (m_selectMenu == 1) ? m_menuNum : m_selectMenu - 1;
				m_lock = true;
			}
			else if (stickY < threshold)
			{
				m_selectMenu = (m_selectMenu % m_menuNum) + 1;
				m_lock = true;
			}
			else if (std::abs(stickY) < 0.25f)
			{
				m_lock = false;
			}
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
	}
}