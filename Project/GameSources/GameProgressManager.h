/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "Singleton.h"
namespace basecross{
	struct GameProgressContext {
		bool m_IsClear = false;
		bool m_IsUnlocked = false;
	};
	class GameProgressManager : public SingletonBase<GameProgressManager> {
		friend class SingletonBase<GameProgressManager>;
		
		vector<GameProgressContext> m_GameDates;
	public:
		void Initialize(UINT gameStageCount);

		UINT GetStageSize() const{ return m_GameDates.size(); }
		bool IsUnlocked(UINT index)const;
	};
}
//end basecross
