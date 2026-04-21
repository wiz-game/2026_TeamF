/*!
@file Player.h
@brief �ｽL�ｽ�ｽ�ｽ�ｽ�ｽN�ｽ^�ｽ[�ｽﾈゑｿｽ
*/

#pragma once
#include "stdafx.h"
#include "MainCamera.h"
#include "PNTDXModelDraw.h"
#include "InkCloud.h"

namespace basecross {

class UpDownFloor;

	// GameObject�ｽN�ｽ�ｽ�ｽX�ｽ�ｽ�ｽp�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽuPlayer�ｽv�ｽN�ｽ�ｽ�ｽX�ｽ�ｽ�ｽ`
	class Player : public GameObject // GameObject�ｽN�ｽ�ｽ�ｽX�ｽﾌ継�ｽ�ｽ�ｽy�ｽK�ｽ{�ｽz
	{
		std::weak_ptr<MainCamera> m_camera;
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<UpDownFloor> m_currentFloor;//現在乗っている床
		
		std::shared_ptr<InkCloud> m_targetCloud;
		float m_height;
		float m_radius;

		float m_moveSpeed;
		float m_maxSpeed;	//譛鬮倬
		float m_accel;
		Vec3 m_pos;
		Vec3 m_forward;
		Vec3 m_velocity;

		float m_ink;	//繧､繝ｳ繧ｯ谿矩㍼
		float m_inkMax;	//繧､繝ｳ繧ｯ谿矩㍼譛螟ｧ蛟､
		float m_inkDecrease;	//繧､繝ｳ繧ｯ貂帛ｰ鷹㍼
		float m_fade;	//繧､繝ｳ繧ｯ縺ｮ謗繧
		bool m_isDraw;	//繧､繝ｳ繧ｯ縺悟�縺帙ｋ縺九←縺�°

		Vec3 m_externalVelocity; // �ｽO�ｽ�ｽ�ｽ�ｽ�ｽ�ｽﾌ托ｿｽ�ｽx
		
	public :
		// �ｽX�ｽe�ｽ[�ｽW�ｽ�ｽ�ｽ�ｽ�ｽ�ｽ�ｽﾉゑｿｽ�ｽ�ｽ�ｽR�ｽ�ｽ�ｽX�ｽg�ｽ�ｽ�ｽN�ｽ^�ｽy�ｽK�ｽ{�ｽz
		Player(const std::shared_ptr<Stage>& stage) :
			GameObject(stage), // �ｽX�ｽe�ｽ[�ｽW�ｽ�ｽGameObject�ｽﾉ渡�ｽ�ｽ�ｽy�ｽK�ｽ{�ｽz
			m_height(1.0f),
			m_radius(0.49f),
			m_moveSpeed(0.0f),
			m_maxSpeed(5.0f),
			m_accel(0.99f),
			m_pos(Vec3(0.0f, 0.5f, 0.0f)),
			m_forward(Vec3(0)),
			m_velocity(Vec3(0)),
			m_ink(0.0f),
			m_inkMax(10.0f),
			m_inkDecrease(10.0f),
			m_isDraw(true),
			m_fade(0.0f),
			m_externalVelocity(Vec3(0))
		{
		}

		void OnCreate() override; // �ｽ�ｽ�ｽ�ｽ�ｽﾝ抵ｿｽp�ｽﾌ関撰ｿｽ(Unity�ｽ�ｽStart�ｽ�ｽ�ｽ\�ｽb�ｽh�ｽﾉ托ｿｽ�ｽ�ｽ)
		void OnUpdate() override; // �ｽ�ｽ�ｽt�ｽ�ｽ�ｽ[�ｽ�ｽ�ｽ�ｽ�ｽs�ｽ�ｽ�ｽ�ｽ�ｽﾖ撰ｿｽ(Unity�ｽ�ｽUpdate�ｽ�ｽ�ｽ\�ｽb�ｽh�ｽﾉ托ｿｽ�ｽ�ｽ)

		void OnMove();
		void DropInk();
		void OnDied();


		void OnCollisionEnter(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExcute(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExit(std::shared_ptr<GameObject>& obj)override;

		void UpdateMoveFloor();

		// �ｽO�ｽ�ｽ�ｽ�ｽ�ｽ�ｽﾌ移難ｿｽ�ｽ�ｽ�ｽ�ｽ�ｽZ�ｽ�ｽ�ｽ�ｽﾖ撰ｿｽ
		//void AddExternalMove(const Vec3& move);
		//void SetExternalVelocity(const Vec3& v) { m_externalVelocity = v; }
	};

}
//end basecross

