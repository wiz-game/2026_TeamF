#include "stdafx.h"
#include "PNTDXModelDraw.h"
#include "DXModelResource.h"

namespace basecross
{
	PNTDXModelDraw::PNTDXModelDraw(const shared_ptr<GameObject>& gameObjectPtr)
		: SmBaseDraw(gameObjectPtr),
		m_animationIndex(0),
		m_loop(true),
		m_speed(1.0f),
		m_boneIndexByChannel(),
		m_finalBoneMatrices(),
		m_boneGlobalMatrices(),
		m_boneIndexByName(),
		m_clips(),
		m_currentClipIndex(-1),
		m_currentAnimeName(),
		m_currentAnimeTime(0.0f)
	{
		SetDiffuse(bsm::Col4(1, 1, 1, 1));
	}

	PNTDXModelDraw::~PNTDXModelDraw()
	{
	}

	void PNTDXModelDraw::SetAnimationIndex(size_t index)
	{
		m_animationIndex = index;
		BuildChannelBoneMap();
	}

	void PNTDXModelDraw::SetLoop(bool loop)
	{
		m_loop = loop;
	}

	void PNTDXModelDraw::SetSpeed(float speed)
	{
		m_speed = speed;
	}

	// --- 既存の名前指定版（既存の呼び出しを壊さない） ---
	void PNTDXModelDraw::AddAnimation(
		const wstring& name,
		int startSample,
		int sampleLength,
		bool loop,
		float samplesParSecond)
	{
		if (sampleLength <= 0)
		{
			throw BaseException(
				L"SampleLength は 1 以上である必要があります",
				name,
				L"PNTDXModelDraw::AddAnimation()"
			);
		}
		if (samplesParSecond <= 0.0f)
		{
			throw BaseException(
				L"SamplesParSecond は 0 より大きい必要があります",
				name,
				L"PNTDXModelDraw::AddAnimation()"
			);
		}

		AnimationClip clip{};
		clip.name = name;
		clip.startSample = startSample;
		clip.sampleLength = sampleLength;
		clip.loop = loop;
		clip.samplesParSecond = samplesParSecond;
		clip.animationIndex = 0; // 既存の挙動と互換性を保つ（デフォルトで 0 を参照）

		m_clips.push_back(clip);

		if (m_currentClipIndex < 0)
		{
			ChangeCurrentAnimation(name, 0.0f);
		}
	}

	// --- インデックス指定版 ---
	void PNTDXModelDraw::AddAnimation(
		size_t animationIndex,
		const wstring& clipName,
		int startSample,
		int sampleLength,
		bool loop,
		float samplesParSecond)
	{
		auto skinned = dynamic_pointer_cast<DXModelResource>(GetMultiMeshResource());
		if (!skinned)
		{
			throw BaseException(
				L"SkinnedModelResource が設定されていません",
				clipName,
				L"PNTDXModelDraw::AddAnimation(index)"
			);
		}
		auto model = skinned->GetDXModel();
		if (!model)
		{
			throw BaseException(
				L"DXModel が取得できません",
				clipName,
				L"PNTDXModelDraw::AddAnimation(index)"
			);
		}
		if (animationIndex >= model->animations.size())
		{
			throw BaseException(
				L"指定アニメーションインデックスが範囲外です",
				clipName,
				L"PNTDXModelDraw::AddAnimation(index)"
			);
		}

		if (sampleLength <= 0)
		{
			throw BaseException(
				L"SampleLength は 1 以上である必要があります",
				clipName,
				L"PNTDXModelDraw::AddAnimation(index)"
			);
		}
		if (samplesParSecond <= 0.0f)
		{
			throw BaseException(
				L"SamplesParSecond は 0 より大きい必要があります",
				clipName,
				L"PNTDXModelDraw::AddAnimation(index)"
			);
		}

		// --- チャネル名がボーンにマップされるか検査してログ出力 ---
		{
			const auto& anim = model->animations[animationIndex];
			int total = static_cast<int>(anim.channels.size());
			int matched = 0;
			std::vector<std::wstring> missingNames;
			for (const auto& ch : anim.channels)
			{
				int bi = FindBoneIndexByName(model->bones, ch.nodeName);
				if (bi >= 0) ++matched;
				else
				{
					if (missingNames.size() < 8) missingNames.push_back(ch.nodeName);
				}
			}
		}

		AnimationClip clip{};
		clip.name = clipName;
		clip.startSample = startSample;
		clip.sampleLength = sampleLength;
		clip.loop = loop;
		clip.samplesParSecond = samplesParSecond;
		clip.animationIndex = animationIndex;

		m_clips.push_back(clip);

		if (m_currentClipIndex < 0)
		{
			ChangeCurrentAnimation(clipName, 0.0f);
		}
	}

