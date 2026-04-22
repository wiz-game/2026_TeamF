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
		
	}
	void TextureCollision::OnDraw() {
		CreateMeshCollision();
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

	void TextureCollision::CreateAlphaMask(BufferContext& bufferContext, CoordContext& coordContext) {
		auto& app = App::GetApp();
		auto deviceResource = app->GetDeviceResources();
		auto device = deviceResource->GetD3DDevice();
		auto context = deviceResource->GetD3DDeviceContext();

		//設定されているテスクチャデータを取得(本来はインクのSRVをここで取得)
		ID3D11Texture2D* texResource = nullptr;
		D3D11_TEXTURE2D_DESC srvDesc;
		GetSrvResource(&texResource, &srvDesc);
		if (!texResource) {
			return;
		}

		coordContext.m_SizeX = srvDesc.Width;
		coordContext.m_SizeY = srvDesc.Height;

		int maskSize = coordContext.m_SizeX * coordContext.m_SizeY;

		auto object = GetGameObject();
		auto draw = object->GetComponent<SmBaseDraw>();

		//srvから情報を取得
		auto srv = draw->GetTextureResource()->GetShaderResourceView();
		//入力はテクスチャなので入力型は適当にint
		DX11ComputeShader<int> shader = DX11ComputeShader<int>();
		bufferContext = BufferContext(sizeof(int), maskSize);
		bufferContext.CreateUAV();
		bufferContext.CreateSRV();

		shader.AddUAV(bufferContext.m_UAV.Get());
		shader.AddSRV(srv.Get());

		shader.Initialize({ 8,8,1,coordContext.m_SizeX,coordContext.m_SizeY,1 });
		shader.SetShader(GenerateMaskShader::GetPtr()->GetShader());

		TextureSizeConstantData cb;
		cb.width = coordContext.m_SizeX;
		cb.height = coordContext.m_SizeY;
		shader.SetConstantBuffer(cb, TextureSizeConstantBuffer::GetPtr()->GetBuffer());

		shader.Execute();
	}

	void TextureCollision::CreateMeshCollision() {
		auto start = std::chrono::steady_clock::now();

		CoordContext context = CoordContext();
		BufferContext labelBuffer = {};
		CreateAlphaMask(labelBuffer, context);

		auto end = std::chrono::steady_clock::now();
		auto maskDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		start = std::chrono::steady_clock::now();

		vector<int> cellLabels(context.m_SizeX * context.m_SizeY, 0);
		DX11ComputeShader<int> shader = DX11ComputeShader<int>();
		int labelSize = cellLabels.size();

		BufferContext cellLabelOutput = BufferContext(sizeof(int), labelSize);
		cellLabelOutput.CreateSRV();
		cellLabelOutput.CreateUAV();

		BufferContext convertFlag = BufferContext(sizeof(int), 1);
		convertFlag.CreateUAV();

		shader.AddSRV(labelBuffer.m_SRV.Get());
		shader.AddUAV(cellLabelOutput.m_UAV.Get());
		shader.AddUAV(convertFlag.m_UAV.Get());

		shader.Initialize({ 8,8,1,context.m_SizeX,context.m_SizeY,1 });
		shader.SetShader(FloorFillShader::GetPtr()->GetShader());
		TextureSizeConstantData cb;
		cb.width = context.m_SizeX;
		cb.height = context.m_SizeY;
		shader.SetConstantBuffer(cb, TextureSizeConstantBuffer::GetPtr()->GetBuffer());

		vector<int> isConverted = {0};

		end = std::chrono::steady_clock::now();
		auto shaderInitializeDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		start = std::chrono::steady_clock::now();

		const int maxLoop = context.m_SizeX + context.m_SizeY;
		const int checkDuration = 100;
		for (int i = 0; i < maxLoop; i++) {
			shader.SetSRV(0, labelBuffer.m_SRV.Get());
			shader.SetUAV(0, cellLabelOutput.m_UAV.Get());
			shader.Execute();
			swap(labelBuffer, cellLabelOutput);
		}
		labelBuffer.ReadBuffer(cellLabels.data());
		end = std::chrono::steady_clock::now();
		auto shaderDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		start = std::chrono::steady_clock::now();
		
		unordered_map<int, vector<int>> labelGroup;
		for (int i = 0; i < labelSize; i++) {
			int label = cellLabels[i];
			if (label != -1) {
				labelGroup[label].push_back(i);
			}
		}

		end = std::chrono::steady_clock::now();
		auto mappingDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

		float allDuration = maskDuration + shaderInitializeDuration + shaderDuration + mappingDuration;
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
