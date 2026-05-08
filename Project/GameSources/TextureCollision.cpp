/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include <filesystem>

namespace basecross{
	

	TextureCollision::TextureCollision(const shared_ptr<GameObject>& ptr) : Component(ptr){}

	void TextureCollision::OnCreate() {

		vector<VertexPositionColor> vertices = {
			{{0.0f,0.0f,0.0f},{1,1,1}},
			{{0.0f,0.0f,1.0f},{1,1,1}}
		};
		vector<uint16_t> indices{
			0,1
		};

		App::GetApp()->RegisterResource(L"DEFAULT_PC_LINE", MeshResource::CreateMeshResource(vertices, indices, false));
	}
	void TextureCollision::OnUpdate() {

	}
	void TextureCollision::OnDraw() {
		CreateMeshCollision();
		for (auto& triangles : m_ContourTriangles) {
			for (auto& triangle : triangles) {
				Vec3 dir = triangle[1] - triangle[0];
				float length = dir.length();
				DrawLine(triangle[0], dir.normalize(), length);

				dir = triangle[2] - triangle[1];
				length = dir.length();
				DrawLine(triangle[1], dir.normalize(), length);

				dir = triangle[0] - triangle[2];
				length = dir.length();
				DrawLine(triangle[2], dir.normalize(), length);
			}
		}
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

		//カラーマスク抽出
		BufferContext labelBuffer = {};
		CreateAlphaMask(labelBuffer, m_TextureContext);

		auto end = std::chrono::steady_clock::now();
		auto maskDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		start = std::chrono::steady_clock::now();

		//マスクの塊をグループ化
		vector<int> cellLabels(m_TextureContext.m_SizeX * m_TextureContext.m_SizeY, 0);
		DX11ComputeShader<int> shader = DX11ComputeShader<int>();
		int labelSize = (int)cellLabels.size();

		BufferContext cellLabelOutput = BufferContext(sizeof(int), labelSize);
		cellLabelOutput.CreateSRV();
		cellLabelOutput.CreateUAV();

		BufferContext convertFlag = BufferContext(sizeof(int), 1);
		convertFlag.CreateUAV();

		shader.AddSRV(labelBuffer.m_SRV.Get());
		shader.AddUAV(cellLabelOutput.m_UAV.Get());

		shader.Initialize({ 8,8,1,m_TextureContext.m_SizeX,m_TextureContext.m_SizeY,1 });
		shader.SetShader(UnionFindFirst::GetPtr()->GetShader());
		TextureSizeConstantData cb;
		cb.width = m_TextureContext.m_SizeX;
		cb.height = m_TextureContext.m_SizeY;
		shader.SetConstantBuffer(cb, TextureSizeConstantBuffer::GetPtr()->GetBuffer());

		end = std::chrono::steady_clock::now();
		auto shaderInitializeDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		start = std::chrono::steady_clock::now();

		shader.Execute();
		swap(labelBuffer, cellLabelOutput);
		shader.SetShader(UnionFindSecond::GetPtr()->GetShader());

		shader.AddUAV(convertFlag.m_UAV.Get());
		const int maxLoop = m_TextureContext.m_SizeX + m_TextureContext.m_SizeY;
		const int checkDuration = 50;
		for (int i = 0; i < maxLoop; i++) {
			shader.SetSRV(0, labelBuffer.m_SRV.Get());
			shader.SetUAV(0, cellLabelOutput.m_UAV.Get());
			shader.Execute();
			swap(labelBuffer, cellLabelOutput);

			int flag = 0;
			convertFlag.ReadBuffer(&flag);
			if (flag == 0) {
				break;
			}
			convertFlag.ResetUAV();
		}
		labelBuffer.ReadBuffer(cellLabels.data());
		end = std::chrono::steady_clock::now();
		auto shaderDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		start = std::chrono::steady_clock::now();
		
		
		vector<IndexInfo> indexGroup;
		indexGroup.reserve(labelSize);
		for (int i = 0; i < labelSize; i++) {
			int& label = cellLabels[i];
			if (label != -1) {
				indexGroup.push_back({ i,label });
			}
		}
		sort(indexGroup.begin(), indexGroup.end(), [&](const IndexInfo& a, const IndexInfo& b) {return a.label < b.label; });

		
		vector<GroupInfo> groups;
		groups.reserve(100);

		if (!indexGroup.empty()) {
			int startIndex = 0;
			int currentId = indexGroup[0].label;
			int  i = 1;
			for (size_t size = indexGroup.size(); i < size; i++) {
				int newId = indexGroup[i].label;
				if (newId != currentId) {
					groups.push_back({ currentId,startIndex,i - startIndex });
					currentId = newId;
					startIndex = i;
				}
			}
			groups.push_back({ currentId,startIndex,i - startIndex });
		}

		end = std::chrono::steady_clock::now();
		auto mappingDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
		start = std::chrono::steady_clock::now();
		//メッシュ作成
		CreateTextureMesh(cellLabels, groups, m_TextureContext);
		end = std::chrono::steady_clock::now();
		auto createMeshDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

		float allDuration = maskDuration + shaderInitializeDuration + shaderDuration + mappingDuration + createMeshDuration;
		int checker = 0;
	}