	// --- 名前からアニメーションインデックスを見つける版 ---
	void PNTDXModelDraw::AddAnimation(
		const wstring& animationName,
		const wstring& clipName,
		int startSample,
		int sampleLength,
		bool loop,
		float samplesParSecond)
	{
		auto skinned = dynamic_pointer_cast<DXModelResource>(GetMultiMeshResource());
		if (!skinned)
		{
			throw BaseException(
				L"SkinnedModelResource が設定されていません",
				clipName,
				L"PNTDXModelDraw::AddAnimation(name)"
			);
		}
		auto model = skinned->GetDXModel();
		if (!model)
		{
			throw BaseException(
				L"DXModel が取得できません",
				clipName,
				L"PNTDXModelDraw::AddAnimation(name)"
			);
		}
		auto it = std::find_if(
			model->animations.begin(), model->animations.end(),
			[&](const dxm::RuntimeAnimation& anim) { return anim.name == animationName; });
		if (it == model->animations.end())
		{
			throw BaseException(
				L"指定アニメーション名が見つかりません",
				animationName,
				L"PNTDXModelDraw::AddAnimation(name)"
			);
		}
		size_t index = std::distance(model->animations.begin(), it);
		AddAnimation(index, clipName, startSample, sampleLength, loop, samplesParSecond);
	}

	void PNTDXModelDraw::ChangeCurrentAnimation(const wstring& animationName, float startTime)
	{
		auto animName = GetCurrentAnimation();
		if (animationName == animName)
		{
			// 同じアニメーションなら何もしない
			return;
		}

		for (int i = 0; i < static_cast<int>(m_clips.size()); ++i)
		{
			if (m_clips[i].name == animationName)
			{
				m_currentClipIndex = i;
				m_currentAnimeName = animationName;
				m_currentAnimeTime = (startTime < 0.0f) ? 0.0f : startTime;

				// クリップに紐づくアニメーションへチャネルマップを更新
				BuildChannelBoneMap();
				return;
			}
		}

		throw BaseException(
			L"指定されたアニメーションが登録されていません",
			animationName,
			L"PNTDXModelDraw::ChangeCurrentAnimation()"
		);
	}

	const wstring& PNTDXModelDraw::GetCurrentAnimation() const
	{
		return m_currentAnimeName;
	}

	float PNTDXModelDraw::GetCurrentAnimationTime() const
	{
		return m_currentAnimeTime;
	}

	void PNTDXModelDraw::SetCurrentAnimationTime(float time)
	{
		m_currentAnimeTime = time;
	}

	void PNTDXModelDraw::UpdateAnimation(float m_elapsedTime)
	{
		m_currentAnimeTime += m_elapsedTime * m_speed;
		UpdatePose();
	}

	void PNTDXModelDraw::OnCreate()
	{
		// 既存の Draw 系と同様、OnCreate 時点ではリソース必須にしない。
	}

