#include "stdafx.h"
#include "CharacterController.h"
#include "JoltManager.h"

namespace basecross
{
    CharacterController::CharacterController(const shared_ptr<GameObject>& owner)
        : Component(owner),
          m_desiredVelocity(0, 0, 0)
    {
    }

    CharacterController::~CharacterController()
    {
        m_character.reset();
    }

    void CharacterController::Initialize(const Settings& settings)
    {
        m_settings = settings;

        m_pPhysicsSystem = JoltManager::GetActiveSystem();
        if (!m_pPhysicsSystem) return;

        // Transformから初期位置を取得
        auto transComp = GetGameObject()->GetComponent<Transform>();
        if (!transComp) return;

        Vec3 pos = transComp->GetPosition();
        Quat q = transComp->GetQuaternion();

        // カプセル形状を作成（半分の高さ - 半径がカプセルの円筒部分）
        float halfHeight = (m_settings.height * 0.5f) - m_settings.radius;
        if (halfHeight < 0.0f) halfHeight = 0.0f;

        JPH::RefConst<JPH::Shape> standingShape = JPH::RotatedTranslatedShapeSettings(
            JPH::Vec3(0, m_settings.height * 0.5f, 0),
            JPH::Quat::sIdentity(),
            new JPH::CapsuleShape(halfHeight, m_settings.radius)
        ).Create().Get();

        // CharacterVirtualの設定
        JPH::CharacterVirtualSettings characterSettings;
        characterSettings.mShape = standingShape;
        characterSettings.mMass = m_settings.mass;
        characterSettings.mMaxSlopeAngle = JPH::DegreesToRadians(m_settings.maxSlopeAngle);
        characterSettings.mMaxStrength = m_settings.maxStrength;
        characterSettings.mPredictiveContactDistance = m_settings.predictiveContactDistance;
        characterSettings.mPenetrationRecoverySpeed = m_settings.penetrationRecoverySpeed;
        characterSettings.mUp = JPH::Vec3::sAxisY();
        characterSettings.mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -m_settings.radius);

        // CharacterVirtualを作成
        m_character = std::make_unique<JPH::CharacterVirtual>(
            &characterSettings,
            JPH::RVec3(pos.x, pos.y, pos.z),
            JPH::Quat(q.x, q.y, q.z, q.w),
            m_pPhysicsSystem
        );

        // レイヤーの設定
        m_character->SetCharacterVsCharacterCollision(nullptr);
    }

    void CharacterController::OnCreate()
    {
    }

    void CharacterController::OnUpdate()
    {
        if (!m_pPhysicsSystem || !m_character) return;

        // デルタタイムの取得（固定値を使用、実際のアプリではApp::GetApp()->GetElapsedTime()などで取得）
        float deltaTime = App::GetApp()->GetElapsedTime();

        UpdateCharacter(deltaTime);

        // Transformを更新
        auto pTransform = GetGameObject()->GetComponent<Transform>();
        if (pTransform) {
            JPH::RVec3 joltPos = m_character->GetPosition();
            pTransform->SetPosition(
                static_cast<float>(joltPos.GetX()),
                static_cast<float>(joltPos.GetY()),
                static_cast<float>(joltPos.GetZ())
            );
        }
    }

    void CharacterController::UpdateCharacter(float deltaTime)
    {
        if (!m_character) return;

        // 重力を適用
        JPH::Vec3 gravity = m_pPhysicsSystem->GetGravity();
        JPH::Vec3 currentVelocity = m_character->GetLinearVelocity();

        JPH::Vec3 newVelocity;

        // 地面にいる場合
        if (m_character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround) {
            // ジャンプ中（上向きの速度がある）場合はY速度を維持
            if (currentVelocity.GetY() > 0.0f) {
                newVelocity = JPH::Vec3(m_desiredVelocity.x, currentVelocity.GetY(), m_desiredVelocity.z);
            }
            else {
                // 地面に静止している場合はY速度を0に
                newVelocity = JPH::Vec3(m_desiredVelocity.x, 0.0f, m_desiredVelocity.z);
            }
        }
        else {
            // 空中では重力を加算
            newVelocity = currentVelocity + gravity * deltaTime;
            newVelocity.SetX(m_desiredVelocity.x);
            newVelocity.SetZ(m_desiredVelocity.z);
        }

        m_character->SetLinearVelocity(newVelocity);

        // 衝突判定用のフィルター
        JPH::DefaultBroadPhaseLayerFilter broadPhaseFilter(
            m_pPhysicsSystem->GetObjectVsBroadPhaseLayerFilter(),
            m_settings.objectLayer
        );

        JPH::DefaultObjectLayerFilter objectFilter(
            m_pPhysicsSystem->GetObjectLayerPairFilter(),
            m_settings.objectLayer
        );

        JPH::BodyFilter bodyFilter;
        JPH::ShapeFilter shapeFilter;
        JPH::TempAllocatorMalloc tempAllocatorMalloc;

        // 床の速度を反映
        m_character->UpdateGroundVelocity();

        // 更新を実行
        m_character->Update(
            deltaTime,
            gravity,
            broadPhaseFilter,
            objectFilter,
            bodyFilter,
            shapeFilter,
            tempAllocatorMalloc
        );
    }

    void CharacterController::SetLinearVelocity(const Vec3& velocity)
    {
        m_desiredVelocity = velocity;
    }

    Vec3 CharacterController::GetLinearVelocity() const
    {
        if (!m_character) return Vec3(0, 0, 0);
        JPH::Vec3 vel = m_character->GetLinearVelocity();
        return Vec3(vel.GetX(), vel.GetY(), vel.GetZ());
    }

    void CharacterController::SetPosition(const Vec3& position)
    {
        if (!m_character) return;
        m_character->SetPosition(JPH::RVec3(position.x, position.y, position.z));
    }

    Vec3 CharacterController::GetPosition() const
    {
        if (!m_character) return Vec3(0, 0, 0);
        JPH::RVec3 pos = m_character->GetPosition();
        return Vec3(
            static_cast<float>(pos.GetX()),
            static_cast<float>(pos.GetY()),
            static_cast<float>(pos.GetZ())
        );
    }

    bool CharacterController::IsOnGround() const
    {
        if (!m_character) return false;
        return m_character->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround;
    }

    bool CharacterController::IsSupported() const
    {
        if (!m_character) return false;
        return m_character->IsSupported();
    }

    Vec3 CharacterController::GetGroundNormal() const
    {
        if (!m_character) return Vec3(0, 1, 0);
        JPH::Vec3 normal = m_character->GetGroundNormal();
        return Vec3(normal.GetX(), normal.GetY(), normal.GetZ());
    }
}