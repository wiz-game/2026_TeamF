#pragma once

// -----------------------------------------------------------------------
// Jolt Physics の設定定義
// ※ 必ず #include <Jolt/Jolt.h> の前に定義する必要があります
// -----------------------------------------------------------------------

// Debug / Release 共通で必要な設定
#define JPH_PROFILE_ENABLED
#define JPH_DEBUG_RENDERER
#define JPH_FLOATING_POINT_EXCEPTIONS_ENABLED
#define JPH_OBJECT_STREAM
#define JPH_DISABLE_CUSTOM_ALLOCATOR

// -----------------------------------------------------------------------
// Jolt 本体のインクルード
// -----------------------------------------------------------------------
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
