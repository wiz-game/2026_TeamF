/*!
@file MoveObj.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class MoveObj : public BaseEnemy
	{
		std::shared_ptr<InkCloud> m_targetInk;
		std::shared_ptr<InkCloud> FindNearestInk(
			const std::vector<std::shared_ptr<GameObject>>& objs);
	public:
		// 構築と破棄
		MoveObj(const shared_ptr<Stage>& stage) :
			BaseEnemy(stage)
		{
		}
		virtual ~MoveObj()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
		bool m_justErased = false;
	protected :
		//virtual void UpdateInkErase() override; // インクの消去
		virtual void UpdatePatrol() override; // 徘徊

		void UpdateErase(std::shared_ptr<InkCloud> target);
	};

}
//end basecross
