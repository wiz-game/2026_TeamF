/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include <filesystem>
namespace basecross{
	TextureCollision::TextureCollision(const shared_ptr<GameObject>& ptr):Collision(ptr){}

	void TextureCollision::OnCreate() {
		CreateMeshCollision();
	}
	void TextureCollision::OnDraw() {

	}
	void TextureCollision::GetSrvResource(ID3D11Texture2D** texture, D3D11_TEXTURE2D_DESC* desc) {
		auto object = GetGameObject();
		auto draw = object->GetComponent<SmBaseDraw>();

		//srvから情報を取得
		auto srv = draw->GetTextureResource()->GetShaderResourceView();
		ID3D11Resource* gpuResource = nullptr;
		srv->GetResource(&gpuResource);

		gpuResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)texture);
		(*texture)->GetDesc(desc);
	}

	uint8_t* TextureCollision::ReadColorData(ID3D11DeviceContext2* context, ID3D11Texture2D* texture, UINT& rowPitch) {
		//情報を読み取り
		D3D11_MAPPED_SUBRESOURCE mapped;
		auto result = context->Map(texture, 0, D3D11_MAP_READ, 0, &mapped);
		if (FAILED(result)) {
			return nullptr;
		}

		uint8_t* data = (uint8_t*)mapped.pData;
		rowPitch = mapped.RowPitch;

		context->Unmap(texture, 0);

		return data;
	}

	vector<vector<UINT>>& TextureCollision::CreateAlphaMask() {
		auto& app = App::GetApp();
		auto deviceResource = app->GetDeviceResources();
		auto device = deviceResource->GetD3DDevice();
		auto context = deviceResource->GetD3DDeviceContext();

		//設定されているテスクチャデータを取得(本来はインクのSRVをここで取得)
		ID3D11Texture2D* srvTexture = nullptr;
		D3D11_TEXTURE2D_DESC srvDesc;
		GetSrvResource(&srvTexture, &srvDesc);
		if (!srvTexture) {
			return;
		}

		//読み取り用データを作成
		D3D11_TEXTURE2D_DESC desc = srvDesc;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		ID3D11Texture2D* stagingTexture = nullptr;
		auto result = device->CreateTexture2D(&desc, nullptr, &stagingTexture);
		if (FAILED(result)) {
			return;
		}
		//データをコピー
		context->CopyResource(stagingTexture, srvTexture);

		//カラーデータを取得
		UINT rowPitch;
		uint8_t* data = ReadColorData(context, stagingTexture, rowPitch);
		if (!data) {
			return;
		}
		UINT width = desc.Width;
		UINT height = desc.Height;

		vector<vector<UINT>> alphaMasks;
		//テスト出力
		std::filesystem::path path = "test.txt";

		ofstream ofs(path);

		//データのα値をもとに2値化
		for (int y = 0; y < height; y++) {
			alphaMasks.push_back({});
			for (int x = 0; x < width; x++) {
				uint8_t a = data[y * rowPitch + x * 4 + 3];
				UINT mask = (a > 0 ? 1 : 0);
				alphaMasks[y].push_back(mask);
				ofs << mask;
			}
			ofs << endl;
		}

		stagingTexture->Release();

		return alphaMasks;
	}

	void TextureCollision::CreateMeshCollision() {
		vector<vector<UINT>> alphaMasks = CreateAlphaMask();


	}


	bool TextureCollision::SimpleCollisionCall(const shared_ptr<Collision>& Src) {
		return false;
	}
	void TextureCollision::CollisionCall(const shared_ptr<Collision>& Src) {

	}
	bsm::Vec3 TextureCollision::GetCenterPosition()const {
		return Vec3();
	}
	AABB TextureCollision::GetEnclosingAABB()const {
		return AABB();
	}
	AABB TextureCollision::GetWrappedAABB()const {
		return AABB();
	}

}
//end basecross
