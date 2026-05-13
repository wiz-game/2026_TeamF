#include "stdafx.h"
#include "BeltConveyorTex.h"

namespace basecross
{
	void BeltConveyorTex::OnCreate()
	{
		// 頂点データの作成する
		for (int i = 0; i < params.sides + 1; i++) // 1周して戻ってきて、出発点にも頂点が必要なため、分割数より1回多くループする
		{
			float fSides = static_cast<float>(params.sides); // 円の分割数をfloat型に変換する
			float radian = XMConvertToRadians(i * 360.0f / fSides); // 頂点を配置する角度を算出する

			float x = params.radiusX * cosf(radian);
			float z = params.radiusZ * sinf(radian);

			float halfH = params.height / 2.0f; // ベルトコンベアの板ポリの中心を原点にするために、高さの半分を計算する

			//テクスチャの向きを９０度回転させるために、x方向の頂点とz方向の頂点を両方作成する
			vertices.push_back({ Vec3(x, halfH, z), Vec2(0.0f, i / fSides) }); // x方向の頂点
			vertices.push_back({ Vec3(x, -halfH, z), Vec2(params.textureLoops, i / fSides) }); // z方向の頂点
		}

		uint16_t baseIndices[] = {
			1, 0, 2,
			1, 2, 3,
		};

		std::vector<uint16_t> indices;
		for (int i = 0; i < params.sides; i++)
		{

			for (auto baseIndex : baseIndices)
			{
				indices.push_back(baseIndex + 2 * i); // ベースインデックスを２ずつずらして設定していく
			}
		}

		// ドローコンポーネントを追加する
		auto drawComp = AddComponent<BcPTStaticDraw>();
		drawComp->CreateOriginalMesh(vertices, indices);
		drawComp->SetOriginalMeshUse(true);	// 独自メッシュ(プログラム内で作成)を使用することを宣言する
		if (params.textureKey != L"")
		{
			drawComp->SetTextureResource(params.textureKey);// 使用するテクスチャを設定する
		}
		//drawComp->SetBlendState(BlendState::Additive);// 加算合成されるように設定する
		drawComp->SetDepthStencilState(DepthStencilState::Read);// 他の板ポリと干渉しないようにする
		
		drawComp->SetSamplerState(SamplerState::AnisotropicWrap);// 異方性フィルタリングを使用する

		auto trans = GetComponent<Transform>();
		//trans->SetRotation(Vec3(0.0f, 0.0f, XM_PIDIV2));// ベルトコンベアの板ポリを横向きにする

		SetAlphaActive(true); // 透過処理を有効にする
	}

	void BeltConveyorTex::OnUpdate()
	{
		auto& app = App::GetApp();
		float delta = app->GetElapsedTime();
		for (auto& vertex : vertices) // テクスチャアニメーションさせる
		{
			vertex.textureCoordinate += delta * params.uvOffsetSpeed; // すべての頂点のUVを秒速でずらす
		}
		auto drawComp = GetComponent<BcPTStaticDraw>();
		drawComp->UpdateVertices(vertices); // 頂点データの変更をドローコンポーネントに伝える
	}
}