/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include <filesystem>
namespace basecross {
	string DebugLog::m_SaveFilename = "";
	void DebugLog::Save() {
		if (m_SaveFilename.empty()) {
			auto now = chrono::system_clock::now();
			time_t t = chrono::system_clock::to_time_t(now);

			tm localTime = {};
			localtime_s(&localTime, &t);

			stringstream ss;

			ss << put_time(&localTime,"%Y_%m_%d_%H_%M_%S");

			m_SaveFilename = "Log_" + ss.str() + ".txt";
		}

		ofstream ofs(m_SaveFilename, ios::binary | ios::app);

		auto device = App::GetApp()->GetInputDevice();
		auto pad = device.GetControlerVec()[0];
		if (ofs) {
			ofs.write(reinterpret_cast<const char*>(&pad), sizeof(pad));
		}
	}
	vector<CONTROLER_STATE> DebugLog::Load(const string& filename) {
		ifstream ifs(filename, ios::binary);
		if (!ifs) return {};

		vector<CONTROLER_STATE> states;
		CONTROLER_STATE state;
		while (ifs.read(reinterpret_cast<char*>(&state), sizeof(state))) {
			states.push_back(state);
		}

		return states;
	}


	TextureCollision::TextureCollision(const shared_ptr<GameObject>& ptr) : Component(ptr) {}