	void PNTDXModelDraw::OnUpdate()
	{
		auto multiMesh = GetMultiMeshResource();
		if (!multiMesh)
		{
			return;
		}

		const size_t meshCount = multiMesh->GetMeshVecCount();
		for (size_t mi = 0; mi < meshCount; ++mi)
		{
			auto& dst = GetVecMultiLocalBones(mi);
			dst.resize(m_finalBoneMatrices.size());
			for (size_t bi = 0; bi < m_finalBoneMatrices.size(); ++bi)
			{
				dst[bi] = m_finalBoneMatrices[bi];
			}
		}
	}

	const vector<bsm::Mat4x4>* PNTDXModelDraw::GetVecLocalBonesPtr() const
	{
		if (m_finalBoneMatrices.empty())
		{
			return nullptr;
		}
		return &m_finalBoneMatrices;
	}

	const vector<bsm::Mat4x4>* PNTDXModelDraw::GetVecMultiLocalBonesPtr(size_t index) const
	{
		(void)index;
		return GetVecLocalBonesPtr();
	}

	void PNTDXModelDraw::OnDraw()
	{
		auto multiMesh = GetMultiMeshResource();
		if (!multiMesh)
		{
			return;
		}

		const auto* bones = GetVecLocalBonesPtr();
		auto& vec = multiMesh->GetMeshVec();

		for (size_t i = 0; i < vec.size(); ++i)
		{
			if (!GetMultiMeshIsDraw(i))
			{
				continue;
			}

			const bool canSkin = (bones != nullptr) && vec[i].m_IsSkining && (vec[i].m_BoneCount > 0);

			if (GetOwnShadowActive())
			{
				if (canSkin)
				{
					DrawModel<VSPNTBoneShadow, PSPNTStaticShadow>(vec[i]);
				}
				else
				{
					DrawModel<VSPNTStaticShadow, PSPNTStaticShadow>(vec[i]);
				}
			}
			else
			{
				if (canSkin)
				{
					DrawModel<VSPNTBone, PSPNTStatic>(vec[i]);
				}
				else
				{
					DrawModel<VSPNTStatic, PSPNTStatic>(vec[i]);
				}
			}
		}

		App::GetApp()->GetDeviceResources()->InitializeStates();
	}

	void PNTDXModelDraw::SetMultiMeshResource(const shared_ptr<MultiMeshResource>& meshResourcePtr)
	{
		SmBaseDraw::SetMultiMeshResource(meshResourcePtr);
		BoneInit();

		BuildBoneIndexMap();
		BuildChannelBoneMap();
	}

	int PNTDXModelDraw::FindBoneIndexByName(const vector<dxm::RuntimeBone>& bones, const wstring& name)
	{
		for (int i = 0; i < static_cast<int>(bones.size()); ++i)
		{
			if (bones[i].name == name)
			{
				return i;
			}
		}
		return -1;
	}

	float PNTDXModelDraw::WrapTimeTicks(float timeTicks, float durationTicks, bool loop)
	{
		if (durationTicks <= 0.0f)
		{
			return 0.0f;
		}
		if (loop)
		{
			float t = fmodf(timeTicks, durationTicks);
			if (t < 0.0f)
			{
				t += durationTicks;
			}
			return t;
		}
		if (timeTicks < 0.0f)
		{
			return 0.0f;
		}
		if (timeTicks > durationTicks)
		{
			return durationTicks;
		}
		return timeTicks;
	}

	XMFLOAT3 PNTDXModelDraw::SampleVector(const vector<dxm::RuntimeKeyVector>& keys, float timeTicks)
	{
		if (keys.empty())
		{
			return XMFLOAT3(0, 0, 0);
		}
		if (keys.size() == 1)
		{
			return keys[0].value;
		}

		size_t idx = 0;
		while (idx + 1 < keys.size() && timeTicks >= keys[idx + 1].time)
		{
			++idx;
		}
		if (idx + 1 >= keys.size())
		{
			return keys.back().value;
		}

		const auto& k0 = keys[idx];
		const auto& k1 = keys[idx + 1];
		const float span = (k1.time - k0.time);
		const float t = (span <= 0.0f) ? 0.0f : (timeTicks - k0.time) / span;

		XMFLOAT3 out;
		out.x = k0.value.x + (k1.value.x - k0.value.x) * t;
		out.y = k0.value.y + (k1.value.y - k0.value.y) * t;
		out.z = k0.value.z + (k1.value.z - k0.value.z) * t;
		return out;
	}

