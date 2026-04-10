/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	
	class TextureCollision : public Collision {
		struct MaskData {
			UINT m_Mask = 0;
			bool m_IsVisited = false;
		};
		struct CoordContext {
			int m_SizeX = 0;
			int m_SizeY = 0;
		};

		void GetSrvResource(ID3D11Texture2D** texture, D3D11_TEXTURE2D_DESC* desc);
		uint8_t* ReadColorData(ID3D11DeviceContext2* context,ID3D11Texture2D* texture,UINT& rowPitch);
		vector<MaskData> CreateAlphaMask(CoordContext& coordContext);
		vector<int> BfsTree(vector<MaskData>& masks,const CoordContext& context, int start);
		void IndexToCoord(int index, int width, int& x, int& y);
		void CoordToIndex(int& index, int x, int y, int width);
	public:
		TextureCollision(const shared_ptr<GameObject>& ptr);
		virtual void OnCreate()override;
		virtual void OnDraw()override;

		void CreateMeshCollision();

		virtual bool SimpleCollisionCall(const shared_ptr<Collision>& Src);
		virtual void CollisionCall(const shared_ptr<Collision>& Src);
		virtual bsm::Vec3 GetCenterPosition()const;
		virtual AABB GetEnclosingAABB()const;
		virtual AABB GetWrappedAABB()const;

	};
}
//end basecross
