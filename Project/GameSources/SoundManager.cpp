/*!
@file SoundManager.cpp
@brief TE“hŠÖŚW
*/

#include "stdafx.h"
#include "SoundManager.h"

namespace basecross {
	void SoundManager::RegisterSounds() {
		m_Audio = App::GetApp()->GetXAudio2Manager();
		//BGM
		RegisterSound(L"TITLE_BGM", L"Title.wav");
		RegisterSound(L"STAGESELECT_BGM", L"StageSelect.wav");
		RegisterSound(L"GAMESTAGE_BGM", L"Stage.wav");

		//RegisterSound(L"TITLE_BGM", L"TitleBGM.wav");
		//RegisterSound(L"STAGESELECT_BGM", L"StageSelectBGM.wav");
		//RegisterSound(L"GAMESTAGE_BGM", L"GameStageBGM.wav");
		//SE
		RegisterSound(L"SELECT", L"SelectSE.wav");
		RegisterSound(L"CONFIRM", L"ConfirmSE.wav");
		RegisterSound(L"GAUGE", L"gaugeSE.wav");
		RegisterSound(L"PLAYER_MOVE", L"PlayerMove.wav");
		RegisterSound(L"ELECTRIC", L"Electric.wav");
		RegisterSound(L"ELEVATER_MOVE", L"ElevatorMove.wav");
		RegisterSound(L"TITLE_BGM", L"TitleBGM.wav");
		RegisterSound(L"STAGESELECT_BGM", L"StageSelectBGM.wav");
		RegisterSound(L"GAMESTAGE_BGM", L"GameStageBGM.wav");
		RegisterSound(L"STEELHIT", L"SteelHit.wav");
		RegisterSound(L"PRINTER_SE_1", L"Printer_1.wav");
		RegisterSound(L"PRINTER_SE_2", L"Printer_2.wav");
		RegisterSound(L"RESULT_BGM", L"Result.wav");
		RegisterSound(L"GAMEOVER_BGM", L"GameOver.wav");
	}
	void SoundManager::RegisterSound(const wstring& key, const wstring& fileName) {
		wstring path = App::GetApp()->GetDataDirWString() + L"Sounds/";

		auto audioRes = App::GetApp()->RegisterWav(key, path + fileName);
		m_SoundKeys.push_back(key);
	}
	shared_ptr<SoundItem> SoundManager::PlayLoopSE(const wstring& key, const float volume) {
		if (find(m_SoundKeys.begin(), m_SoundKeys.end(), key) != m_SoundKeys.end()) {
			auto se = m_Audio->Start(key, XAUDIO2_LOOP_INFINITE, volume * m_SEVolume);
			m_PlayerSE.push_back(se);
			return se;
		}
		return nullptr;
	}
	void SoundManager::StopLoopSE(const shared_ptr<SoundItem>& soundItem) {
		if (!soundItem) return;
		auto it = find(m_PlayerSE.begin(), m_PlayerSE.end(), soundItem);
		if (it != m_PlayerSE.end()) {
			m_Audio->Stop(soundItem);
			m_PlayerSE.erase(it);
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
	void SoundManager::Stop(const shared_ptr<SoundItem>& soundItem) {
		soundItem->m_SourceVoice->Stop();
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
		for (auto se : m_PlayerSE) {
			m_Audio->Stop(se);
		}
		m_PlayerSE.clear();
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