	XMFLOAT4 PNTDXModelDraw::SampleQuaternion(const vector<dxm::RuntimeKeyQuaternion>& keys, float timeTicks)
	{
		if (keys.empty())
		{
			return XMFLOAT4(0, 0, 0, 1);
		}
		if (keys.size() == 1)
		{
			return keys[0].value;
		}

		size_t idx = 0;
		while (idx + 1 < keys.size() && timeTicks >= keys[idx + 1].time)
		{
			++idx;
		}
		if (idx + 1 >= keys.size())
		{
			return keys.back().value;
		}

		const auto& k0 = keys[idx];
		const auto& k1 = keys[idx + 1];
		const float span = (k1.time - k0.time);
		const float t = (span <= 0.0f) ? 0.0f : (timeTicks - k0.time) / span;

		XMVECTOR q0 = XMLoadFloat4(&k0.value);
		XMVECTOR q1 = XMLoadFloat4(&k1.value);
		q0 = XMQuaternionNormalize(q0);
		q1 = XMQuaternionNormalize(q1);

		XMVECTOR q = XMQuaternionSlerp(q0, q1, t);
		q = XMQuaternionNormalize(q);

		XMFLOAT4 out;
		XMStoreFloat4(&out, q);
		return out;
	}

	XMMATRIX PNTDXModelDraw::EvaluateChannelLocalMatrix(const dxm::RuntimeAnimationChannel& channel, float timeTicks)
	{
		XMFLOAT3 pos = SampleVector(channel.positionKeys, timeTicks);
		XMFLOAT3 scl = channel.scalingKeys.empty() ? XMFLOAT3(1, 1, 1) : SampleVector(channel.scalingKeys, timeTicks);
		XMFLOAT4 rot = SampleQuaternion(channel.rotationKeys, timeTicks);

		XMMATRIX t = XMMatrixTranslation(pos.x, pos.y, pos.z);
		XMMATRIX r = XMMatrixRotationQuaternion(XMLoadFloat4(&rot));
		XMMATRIX s = XMMatrixScaling(scl.x, scl.y, scl.z);

		return s * r * t;
	}

	void PNTDXModelDraw::BuildChannelBoneMap()
	{
		m_boneIndexByChannel.clear();

		auto skinned = dynamic_pointer_cast<DXModelResource>(GetMultiMeshResource());
		if (!skinned)
		{
			return;
		}

		auto model = skinned->GetDXModel();
		if (!model)
		{
			return;
		}

		if (model->animations.empty())
		{
			return;
		}

		// 使用するアニメーションインデックスを決定（現在のクリップのアニメーションを優先）
		size_t animIndex = m_animationIndex;
		if (m_currentClipIndex >= 0 && m_currentClipIndex < static_cast<int>(m_clips.size()))
		{
			animIndex = m_clips[m_currentClipIndex].animationIndex;
		}

		if (animIndex >= model->animations.size())
		{
			return;
		}

		const auto& anim = model->animations[animIndex];
		m_boneIndexByChannel.resize(anim.channels.size(), -1);

		for (size_t i = 0; i < anim.channels.size(); ++i)
		{
			m_boneIndexByChannel[i] = FindBoneIndexByName(model->bones, anim.channels[i].nodeName);
		}
	}

