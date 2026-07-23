/*!
@file TrapDoorAxis.cpp
@brief トラップドアの実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	TrapDoorB::TrapDoorB(const shared_ptr<Stage>& ptr, const Vec3& scale, const Vec3& rotation, const Vec3& position, const TrapDoorAxisDesc& desc):
		StageObjBase(ptr,scale,rotation,position,L"TrapDoor"),m_AxisDesc(desc){}
	TrapDoorB::~TrapDoorB(){}

	void TrapDoorB::OnCreate() {
		try
		{
			auto& app = App::GetApp();
			auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
			app->RegisterTexture(L"MoveFloor", path + L"MoveFloor.png"); // 画像ファイルを読み込んでアセットとして登録する
			app->RegisterTexture(L"Black", path + L"Black.png"); // 画像ファイルを読み込んでアセットとして登録する
		}
		catch (...) {
		}
		m_CurrentRotation = m_rot;

		shared_ptr<Transform> transform = GetComponent<Transform>();

		//m_pos.y = -9.2f - m_scale.y;
		transform->SetScale(m_scale);
		transform->SetRotation(m_CurrentRotation);
		ApplyCurrentPosition();

		//Drawコンポーネント
		shared_ptr<Texture2DrawComp> draw = AddComponent<Texture2DrawComp>();
		draw->CreateTexture(m_scale.x, m_scale.z);
		draw->SetMeshResource(L"DEFAULT_CUBE");
		draw->SetTextureResource(L"MoveFloor");
		draw->SetTexture2(L"Black");
		draw->SetDiffuse(Col4(1, 1, 1, 1));

		draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CUBE");

		auto coll = AddComponent<CollisionObb>();
		coll->SetFixed(true);

		AddComponent<TextureCollision>();
	}
	void TrapDoorB::OnUpdate() {
		m_IsConnected = m_AxisDesc.port != nullptr ? m_AxisDesc.port->GetConnect() : true;
		Vec3 rotateAmount = {};

		float delta = App::GetApp()->GetElapsedTime();
		float deltaSpeed = abs(m_AxisDesc.speed);
		if (m_IsConnected) {
			if(m_rot.x != 0) rotateAmount.x = m_rot.x < 0 ? 1 : -1;
			if(m_rot.z != 0) rotateAmount.z = m_rot.z < 0 ? 1 : -1;
			rotateAmount *= deltaSpeed;

			if (abs(rotateAmount.x) > abs(m_CurrentRotation.x)) rotateAmount.x = -m_CurrentRotation.x;
			if (abs(rotateAmount.z) > abs(m_CurrentRotation.z)) rotateAmount.z = -m_CurrentRotation.z;
		}
		else {
			if (m_rot.x != 0) rotateAmount.x = m_rot.x > 0 ? 1 : -1;
			if (m_rot.z != 0) rotateAmount.z = m_rot.z > 0 ? 1 : -1;

			rotateAmount *= deltaSpeed;
			Vec3 diff = m_rot - m_CurrentRotation;
			if (abs(rotateAmount.x) > abs(diff.x)) rotateAmount.x = diff.x;
			if (abs(rotateAmount.z) > abs(diff.z)) rotateAmount.z = diff.z;
		}
		m_CurrentRotation += rotateAmount;
		shared_ptr<Transform> transform = GetComponent<Transform>();
		transform->SetRotation(m_CurrentRotation);
		ApplyCurrentPosition();
	}
	void TrapDoorB::ApplyCurrentPosition() {
		shared_ptr<Transform> transform = GetComponent<Transform>();
		Vec3 forward = transform->GetForward();
		if (m_AxisDesc.axis == MoveAxis::Z) {
			forward = transform->GetRight();
		}
		switch (m_AxisDesc.axis) {
		case MoveAxis::X:
			m_CurrentPosition = m_pos + forward * m_scale.z * 0.5f;
			break;
		case MoveAxis::Z:
			m_CurrentPosition = m_pos + forward * m_scale.x * 0.5f;
			break;
		}

		transform->SetPosition(m_CurrentPosition);

	}

	//初期化
	void TrapDoorAxis::OnCreate()
	{
		m_trans = GetComponent<Transform>();
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);

		//m_staticDraw = AddComponent<PNTStaticDraw>();
		//m_staticDraw->SetMeshResource(L"DEFAULT_SPHERE");

		//auto coll = AddComponent<CollisionObb>();
		//coll->SetDrawActive(true);

		//初期のRotationを決めるためのOffsetObjectを作成
		auto offsetObj = GetStage()->AddGameObject<OffsetObject>(GetThis<TrapDoorAxis>());
		auto offsetTrans = offsetObj->GetComponent<Transform>();
		offsetTrans->SetRotation(m_initialRotation);

		//TrapDoorの作成と位置をずらすためのObject
		m_trapDoor = GetStage()->AddGameObject<TrapDoor>(offsetObj, m_trapDoorScale);
		
		//X軸回転かつスピードがマイナスの時は
		if (m_moveAxis == MoveAxis::X && m_speed < 0)
		{
			m_trapDoor->GetComponent<Transform>()->SetRotation(Vec3(0,XM_PI,0));//オブジェクトのYを180度回転させる
		}
	}

	void TrapDoorAxis::OnUpdate()
	{
		if (m_port)
		{
			isConnect = m_port->GetConnect();
		}
		else
		{
			isConnect = true;
		}

		auto scene = App::GetApp()->GetScene<Scene>();
		float delta = App::GetApp()->GetElapsedTime();

		const float LIMIT_ANGLE = (m_speed > 0) ? XM_PIDIV2 : -XM_PIDIV2; //90度の制限

		//通電していれば動く
		if (isConnect)
		{
			m_isMove = true;
			m_trapDoor->SetIsMove(true);
			switch (m_moveAxis)
			{
			case MoveAxis::X:
			{
				bool canMove = (m_speed > 0) ? (m_rotateVec.x < LIMIT_ANGLE) : (m_rotateVec.x > LIMIT_ANGLE);
				if (canMove)
				{
					m_rotateVec.x += m_speed;
					// 制限角度を超えないようにする
					if (m_speed > 0 && m_rotateVec.x > LIMIT_ANGLE) m_rotateVec.x = LIMIT_ANGLE;
					if (m_speed < 0 && m_rotateVec.x < LIMIT_ANGLE) m_rotateVec.x = LIMIT_ANGLE;
				}

				break;
			}
			case MoveAxis::Y:
			{
				bool canMoveY = (m_speed > 0) ? (m_rotateVec.y < LIMIT_ANGLE) : (m_rotateVec.y > LIMIT_ANGLE);
				if (canMoveY)
				{
					m_rotateVec.y += m_speed;
					if (m_speed > 0 && m_rotateVec.y > LIMIT_ANGLE) m_rotateVec.y = LIMIT_ANGLE;
					if (m_speed < 0 && m_rotateVec.y < LIMIT_ANGLE) m_rotateVec.y = LIMIT_ANGLE;

				}

				break;
			}
			case MoveAxis::Z:
			{
				bool canMoveZ = (m_speed > 0) ? (m_rotateVec.z < LIMIT_ANGLE) : (m_rotateVec.z > LIMIT_ANGLE);
				if (canMoveZ)
				{
					m_rotateVec.z += m_speed;
					if (m_speed > 0 && m_rotateVec.z > LIMIT_ANGLE) m_rotateVec.z = LIMIT_ANGLE;
					if (m_speed < 0 && m_rotateVec.z < LIMIT_ANGLE) m_rotateVec.z = LIMIT_ANGLE;
				}

				break;
			}
			}

			m_trans->SetRotation(m_rotateVec);
		}
		else
		{
			// 0に戻る処理（正負どちらからでも0に近づける）
			if (std::abs(m_rotateVec.x) > 0.001f) {
				float returnDir = (m_rotateVec.x > 0) ? -1.0f : 1.0f;
				m_rotateVec.x += std::abs(m_speed) * returnDir;
				if ((returnDir < 0 && m_rotateVec.x < 0) || (returnDir > 0 && m_rotateVec.x > 0)) {
					m_rotateVec.x = 0;
				}
			}

			if (std::abs(m_rotateVec.y) > 0.001f) {
				float returnDir = (m_rotateVec.y > 0) ? -1.0f : 1.0f;
				m_rotateVec.y += std::abs(m_speed) * returnDir;
				if ((returnDir < 0 && m_rotateVec.y < 0) || (returnDir > 0 && m_rotateVec.y > 0)) {
					m_rotateVec.y = 0;
				}
			}

			if (std::abs(m_rotateVec.z) > 0.001f) {
				float returnDir = (m_rotateVec.z > 0) ? -1.0f : 1.0f;
				m_rotateVec.z += std::abs(m_speed) * returnDir;
				if ((returnDir < 0 && m_rotateVec.z < 0) || (returnDir > 0 && m_rotateVec.z > 0)) {
					m_rotateVec.z = 0;
				}
			}

			m_isMove = false;
			m_trapDoor->SetIsMove(false);

			m_trans->SetRotation(m_rotateVec);
		}
	}
}
//end basecross