	void TextureCollision::OnCreate() {
		ID3D11Texture2D* texResource = nullptr;
		D3D11_TEXTURE2D_DESC srvDesc;
		GetSrvResource(&texResource, &srvDesc);
		if (!texResource) {
			return;
		}

		m_TextureContext.m_SizeX = srvDesc.Width;
		m_TextureContext.m_SizeY = srvDesc.Height;

		int textureFullSize = m_TextureContext.m_SizeX * m_TextureContext.m_SizeY;
		m_CB.width = m_TextureContext.m_SizeX;
		m_CB.height = m_TextureContext.m_SizeY;

		m_Labels.resize(textureFullSize);

		m_LabelBuffer = make_shared<BufferContext>(sizeof(int), textureFullSize);
		if(!m_LabelBuffer->CreateUAV()) {
			int checker = 0;
		}
		if(!m_LabelBuffer->CreateSRV()) {
			int checker = 0;
		}

		m_LabelOutputBuffer = make_shared<BufferContext>(sizeof(int), textureFullSize);
		if(!m_LabelOutputBuffer->CreateSRV()) {
			int checker = 0;
		}
		if(!m_LabelOutputBuffer->CreateUAV()){
			int checker = 0;
		}

		m_ConvertFlagBuffer = make_shared<BufferContext>(sizeof(int), 1);
		if (!m_ConvertFlagBuffer->CreateUAV()) {
			int checker = 0;
		}

		//シェーダー初期化
		m_MaskShader = make_shared<DX11ComputeShader>();
		m_UnionFind1Shader = make_shared<DX11ComputeShader>();
		m_UnionFind2Shader = make_shared<DX11ComputeShader>();

		m_MaskShader->Initialize({ 8,8,1,m_TextureContext.m_SizeX,m_TextureContext.m_SizeY,1 });
		m_UnionFind1Shader->Initialize({ 8,8,1,m_TextureContext.m_SizeX,m_TextureContext.m_SizeY,1 });
		m_UnionFind2Shader->Initialize({ 8,8,1,m_TextureContext.m_SizeX,m_TextureContext.m_SizeY,1 });

		m_MaskShader->SetConstantBuffer(m_CB, TextureSizeConstantBuffer::GetPtr()->GetBuffer());
		m_UnionFind1Shader->SetConstantBuffer(m_CB, TextureSizeConstantBuffer::GetPtr()->GetBuffer());
		m_UnionFind2Shader->SetConstantBuffer(m_CB, TextureSizeConstantBuffer::GetPtr()->GetBuffer());

		auto object = GetGameObject();
		auto draw = object->GetComponent<InkDrawComponentTest>();
		//srvから情報を取得
		auto srv = draw->GetInkShaderResourceView();

		//SRV,UAVの場所を仮で取っておく
		m_MaskShader->AddSRV(srv.Get());
		m_MaskShader->AddUAV(m_LabelBuffer->m_UAV.Get());
		m_MaskShader->SetShader(GenerateMaskShader::GetPtr()->GetShader());

		m_UnionFind1Shader->AddSRV(m_LabelBuffer->m_SRV.Get());
		m_UnionFind1Shader->AddUAV(m_LabelOutputBuffer->m_UAV.Get());
		m_UnionFind1Shader->SetShader(UnionFindFirst::GetPtr()->GetShader());

		m_UnionFind2Shader->AddSRV(m_LabelBuffer->m_SRV.Get());
		m_UnionFind2Shader->AddUAV(m_LabelOutputBuffer->m_UAV.Get());
		m_UnionFind2Shader->SetShader(UnionFindSecond::GetPtr()->GetShader());

		InkConnectChecker::Get().AddTextureCollision(GetThis<TextureCollision>());
	}
	void TextureCollision::OnUpdate() {

	}
	void TextureCollision::OnDraw() {
		for (int i = 0; i < m_ElectricContourIndices.size(); i++) {
			if (m_ElectricContourIndices[i] != 0) {
				DrawContour(i);
			}
		}
	}
	void TextureCollision::DrawContour(int index) {
		for (auto& triangle : m_ContourTriangles[index]) {
			Vec3 dir = triangle.m_B - triangle.m_A;
			float length = dir.length();
			DrawLine(triangle.m_A, dir.normalize(), length);

			dir = triangle.m_C - triangle.m_B;
			length = dir.length();
			DrawLine(triangle.m_B, dir.normalize(), length);

			dir = triangle.m_A - triangle.m_C;
			length = dir.length();
			DrawLine(triangle.m_C, dir.normalize(), length);
		}
	}
	void TextureCollision::ClearElectricIndex() {
		for (auto& index : m_ElectricContourIndices) {
			index = 0;
		}
	}
	void TextureCollision::AddElectricIndex(int index) {
		if (m_ElectricContourIndices.size() <= index) return;
		m_ElectricContourIndices[index] = 1;
	}
	bool TextureCollision::IsElectrified(int index) {
		if (m_ElectricContourIndices.size() <= index) return false;
		return m_ElectricContourIndices[index];
	}
	void TextureCollision::GetSrvResource(ID3D11Texture2D** texture, D3D11_TEXTURE2D_DESC* desc) {
		auto object = GetGameObject();
		auto draw = object->GetComponent<InkDrawComponentTest>(false);
		if (!draw) return;
		//srvから情報を取得
		auto srv = draw->GetInkShaderResourceView().Get();
		ID3D11Resource* gpuResource = nullptr;
		srv->GetResource(&gpuResource);

		gpuResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)texture);
		(*texture)->GetDesc(desc);
	}

	void TextureCollision::CreateAlphaMask() {
		m_MaskShader->Execute();
	}
	void TextureCollision::ProcessCPU() {
		auto start = std::chrono::steady_clock::now();
		int labelSize = (int)m_Labels.size();

		vector<char> checkList(labelSize, 0);
		vector<int> groupId;
		groupId.reserve(20);
		for (int i = 0; i < labelSize; i++) {
			int& label = m_Labels[i];
			if (label != -1 && checkList[label] != 1) {
				groupId.push_back(i);
				checkList[label] = 1;
			}
		}
		//メッシュ作成
		CreateTextureMesh(m_Labels, groupId, m_TextureContext);
	}
	void TextureCollision::ProcessGPU() {
		//カラーマスク抽出
		m_MaskShader->Execute();

		m_UnionFind1Shader->Execute();
		swap(m_LabelBuffer, m_LabelOutputBuffer);

		const int maxLoop = 32;
		for (int i = 0; i < maxLoop; i++) {
			m_UnionFind2Shader->SetSRV(0, m_LabelBuffer->m_SRV.Get());
			m_UnionFind2Shader->SetUAV(0, m_LabelOutputBuffer->m_UAV.Get());
			m_UnionFind2Shader->Execute();
			swap(m_LabelBuffer, m_LabelOutputBuffer);
		}
		m_LabelBuffer->ReadBuffer(m_Labels.data());
	}
	void TextureCollision::CreateMeshCollision() {
		ProcessGPU();
		ProcessCPU();
	}

	void TextureCollision::CreateTextureMesh(vector<int>& cells, vector<int>& groupIDs, CoordContext& context) {
		auto start = std::chrono::steady_clock::now();

		vector<Vec2> findIndices = {
			{ 0,-1},{  1,-1},{ 1,0},{ 1, 1},
			{ 0, 1},{ -1, 1},{-1,0},{-1,-1},
		};
		cv::Mat mask(context.m_SizeY, context.m_SizeX, CV_8UC1);
		for (int i = 0; i < cells.size(); i++) {
			int x = i % context.m_SizeX;
			int y = i / context.m_SizeX;
			mask.at<uchar>(y, x) =
				cells[i] != -1 ? 255 : 0;
		}

		vector<vector<cv::Point>> contours;
		cv::findContours(mask, contours, m_ContourHierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
		m_CvContours.clear();
		m_CvContours.resize(contours.size());
		for (int i = 0; i < contours.size();i++) {
			double epsilon = 1.0f;//cv::arcLength(contours[i], true) * 0.005f;
			cv::approxPolyDP(contours[i], m_CvContours[i], epsilon, true);
		}

		m_ContourTriangles.clear();
		m_ContourTriangles.reserve(m_CvContours.size());
		using p2tStorage = vector<unique_ptr<p2t::Point>>;
		using p2tPolyline = vector<p2t::Point*>;
		vector<p2tStorage> storages;
		storages.resize(m_CvContours.size());
		vector<p2tPolyline> polylines;
		polylines.resize(m_CvContours.size());
		for (int i = 0; i < m_CvContours.size(); i++) {
			double area = cv::contourArea(m_CvContours[i], true);
			bool isHole = m_ContourHierarchy[i][3] != -1;

			if ( (area < 0 && !isHole ) || (area > 0 && isHole)) {
				std::reverse(m_CvContours[i].begin(),m_CvContours[i].end());
			}
			for (auto& point : m_CvContours[i]) {
				if (!storages[i].empty()) {
					auto prev = storages[i].back().get();
					if (point.x == prev->x && point.y == prev->y) {
						continue;
					}
				}
				
				storages[i].push_back(make_unique<p2t::Point>((double)point.x, (double)point.y));
				polylines[i].push_back(storages[i].back().get());
			}	
		}
		for (int i = 0; i < m_CvContours.size(); i++) {
			if (m_ContourHierarchy[i][3] == -1) {
				if (polylines[i].size() < 3) continue;

				p2t::CDT cdt(polylines[i]);

				int hole = m_ContourHierarchy[i][2];
				while (hole != -1) {
					const double MIN_HOLE_AREA = 10.0;
					if (polylines[hole].size() >= 3 && abs(cv::contourArea(m_CvContours[hole])) >= MIN_HOLE_AREA){
						cdt.AddHole(polylines[hole]);
					}
					hole = m_ContourHierarchy[hole][0];
				}
				try {
					cdt.Triangulate();
				}
				catch (const std::exception& e) {
					ofstream ofs("TriangleErrorLog.txt");
					ofs << e.what() << endl;
					for (auto* p : polylines[i])
					{
						ofs << p->x << ", " << p->y << endl;
					}
					hole = m_ContourHierarchy[i][2];
					while (hole != -1) {
						if (polylines[hole].size() >= 3) {
							ofs << "内円" << endl;
							for (auto* p : polylines[hole])
							{
								ofs << p->x << ", " << p->y << endl;
							}
						}
						hole = m_ContourHierarchy[hole][0];
					}
					throw;
				}
				auto triangles = cdt.GetTriangles();
				m_ContourTriangles.push_back(CalcContourWorldTriangle(triangles));
			}
		}
		m_ElectricContourIndices.resize(m_ContourTriangles.size(), 0);
	}

	vector<TRIANGLE> TextureCollision::CalcContourWorldTriangle(const vector<p2t::Triangle*>& triangles) {
		vector<TRIANGLE> worldPositions;
		worldPositions.reserve(triangles.size());

		auto transform = GetGameObject()->GetComponent<Transform>();
		Vec3 position = transform->GetPosition();
		Vec3 scale = transform->GetScale();

		auto GetPosition = [&](int x, int y) {
			if (y < 0) {
				y = 0;
			}
			if (x < 0) {
				x = 0;
			}
			float px = (float)x / m_TextureContext.m_SizeX;
			float py = (float)y / m_TextureContext.m_SizeY;
			Vec3 vertexPosition = Vec3((px - 0.5f) * scale.x, scale.y * 0.5f, -(py - 0.5f) * scale.z);

			int vertexId = y * m_TextureContext.m_SizeX + x;

			if (x >= m_TextureContext.m_SizeX - 1 || m_Labels[vertexId + 1] == -1) {
				vertexPosition.x += scale.x / m_TextureContext.m_SizeX;
			}
			if (y >= m_TextureContext.m_SizeY - 1 || m_Labels[vertexId + m_TextureContext.m_SizeX] == -1) {
				vertexPosition.z -= scale.z / m_TextureContext.m_SizeY;
			}

			Vec3 worldPosition = vertexPosition + position;
			return worldPosition;
			};

		for (auto* triangle : triangles) {
			
			auto p1 = triangle->GetPoint(0);
			auto p2 = triangle->GetPoint(1);
			auto p3 = triangle->GetPoint(2);

			worldPositions.push_back({
				GetPosition(p1->x,p1->y),
				GetPosition(p2->x,p2->y),
				GetPosition(p3->x,p3->y),
				Mat4x4() });
		}
		return worldPositions;
	}

	void TextureCollision::DrawLine(Vec3 position, Vec3 dir, float length) {
		if (dir.lengthSqr() < 0.0001f) {
			return;
		}
		auto meshResource = App::GetApp()->GetResource<MeshResource>(L"DEFAULT_PC_LINE");
		auto Dev = App::GetApp()->GetDeviceResources();
		auto pD3D11DeviceContext = Dev->GetD3DDeviceContext();
		auto RenderState = Dev->GetRenderState();

		Quat quaternion = Quat();

		XMVECTOR direction = XMVector3Normalize(dir);
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);

		// ワールド行列（向き）を作る
		XMMATRIX mat = XMMatrixInverse(nullptr, XMMatrixLookToLH(XMVectorZero(), direction, up));
		
		// クォータニオンに変換
		quaternion = (Quat)XMQuaternionRotationMatrix(mat);

		Mat4x4 world = (Mat4x4)XMMatrixScaling(1.0f, 1.0f, length);
		world *= (Mat4x4)XMMatrixRotationQuaternion(quaternion);
		world *= (Mat4x4)XMMatrixTranslation(position.x, position.y, position.z);
		world.transpose();
		//行列の定義
		bsm::Mat4x4 ViewMat, ProjMat;
		
		//カメラを得る
		auto CameraPtr = GetGameObject()->OnGetDrawCamera();
		//ビューと射影行列を得る
		ViewMat = CameraPtr->GetViewMatrix();
		//転置する
		ViewMat.transpose();
		//転置する
		ProjMat = CameraPtr->GetProjMatrix();
		ProjMat.transpose();
		//コンスタントバッファの準備
		SimpleConstants sb;
		sb.World = world;
		sb.View = ViewMat;
		sb.Projection = ProjMat;
		//エミッシブ
		sb.Emissive = Col4(0, 0, 0, 0);
		//デフィーズはすべて通す
		sb.Diffuse = Col4(1, 1, 1, 1);
		//コンスタントバッファの更新
		pD3D11DeviceContext->UpdateSubresource(CBSimple::GetPtr()->GetBuffer(), 0, nullptr, &sb, 0, 0);

		//ストライドとオフセット
		UINT stride = sizeof(VertexPositionColor);
		UINT offset = 0;
		//頂点バッファのセット
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, meshResource->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		//インデックスバッファのセット
		pD3D11DeviceContext->IASetIndexBuffer(meshResource->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

		//描画方法（ライン）
		pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		//コンスタントバッファの設定
		ID3D11Buffer* pConstantBuffer = CBSimple::GetPtr()->GetBuffer();
		ID3D11Buffer* pNullConstantBuffer = nullptr;
		//頂点シェーダに渡す
		pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
		//ピクセルシェーダに渡す
		pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
		//シェーダの設定
		pD3D11DeviceContext->VSSetShader(VSPCStatic::GetPtr()->GetShader(), nullptr, 0);
		pD3D11DeviceContext->PSSetShader(PSPCStatic::GetPtr()->GetShader(), nullptr, 0);
		//インプットレイアウトの設定
		pD3D11DeviceContext->IASetInputLayout(VSPCStatic::GetPtr()->GetInputLayout());
		//ブレンドステート
		//透明処理しない
		pD3D11DeviceContext->OMSetBlendState(RenderState->GetOpaque(), nullptr, 0xffffffff);
		//デプスステンシルステート
		pD3D11DeviceContext->OMSetDepthStencilState(RenderState->GetDepthDefault(), 0);
		//ラスタライザステート(ワイアフレーム)
		pD3D11DeviceContext->RSSetState(RenderState->GetWireframe());
		pD3D11DeviceContext->DrawIndexed(meshResource->GetNumIndicis(), 0, 0);
		//後始末
		Dev->InitializeStates();
	}
	void TextureMeshManager::Reload() {
		vector<thread> threads;

		for (auto& meshCollision : m_ReloadMeshCollisions) {
			meshCollision->ProcessGPU();
			thread t([meshCollision]() { meshCollision->ProcessCPU(); });
			threads.push_back(move(t));
		}

		for (auto& t : threads) {
			t.join();
		}
		m_ReloadMeshCollisions.clear();

		InkConnectChecker::Get().CheckConnect();
	}

	bool InkConnectChecker::IsConnectedSupplyToInk(const OBB& supplyOBB, const AABB& supplyAABB, const vector<TRIANGLE>& triangles) {
		for (auto& triangle : triangles) {
			if (!HitTest::AABB_AABB(supplyAABB, triangle.GetWrappedAABB())) continue;
			if (!HitTest::CollisionTestOBBTriangle(supplyOBB, triangle)) continue;
			return true;
		}
		return false;
	}
	bool InkConnectChecker::IsConnectedInkToInk(const vector<TRIANGLE>& triangles,const shared_ptr<TextureCollision>& fromCollision) {
		for (auto& weakCollision : m_TextureCollisions) {
			auto collision = weakCollision.lock();
			if (!collision) continue;

			size_t contourCount = collision->GetContourCount();
			for (int i = 0; i < contourCount; i++) {
				if (collision->IsElectrified(i)) continue;
				auto otherTriangles = collision->GetTriangles(i);
				bool isConnected = false;

				for (auto& triangle : triangles) {
					for (auto& otherTriangle : otherTriangles) {
						if (!HitTest::AABB_AABB(triangle.GetWrappedAABB(), otherTriangle.GetWrappedAABB())) continue;
						if (!HitTest::CollisionTestTriangle(triangle, otherTriangle)) continue;
						isConnected = true;
						break;
					}
					if (isConnected) break;
				}
				if (isConnected) {
					collision->AddElectricIndex(i);
					IsConnectedInkToInk(otherTriangles, collision);
					//return true;
				}
				
			}
		}
		for (auto& weakPort : m_Ports) {
			auto port = weakPort.lock();
			if (!port) continue;
			auto portCollision = port->GetComponent<CollisionObb>();
			auto portAABB = portCollision->GetWrappedAABB();
			auto portOBB = portCollision->GetObb();
			if (IsConnectedInkToPort(portOBB, portAABB, triangles)) {
				port->GetComponent<PNTStaticDraw>()->SetDiffuse(Col4(1, 0, 1, 1));
			}
			else {
				//port->GetComponent<PNTStaticDraw>()->SetDiffuse(Col4(1, 0, 0, 1));
			}
		}
		return false;
	}
	bool InkConnectChecker::IsConnectedInkToPort(const OBB& portOBB, const AABB& portAABB, const vector<TRIANGLE>& triangles) {
		for (auto& triangle : triangles) {
			if (!HitTest::AABB_AABB(portAABB, triangle.GetWrappedAABB())) continue;
			if (!HitTest::CollisionTestOBBTriangle(portOBB, triangle)) continue;
			return true;
		}
		return false;
	}

	void InkConnectChecker::Initialize() {
		m_Ports.clear();
		m_PowerSupplies.clear();
		m_TextureCollisions.clear();
	}
	vector<pair<weak_ptr<PowerSupply>, weak_ptr<Port>>> InkConnectChecker::CheckConnect() {
		//通電情報を初期化
		for (auto& weakCollision : m_TextureCollisions) {
			auto collision = weakCollision.lock();
			if (!collision) continue;

			collision->ClearElectricIndex();
		}

		vector<pair<weak_ptr<PowerSupply>, weak_ptr<Port>>> result;
		for (auto& weakSupply : m_PowerSupplies) {
			auto supply = weakSupply.lock();
			if (!supply) continue;
			auto supplyCollision = supply->GetComponent<CollisionObb>();
			auto supplyAABB = supplyCollision->GetWrappedAABB();
			auto supplyOBB = supplyCollision->GetObb();

			for (auto& weakCollision : m_TextureCollisions) {
				auto collision = weakCollision.lock();
				if (!collision) continue;

				size_t contourCount = collision->GetContourCount();
				for (int i = 0; i < contourCount; i++) {
					auto triangles = collision->GetTriangles(i);
					bool isConnectedSupply = IsConnectedSupplyToInk(supplyOBB, supplyAABB, triangles);
					
					if (isConnectedSupply) {
						collision->AddElectricIndex(i);
						IsConnectedInkToInk(triangles, collision);
					}
				}
			}
		}
		return result;
	}
}
//end basecross
