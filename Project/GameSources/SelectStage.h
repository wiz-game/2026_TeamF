///*!
//@file GameStage.h
//@brief �Q�[���X�e�[�W
//*/
//
#pragma once
#include "stdafx.h"

namespace basecross {
	class NumberSprite;
	//--------------------------------------------------------------------------------------
	//	�Q�[���X�e�[�W�N���X
	//--------------------------------------------------------------------------------------
	class SelectStage : public Stage
	{
		int m_SelectIndex;
		int m_MaxSelectIndex;

		vector<shared_ptr<Sprite>> m_sprite_Buttons;
		int m_ButtonScaleIndex = 1;
		float m_ButtonScaleTimer;
		float m_ButtonScaleRation;


		shared_ptr<Stage> m_Title;
		int m_MaxPageNum;
		int m_PageNum = 1;
		Vec2 m_Scale;
		wstring m_PageName = L"SelectPage1";

		//�e�X�g�p
		shared_ptr<NumberSprite> m_SelectingSprite;
		std::shared_ptr<SoundItem> m_sSelectBGM;	//BGM

		//BGM�̉���
		float m_BGMVolume = 0.5f;

		float m_TimeCount = 0.5f;


		void CreateViewLight(); //�r���[�̍쐬
		void RegisterResources();
	public:
		//�\�z�Ɣj��
		SelectStage();
		virtual ~SelectStage();
		
		virtual void OnCreate()override; //������
		virtual void OnUpdate()override; //�X�V

		void SpriteMove();

		//�X�e�[�W�I���X�v���C�g�̍쐬
		void SpriteCreation();

		bool Timer(float deltaTime, float& count, float time, bool loop = false);
	};
}
//end basecross

