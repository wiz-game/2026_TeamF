#pragma once
#include "stdafx.h"
#include "DXModel.h"

namespace basecross
{
	class DXModelResource : public MultiMeshResource
	{
		friend class ObjectFactory;

		DXModelResource();

	public:
		virtual ~DXModelResource();

		void SetDXModel(const shared_ptr<dxm::DXModel>& model);
		shared_ptr<dxm::DXModel> GetDXModel() const;

		// ƒ‚ƒfƒ‹“o˜^: ƒƒ‚ƒŠ‚©‚ç“o˜^
		static shared_ptr<DXModelResource> RegisterDXModel(
			const wstring& key,
			const shared_ptr<dxm::DXModel>& model,
			bool accessWrite = false
		);

	private:
		shared_ptr<dxm::DXModel> m_dxModel;
	};
}