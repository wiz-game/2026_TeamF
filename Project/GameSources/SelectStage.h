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

		shared_ptr<Sprite> m_Title;

		//�e�X�g�p
		shared_ptr<NumberSprite> m_SelectingSprite;
		std::shared_ptr<SoundItem> m_sSelectBGM;	//BGM

		// �e�X�e�[�W�̉摜�X�v���C�g
		std::vector<std::shared_ptr<Sprite>> m_StageSprites; 


		//BGM�̉���
		float m_BGMVolume = 0.5f;

		void CreateViewLight(); //�r���[�̍쐬
		void RegisterResources();
	public:
		//�\�z�Ɣj��
		SelectStage();
		virtual ~SelectStage();
		
		virtual void OnCreate()override; //������
		virtual void OnUpdate()override; //�X�V

		//�X�e�[�W�I���X�v���C�g�̍쐬
		void SpriteCreation();

		//�^�C�g���R���g���[��
		void TitleControl();

	};
}
//end basecross

