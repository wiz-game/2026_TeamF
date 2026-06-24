///*!
//@file GameStage.h
//@brief �Q�[���X�e�[�W
//*/
//
#pragma once
#include "stdafx.h"
//#include "JoltManager.h"
namespace basecross {
	class Sprite;

	//--------------------------------------------------------------------------------------
	//	�Q�[���X�e�[�W�N���X
	//--------------------------------------------------------------------------------------
	class TitleStage : public Stage
	{
		shared_ptr<Sprite> m_Title;
		shared_ptr<Sprite> m_sprite_Button;

		//BGM�̉���
		float m_BGMVolume = 0.5f;

		void CreateViewLight(); //�r���[�̍쐬
		void RegisterResources();
		std::shared_ptr<SoundItem> m_titleBGM;	//BGM
	
		float m_ButtonScaleTimer;//�A�j���[�V�������Ă��Ȃ�����-1
		float m_ButtonScaleRation;//���݂�scale�{��

	public:
		//�\�z�Ɣj��
		TitleStage() :
			Stage(),
			m_ButtonScaleTimer(-1),
			m_ButtonScaleRation(0.0f)
		{
		}
		virtual ~TitleStage();
		
		virtual void OnCreate()override; //������
		virtual void OnUpdate()override; //�X�V
		void SpriteMove();

	};
}
//end basecross

