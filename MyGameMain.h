#pragma once
#include "MyPG.h"

extern  void  MyGameMain_Finalize( );
extern  void  MyGameMain_Initialize( );
extern  void  MyGameMain_UpDate( );
extern  void  MyGameMain_Render2D( );

enum class TaskFlag
{
	Non,		//�^�X�N�����I�����
	Title,		//�^�C�g��
	Game,		//�Q�[���{��
	Ending,		//�G���f�B���O

	//�ȉ��K�v�ɉ����Ēǉ�
};
