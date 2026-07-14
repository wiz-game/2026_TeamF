#include"stdafx.h"
#include"Project.h"
#include"Scene.h"
#include"PauseMenu.h"
#include "GameProgressManager.h"
namespace basecross
{
	void PauseMenu::OnCreate()
	{
		auto& app = App::GetApp();
		auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		app->RegisterTexture(L"Pause", path + L"Pause.png"); // 画像ファイルを読み込んでアセットとして登録する
		app->RegisterTexture(L"Cursor", path + L"SelectCursor.png"); // 画像ファイルを読み込んでアセットとして登録する
		app->RegisterTexture(L"Cancel", path + L"PB_Cancel.png"); // 画像ファイルを読み込んでアセットとして登録する
		app->RegisterTexture(L"Restart", path + L"PB_Restart.png"); // 画像ファイルを読み込んでアセットとして登録する
		app->RegisterTexture(L"Option", path + L"PB_Option.png"); // 画像ファイルを読み込んでアセットとして登録する
		app->RegisterTexture(L"ToStageSelect", path + L"PB_ToStageSelect.png"); // 画像ファイルを読み込んでアセットとして登録する
		app->RegisterTexture(L"ToTitle", path + L"PB_ToTitle.png"); // 画像ファイルを読み込んでアセットとして登録する

		auto sWidth = app->GetGameWidth() * 2;
		auto sHeight = app->GetGameHeight() * 2;

		m_menuPosList = {
			Vec2(0.0f, 75.0f),
			Vec2(0.0f, 0.0f),
			Vec2(0.0f, -75.0f),
			Vec2(0.0f, -150.0f),
			Vec2(0.0f, -225.0f),
		};

		//画面の明るさフィルター
		auto ScreenFilter = ObjectFactory::Create<SpriteGeneric>(GetStage());
		ScreenFilter->SetSize(sWidth, sHeight, Vec2(0.5f, 0.5f));
		ScreenFilter->SetPos(Vec2(0, 0));
		ScreenFilter->SetTexture(L"Pause");
		ScreenFilter->SetColor(Col4(0.0f, 0.0f, 0.0f, 0.55f));
		m_menuLabel.push_back(ScreenFilter);

		//ポーズ
		auto PauseText = ObjectFactory::Create<SpriteGeneric>(GetStage());
		PauseText->SetSize(400.0f, 200.0f, Vec2(0.5f, 0.5f));
		PauseText->SetPos(Vec2(0, 200));
		PauseText->SetTexture(L"Pause");
		PauseText->SetColor(Col4(1, 1, 1, 1));
		m_menuLabel.push_back(PauseText);

		//選択中のボタンに出るカーソル
		auto Cursor = ObjectFactory::Create<SpriteGeneric>(GetStage());
		Cursor->SetSize(250.0f, 100.0f, Vec2(0.5f, 0.5f));
		Cursor->SetPos(m_menuPosList[m_selectIndex]);
		Cursor->SetTexture(L"Cursor");
		Cursor->SetColor(Col4(0, 0, 0, 1));
		m_menuLabel.push_back(Cursor);
		m_menuCursor = Cursor;

		//続ける(continue)ボタン
		auto continueButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		continueButton->SetSize(200.0f, 100.0f, Vec2(0.5f, 0.5f));
		continueButton->SetPos(m_menuPosList[0]);
		continueButton->SetTexture(L"Cancel");
		continueButton->SetColor(Col4(1, 1, 1, 1));
		m_menuButton.push_back(continueButton);

		//リトライボタン
		auto retryButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		retryButton->SetSize(200.0f, 100.0f, Vec2(0.5f, 0.5f));
		retryButton->SetPos(m_menuPosList[1]);
		retryButton->SetTexture(L"Restart");
		retryButton->SetColor(Col4(1, 1, 1, 1));
		m_menuButton.push_back(retryButton);

		//設定ボタン
		auto optionButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		optionButton->SetSize(200.0f, 100.0f, Vec2(0.5f, 0.5f));
		optionButton->SetPos(m_menuPosList[2]);
		optionButton->SetTexture(L"Option");
		optionButton->SetColor(Col4(1, 1, 1, 1));
		m_menuButton.push_back(optionButton);

		//ステージセレクトに戻るボタン
		auto toStageSelectButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		toStageSelectButton->SetSize(200.0f, 100.0f, Vec2(0.5f, 0.5f));
		toStageSelectButton->SetPos(m_menuPosList[3]);
		toStageSelectButton->SetTexture(L"ToStageSelect");
		toStageSelectButton->SetColor(Col4(1, 1, 1, 1));
		m_menuButton.push_back(toStageSelectButton);

		//タイトルボタン
		auto titleButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		titleButton->SetSize(200.0f, 100.0f, Vec2(0.5f, 0.5f));
		titleButton->SetPos(m_menuPosList[4]);
		titleButton->SetTexture(L"ToTitle");
		titleButton->SetColor(Col4(1, 1, 1, 1));
		m_menuButton.push_back(titleButton);
	}

