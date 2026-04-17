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
	IMPLEMENT_DX11_COMPUTE_SHADER(FloorFillShader, App::GetApp()->GetShadersPath() + L"FloorFill.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(TextureSizeConstantBuffer)

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
		auto srv = draw->GetTextureResource()->GetShaderResourceView().Get();
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

	vector<MaskData> TextureCollision::CreateAlphaMask(CoordContext& coordContext) {
		auto start = std::chrono::steady_clock::now();
		auto& app = App::GetApp();
		auto deviceResource = app->GetDeviceResources();
		auto device = deviceResource->GetD3DDevice();
		auto context = deviceResource->GetD3DDeviceContext();

		vector<MaskData> errorList = {};
		//設定されているテスクチャデータを取得(本来はインクのSRVをここで取得)
		ID3D11Texture2D* texResource = nullptr;
		D3D11_TEXTURE2D_DESC srvDesc;
		GetSrvResource(&texResource, &srvDesc);
		if (!texResource) {
			return errorList;
		}

		coordContext.m_SizeX = srvDesc.Width;
		coordContext.m_SizeY = srvDesc.Height;

		int maskSize = coordContext.m_SizeX * coordContext.m_SizeY;

		auto object = GetGameObject();
		auto draw = object->GetComponent<SmBaseDraw>();

		//srvから情報を取得
		auto srv = draw->GetTextureResource()->GetShaderResourceView().Get();

		vector<MaskData> alphaMasks = {};
		//入力はテクスチャなので入力型は適当にint
		DX11ComputeShader<int> shader = DX11ComputeShader<int>();
		BufferContext maskBuffer = BufferContext(sizeof(MaskData), maskSize);
		maskBuffer.CreateUAV();

		shader.AddUAV(maskBuffer.m_UAV.Get());
		shader.AddSRV(srv);

		shader.Initialize({ 8,8,1,coordContext.m_SizeX,coordContext.m_SizeY,1 });
		shader.SetShader(GenerateMaskShader::GetPtr()->GetShader());

		TextureSizeConstantData cb;
		cb.width = coordContext.m_SizeX;
		cb.height = coordContext.m_SizeY;
		shader.SetConstantBuffer(cb, TextureSizeConstantBuffer::GetPtr()->GetBuffer());

		shader.Execute({});

		shader.GetResult(alphaMasks, maskBuffer);

		auto end = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

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
				(int)(search + context.m_SizeX),
				(int)(search - context.m_SizeX)
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

		//auto start = std::chrono::steady_clock::now();

		//vector<int> cellLabels(alphaMasks.size(), 0);
		//for (int i = 0; i < cellLabels.size(); i++) {
		//	//透明部分は-1
		//	if (alphaMasks[i].m_Mask == 0) {
		//		cellLabels[i] = -1;
		//		continue;
		//	}
		//	cellLabels[i] = i;
		//}

		//auto end = std::chrono::steady_clock::now();
		//auto initializeDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		//start = std::chrono::steady_clock::now();

		//DX11ComputeShader<int> shader = DX11ComputeShader<int>();
		//shader.Initialize({ 8,8,1,context.m_SizeX,context.m_SizeY,1 }, cellLabels.size());
		//shader.SetShader(FloorFillShader::GetPtr()->GetShader());
		//TextureSizeConstantData cb;
		//cb.width = context.m_SizeX;
		//cb.height = context.m_SizeY;
		//shader.SetConstantBuffer(cb, TextureSizeConstantBuffer::GetPtr()->GetBuffer());

		//vector<int> isConverted = {0};

		//shader.RegisterResult(ResultBufferContext(cellLabels.data(), sizeof(cellLabels[0]), cellLabels.size()));
		//shader.RegisterResult(ResultBufferContext(isConverted.data(), sizeof(isConverted[0]), 1));

		//end = std::chrono::steady_clock::now();
		//auto shaderInitializeDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		//start = std::chrono::steady_clock::now();

		//const int maxLoop = context.m_SizeX * context.m_SizeY;
		//for (int i = 0; i < maxLoop; i++) {
		//	shader.ResetUAV(1);
		//	shader.Execute(cellLabels);
		//	shader.GetResult(cellLabels, 0);
		//	shader.GetResult(isConverted, 1);
		//	//最後の要素はラベルが0のときにループを抜けるためのダミー
		//	if (isConverted[0] != 1) {
		//		break;
		//	}
		//}
		//end = std::chrono::steady_clock::now();
		//auto shaderDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		//start = std::chrono::steady_clock::now();

		//unordered_map<int, vector<int>> labelGroup;
		//for (auto& label : cellLabels) {
		//	if (label == -1) continue;
		//	labelGroup[label].push_back(label);
		//}

		//end = std::chrono::steady_clock::now();
		//auto mappingDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

		int checker = 0;
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
