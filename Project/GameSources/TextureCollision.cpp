/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include <filesystem>

namespace basecross {

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

		auto end = std::chrono::steady_clock::now();
		auto mappingDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		start = std::chrono::steady_clock::now();
		//メッシュ作成
		CreateTextureMesh(m_Labels, groupId, m_TextureContext);
		end = std::chrono::steady_clock::now();
		auto createMeshDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

		float allDuration = mappingDuration + createMeshDuration;
		int checker = 0;
	}
	void TextureCollision::ProcessGPU() {
		auto start = std::chrono::steady_clock::now();

		//カラーマスク抽出
		m_MaskShader->Execute();

		auto end = std::chrono::steady_clock::now();
		auto maskDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

		start = std::chrono::steady_clock::now();

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
		end = std::chrono::steady_clock::now();
		auto shaderDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

		float allDuration = maskDuration + shaderDuration;
		int checker = 0;
	}
	void TextureCollision::CreateMeshCollision() {
		ProcessGPU();
		ProcessCPU();
	}

	void TextureCollision::CreateTextureMesh(vector<int>& cells, vector<int>& groupIDs, CoordContext& context) {
		auto start = std::chrono::steady_clock::now();

		//塊の輪郭抽出
		m_Contours.clear();
		m_Contours.resize(groupIDs.size());
		vector<Vec2> findIndices = {
			{ 0,-1},{  1,-1},{ 1,0},{ 1, 1},
			{ 0, 1},{ -1, 1},{-1,0},{-1,-1},
		};
		
		int page = 1;
		for (int i = 0; i < groupIDs.size();i++) {
			GetContour(cells, groupIDs[i], m_Contours[i]);
		}
		auto end = std::chrono::steady_clock::now();
		auto contourDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

		m_ContourTriangles.clear();
		m_ContourTriangles.reserve(m_Contours.size());

		float totalDouglasDuration = 0.0f;
		float totalEarClippingDuration = 0.0f;
		vector<int> tempContour;

		DouglasPeucker douglasPeucker;
		//頂点最適化
		for (auto& contour : m_Contours) {
			start = std::chrono::steady_clock::now();
			douglasPeucker.Initialize(contour, m_TextureContext);
			tempContour.clear();
			douglasPeucker.Calc(0, (int)contour.size() - 1, 2.0f, tempContour);
			contour = tempContour;
			end = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
			totalDouglasDuration += duration;

			start = std::chrono::steady_clock::now();
			vector<TRIANGLE> triangles = EarClipping::Calc(CalcContourWorldPosition(contour));
			m_ContourTriangles.push_back(triangles);
			end = std::chrono::steady_clock::now();
			duration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
			totalEarClippingDuration += duration;
		}
		m_ElectricContourIndices.resize(m_ContourTriangles.size(), 0);
		int checker = 0;
	}

	bool TextureCollision::IsContour(int current, vector<int>& cells, int& groupID) {
		struct Vec2Int {
			int x, y;
		};
		vector<Vec2Int> findIndices = {
			{ 0,-1},{  1,-1},{ 1,0},{ 1, 1},
			{ 0, 1},{ -1, 1},{-1,0},{-1,-1},
		};
		int x, y;
		x = current % m_TextureContext.m_SizeX;
		y = current / m_TextureContext.m_SizeX;

		if (x == 0 || x == (int)m_TextureContext.m_SizeX - 1 || y == 0 || y == (int)m_TextureContext.m_SizeY - 1) {
			return true;
		}
		for (int i = 0; i < 8; i++) {
			int index = current + findIndices[i].y * m_TextureContext.m_SizeX + findIndices[i].x;
			if (cells[index] != groupID || cells[index] == -1) {
				return true;
			}
		}
		return false;
	}
	void TextureCollision::GetContour(vector<int>& cells,int& groupID, vector<int>& out) {
		out.reserve(cells.size() * 0.5f);
		vector<char> visited(cells.size(), 0);
		struct Vec2Int {
			int x, y;
		};
		vector<Vec2Int> findIndices = {
			{ 0,-1},{  1,-1},{ 1,0},{ 1, 1},
			{ 0, 1},{ -1, 1},{-1,0},{-1,-1},
		};

		int startIndex = groupID;
		int currentIndex = startIndex;
		visited[currentIndex] = 1;

		out.push_back(currentIndex);
		int beforeDir = 2;
		do {

			for (int i = 0; i < 8; i++) {
				int dir = (beforeDir + i) % 8;
				int next = currentIndex + findIndices[dir].y * m_TextureContext.m_SizeX + findIndices[dir].x;

				int x = next % m_TextureContext.m_SizeX;
				int y = next / m_TextureContext.m_SizeX;

				if (x < 0 || x > m_TextureContext.m_SizeX - 1 || y < 0 || y > m_TextureContext.m_SizeY - 1) {
					continue;
				}
				int nextID = cells[next];
				if (nextID == groupID && IsContour(next,cells,groupID)) {
					visited[next] = 1;
					beforeDir = (dir + 6) % 8;
					currentIndex = next;
					out.push_back(currentIndex);
					if (startIndex == currentIndex) {
						int checker = 0;
					}
					break;
				}
			}
			if (startIndex == currentIndex) {
				int checker = 0;
			}
		} while (startIndex != currentIndex);

		out.pop_back();
	}

	vector<Vec3> TextureCollision::CalcContourWorldPosition(const vector<int>& contour) {
		vector<Vec3> worldPositions;
		worldPositions.reserve(contour.size());

		auto transform = GetGameObject()->GetComponent<Transform>();
		Vec3 position = transform->GetPosition();
		Vec3 scale = transform->GetScale();
		for (int i = 0; i < contour.size(); i++) {
			int vertexId = contour[i];

			int x = vertexId % m_TextureContext.m_SizeX;
			int y = vertexId / m_TextureContext.m_SizeX;
			float px = (float)x / m_TextureContext.m_SizeX;
			float py = (float)y / m_TextureContext.m_SizeY;
			Vec3 vertexPosition = Vec3((px - 0.5f) * scale.x, scale.y * 0.5f, -(py - 0.5f) * scale.z);
			if (x >= m_TextureContext.m_SizeX - 1 || m_Labels[vertexId + 1] == -1) {
				vertexPosition.x += scale.x / m_TextureContext.m_SizeX;
			}
			if (y >= m_TextureContext.m_SizeY - 1 || m_Labels[vertexId + m_TextureContext.m_SizeX] == -1) {
				vertexPosition.z -= scale.z / m_TextureContext.m_SizeY;
			}

			Vec3 worldPosition = vertexPosition + position;
			worldPositions.push_back(worldPosition);
		}
		return worldPositions;
	}

	void TextureCollision::IndexToCoord(int index, int width, int& x, int& y) {
		x = index % width;
		y = index / width;
	}
	void TextureCollision::CoordToIndex(int& index, int x, int y, int width) {
		index = y * width + x;
	}

	void TextureCollision::DrawLine(Vec3 position, Vec3 dir, float length) {
		GenericDraw;
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

	inline float DouglasPeucker::CalcDistance(Vec2& start, Vec2& end, Vec2& point) {
		float steX = end.x - start.x;
		float steY = end.y - start.y;

		float stpX = point.x - start.x;
		float stpY = point.y - start.y;

		float steLengthSqr = steX * steX + steY * steY;
		if (steLengthSqr == 0.0f) {
			return stpX * stpX + stpY * stpY;
		}

		float t = (stpX * steX + stpY * steY) / steLengthSqr;
		t = std::clamp(t, 0.0f, 1.0f);

		float closestX = start.x + steX * t;
		float closestY = start.y + steY * t;

		float distX = point.x - closestX;
		float distY = point.y - closestY;

		return distX * distX + distY * distY;
	}
	void DouglasPeucker::Initialize(const vector<int>& points, const CoordContext& context) {
		m_Points = points;
		m_Positions.resize(m_Points.size());
		for (int i = 0; i < m_Points.size(); i++) {
			m_Positions[i] = Vec2(static_cast<float>(points[i] % context.m_SizeX), static_cast<float>(points[i] / context.m_SizeX));
		}
	}
	void DouglasPeucker::Calc(int start, int end, float epsilon, vector<int>& output) {
		int calcSize = end - start;
		if (calcSize <= 1) {
			if (calcSize > 0) {
				output.push_back(m_Points[start]);
				output.push_back(m_Points[end]);
			}
			return;
		}

		float maxDist = -10000000.0f;
		int index = -1;

		Vec2& startPoint = m_Positions[start];
		Vec2& endPoint = m_Positions[end];

		for (int i = start + 1; i < end; i++) {
			float dist = CalcDistance(startPoint, endPoint, m_Positions[i]);

			if (dist > maxDist) {
				maxDist = dist;
				index = i;
			}
		}

		if (maxDist > epsilon * epsilon) {
			Calc(start,     index, epsilon, output);
			output.pop_back();
			Calc(index,       end, epsilon, output);
		}
		else {
			output.push_back(m_Points[start]);
			output.push_back(m_Points[end]);
		}

	}


	bool EarClipping::IsAngleThen180(const Vec3& point, const Vec3& a, const Vec3& b) {
		return ((a.x - point.x) * (b.z - point.z) - (a.z - point.z) * (b.x - point.x)) > 0;
	}
	bool EarClipping::IsContainInTriangle(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& point) {
		bool b1 = IsAngleThen180(a, b, point);
		bool b2 = IsAngleThen180(b, c, point);
		bool b3 = IsAngleThen180(c, a, point);

		return (b1 == b2 && b2 == b3);
	}
	vector<TRIANGLE> EarClipping::Calc(const vector<Vec3>& points) {
		if (points.size() < 3) return {};
		vector<Vec3> ear = points;
		vector<TRIANGLE> triangles;
		triangles.reserve(ear.size());
		while (ear.size() > 3) {
			for (int i = 0, size = (int)ear.size(); i < size; i++) {
				int left, right;
				left = i >= size - 1 ? 0 : i + 1;
				right = i == 0 ? size - 1 : i - 1;

				if (IsAngleThen180(ear[i], ear[right], ear[left])) {
					bool isContained = false;
					for (int j = 0; j < size; j++) {
						if (j == i || j == left || j == right) continue;

						if (IsContainInTriangle(ear[right], ear[i], ear[left], ear[j])) {
							isContained = true;
						}
					}
					if (!isContained) {
						TRIANGLE triangle;
						triangle.Set(ear[i], ear[left], ear[right], Mat4x4());
						triangles.push_back(triangle);
						ear.erase(ear.begin() + i);
						break;
					}
				}
			}
		}
		TRIANGLE triangle;
		triangle.Set(ear[0], ear[1], ear[2], Mat4x4());
		triangles.push_back(triangle);

		return triangles;
	}

	void TextureMeshManager::Reload() {
		vector<thread> threads;
		for (int i = 0, size = m_ReloadMeshCollisions.size(); i < size; i++) {
			
		}
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