	void PNTDXModelDraw::BuildBoneIndexMap()
	{
		m_boneIndexByName.clear();

		auto skinned = dynamic_pointer_cast<DXModelResource>(GetMultiMeshResource());
		if (!skinned)
		{
			return;
		}

		auto model = skinned->GetDXModel();
		if (!model)
		{
			return;
		}

		const auto& bones = model->bones;
		m_boneIndexByName.reserve(bones.size());

		for (int i = 0; i < static_cast<int>(bones.size()); ++i)
		{
			if (m_boneIndexByName.find(bones[i].name) == m_boneIndexByName.end())
			{
				m_boneIndexByName.emplace(bones[i].name, i);
			}
		}
	}

	int PNTDXModelDraw::FindBoneIndexByNameFast(const wstring& name) const
	{
		const auto it = m_boneIndexByName.find(name);
		if (it == m_boneIndexByName.end())
		{
			return -1;
		}
		return it->second;
	}

	void PNTDXModelDraw::UpdatePose()
	{
		auto skinned = dynamic_pointer_cast<DXModelResource>(GetMultiMeshResource());
		if (!skinned)
		{
			return;
		}

		auto model = skinned->GetDXModel();
		if (!model)
		{
			return;
		}

		const auto& bones = model->bones;
		if (bones.empty())
		{
			return;
		}

		if (bones.size() > 512)
		{
			throw BaseException(
				L"ボーン数が SimpleConstants::Bones の上限(512)を超えています",
				L"if (bones.size() > 512)",
				L"PNTDXModelDraw::UpdatePose()"
			);
		}

		if (model->animations.empty())
		{
			return;
		}

		// クリップごとのanimationIndexを参照
		size_t animIndex = m_animationIndex;
		if (m_currentClipIndex >= 0 && m_currentClipIndex < static_cast<int>(m_clips.size()))
		{
			animIndex = m_clips[m_currentClipIndex].animationIndex;
		}

		if (animIndex >= model->animations.size())
		{
			animIndex = 0;
		}

		const auto& anim = model->animations[animIndex];
		const float ticksPerSecond = (anim.ticksPerSecond > 0.0f) ? anim.ticksPerSecond : 30.0f;

		float timeTicks = 0.0f;

		if (m_currentClipIndex >= 0 && m_currentClipIndex < static_cast<int>(m_clips.size()))
		{
			const auto& clip = m_clips[m_currentClipIndex];

			const float sampleTime = m_currentAnimeTime * clip.samplesParSecond;
			const float wrappedSample = WrapTimeTicks(
				sampleTime,
				static_cast<float>(clip.sampleLength),
				clip.loop
			);

			const float ticksPerSample = ticksPerSecond / clip.samplesParSecond;
			timeTicks = (static_cast<float>(clip.startSample) + wrappedSample) * ticksPerSample;

			timeTicks = WrapTimeTicks(timeTicks, anim.m_duration, true);
		}
		else
		{
			timeTicks = m_currentAnimeTime * ticksPerSecond;
			timeTicks = WrapTimeTicks(timeTicks, anim.m_duration, m_loop);
		}

		vector<XMMATRIX> local(bones.size());
		for (size_t i = 0; i < bones.size(); ++i)
		{
			local[i] = bones[i].localTransform;
		}

		for (size_t ci = 0; ci < anim.channels.size(); ++ci)
		{
			const int bi = (ci < m_boneIndexByChannel.size()) ? m_boneIndexByChannel[ci] : -1;
			if (bi < 0)
			{
				continue;
			}
			local[bi] = EvaluateChannelLocalMatrix(anim.channels[ci], timeTicks);
		}

		vector<XMMATRIX> global(bones.size());
		for (size_t i = 0; i < bones.size(); ++i)
		{
			const int parent = bones[i].parentIndex;
			if (parent < 0)
			{
				global[i] = local[i];
			}
			else
			{
				global[i] = local[i] * global[parent];
			}
		}

		m_boneGlobalMatrices.clear();
		m_boneGlobalMatrices.resize(bones.size());
		for (size_t i = 0; i < bones.size(); ++i)
		{
			bsm::Mat4x4 out;
			XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&out), global[i]);
			m_boneGlobalMatrices[i] = out;
		}

		bsm::Mat4x4 cancellation;
		cancellation.identity();
		// ルートモーション処理
		if (m_rootMotion)
		{
			Vec3 currentBonePos = m_boneGlobalMatrices[m_rootMotionBoneIndex].transInMatrix();
			if (m_isFirstFrame)
			{
				m_prevRootMotionOffset = currentBonePos;
				m_firstFrameRootPos = currentBonePos;
				m_isFirstFrame = false;
			}
			else
			{
				Vec3 offsetTranslation = currentBonePos - m_prevRootMotionOffset;
				m_prevRootMotionOffset = currentBonePos;
				cancellation = bsm::Mat4x4(XMMatrixTranslation(-currentBonePos.x, -currentBonePos.y + m_firstFrameRootPos.y, -currentBonePos.z));
			}
		}

		m_finalBoneMatrices.clear();
		m_finalBoneMatrices.resize(bones.size());

		for (size_t i = 0; i < bones.size(); ++i)
		{
			const XMMATRIX skin = bones[i].offsetMatrix * global[i];

			bsm::Mat4x4 out;
			XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&out), skin);
			m_finalBoneMatrices[i] = out * cancellation;
		}
	}

	bsm::Mat4x4 PNTDXModelDraw::GetBoneWorldMatrix(size_t boneIndex) const
	{
		auto skinned = dynamic_pointer_cast<DXModelResource>(GetMultiMeshResource());
		if (!skinned)
		{
			throw BaseException(
				L"SkinnedModelResource が設定されていません",
				L"if (!skinned)",
				L"PNTDXModelDraw::GetBoneWorldMatrix(size_t)"
			);
		}

		if (m_boneGlobalMatrices.empty())
		{
			return bsm::Mat4x4(0);
		}

		if (boneIndex >= m_boneGlobalMatrices.size())
		{
			throw BaseException(
				L"指定ボーンインデックスが範囲外です",
				L"if (boneIndex >= m_boneGlobalMatrices.size())",
				L"PNTDXModelDraw::GetBoneWorldMatrix(size_t)"
			);
		}

		const bsm::Mat4x4 ownerWorld = GetGameObject()->GetComponent<Transform>()->GetWorldMatrix();
		return m_boneGlobalMatrices[boneIndex] * ownerWorld;
	}

	// 既存の名前指定版はそのまま
	bsm::Mat4x4 PNTDXModelDraw::GetBoneWorldMatrix(const wstring& boneName) const
	{
		auto skinned = dynamic_pointer_cast<DXModelResource>(GetMultiMeshResource());
		if (!skinned)
		{
			throw BaseException(
				L"SkinnedModelResource が設定されていません",
				L"if (!skinned)",
				L"PNTDXModelDraw::GetBoneWorldMatrix()"
			);
		}

		if (m_boneGlobalMatrices.empty())
		{
			return bsm::Mat4x4(0);
		}

		const int boneIndex = FindBoneIndexByNameFast(boneName);
		if (boneIndex < 0)
		{
			throw BaseException(
				L"指定ボーンが見つかりません",
				L"Bone not found",
				L"PNTDXModelDraw::GetBoneWorldMatrix()"
			);
		}

		if (static_cast<size_t>(boneIndex) >= m_boneGlobalMatrices.size())
		{
			throw BaseException(
				L"ボーン行列キャッシュのサイズが不正です",
				L"if (boneIndex >= m_boneGlobalMatrices.size())",
				L"PNTDXModelDraw::GetBoneWorldMatrix()"
			);
		}

		const bsm::Mat4x4 ownerWorld = GetGameObject()->GetComponent<Transform>()->GetWorldMatrix();
		return m_boneGlobalMatrices[boneIndex] * ownerWorld;
	}
}