	void PauseMenu::OnUpdate()
	{
		SelectMenu();
		//カーソル位置更新
		Vec2 cursorPos = m_menuPosList[m_selectMenu - 1];
		m_menuCursor->SetPos(cursorPos);
	}

	void PauseMenu::OnDraw()
	{
		for (auto obj : m_menuLabel)
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
		auto stage = GetStage();
		auto device = app->GetInputDevice();
		auto pad = device.GetControlerVec()[0];

		//std::wstringstream wss(L"");
		//wss << m_selectMenu;

		float stickY = pad.fThumbLY;
		float threshold = 0.25f;

		if (!m_lock)
		{
			if (stickY > threshold || pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				SoundManager::Get().PlaySE(L"SELECT", 1.0f);
				m_selectMenu = (m_selectMenu == 1) ? m_menuNum : m_selectMenu - 1;
				//上移動
				m_selectIndex = m_selectIndex--;
				if (m_selectIndex < 0)
				{
					m_selectIndex = static_cast<int>(m_menuPosList.size()) - 1;
				}
				m_lock = true;
			}
			else if (stickY < -threshold || pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				SoundManager::Get().PlaySE(L"SELECT", 1.0f);
				m_selectMenu = (m_selectMenu % m_menuNum) + 1;
				//下移動
				m_selectIndex = m_selectIndex++;
				if (m_selectIndex >= m_menuPosList.size())
				{
					m_selectIndex = 0;
				}
				m_lock = true;
			}
		}
		else if (std::abs(stickY) < 0.25f)
		{
			m_lock = false;
		}

		if (pad.wPressedButtons & XINPUT_GAMEPAD_A)
		{
			m_lock = true;
			SoundManager::Get().PlaySE(L"CONFIRM", 1.0f);
			switch (m_selectMenu)
			{
			case 1:
				//コンティニュー
				ClosePause();
				break;
			case 2: {
				//リトライ
				int currentStage = GameProgressManager::Get().GetCurrentStage();
				PostEvent(0.50f, GetThis<PauseMenu>(), scene, L"ToGameStage", make_shared<int>(currentStage));
				break;
			}
			case 3:
				//設定
				ToOption();
				break;
			case 4:
				//ステージセレクトヘ
				PostEvent(0.05f, GetThis<PauseMenu>(), scene, L"ToSelectStage");
				break;
			case 5:
				//タイトルへ
				PostEvent(0.05f, GetThis<PauseMenu>(), scene, L"ToTitleStage");
				break;
			}
		}
		if (pad.wPressedButtons & XINPUT_GAMEPAD_B)
		{
			SoundManager::Get().PlaySE(L"SELECT", 1.0f);
			ClosePause();
		}

		//scene->SetDebugString(wss.str());
	}

	void PauseMenu::ClosePause()
	{
		auto stage = GetStage();
		auto objs = stage->GetGameObjectVec();
		for (auto& obj : objs)
		{
			obj->SetUpdateActive(true);
		}
		SetPause(false);
		//BGMを再開
		SoundManager::Get().PauseBGM(false);
	}

	void PauseMenu::ToOption()
	{
		bool a = false;
		SetPause(false);
		//m_option->ToPause();
	}
}