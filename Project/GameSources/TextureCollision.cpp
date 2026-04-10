/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include <filesystem>
#include <queue>

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

	vector<TextureCollision::MaskData> TextureCollision::CreateAlphaMask(CoordContext& coordContext) {
		auto& app = App::GetApp();
		auto deviceResource = app->GetDeviceResources();
		auto device = deviceResource->GetD3DDevice();
		auto context = deviceResource->GetD3DDeviceContext();

		vector<MaskData> errorList = {};
		//設定されているテスクチャデータを取得(本来はインクのSRVをここで取得)
		ID3D11Texture2D* srvTexture = nullptr;
		D3D11_TEXTURE2D_DESC srvDesc;
		GetSrvResource(&srvTexture, &srvDesc);
		if (!srvTexture) {
			return errorList;
		}

		//読み取り用データを作成
		D3D11_TEXTURE2D_DESC desc = srvDesc;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		ID3D11Texture2D* stagingTexture = nullptr;
		auto result = device->CreateTexture2D(&desc, nullptr, &stagingTexture);
		if (FAILED(result)) {
			return errorList;
		}
		//データをコピー
		context->CopyResource(stagingTexture, srvTexture);

		//カラーデータを取得
		UINT rowPitch;
		uint8_t* data = ReadColorData(context, stagingTexture, rowPitch);
		if (!data) {
			return errorList;
		}
		coordContext.m_SizeX = desc.Width;
		coordContext.m_SizeY = desc.Height;

		vector<MaskData> alphaMasks;
		//テスト出力
		std::filesystem::path path = "test.txt";

		ofstream ofs(path);

		//データのα値をもとに2値化
		for (int y = 0; y < coordContext.m_SizeY; y++) {
			for (int x = 0; x < coordContext.m_SizeX; x++) {
				uint8_t a = data[y * rowPitch + x * 4 + 3];
				UINT mask = (a > 0 ? 1 : 0);
				alphaMasks.push_back({ mask, false });
				ofs << mask;
			}
			ofs << endl;
		}

		stagingTexture->Release();

		return alphaMasks;
	}

	vector<int> TextureCollision::BfsTree(vector<MaskData>& masks, const CoordContext& context, int start) {
		vector<int> area;
		queue<int> searchQueue;
		searchQueue.push(start);

		while (!searchQueue.empty()) {
			int search = searchQueue.front();
			searchQueue.pop();
			area.push_back(search);

			vector<int> searchIndex = {
				search + context.m_SizeX,
				search - context.m_SizeX
			};
			int coordX, coordY;
			IndexToCoord(search, context.m_SizeX, coordX, coordY);
			if (coordX > 0) {
				searchIndex.push_back(search - 1);
			}
			if (coordX < context.m_SizeX - 1) {
				searchIndex.push_back(search + 1);
			}

			for (auto& index : searchIndex) {
				if (index < 0 || index >= masks.size()) {
					continue;
				}
				int mask = masks[index].m_Mask;
				bool isVisited = masks[index].m_IsVisited;

				if (mask != 0 && !isVisited) {
					masks[index].m_IsVisited = true;
					searchQueue.push(index);
				}
			}

		}
		return area;
	}

	void TextureCollision::CreateMeshCollision() {
		CoordContext context = CoordContext();
		vector<MaskData> alphaMasks = CreateAlphaMask(context);

		vector<vector<int>> maskAreaGroup;
		
		//領域ごとに分割(この領域の数のメッシュを生成する)
		for (int i = 0; i < alphaMasks.size(); i++) {
			int x = 0, y = 0;
			IndexToCoord(i, context.m_SizeX, x, y);
			int mask = alphaMasks[i].m_Mask;
			bool isVisited = alphaMasks[i].m_IsVisited;

			if (mask != 0 && !isVisited) {
				auto area = BfsTree(alphaMasks, context, i);
				maskAreaGroup.push_back(area);
			}
		}


	}
	void TextureCollision::IndexToCoord(int index, int width, int& x, int& y) {
		x = index % width;
		y = index / width;
	}
	void TextureCollision::CoordToIndex(int& index, int x, int y, int width) {
		index = y * width + x;
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
