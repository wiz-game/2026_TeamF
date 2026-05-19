/*!
@file Electrified.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{

	class GameObject;
	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class Electrified : public Component
	{
	public:
		Electrified(const shared_ptr<GameObject>& owner) :
			Component(owner)
		{
		}
		virtual ~Electrified() = default;

		void OnUpdate() override {}
		void OnDraw() override {}

		//毎フレーム通電更新
		virtual void UpdateElectrified();

		//外部から電気を流し込む
		void SetPowered() { m_nextPowered = true; }

		//現在通電しているかどうか
		bool IsPowered() const { return m_currentPowered; }

		//電源かどうかの設定
		void SetAsSource(bool isSource) { m_isSource = isSource; }

		//接触開始・終了
		virtual void OnElectrifiedEnter(std::shared_ptr<GameObject>& other);
		virtual void OnElectrifiedExit(std::shared_ptr<GameObject>& other);

	protected :
		//電源元かどうか
		bool m_isSource = false;

		//現在フレームの状態
		bool m_currentPowered = false;

		//次フレームの状態
		bool m_nextPowered = false;

		//接触している相手
		std::vector<std::weak_ptr<GameObject>> m_contactObjects;
	};

}
//end basecross
