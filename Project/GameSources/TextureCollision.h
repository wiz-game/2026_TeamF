/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "Singleton.h"
#include "ComputeShader.h"
#include "ProjectShader.h"
#include "Port.h"
#include <opencv2/opencv.hpp>
#include <poly2tri/poly2tri.h>
#include <queue>
namespace basecross{
	struct CoordContext {
		UINT m_SizeX = 0;
		UINT m_SizeY = 0;
	};
	struct Contour {
		AABB m_Aabb;
		vector<TRIANGLE> m_Triangles;

		Contour(const vector<TRIANGLE>& triangles) : m_Triangles(triangles), m_Aabb{ Vec3(numeric_limits<float>::max()),Vec3(numeric_limits<float>::lowest()) } {}
		void CalcAABB(const shared_ptr<Transform>& transform);
		vector<TRIANGLE> GetWorldTriangles(const shared_ptr<Transform>& transform)const;
	};

	struct TextureSnapShot {
		vector<int> m_Data;
		CoordContext m_Context;
		shared_ptr<Transform> m_Transform;
		vector<vector<cv::Point>> m_CvContours;
	};

	class TextureCollision : public Component {
		CoordContext m_TextureContext;
		vector<int> m_Labels;
		vector<int> m_ElectricContourIndices;
		vector<int> m_WaitElectricContourIndices;

		float m_EffectSpawnTimer;
		float m_EffectSpawnInterval;
		vector<int> m_ElectricEffectHandles;

		vector<Contour> m_Contour;
		vector<Contour> m_WaitContour;
		vector<vector<cv::Point>> m_CvContours;

		shared_ptr<DX11ComputeShader> m_MaskShader;
		shared_ptr<BufferContext> m_LabelBuffer;
		TextureSizeConstantData m_CB;

		void GetSrvResource(ID3D11Texture2D** texture, D3D11_TEXTURE2D_DESC* desc);
		vector<TRIANGLE> CalcContourWorldTriangle(const vector<p2t::Triangle*>& contour, const TextureSnapShot& snapShot);

		void DrawLine(Vec3 position, Vec3 dir,float length);
	public:
		TextureCollision(const shared_ptr<GameObject>& ptr);
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDraw()override;

		void ProcessGPU();
		void ProcessCPU();

		size_t GetContourCount()const { return m_Contour.size(); }
		vector<TRIANGLE> GetWorldTriangles(int index)const;
		const AABB& GetContourAABB(int index)const { return m_Contour[index].m_Aabb; }
		void DrawContour(int index);

		void ClearElectricIndex();
		void SetElectricfield(int index);
		bool IsElectrified(int index);

		void CreateMeshInThread(const TextureSnapShot& snapShot,vector<Contour>& result);
		TextureSnapShot SnapShot();
		void ApplyThreadResult(vector<Contour>& result) {
			m_WaitContour = result;
			m_WaitElectricContourIndices.resize(m_WaitContour.size(), 0);
		}
	};


	//スレッドプール(後で別ファイルに移行)
	class ThreadPool : public SingletonBase<ThreadPool>{
		friend class SingletonBase<ThreadPool>;

		vector<thread> m_Workers;
		queue<function<void()>> m_Tasks;

		mutex m_Mutex;
		condition_variable m_Condition;
		condition_variable m_WaitCondition;

		bool m_ThreadStop;
		int m_RunningTask;

		void Worker();
	public:

		void Initialize(size_t numThreads);
		void Destory();
		void Execute(function<void()> task);
		void Wait();

	};

	struct MeshResult {
		shared_ptr<TextureCollision> m_Ptr;
		vector<Contour> m_Result;
		~MeshResult() {
			
		}
	};
	class TextureMeshManager : public SingletonBase<TextureMeshManager> {
		friend class SingletonBase<TextureMeshManager>;
		vector<shared_ptr<TextureCollision>> m_ReloadMeshCollisions;

		unordered_map<shared_ptr<TextureCollision>, TextureSnapShot> m_Pending;
		unordered_map<shared_ptr<TextureCollision>, TextureSnapShot> m_Proccess;
		queue<MeshResult> m_ResultQueue;
		atomic_int m_ProccessCount;

		mutex m_Mutex;

		void DecreeseProccessCount();
	public:
		void Update();
		void Clear();
		void AddReload(const shared_ptr<TextureCollision>& meshCollision);
		void Reload();
	};

	class PowerSupply;
	class InkConnectChecker : public SingletonBase<InkConnectChecker> {
		friend class SingletonBase<InkConnectChecker>;

		//ステージに存在する電源
		vector<weak_ptr<PowerSupply>> m_PowerSupplies;
		//ステージに存在するポート
		vector<weak_ptr<Port>> m_Ports;
		//ステージに存在するインク当たり判定
		vector<weak_ptr<TextureCollision>> m_TextureCollisions;

		bool IsConnectedSupplyToInk(const OBB& supplyOBB, const AABB& supplyAABB, const vector<TRIANGLE>& triangles);
		bool IsConnectedInkToInk(const vector<TRIANGLE>& triangles, const AABB& inkAABB);
		bool IsConnectedInkToPort(const OBB& portOBB, const AABB& portAABB, const vector<TRIANGLE>& triangles);
	public:

		void Initialize();
		vector<pair<weak_ptr<PowerSupply>, weak_ptr<Port>>> CheckConnect();

		void AddTextureCollision(const shared_ptr<TextureCollision>& collision) {
			m_TextureCollisions.push_back(collision);
			TextureMeshManager::Get().AddReload(collision);
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
