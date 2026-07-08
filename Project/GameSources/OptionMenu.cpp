#include"stdafx.h"
#include"Project.h"
#include"Scene.h"
#include"OptionMenu.h"

namespace basecross
{
	void OptionMenu::OnCreate()
	{
		m_pause = GetStage()->GetSharedGameObject<PauseMenu>(L"Pause");

		auto& app = App::GetApp();
		auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		app->RegisterTexture(L"OptionText", path + L"Option.png"); // 画像ファイルを読み込んでアセットとして登録する
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

		//オプション
		auto OptionText = ObjectFactory::Create<SpriteGeneric>(GetStage());
		OptionText->SetSize(400.0f, 200.0f, Vec2(0.5f, 0.5f));
		OptionText->SetPos(Vec2(0, 200));
		OptionText->SetTexture(L"OptionText");
		OptionText->SetColor(Col4(1, 1, 1, 1));
		m_menuLabel.push_back(OptionText);

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

		////ステージセレクトに戻るボタン
		//auto toStageSelectButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		//toStageSelectButton->SetSize(200.0f, 100.0f, Vec2(0.5f, 0.5f));
		//toStageSelectButton->SetPos(m_menuPosList[3]);
		//toStageSelectButton->SetTexture(L"ToStageSelect");
		//toStageSelectButton->SetColor(Col4(1, 1, 1, 1));
		//m_menuButton.push_back(toStageSelectButton);

		////タイトルボタン
		//auto titleButton = ObjectFactory::Create<SpriteGeneric>(GetStage());
		//titleButton->SetSize(200.0f, 100.0f, Vec2(0.5f, 0.5f));
		//titleButton->SetPos(m_menuPosList[4]);
		//titleButton->SetTexture(L"ToTitle");
		//titleButton->SetColor(Col4(1, 1, 1, 1));
		//m_menuButton.push_back(titleButton);
	}

	void OptionMenu::OnUpdate()
	{
		SelectMenu();
		//カーソル位置更新
		Vec2 cursorPos = m_menuPosList[m_selectMenu - 1];
		m_menuCursor->SetPos(cursorPos);
	}

	void OptionMenu::OnDraw()
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

	void OptionMenu::SelectMenu()
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
			switch (m_selectMenu)
			{
			case 1:
				//コンティニュー
				break;
			case 2:
				//リトライ

				break;
			case 3:
				//ポーズに戻る
				ToPause();
				break;
			//case 4:
			//	//ステージセレクトヘ

			//	break;
			//case 5:
			//	//タイトルへ

			//	break;
			}
		}

		if (pad.wPressedButtons & XINPUT_GAMEPAD_START)
		{
			CloseOption();
		}
			//scene->SetDebugString(wss.str());
	}

	void OptionMenu::CloseOption()
	{
		SetOption(false);

		auto pause = GetStage()->GetSharedGameObject<PauseMenu>(L"Pause");
		pause->ClosePause();
	}

	void OptionMenu::ToPause()
	{
		auto pause = GetStage()->GetSharedGameObject<PauseMenu>(L"Pause");
		pause->SetPause(true);
		SetOption(false);
	}
}