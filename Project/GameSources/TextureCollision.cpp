/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include <filesystem>
#include <queue>

namespace basecross{
	IMPLEMENT_DX11_COMPUTE_SHADER(GenerateMaskShader, App::GetApp()->GetShadersPath() + L"GenerateMaskShader.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(TextureSizeConstantBuffer)

	TextureCollision::TextureCollision(const shared_ptr<GameObject>& ptr):Collision(ptr){}

	void TextureCollision::OnCreate() {
		CreateMeshCollision();
	}
	void TextureCollision::OnDraw() {

	}
	void TextureCollision::GetSrvResource(ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* desc) {
		auto object = GetGameObject();
		auto draw = object->GetComponent<SmBaseDraw>();

		//srvから情報を取得
		*srv = draw->GetTextureResource()->GetShaderResourceView().Get();
		ID3D11Resource* gpuResource = nullptr;
		(*srv)->GetResource(&gpuResource);

		ID3D11Texture2D* texture = nullptr;

		gpuResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)texture);
		texture->GetDesc(desc);
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

	vector<MaskData> TextureCollision::CreateAlphaMask(CoordContext& coordContext) {
		auto& app = App::GetApp();
		auto deviceResource = app->GetDeviceResources();
		auto device = deviceResource->GetD3DDevice();
		auto context = deviceResource->GetD3DDeviceContext();

		vector<MaskData> errorList = {};
		//設定されているテスクチャデータを取得(本来はインクのSRVをここで取得)
		ID3D11ShaderResourceView* srv = nullptr;
		D3D11_TEXTURE2D_DESC srvDesc;
		GetSrvResource(&srv, &srvDesc);
		if (!srv) {
			return errorList;
		}

		coordContext.m_SizeX = srvDesc.Width;
		coordContext.m_SizeY = srvDesc.Height;

		int maskSize = coordContext.m_SizeX * coordContext.m_SizeY;
		auto start = std::chrono::steady_clock::now();
		//入力はテクスチャなので入力型は適当にint
		DX11ComputeShader<int,MaskData> shader = DX11ComputeShader<int,MaskData>();
		shader.Initialize(256, maskSize, maskSize);
		shader.SetShader(GenerateMaskShader::GetPtr()->GetShader());
		shader.UseTexture(srv);

		TextureSizeConstantData cb;

		cb.width = coordContext.m_SizeX;
		shader.SetConstantBuffer(&cb, TextureSizeConstantBuffer::GetPtr()->GetBuffer());

		vector<MaskData> alphaMasks = shader.Execute({});
		auto end = std::chrono::steady_clock::now();

		auto duration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

		//テスト出力
		std::filesystem::path path = "test.txt";

		ofstream ofs(path);

		//データのα値をもとに2値化
		for (int y = 0; y < coordContext.m_SizeY; y++) {
			for (int x = 0; x < coordContext.m_SizeX; x++) {
				ofs << alphaMasks[y * coordContext.m_SizeX + x].m_Mask;
			}
			ofs << endl;
		}

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
