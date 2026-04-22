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
	class Electrified
	{
	public:
		//毎フレーム通電更新
		virtual void UpdateElectrifed();

		//接触開始・終了
		virtual void OnElectrifiedEnter(std::shared_ptr<GameObject>& other);
		virtual void OnElectrifiedExit(std::shared_ptr<GameObject>& other);

		//電力量取得
		float GeteEnergy() const { return m_energy; }

		//外部から電力を貰う
		void InjectEnergy(float amount);

	protected :
		//現在フレームの電力量
		float m_energy = 0.0f;

		//次フレーム用の電力量
		float m_nextEnergy = 0.0f;

		//接触している相手
		std::vector<std::weak_ptr<GameObject>> m_contactObjects;
	};

}
//end basecross
