/*!
@file SoundManager.cpp
@brief ƒTƒEƒ“ƒhŠÖŒW
*/

#include "stdafx.h"
#include "SoundManager.h"

namespace basecross {
	void SoundManager::RegisterSounds() {
		m_Audio = App::GetApp()->GetXAudio2Manager();

	}
	void SoundManager::RegisterSound(const wstring& key, const wstring& fileName) {
		wstring path = App::GetApp()->GetDataDirWString() + L"Sounds/";

		auto audioRes = App::GetApp()->RegisterWav(key, path + fileName);
		m_SoundKeys.push_back(key);
	}
	void SoundManager::PlayLoopSE(const wstring& key, const float volume) {
		if (find(m_SoundKeys.begin(), m_SoundKeys.end(), key) != m_SoundKeys.end()) {
			auto se = m_Audio->Start(key, XAUDIO2_LOOP_INFINITE, volume * m_SEVolume);
			m_PlayingSE.insert(pair<wstring, shared_ptr<SoundItem>>(key, se));
		}
	}
	void SoundManager::StopLoopSE(const wstring& key) {
		if (m_PlayingSE.find(key) != m_PlayingSE.end()) {
			m_Audio->Stop(m_PlayingSE[key]);
			m_PlayingSE.erase(key);
		}
	}
	shared_ptr<SoundItem> SoundManager::PlaySE(const wstring& key, const float volume) {
		if (m_Audio == nullptr) {
			m_Audio = App::GetApp()->GetXAudio2Manager();
		}
		auto se = m_Audio->Start(key, 0, volume * m_SEVolume);
		return se;
	}
	shared_ptr<SoundItem> SoundManager::PlayBGM(const wstring& key, const float volume) {
		if (m_Audio == nullptr) {
			m_Audio = App::GetApp()->GetXAudio2Manager();
		}
		if (m_Bgm != nullptr) {
			StopBGM();
		}
		m_Bgm = m_Audio->Start(key, XAUDIO2_LOOP_INFINITE, volume * m_BGMVolume);
		return m_Bgm;
	}
	void SoundManager::SetBGMVolume() {
		if (m_Bgm != nullptr) {
			m_Bgm->m_SourceVoice->SetVolume(m_BGMVolume);
		}
	}
	void SoundManager::PauseBGM(bool flag) {
		if (m_Bgm != nullptr) {
			m_Bgm->m_SourceVoice->Stop(XAUDIO2_PLAY_TAILS);

			if (!flag) {
				m_Bgm->m_SourceVoice->Start();
			}
		}
	}
	void SoundManager::StopBGM() {
		if (m_Audio == nullptr) {
			m_Audio = App::GetApp()->GetXAudio2Manager();
		}
		if (m_Bgm != nullptr) {
			m_Audio->Stop(m_Bgm);
			m_Bgm = nullptr;
	}
		}
	void SoundManager::StopAll() {
		StopBGM();
		for (auto se : m_PlayingSE) {
			m_Audio->Stop(se.second);
		}
		m_PlayingSE.clear();
	}
	bool SoundManager::IsSoundRunning(const shared_ptr<SoundItem>& soundItem) {
		if (soundItem->m_SourceVoice) {
			XAUDIO2_VOICE_STATE state;
			soundItem->m_SourceVoice->GetState(&state);
			return (state.BuffersQueued > 0) != 0;
		}
		return false;
	}
}
