#pragma once
#include "stdafx.h"

namespace basecross {
	class BeltConveyorTex : public GameObject
	{
	public:
		struct InitParams
		{
			std::wstring textureKey; // テクスチャリソースのキー
			int sides; // 円形の分割数
			float height; // オーラの高さ
			float radiusX; // ｘ方向の半径
			float radiusZ; // ｚ方向の半径
			Col4 topColor; // 上部の色
			Col4 bottomColor; // 下部の色
			Vec2 uvOffsetSpeed; // UVアニメーションの秒速
			float textureLoops; // テクスチャの「u座標」

			InitParams(const wstring& textureKey, int sides, float height, float radiusX, float radiusZ, const Col4& topColor, const Col4& bottomColor, const Vec2& speed, float textureLoops)
				: textureKey(textureKey), sides(sides), height(height), radiusX(radiusX), radiusZ(radiusZ), topColor(topColor), bottomColor(bottomColor), uvOffsetSpeed(speed), textureLoops(textureLoops)
			{
			}

			InitParams()
				: InitParams(L"", 30, 1.0f, 1.0f, 1.0f, Col4(1.0f), Col4(1.0f), Vec2(0.0f), 1.0f)
			{
			}


		};

		
	private:
		std::vector<VertexPositionNormalTexture> vertices; // 頂点データを保持するベクター
		InitParams params; // 初期化パラメータを保持する構造体

	public:
		BeltConveyorTex(const std::shared_ptr<Stage>& stage ,const InitParams& params)
			: GameObject(stage), 
			params(params)
		{
		}
		void OnCreate() override;
		void OnUpdate() override;


		//速度の変更
		Vec2 SetUVOffsetSpeed(const Vec2& speed) {
			params.uvOffsetSpeed = speed;
			return params.uvOffsetSpeed;
		}
	};
}