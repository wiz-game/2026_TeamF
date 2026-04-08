/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{

	class TextureCollision : public Collision {
		void GetSrvResource(ID3D11Texture2D** texture, D3D11_TEXTURE2D_DESC* desc);
		uint8_t* ReadColorData(ID3D11DeviceContext2* context,ID3D11Texture2D* texture,UINT& rowPitch);
		vector<vector<UINT>>& CreateAlphaMask();
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
