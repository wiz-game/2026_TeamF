/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	struct TextureSizeConstantData {
		int width;
		int height;
		int padding[2];
	};

	DECLARE_DX11_COMPUTE_SHADER(GenerateMaskShader)
	DECLARE_DX11_COMPUTE_SHADER(FloorFillShader)
	DECLARE_DX11_CONSTANT_BUFFER(TextureSizeConstantBuffer, TextureSizeConstantData)

	struct MaskData {
		UINT m_Mask = 0;
		bool m_IsVisited = false;
	};

	class TextureCollision : public Collision {
		
		struct CoordContext {
			UINT m_SizeX = 0;
			UINT m_SizeY = 0;
		};

		void GetSrvResource(ID3D11Texture2D** texture, D3D11_TEXTURE2D_DESC* desc);
		void CreateAlphaMask(BufferContext& bufferContext,CoordContext& coordContext);
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
