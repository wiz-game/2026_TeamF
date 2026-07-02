/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include <filesystem>
namespace basecross {

	void Contour::CalcAABB(const shared_ptr<Transform>& transform) {
		auto triangles = GetWorldTriangles(transform);
		for (auto& triangle : triangles) {
			for (int i = 0; i < 3; i++) {
				m_Aabb.m_Min.x = min(m_Aabb.m_Min.x, triangle[i].x);
				m_Aabb.m_Min.y = min(m_Aabb.m_Min.y, triangle[i].y);
				m_Aabb.m_Min.z = min(m_Aabb.m_Min.z, triangle[i].z);

				m_Aabb.m_Max.x = max(m_Aabb.m_Max.x, triangle[i].x);
				m_Aabb.m_Max.y = max(m_Aabb.m_Max.y, triangle[i].y);
				m_Aabb.m_Max.z = max(m_Aabb.m_Max.z, triangle[i].z);
			}
			
		}
	}
	vector<TRIANGLE> Contour::GetWorldTriangles(const shared_ptr<Transform>& transform)const {
		Vec3 position = transform->GetWorldPosition();
		Vec3 scale = transform->GetScale();
		vector<TRIANGLE> worldTriangles;

		size_t triangleSize = m_Triangles.size();
		worldTriangles.resize(triangleSize);
		for (int i = 0; i < triangleSize; i++) {
			const TRIANGLE& triangle = m_Triangles[i];

			for (int j = 0; j < 3; j++) {
				worldTriangles[i][j] = triangle[j] * scale;
				worldTriangles[i][j] += position;
			}
		}
		return worldTriangles;
	}
	TextureCollision::TextureCollision(const shared_ptr<GameObject>& ptr) : 
		Component(ptr),m_EffectSpawnInterval(0.1f),m_EffectSpawnTimer(0.0f),
		m_CB{}
	{}

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

		m_Labels.resize(textureFullSize,0);

		m_LabelBuffer = make_shared<BufferContext>(sizeof(int), textureFullSize);
		if(!m_LabelBuffer->CreateUAV()) {
			int checker = 0;
		}
		if(!m_LabelBuffer->CreateSRV()) {
			int checker = 0;
		}

		//シェーダー初期化
		m_MaskShader = make_shared<DX11ComputeShader>();
		m_MaskShader->Initialize({ 8,8,1,m_TextureContext.m_SizeX,m_TextureContext.m_SizeY,1 });

