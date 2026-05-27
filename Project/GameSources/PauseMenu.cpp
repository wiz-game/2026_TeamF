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

		//ゲーム再開ボタン
		auto cancelButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		cancelButton->SetSize(400.0f, 200.0f, Vec2(0.5f, 0.5f));
		cancelButton->SetPos(Vec3(0, 200, 0));
		cancelButton->SetTexture(L"Pause");
		cancelButton->SetColor(Col4(1, 1, 1, 1));
		m_menuButton.push_back(cancelButton);
	}

	void PauseMenu::OnUpdate()
	{
		SelectMenu();
	}

	void PauseMenu::OnDraw()
	{
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