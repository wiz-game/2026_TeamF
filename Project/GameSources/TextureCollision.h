/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

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
		vector<vector<int>> m_Contours;
		vector<vector<vector<Vec3>>> m_ContourTriangles;
		shared_ptr<MeshResource> m_MeshResource;
		void GetSrvResource(ID3D11Texture2D** texture, D3D11_TEXTURE2D_DESC* desc);
		void CreateAlphaMask(BufferContext& bufferContext,CoordContext& coordContext);
		void CreateTextureMesh(vector<int>& cells,vector<GroupInfo>& groups, CoordContext& context);
		void IndexToCoord(int index, int width, int& x, int& y);
		void CoordToIndex(int& index, int x, int y, int width);

		void GetContour(vector<int>& cells,GroupInfo& group, vector<int>& out);
		vector<Vec3> CalcContourWorldPosition(const vector<int>& contour);

		void DrawLine(Vec3 position, Vec3 dir,float length);
	public:
		TextureCollision(const shared_ptr<GameObject>& ptr);
		virtual void OnCreate()override;
		virtual void OnUpdate()override;
		virtual void OnDraw()override;

		void CreateMeshCollision();

	};

	class DouglasPeucker {
		static int count;
		static float CalcDistance(Vec2& start, Vec2& end, Vec2& point);
	public:
		static void Calc(const vector<int>& points,int start,int end, float epsilon, const CoordContext& context, vector<int>& output);
	};

	class EarClipping {
		static bool IsAngleThen180(const Vec3& point, const Vec3& a, const Vec3& b);
		static bool IsContainInTriangle(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& point);
	public:
		static vector<vector<Vec3>> Calc(const vector<Vec3>& points);
	};
}
//end basecross