		auto object = GetGameObject();
		auto draw = object->GetComponent<InkDrawComp>();
		//srvから情報を取得
		auto srv = draw->GetSRV();
		//SRV,UAVの場所を仮で取っておく
		m_MaskShader->AddSRV(srv.Get());
		m_MaskShader->AddUAV(m_LabelBuffer->m_UAV.Get());
		m_MaskShader->SetShader(GenerateMaskShader::GetPtr()->GetShader());
		InkConnectChecker::Get().AddTextureCollision(GetThis<TextureCollision>());
	}
	void TextureCollision::OnUpdate() {
		m_EffectSpawnTimer += App::GetApp()->GetElapsedTime();
		if (m_EffectSpawnTimer < m_EffectSpawnInterval) return;
		m_EffectSpawnTimer = 0;
		const int spawnEffectCount = 5;
		for (int i = 0; i < m_ElectricContourIndices.size(); i++) {
			if (m_ElectricContourIndices[i] == 0)continue;
			int handle = -1;
			for (int j = 0; j < spawnEffectCount; j++) {
				int spwanTriangle = rand() % m_Contour[i].m_Triangles.size();
				vector<TRIANGLE> triangles = m_Contour[i].GetWorldTriangles(GetGameObject()->GetComponent<Transform>());
				Vec3 position = triangles[spwanTriangle].GetCenter();
				EffectManager::g_Instance->PlayEffect(handle, L"ELECTRIC", position, 0);
			}
		}
		if (!m_WaitContour.empty()) {
			m_Contour = m_WaitContour;
			m_ElectricContourIndices = m_WaitElectricContourIndices;
			m_WaitContour.clear();
			m_WaitElectricContourIndices.clear();
		}
	}
	void TextureCollision::OnDraw() {
		
	}
	void TextureCollision::DrawContour(int index) {
		for (auto& triangle : GetWorldTriangles(index)) {
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
	void TextureCollision::SetElectricfield(int index) {
		if (m_ElectricContourIndices.size() <= index) return;
		m_ElectricContourIndices[index] = 1;
	}
	bool TextureCollision::IsElectrified(int index) {
		if (m_ElectricContourIndices.size() <= index) return true;
		return m_ElectricContourIndices[index];
	}
	void TextureCollision::GetSrvResource(ID3D11Texture2D** texture, D3D11_TEXTURE2D_DESC* desc) {
		auto object = GetGameObject();
		auto draw = object->GetComponent<InkDrawComp>(false);
		if (!draw) return;
		//srvから情報を取得
		auto srv = draw->GetSRV().Get();
		ID3D11Resource* gpuResource = nullptr;
		srv->GetResource(&gpuResource);

		gpuResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)texture);
		(*texture)->GetDesc(desc);
	}

	void TextureCollision::ProcessCPU() {
		//メッシュ作成
		//CreateTextureMesh(m_Labels, m_TextureContext);
	}
	void TextureCollision::ProcessGPU() {
		//カラーマスク抽出
		m_MaskShader->SetConstantBuffer(m_CB, TextureSizeConstantBuffer::GetPtr()->GetBuffer());
		m_MaskShader->Execute();
		m_LabelBuffer->ReadBuffer(m_Labels.data());
	}


	vector<TRIANGLE> TextureCollision::GetWorldTriangles(int index)const {
		auto& gameObject = GetGameObject();
		auto transform = gameObject->GetComponent<Transform>();

		return m_Contour[index].GetWorldTriangles(transform);
	}
	void TextureCollision::CreateMeshInThread(const TextureSnapShot& snapShot, vector<Contour>& result) {
		cv::Mat mask(snapShot.m_Context.m_SizeY, snapShot.m_Context.m_SizeX, CV_8UC1);
		for (UINT y = 0; y < snapShot.m_Context.m_SizeY; y++) {
			uchar* row = mask.ptr<uchar>(y);

			UINT offset = y * snapShot.m_Context.m_SizeX;
			for (UINT x = 0; x < snapShot.m_Context.m_SizeX; x++) {
				row[x] = snapShot.m_Data[offset + x];
				if (row[x] == 255) {
					int checker = 0;
				}
			}
		}

		vector<vector<cv::Point>> contours;
		vector<cv::Vec4i> contourHierarchy;
		cv::findContours(mask, contours, contourHierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

		m_CvContours.clear();
		m_CvContours.resize(contours.size());
		for (int i = 0; i < contours.size(); i++) {
			double epsilon = 1.0f;
			cv::approxPolyDP(contours[i], m_CvContours[i], epsilon, true);
		}

		size_t contourSize = m_CvContours.size();
		result.clear();

		using p2tStorage = vector<unique_ptr<p2t::Point>>;
		using p2tPolyline = vector<p2t::Point*>;
		vector<p2tStorage> storages;
		vector<p2tPolyline> polylines;

		storages.resize(contourSize);
		polylines.resize(contourSize);
		result.reserve(contourSize);

		int contourCount = 0;
		for (size_t i = 0, size = m_CvContours.size(); i < size; i++) {
			double area = cv::contourArea(m_CvContours[i], true);
			bool isHole = contourHierarchy[i][3] != -1;
			if (!isHole) {
				contourCount++;
			}
			if ((area < 0 && !isHole) || (area > 0 && isHole)) {
				std::reverse(m_CvContours[i].begin(), m_CvContours[i].end());
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

		for (size_t i = 0, size = m_CvContours.size(); i < size; i++) {
			if (contourHierarchy[i][3] == -1) {
				if (polylines[i].size() < 3) continue;

				p2t::CDT cdt(polylines[i]);

				int hole = contourHierarchy[i][2];
				while (hole != -1) {
					const double MIN_HOLE_AREA = 10.0;
					if (polylines[hole].size() >= 3 && abs(cv::contourArea(m_CvContours[hole])) >= MIN_HOLE_AREA) {
						cdt.AddHole(polylines[hole]);
					}
					hole = contourHierarchy[hole][0];
				}
				
				cdt.Triangulate();
				auto triangles = cdt.GetWorldTriangles();

				Contour data = Contour(CalcContourWorldTriangle(triangles,snapShot));
				data.CalcAABB(snapShot.m_Transform);
				result.push_back(data);
			}
		}
	}

	TextureSnapShot TextureCollision::SnapShot() {
		TextureSnapShot snapShot;
		snapShot.m_Context = m_TextureContext;
		snapShot.m_Data = m_Labels;
		snapShot.m_Transform = GetGameObject()->GetComponent<Transform>();
		return snapShot;
	}
	vector<TRIANGLE> TextureCollision::CalcContourWorldTriangle(const vector<p2t::Triangle*>& triangles, const TextureSnapShot& snapShot) {
		vector<TRIANGLE> worldPositions;
		worldPositions.reserve(triangles.size());

		auto& labels = snapShot.m_Data;
		auto& context = snapShot.m_Context;

		auto GetPosition = [&](int x, int y) {
			if (y < 0) {
				y = 0;
			}
			if (x < 0) {
				x = 0;
			}
			float px = (float)x / (float)context.m_SizeX;
			float py = (float)y / (float)context.m_SizeY;
			Vec3 vertexPosition = Vec3((px - 0.5f),0.5f, -(py - 0.5f));

			int vertexId = y * (int)context.m_SizeX + x;

			if (x >= (int)context.m_SizeX - 1 || labels[vertexId + 1] == -1) {
				vertexPosition.x += 1.0f / (float)context.m_SizeX;
			}
			if (y >= (int)context.m_SizeY - 1 || labels[vertexId + context.m_SizeX] == -1) {
				vertexPosition.z -= 1.0f / (float)context.m_SizeY;
			}
			return vertexPosition;
			};

		for (auto* triangle : triangles) {

			auto p1 = triangle->GetPoint(0);
			auto p2 = triangle->GetPoint(1);
			auto p3 = triangle->GetPoint(2);

			worldPositions.push_back({
				GetPosition((int)p1->x,(int)p1->y),
				GetPosition((int)p2->x,(int)p2->y),
				GetPosition((int)p3->x,(int)p3->y),
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

	void ThreadPool::Initialize(size_t numThread) {
		m_ThreadStop = false;
		m_RunningTask = 0;
		for (size_t i = 0; i < numThread; i++)
		{
			m_Workers.emplace_back([this](){ Worker(); });
		}
	}

	void ThreadPool::Worker() {
		while (true) {
			function<void()> task;
			{
				unique_lock lock(m_Mutex);

				//条件が達成されるまでここで止まる(スレッドが止まるかマスクが追加されるまで)
				m_Condition.wait(lock, [this]() {
					return m_ThreadStop || !m_Tasks.empty();
					});
				//この時点でスレッドが止まったうえ、タスクがない場合は終了
				if (m_ThreadStop && m_Tasks.empty())
					return;

				task = move(m_Tasks.front());
				m_Tasks.pop();

				m_RunningTask++;
			}
			task();

			{
				unique_lock lock(m_Mutex);
				m_RunningTask--;
				if (m_Tasks.empty() && m_RunningTask <= 0) {
					m_WaitCondition.notify_all();
				}
			}
		}
	}
	void ThreadPool::Destory() {
		{
			std::lock_guard lock(m_Mutex);
			m_ThreadStop = true;
		}
		m_Condition.notify_all();

		for (auto& worker : m_Workers){
			if (worker.joinable())
				worker.join();
		}
	}

	
	void ThreadPool::Execute(function<void()> task) {
		{
			lock_guard lock(m_Mutex);
			m_Tasks.push(task);
		}
		m_Condition.notify_one();
	}
	void ThreadPool::Wait() {
		{
			unique_lock lock(m_Mutex);
			m_WaitCondition.wait(lock, [&]() {return m_Tasks.empty() && m_RunningTask <= 0; });
		}
	}

	void TextureMeshManager::DecreeseProccessCount() {
		if (m_ProccessCount <= 0) return;
		m_ProccessCount--;
		if (m_ProccessCount == 0) {
			while (!m_ResultQueue.empty()) {
				MeshResult result;
				{
					lock_guard lock(m_Mutex);
					result = m_ResultQueue.front();
					m_ResultQueue.pop();
				}

				result.m_Ptr->ApplyThreadResult(result.m_Result);
			}
			{
				lock_guard lock(m_Mutex);
				InkConnectChecker::Get().CheckConnect();
			}
		}
	}


	void TextureMeshManager::Clear() {
		m_Proccess.clear();
		m_Pending.clear();
	}
	void TextureMeshManager::AddReload(const shared_ptr<TextureCollision>& meshCollision) {
		m_Pending[meshCollision.get()] = meshCollision->SnapShot();
	}

	void TextureMeshManager::Reload() {
		if (m_ProccessCount <= 0) {

			m_Proccess = m_Pending;
			m_Pending.clear();

			m_ProccessCount = m_Proccess.size();

			for (auto& proccess : m_Proccess) {
				proccess.first->ProcessGPU();

				ThreadPool::Get().Execute([&, proccess]() {
					MeshResult result;
					result.m_Ptr = proccess.first;
					proccess.first->CreateMeshInThread(proccess.second, result.m_Result);
					{
						lock_guard lock(m_Mutex);
						m_ResultQueue.push(result);
					}
					DecreeseProccessCount();
					});
			}
		}
		InkConnectChecker::Get().CheckConnect();
	}

	bool InkConnectChecker::IsConnectedSupplyToInk(const OBB& supplyOBB, const AABB& supplyAABB, const vector<TRIANGLE>& triangles) {
		for (auto& triangle : triangles) {
			if (!HitTest::AABB_AABB(supplyAABB, triangle.GetWrappedAABB(), Vec3(0.0f, 0.5f, 0.0f))) continue;
			if (!HitTest::CollisionTestOBBTriangle(supplyOBB, triangle)) continue;
			return true;
		}
		return false;
	}
	bool InkConnectChecker::IsConnectedInkToInk(const vector<TRIANGLE>& triangles,const AABB& inkAABB) {
		for (auto& weakCollision : m_TextureCollisions) {
			auto collision = weakCollision.lock();
			if (!collision) continue;

			size_t contourCount = collision->GetContourCount();
			for (int i = 0; i < contourCount; i++) {
				if (collision->IsElectrified(i)) continue;
				const auto& otherInkAABB = collision->GetContourAABB(i);
				if (!HitTest::AABB_AABB(inkAABB, otherInkAABB,Vec3(0.0f,0.5f,0.0f))) continue;

				const auto& otherTriangles = collision->GetWorldTriangles(i);
				bool isConnected = false;

				for (auto& triangle : triangles) {
					for (auto& otherTriangle : otherTriangles) {
						if (!HitTest::AABB_AABB(triangle.GetWrappedAABB(), otherTriangle.GetWrappedAABB(), Vec3(0.0f, 0.5f, 0.0f))) continue;
						if (!HitTest::CollisionTestTriangle(triangle, otherTriangle)) continue;
						isConnected = true;
						break;
					}
					if (isConnected) break;
				}
				if (isConnected) {
					collision->SetElectricfield(i);
					IsConnectedInkToInk(otherTriangles, otherInkAABB);
				}
				
			}
		}
		for (auto& weakPort : m_Ports) {
			auto port = weakPort.lock();
			if (!port) continue;
			auto portCollision = port->GetComponent<CollisionObb>();
			auto portAABB = portCollision->GetWrappedAABB();
			auto portOBB = portCollision->GetObb();

			if (!HitTest::AABB_AABB(inkAABB, portAABB, Vec3(0.0f, 0.5f, 0.0f))) continue;
			if (IsConnectedInkToPort(portOBB, portAABB, triangles)) {
				port->SetConnect(true);
				//port->GetComponent<PNTStaticDraw>()->SetDiffuse(Col4(1, 0, 1, 1));
			}
		}
		return false;
	}
	bool InkConnectChecker::IsConnectedInkToPort(const OBB& portOBB, const AABB& portAABB, const vector<TRIANGLE>& triangles) {
		for (auto& triangle : triangles) {
			if (!HitTest::AABB_AABB(portAABB, triangle.GetWrappedAABB(), Vec3(0.0f, 0.5f, 0.0f))) continue;
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
		for (auto& weakPort : m_Ports) {
			auto port = weakPort.lock();
			if (!port) continue;

			port->SetConnect(false);
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
					const auto& inkAABB = collision->GetContourAABB(i);
					if (!HitTest::AABB_AABB(supplyAABB, inkAABB, Vec3(0.0f, 0.5f, 0.0f))) continue;

					const auto& triangles = collision->GetWorldTriangles(i);
					bool isConnectedSupply = IsConnectedSupplyToInk(supplyOBB, supplyAABB, triangles);
					
					if (isConnectedSupply) {
						collision->SetElectricfield(i);
						IsConnectedInkToInk(triangles, inkAABB);
					}
				}
			}
		}
		return result;
	}
}
//end basecross
