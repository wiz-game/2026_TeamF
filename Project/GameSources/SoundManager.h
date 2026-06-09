/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "Singleton.h"

namespace basecross{
	class SoundManager : public SingletonBase<SoundManager> {
	protected:
		friend class SingletonBase<SoundManager>;
		SoundManager() : m_SEVolume(1.0f), m_BGMVolume(0.5f) {}
	public:
		void RegisterSounds();
		void RegisterSound(const wstring& key, const wstring& fileName);
		shared_ptr<SoundItem> PlayLoopSE(const wstring& key, const float volume = 1.0f);
		void StopLoopSE(const shared_ptr<SoundItem>& soundItem);

		shared_ptr<SoundItem> PlaySE(const wstring& key, const float volume = 5.0f);
		shared_ptr<SoundItem> PlayBGM(const wstring& key, const float volume = 1.0f);
		void Stop(const shared_ptr<SoundItem>& soundItem);
		void StopAll();
		void StopBGM();
		void PauseBGM(bool flag);

		float GetSEVolume() {
			return m_SEVolume;
		}
		float GetBGMVolume() {
			return m_BGMVolume;
		}
		void SEVolumeDown(float volume) {
			m_SEVolume -= volume;
			m_SEVolume = max(m_SEVolume, 0.0f);
		}
		void SEVolumeUp(float volume) {
			m_SEVolume += volume;
			m_SEVolume = min(m_SEVolume, 1.0f);
		}
		void BGMVolumeDown(float volume) {
			m_BGMVolume -= volume;
			m_BGMVolume = max(m_BGMVolume, 0.0f);
			SetBGMVolume();
		}
		void BGMVolumeUp(float volume) {
			m_BGMVolume += volume;
			m_BGMVolume = min(m_BGMVolume, 1.0f);
			SetBGMVolume();
		}

		bool IsSoundRunning(const shared_ptr<SoundItem>& soundItem);
	private:
		vector<wstring> m_SoundKeys;
		shared_ptr<XAudio2Manager> m_Audio;
		shared_ptr<SoundItem> m_Bgm;

		vector<shared_ptr<SoundItem>> m_PlayerSE;
		//map<wstring, shared_ptr<SoundItem>> m_PlayingSE;

		float m_SEVolume;
		float m_BGMVolume;

		void SetBGMVolume();
	};

}
//end basecross
