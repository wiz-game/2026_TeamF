/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "Singleton.h"
#include "ComputeShader.h"
#include "ProjectShader.h"
#include "InkDrawComponentTest.h"
#include "Port.h"
namespace basecross{

	struct CoordContext {
		UINT m_SizeX = 0;
		UINT m_SizeY = 0;
	};
	class TextureCollision : public Component {
		struct IndexInfo {
			int index;
			int label;
		};
		struct GroupInfo {
			int id;
			int startIndex;
			int count;
		};

		CoordContext m_TextureContext;
		vector<int> m_Labels;
		vector<vector<int>> m_Contours;
		vector<int> m_ElectricContourIndices;

		//m_ContourTriangles[] : 輪郭
		//m_ContourTriangles[][] : 輪郭に含まれるポリゴン
		vector<vector<TRIANGLE>> m_ContourTriangles;

		shared_ptr<DX11ComputeShader> m_MaskShader;
		shared_ptr<DX11ComputeShader> m_UnionFind1Shader;
		shared_ptr<DX11ComputeShader> m_UnionFind2Shader;
		shared_ptr<BufferContext> m_LabelBuffer;
		shared_ptr<BufferContext> m_LabelOutputBuffer;
		shared_ptr<BufferContext> m_ConvertFlagBuffer;
		TextureSizeConstantData m_CB;

		void GetSrvResource(ID3D11Texture2D** texture, D3D11_TEXTURE2D_DESC* desc);
		void CreateAlphaMask();
		void CreateTextureMesh(vector<int>& cells,vector<int>& groupIDs, CoordContext& context);
		void IndexToCoord(int index, int width, int& x, int& y);
		void CoordToIndex(int& index, int x, int y, int width);

		void GetContour(vector<int>& cells,int& groupID, vector<int>& out);
		vector<Vec3> CalcContourWorldPosition(const vector<int>& contour);

		void DrawLine(Vec3 position, Vec3 dir,float length);
	public:
		TextureCollision(const shared_ptr<GameObject>& ptr);
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDraw()override;

		void CreateMeshCollision();

		void ProcessGPU();
		void ProcessCPU();

		size_t GetContourCount()const { return m_ContourTriangles.size(); }
		vector<TRIANGLE> GetTriangles(int index) { return m_ContourTriangles[index]; }
		void DrawContour(int index);
		void AddElectricIndex(int index);
		bool IsElectrified(int index);
	};

	class DouglasPeucker {
		vector<int> m_Points;
		vector<Vec2> m_Positions;
		inline float CalcDistance(Vec2& start, Vec2& end, Vec2& point);
	public:
		void Initialize(const vector<int>& points, const CoordContext& context);
		void Calc(int start,int end, float epsilon, vector<int>& output);
	};

	class EarClipping {
		static bool IsAngleThen180(const Vec3& point, const Vec3& a, const Vec3& b);
		static bool IsContainInTriangle(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& point);
	public:
		static vector<TRIANGLE> Calc(const vector<Vec3>& points);
	};


	class TextureMeshManager : public SingletonBase<TextureMeshManager> {
		friend class SingletonBase<TextureMeshManager>;
		vector<shared_ptr<TextureCollision>> m_ReloadMeshCollisions;
	public:
		void AddReload(const shared_ptr<TextureCollision>& meshCollision) {
			m_ReloadMeshCollisions.push_back(meshCollision);
		}

		void Reload();
	};

	class InkConnectChecker : public SingletonBase<InkConnectChecker> {
		friend class SingletonBase<InkConnectChecker>;

		//ステージに存在する電源
		vector<weak_ptr<PowerSupply>> m_PowerSupplies;
		//ステージに存在するポート
		vector<weak_ptr<Port>> m_Ports;
		//ステージに存在するインク当たり判定
		vector<weak_ptr<TextureCollision>> m_TextureCollisions;

		bool IsConnectedSupplyToInk(const OBB& supplyOBB, const AABB& supplyAABB, const vector<TRIANGLE>& triangles);
		bool IsConnectedInkToInk(const vector<TRIANGLE>& triangles, const shared_ptr<TextureCollision>& fromCollision);
		bool IsConnectedInkToPort(const OBB& portOBB, const AABB& portAABB, const vector<TRIANGLE>& triangles);
	public:

		void Initialize();
		vector<pair<weak_ptr<PowerSupply>, weak_ptr<Port>>> CheckConnect();

		void AddTextureCollision(const shared_ptr<TextureCollision>& collision) {
			m_TextureCollisions.push_back(collision);
		}
		void AddPowerSupply(const shared_ptr<PowerSupply>& supply) {
			m_PowerSupplies.push_back(supply);
		}
		void AddPort(const shared_ptr<Port>& port) {
			m_Ports.push_back(port);
		}
	};

}
//end basecross
