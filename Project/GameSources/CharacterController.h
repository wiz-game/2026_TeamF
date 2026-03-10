#pragma once
#include "Project.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

#include "JoltLayers.h"

namespace basecross
{
    /**
     * Jolt PhysicsのCharacterVirtualを用いたキャラクターコントローラー
     * プレイヤーなどの移動制御に使用します。
     */
    class CharacterController : public Component
    {
    public:
        struct Settings {
            float height = 1.8f;           // キャラクターの高さ
            float radius = 0.3f;           // カプセルの半径
            float mass = 70.0f;            // 質量
            float maxSlopeAngle = 45.0f;   // 登れる最大斜面角度（度）
            float maxStrength = 100.0f;    // 押す力の最大値
            float predictiveContactDistance = 0.1f; // 予測接触距離
            float penetrationRecoverySpeed = 1.0f;  // 貫通回復速度
            JPH::ObjectLayer objectLayer = Layers::MOVING;
        };

        explicit CharacterController(const std::shared_ptr<GameObject>& owner);
        virtual ~CharacterController();

        // コピー禁止
        CharacterController(const CharacterController&) = delete;
        CharacterController& operator=(const CharacterController&) = delete;

        void Initialize(const Settings& settings);

        virtual void OnCreate() override;
        virtual void OnUpdate() override;
        virtual void OnDraw() override {}

        // 移動制御
        void SetLinearVelocity(const Vec3& velocity);
        Vec3 GetLinearVelocity() const;
        void SetPosition(const Vec3& position);
        Vec3 GetPosition() const;

        // 状態取得
        bool IsOnGround() const;
        bool IsSupported() const;
        Vec3 GetGroundNormal() const;

        // 内部オブジェクトへのアクセス
        JPH::CharacterVirtual* GetCharacter() { return m_character.get(); }

    private:
        void UpdateCharacter(float deltaTime);

        std::unique_ptr<JPH::CharacterVirtual> m_character;
        Settings m_settings;
        JPH::PhysicsSystem* m_pPhysicsSystem = nullptr;
        Vec3 m_desiredVelocity;
    };
}