	void TextureCollision::CreateTextureMesh(vector<int>& cells, vector<GroupInfo>& groups, CoordContext& context) {
		auto start = std::chrono::steady_clock::now();

		//塊の輪郭抽出
		m_Contours.clear();
		m_Contours.resize(groups.size());
		vector<Vec2> findIndices = {
			{ 0,-1},{  1,-1},{ 1,0},{ 1, 1},
			{ 0, 1},{ -1, 1},{-1,0},{-1,-1},
		};
		
		int page = 1;
		vector<thread> contourThreads;

		for (int i = 0; i < groups.size();i++) {
			GetContour(cells, groups[i], m_Contours[i]);
		}
		auto end = std::chrono::steady_clock::now();
		auto contourDuration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;

		m_ContourTriangles.clear();
		m_ContourTriangles.reserve(m_Contours.size());

		float totalDouglasDuration = 0.0f;
		float totalEarClippingDuration = 0.0f;
		//頂点最適化
		for (auto& contour : m_Contours) {
			start = std::chrono::steady_clock::now();
			vector<int> tempContour;
			DouglasPeucker::Calc(contour, 0, (int)contour.size() - 1, 2.0, m_TextureContext, tempContour);
			contour = tempContour;
			end = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
			totalDouglasDuration += duration;

			start = std::chrono::steady_clock::now();
			vector<vector<Vec3>> triangles = EarClipping::Calc(CalcContourWorldPosition(contour));
			m_ContourTriangles.push_back(triangles);
			end = std::chrono::steady_clock::now();
			duration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
			totalEarClippingDuration += duration;
		}
		int checker = 0;
	}
	void TextureCollision::GetContour(vector<int>& cells,GroupInfo& group, vector<int>& out) {
		struct Vec2Int {
			int x, y;
		};
		vector<Vec2Int> findIndices = {
			{ 0,-1},{  1,-1},{ 1,0},{ 1, 1},
			{ 0, 1},{ -1, 1},{-1,0},{-1,-1},
		};

		int startIndex = group.id;
		int currentIndex = startIndex;
		int befIndex = -1;
		out.reserve(group.count);

		int count = 0;
		int checkDir = 6;

		do {
			for (int i = 0; i < 8; i++) {
				int dir = (checkDir + i) % 8;

				int x, y;
				x = currentIndex % m_TextureContext.m_SizeX;
				y = currentIndex / m_TextureContext.m_SizeX;
				if (findIndices[dir].x == -1 && x <= 0) continue;
				if (findIndices[dir].x == 1 && x >= (int)m_TextureContext.m_SizeX - 1) continue;
				if (findIndices[dir].y == -1 && y <= 0) continue;
				if (findIndices[dir].y == 1 && y >= (int)m_TextureContext.m_SizeY - 1) continue;

				int index = currentIndex + findIndices[dir].y * m_TextureContext.m_SizeX + findIndices[dir].x;
				if (cells[index] != -1 && cells[index] == group.id) {
					befIndex = currentIndex;
					currentIndex = index;
					out.push_back(index);
					checkDir = (dir + 6) % 8;
					break;
				}
			}

			count++;
		} while (currentIndex != startIndex);
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

	int DouglasPeucker::count = 0;

	float DouglasPeucker::CalcDistance(Vec2& start, Vec2& end, Vec2& point) {
		Vec2 startToEnd = end - start;
		Vec2 startToPoint = point - start;

		float sTeDot = startToEnd.dot(startToEnd);
		if (sTeDot == 0) return startToPoint.lengthSqr();

		float t = startToPoint.dot(startToEnd) / sTeDot;
		t = max(0.0f, min(1.0f, t));

		Vec2 closest = start + startToEnd * t;

		return (point - closest).lengthSqr();
	}
	void DouglasPeucker::Calc(const vector<int>& points, int start, int end, float epsilon, const CoordContext& context, vector<int>& output) {
		if (end - start <= 1) {
			vector<int> result = { points[start],points[end] };
			output.insert(output.end(), result.begin(), result.end());
			return;
		}
		count++;
		float maxDist = -10000000.0f;
		int index = -1;

		Vec2 startPoint = Vec2(static_cast<float>(points[start] % context.m_SizeX), static_cast<float>(points[start] / context.m_SizeX));
		Vec2 endPoint = Vec2(static_cast<float>(points[end] % context.m_SizeX), static_cast<float>(points[end] / context.m_SizeX));

		for (int i = start + 1; i < end; i++) {
			Vec2 point = Vec2(static_cast<float>(points[i] % context.m_SizeX), static_cast<float>(points[i] / context.m_SizeX));
			float dist = CalcDistance(startPoint, endPoint, point);

			if (dist > maxDist) {
				maxDist = dist;
				index = i;
			}
		}

		if (maxDist > epsilon * epsilon) {
			vector<int> result1, result2;
			Calc(points, start,     index, epsilon, context, result1);
			Calc(points, index,       end, epsilon, context, result2);

			output.assign(result1.begin(), result1.end() - 1);
			output.insert(output.end(), result2.begin(), result2.end());
		}
		else {

			vector<int> result = { points[start],points[end] };
			output.insert(output.end(), result.begin(), result.end());
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
	vector<vector<Vec3>> EarClipping::Calc(const vector<Vec3>& points) {
		if (points.size() < 3) return {};
		vector<Vec3> ear = points;
		vector<vector<Vec3>> triangles;
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

						if (IsContainInTriangle(ear[i], ear[left], ear[right], ear[j])) {
							isContained = true;
						}
					}
					if (!isContained) {
						triangles.push_back({ ear[i], ear[left], ear[right] });
						ear.erase(ear.begin() + i);
						break;
					}
				}
			}
		}
		triangles.push_back({ ear[0], ear[1], ear[2] });

		return triangles;
	}
}
//end basecross
