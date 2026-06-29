/*!
@file Goal.h
@brief ゴール
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	class Sprite;
	class GoalEreaEffect;
	//--------------------------------------------------------------------------------------
	//	class Goal : public GameObject;
	//--------------------------------------------------------------------------------------
	class Goal : public StageObjBase
	{
		enum class State {
			Wait,			//通電待ち
			Vibrate,		//振動アニメーション
			SpriteScale,	//スプライトアニメーション
			End				//演出終了

		};

		State m_state;

		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<PNTBoneModelDraw> m_draw;
		std::shared_ptr<PNTStaticDraw> m_staticDraw;

		std::shared_ptr<Player> m_player;
		std::shared_ptr<Port> m_port = nullptr;

		std::shared_ptr<Sprite> m_resultSprite;

		std::shared_ptr<SoundItem> m_MoveSound_1;
		std::shared_ptr<SoundItem> m_MoveSound_2;


		Vec3 m_startPos;	   //初期位置
		float m_speed;
		float m_animatimer;	   //共通のタイマー
		Vec2 m_spriteStartPos2D; //スプライトの初期位置

	public:
		// 構築と破棄
		Goal(
			const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Pos,
			std::shared_ptr<Port> port
		) :
			StageObjBase(StagePtr, Scale, Rot, Pos),
			m_port(port),
			m_startPos(Pos),
			m_speed(1.5f),
			m_animatimer(0.0f),
			m_state(State::Wait)//初期状態は通電待ち
		{
		}
		virtual ~Goal()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
		void RegisterResources();

		void VibrateAnimation(float delta);
		void SpriteAnimation(float delta);
	};


	class GoalEreaEffect : public GameObject
	{
	public:
		struct InitParams
		{
			std::wstring textureKey; // テクスチャリソースのキー
			int sides; // 円形の分割数
			float height; // オーラの高さ
			float topRadius; // 上部円の半径
			float bottomRadius; // 下部円の半径
			Col4 topColor; // 上部の色
			Col4 bottomColor; // 下部の色
			Vec2 uvOffsetSpeed; // UVアニメーションの秒速
			float textureLoops; // テクスチャの「u座標」

			InitParams(const wstring& textureKey, int sides, float height, float radiusX, float radiusZ, const Col4& topColor, const Col4& bottomColor, const Vec2& speed, float textureLoops)
				: textureKey(textureKey), sides(sides), height(height), topRadius(radiusX), bottomRadius(radiusZ), topColor(topColor), bottomColor(bottomColor), uvOffsetSpeed(speed), textureLoops(textureLoops)
			{}

			InitParams()
				: InitParams(L"", 30, 1.0f, 1.0f, 1.0f, Col4(1.0f), Col4(1.0f), Vec2(0.0f), 1.0f)
			{}
		};
	private:
		std::vector<VertexPositionNormalTexture> vertices; // 頂点データを保持するベクター
		InitParams params; // 初期化パラメータを保持する構造体

	public:
		GoalEreaEffect(const std::shared_ptr<Stage>& stage, const InitParams& params)
			: GameObject(stage),
			params(params)
		{}
		void OnCreate() override;
		void OnUpdate() override;
	};
}
//end basecross
