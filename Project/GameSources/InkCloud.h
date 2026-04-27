/*!
@file InkCloud.h
@brief インクのコリジョンをまとめるクラス
*/

#pragma once
#include "stdafx.h"
//#include "InkDraw.h"

namespace basecross {
	class InkDraw;
	//--------------------------------------------------------------------------------------
	//	class InkCloud : public GameObject;
	//--------------------------------------------------------------------------------------
	class InkCloud : public GameObject
	{
		std::vector<std::shared_ptr<InkDraw>> m_inkList;	//属するインクのリスト
		std::shared_ptr<CollisionObb> m_combinedColl;	//統合された1つのコリジョン

		bool isPower = false;//自分に電流が流れているか

	public:
		// 構築と破棄
		InkCloud(const shared_ptr<Stage>& stage) :
			GameObject(stage)
		{
		}
		virtual ~InkCloud()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		void AddInk(const std::shared_ptr<InkDraw>& newInk)
		{
			m_inkList.push_back(newInk);
			UpdateCombinaedCollision();
			// 統合コリジョンを更新する処理をここに追加
		}

		void UpdateCombinaedCollision();// 統合コリジョンを更新する関数

		virtual void OnCollisionEnter(shared_ptr<GameObject>& info) override;
		virtual void OnCollisionExit(shared_ptr<GameObject>& info) override;

		bool GetisPower() const
		{
			return isPower;
		}
	};

}
//end basecross
