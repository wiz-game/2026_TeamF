#include"stdafx.h"
#include"PauseMenu.h"

namespace basecross
{
	void PauseMenu::OnCreate()
	{
		//ゲーム最下位ボタン
		m_cancel = ObjectFactory::Create<SpriteGeneric>(GetStage());
		m_cancel->SetSize(100.0f, 100.0f);
		m_cancel->SetColor(Col4(1, 1, 1, 1));
	}

	void PauseMenu::OnUpdate()
	{

	}

	void PauseMenu::OnDraw()
	{

	}
}