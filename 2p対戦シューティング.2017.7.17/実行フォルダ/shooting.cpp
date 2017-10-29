//-----�S��1����������(�L�E�ցE`)----------------------------------------------------------//
//-----DX���C�u�����ō������(^q^)---------------------------------------------------------//
//-----��ƊJ�n��-------------------------------------------------------------------------//
//-----2017�N/7��/10��(��)----------------------------------------------------------------//
//---------------------------------------------------------------------------------------//
//-----��ƏI����-------------------------------------------------------------------------//
//-----2017�N/7��/15��(�y)----------------------------------------------------------------//
//---------------------------------------------------------------------------------------//
//-----�X�V����---------------------------------------------------------------------------//
//-----2017�N/7��/17��(��)----------------------------------------------------------------//
//-----�G���f�B���O�Ȃ�ǉ�----------------------------------------------------------------//
//-----BGM����̏�����ύX-----------------------------------------------------------------//
//----����BGM�̊Ǘ����@���Ɣz����g�킸�ɍς�------------------------------------------------//
//----release�Ŏ��s����ƃV���b�g�̌��ʉ���������o�O����-------------------------------------//
//----------------------------------------------------------------------------------------//

#define _USE_MATH_DEFINES
#include "DxLib.h"
#include <math.h>
#define SCREEN_WIDIH 1000
#define SCREEN_HEIGHT 900
#define SHOTS_MAX 15
#define E_SHOTS_MAX 15
#define MIRRORBALL_MAX 4

//�L�[�̎���
int key[256];
int gpUpdatekey()
{
	char tmpkey[256];               //���݂̃L�[�̓��͏�Ԃ��i�[
	GetHitKeyStateAll(tmpkey);      //���ׂẴL�[�̓��͏�Ԃ𓾂�
	for (int i = 0; i < 256; ++i)
	{
		if (tmpkey[i] != 0)
		{
			++key[i];
		}
		else
		{
			key[i] = 0;
		}
	}
	return 0;
}
//�V�[���̐؂�ւ��p
enum Scene {
	Title,
	Game,
	End,
};
Scene scene;
struct Obj
{
	int     x;
	int     y;
	int     w;
	int     h;
	int		r;			//�~�̔��a
	int     pic[4];		//�����摜�̊i�[
	int		Bomb_pic[8];
	int		score;
	int		Handle;		//�摜�f�[�^�̃n���h��
	int		animCnt;
	int		direction;	//�����A1�Ńv���X����
	double	degree;		//�e�̔��ˊp�x���i�[
	double	radian = M_PI / 180.0f * degree;
	bool	flag;
	int		speed;
	int		color;
};
Obj TitleGraph;
Obj EndGraph;
Obj back;
Obj back2;
Obj back3;
Obj back4;
Obj Bomb;
Obj me;
Obj shot[SHOTS_MAX];
Obj Enemy;
Obj Eshot[E_SHOTS_MAX];
Obj mirror_ball[MIRRORBALL_MAX];
//�g�p����F�̏�����
int RED     = GetColor(255, 0, 0);
int White   = GetColor(255, 255, 255);
int Blue    = GetColor(0, 100, 255);
int Green   = GetColor(0, 255, 0);
int SkyBlue = GetColor(0, 255, 255);
//�F�̃f�[�^���F�p
int 	  r = 50,					
		  g = 0,
		  b = 50;
//�Q�[���Ɋւ���t���O
bool BGM_flag = false;
bool SE_flag = false;
//BGM��SE�̃n���h��
int BGM;
int SE[3];
//���֘A-----------------------------------------------------------------------------
//BGM������,�V�[���ɂ����BGM��؂�ւ���
void BGM_Change(int &BGM)
{
	//�V�[�����n�܂������x����BGM�����[�h&�Đ����A�؂�ւ�����悤�ɂ���
	if (BGM_flag == false)
	{
		if (scene == Game)
		{
			BGM = LoadSoundMem("BGM/��1.ogg");
			PlaySoundMem(BGM, DX_PLAYTYPE_LOOP);
			BGM_flag = true;		//������true�ɂ��邱�Ƃ�1�x�����������s��
		}
		if (scene == End)
		{
			BGM = LoadSoundMem("BGM/�����f�B.ogg");
			PlaySoundMem(BGM, DX_PLAYTYPE_LOOP);
			BGM_flag = true;		//������true�ɂ��邱�Ƃ�1�x�����������s��
		}
	}
}
//SE�̃��[�h
void SE_Ini()
{
	SE[0] = LoadSoundMem("SE/����.ogg");
	SE[1] = LoadSoundMem("SE/kan.ogg");
	SE[2] = LoadSoundMem("SE/�_���[�W.ogg");
	SE[3] = LoadSoundMem("SE/�V���b�g.ogg");
}
//SE�̍Đ�,�����ɍĐ�������SE���w��
void Play_SE(int &SE)
{
	if (SE_flag == false)
	{
		SE_flag = true;
		PlaySoundMem(SE, DX_PLAYTYPE_BACK);
	}
}
//����
void SE_Explosion()
{
	Play_SE(SE[0]);
}
//����
void SE_Reflect()
{
	if (me.score < 10 && Enemy.score < 10)
	{
		Play_SE(SE[1]);
		SE_flag = false;	//SE�̍Đ��t���O���ăZ�b�g
	}
}
//�L�����N�^�[�ɒe������������
void SE_Hit()
{
	for (int s = 0; s < SHOTS_MAX; ++s)
	{
		//���@�̒e�ƓG
		if (Enemy.flag == true && shot[s].flag == true)
		{
			if (shot[s].y <= Enemy.y && shot[s].y <= Enemy.y + Enemy.h
				&& shot[s].x <= Enemy.x + Enemy.w && shot[s].x >= Enemy.x)
			{
				Play_SE(SE[2]);
				SE_flag = false;
			}
		}
	}
	for (int s = 0; s < E_SHOTS_MAX; ++s)
		//�G�̒e�Ǝ��@
		if (me.flag == true && Eshot[s].flag == true)
		{
			if (Eshot[s].y <= me.y && Eshot[s].y >= me.y - me.h
				&& Eshot[s].x <= me.x + me.w && Eshot[s].x >= me.x)
			{
				Play_SE(SE[2]);
				SE_flag = false;
			}
		}
}
//�V���b�g
void SE_Shot()
{
	//����������I�Ȃ��̂ɂ��Ȃ��ƕʂ�SE(���ڃX)�ɕς���Ă��܂��̂Œ���
		if (me.score < 10 && Enemy.score < 10 && key[KEY_INPUT_DOWN] == 1 && Enemy.flag == true)
		{
			Play_SE(SE[3]);
			SE_flag = false;
		}
		if (me.score < 10 && Enemy.score < 10 && key[KEY_INPUT_X] == 1 && me.flag == true)
		{
			Play_SE(SE[3]);
			SE_flag = false;
		}
}
//--------------------------------------------------------------------------------
//�摜�̏�����
void Graph_Ini()
{
	//�^�C�g��
	TitleGraph.Handle = LoadGraph("�摜/�^�C�g��.bmp");
	//�G���h
	EndGraph.Handle = LoadGraph("�摜/END.bmp");
	//�w�i
	back.Handle = LoadGraph("�摜/�w�i_�C��.bmp");
	back2.Handle = LoadGraph("�摜/�w�i_�C��.bmp");
	back3.Handle = LoadGraph("�摜/�w�i2_�C��.bmp");
	back4.Handle = LoadGraph("�摜/�w�i2_�C��.bmp");
	//�L����
	LoadDivGraph("�摜/Me.png",4, 4, 1, 32, 32, me.pic);
	LoadDivGraph("�摜/Enemy.png",4, 4, 1, 32, 32, Enemy.pic);
	//����
	LoadDivGraph("�摜/bomb.png",8, 8, 1, 32, 32, Bomb.Bomb_pic);
	//�w�i
	back.x = 0;
	back.y = 0;
	back2.x = 0;
	back2.y = SCREEN_HEIGHT;
	back3.x = 0;
	back3.y = 0;
	back4.x = 0;
	back4.y = SCREEN_HEIGHT;
}
//����������
void Bomb_Ini()
{
	Bomb.animCnt = 0;
}
//���@������
void Me_Ini()
{
	me.x = SCREEN_WIDIH / 2;
	me.y = SCREEN_HEIGHT;
	me.w = 32;
	me.h = 32;
	me.animCnt = 0;
	me.direction = 1;
	me.flag = true;
	me.score = 0;
	me.color = RED;
	me.speed = 5;
}
//���@�̒e�̏�����
void Shot_Ini()
{
	for (int s = 0; s < SHOTS_MAX; ++s)
	{
		shot[s].x = me.x + (me.w / 2);
		shot[s].y = me.y;
		shot[s].r = 2;
		shot[s].speed = 8;
		shot[s].color = RED;
		shot[s].flag = false;
	}
}
//�G������
void Enemy_Ini()
{
	Enemy.x = SCREEN_WIDIH / 2;
	Enemy.y = SCREEN_HEIGHT / 10;
	Enemy.w = 32;
	Enemy.h = 32;
	Enemy.animCnt = 0;
	Enemy.direction = -1;
	Enemy.flag = true;
	Enemy.score = 0;
	Enemy.color = Blue;
	Enemy.speed = 5;
}
//�G�̒e�̏�����
void EnemyShot_Ini()
{
	for (int e_s = 0; e_s < E_SHOTS_MAX; ++e_s)
	{
		Eshot[e_s].x = Enemy.x + (Enemy.w / 2);
		Eshot[e_s].y = Enemy.y;
		Eshot[e_s].r = 2;
		Eshot[e_s].speed = 8;
		Eshot[e_s].color = Blue;
		Eshot[e_s].flag = false;
	}
}
//�e��������Ɠ���̊p�x�ɋO����ς���~���[�{�[���̏�����
void MirrorBall_Ini()
{
	//�\�[�X���Z���Ȃ����by�A�RT
	//Obj data[4] =
	//{
	//	{ (SCREEN_WIDIH / 2  , _ , _, _ ,}



	//};
	//for (int b = 0; b < MIRRORBALL_MAX; ++b)
	//	mirror_ball[b] = data[b];

	//�\�[�X���ǂ݂₷��������
	for (int b = 0; b < MIRRORBALL_MAX; ++b)
	{
		switch (b)
		{
		case 0:
			mirror_ball[b].x = (SCREEN_WIDIH / 2);
			mirror_ball[b].y = (200);
			mirror_ball[b].r = 25;
			mirror_ball[b].speed = 4;
			mirror_ball[b].direction = -1;
			mirror_ball[b].flag = true;
			break;
		case 1:
			mirror_ball[b].x = (SCREEN_WIDIH / 2);
			mirror_ball[b].y = ((SCREEN_HEIGHT / 2) - 70);
			mirror_ball[b].r = 25;
			mirror_ball[b].speed = 25;
			mirror_ball[b].direction = 1;
			mirror_ball[b].flag = true;
			break;
		case 2:
			mirror_ball[b].x = (SCREEN_WIDIH / 2);
			mirror_ball[b].y = ((SCREEN_HEIGHT / 2) + 70);
			mirror_ball[b].r = 25;
			mirror_ball[b].speed = 25;
			mirror_ball[b].direction = -1;
			mirror_ball[b].flag = true;
			break;
		case 3:
			mirror_ball[b].x = (SCREEN_WIDIH / 2);
			mirror_ball[b].y = (SCREEN_HEIGHT - 200);
			mirror_ball[b].r = 25;
			mirror_ball[b].speed = 4;
			mirror_ball[b].direction = 1;
			mirror_ball[b].flag = true;
			break;
		}	
	}
}
//�w�i�̓���
void Back_Move()
{
	//�w�i�X�N���[��---------------------------------------
	back.y  -= 15;
	back2.y -= 15;
	back3.y -= 20;
	back4.y -= 20;
	if (back.y <= -SCREEN_HEIGHT)
	{
		back.y = 0;
	}
	if (back2.y <= 0)
	{
		back2.y = SCREEN_HEIGHT;
	}
	if (back3.y <= -SCREEN_HEIGHT)
	{
		back3.y = 0;
	}
	if (back4.y <= 0)
	{
		back4.y = SCREEN_HEIGHT;
	}
	//-----------------------------------------------------
}
//�����X�V
void Bomb_Update()
{
	++Bomb.animCnt;
}
//���@�X�V
void Me_Update()
{
	if (me.flag == true && scene == Game)
	{
		if (key[KEY_INPUT_C] > 0 && me.x + me.w <= SCREEN_WIDIH)
		{
			me.x += me.speed;
		}
		if (key[KEY_INPUT_Z] > 0 && me.x >= 0)
		{
			me.x -= me.speed;
		}
		for (int s = 0; s < SHOTS_MAX; ++s)
		{
			if (key[KEY_INPUT_X] == 1 && shot[s].flag == false)
			{
				SE_Shot();
				//�e�̍Đݒ�
				shot[s].flag = true;
				shot[s].x = me.x + (me.w / 2);
				shot[s].y = me.y;
				break;
			}
		}
	}
	++me.animCnt;
}
//�e�̍X�V
void Shot_Update()
{
	for (int s = 0; s < SHOTS_MAX; ++s)
	{
		if (shot[s].flag == true)
		{
			shot[s].y -= shot[s].speed;
		}
		if (shot[s].y <= 0 && shot[s].flag == true)
		{
			shot[s].flag = false;
		}
	}
}
//�G�̓���
void Enemy_Update()
{
	if (Enemy.flag == true && scene == Game)
	{
		if (key[KEY_INPUT_RIGHT] > 0 && Enemy.x + Enemy.w <= SCREEN_WIDIH)
		{
			Enemy.x += Enemy.speed;
		}
		if (key[KEY_INPUT_LEFT] > 0 && Enemy.x >= 0)
		{
			Enemy.x -= Enemy.speed;
		}
		for (int e_s = 0; e_s < E_SHOTS_MAX; ++e_s)
		{
			if (key[KEY_INPUT_DOWN] == 1 && Eshot[e_s].flag == false)
			{
				SE_Shot();
				//�e�̍Đݒ�
				Eshot[e_s].flag = true;
				Eshot[e_s].x = Enemy.x + (Enemy.w / 2);
				Eshot[e_s].y = Enemy.y;
				break;
			}
		}
	}
	++Enemy.animCnt;
}
//�~���[�{�[���̓���
void MirrorBall_Update()
{
	for (int b = 0; b < MIRRORBALL_MAX; ++b)
	{
		if (mirror_ball[b].flag == true)
		{
			if (mirror_ball[b].x + mirror_ball[b].r >= SCREEN_WIDIH)
			{
				mirror_ball[b].direction = -1;
			}
			if (mirror_ball[b].x - mirror_ball[b].r <= 0)
			{
				mirror_ball[b].direction = 1;
			}
		}
		mirror_ball[b].x += mirror_ball[b].speed * mirror_ball[b].direction;
	}
}
//�G�̒e�̍X�V
void Eshot_Update()
{
	for (int e_s = 0; e_s < E_SHOTS_MAX; ++e_s)
	{
		if (Eshot[e_s].flag == true)
		{
			Eshot[e_s].y += Eshot[e_s].speed;
		}
		if (Eshot[e_s].y >= SCREEN_HEIGHT && Eshot[e_s].flag == true)
		{
			Eshot[e_s].flag = false;
		}
	}
}
//�e�̂����蔻��
void HitCheck()
{
	for (int s = 0; s < SHOTS_MAX; ++s)
	{
		//���@�̒e�ƓG
		if (Enemy.flag == true && shot[s].flag == true)
		{
			if (shot[s].y <= Enemy.y && shot[s].y <= Enemy.y + Enemy.h
				&& shot[s].x <= Enemy.x + Enemy.w && shot[s].x >= Enemy.x)
			{
				SE_Hit();
				shot[s].flag = false;
				me.score += 1;
			}
		}
	}
	for(int s = 0; s < E_SHOTS_MAX; ++s)
		//�G�̒e�Ǝ��@
		if (me.flag == true && Eshot[s].flag == true)
		{
			if (Eshot[s].y <= me.y && Eshot[s].y >= me.y - me.h 
			 && Eshot[s].x <= me.x + me.w && Eshot[s].x >= me.x)
			{
				SE_Hit();
				Eshot[s].flag = false;
				Enemy.score += 1;
			}	
		}
	//�~���[�{�[���Ǝ��@�̒e�̔���
	for (int s = 0; s < SHOTS_MAX; ++s)
	{
		for (int b = 0; b < MIRRORBALL_MAX; ++b)
		{
			if (shot[s].y <= mirror_ball[b].y + mirror_ball[b].r && shot[s].y >= mirror_ball[b].y - mirror_ball[b].r
				&& shot[s].x >= mirror_ball[b].x  && shot[s].x <= mirror_ball[b].x + mirror_ball[b].r)
			{
				SE_Reflect();
				shot[s].direction = 1;
				shot[s].degree = 55;
			}
			else if (shot[s].y <= mirror_ball[b].y + mirror_ball[b].r && shot[s].y >= mirror_ball[b].y - mirror_ball[b].r
				     && shot[s].x >= mirror_ball[b].x - mirror_ball[b].r && shot[s].x <= mirror_ball[b].x)
			{
				SE_Reflect();
				shot[s].direction = -1;
				shot[s].degree = 55;
			}
		}
			//���@�̒e�ƕǂ̔���
		if (shot[s].x >= SCREEN_WIDIH)
		{
			//�E��
			SE_Reflect();
			shot[s].direction = -1;
			shot[s].degree = 35;
		}
		if (shot[s].x <= 0)
		{
			//����
			SE_Reflect();
			shot[s].direction = 1;
			shot[s].degree = 35;
		}
		//���W�̍Đݒ�
		else if(shot[s].flag == false)
		{
			shot[s].degree = 90;
		}
		//�e�̋O���̍X�V
		shot[s].x += shot[s].speed * cos(shot[s].radian) * shot[s].direction;
		shot[s].radian = M_PI / 180.0f * shot[s].degree;
	}
	//�~���[�{�[���ƓG�̒e�̔���
	for (int s = 0; s < E_SHOTS_MAX; ++s)
	{
		for (int b = 0; b < MIRRORBALL_MAX; ++b)
		{
			if (Eshot[s].y <= mirror_ball[b].y + mirror_ball[b].r && Eshot[s].y >= mirror_ball[b].y - mirror_ball[b].r
			    && Eshot[s].x >= mirror_ball[b].x  && Eshot[s].x <= mirror_ball[b].x + mirror_ball[b].r)
			{
				SE_Reflect();
				Eshot[s].direction = 1;
				Eshot[s].degree = -55;
			}
			else if (Eshot[s].y <= mirror_ball[b].y + mirror_ball[b].r && Eshot[s].y >= mirror_ball[b].y - mirror_ball[b].r
				     && Eshot[s].x >= mirror_ball[b].x - mirror_ball[b].r && Eshot[s].x <= mirror_ball[b].x)
			{
				SE_Reflect();
				Eshot[s].direction = -1;
				Eshot[s].degree = -55;
			}
		}
		//�G�̒e�ƕǂ̔���
		if (Eshot[s].x >= SCREEN_WIDIH)
		{
			//�E��
			SE_Reflect();
			Eshot[s].direction = -1;
			Eshot[s].degree = -35;
		}
		if (Eshot[s].x <= 0)
		{
			//����
			SE_Reflect();
			Eshot[s].direction = 1;
			Eshot[s].degree = -35;
		}
		//���W�̍Đݒ�
		else if (Eshot[s].flag == false)
		{
			Eshot[s].degree = -90;
		}
		//�e�̋O���̍X�V
		Eshot[s].x += shot[s].speed * cos(Eshot[s].radian) * Eshot[s].direction;
		Eshot[s].radian = M_PI / 180.0f * Eshot[s].degree;
	}
}
//���s����Ƃ��̎��̕`��
void Win_Check()
{
	if (me.score >= 10)
	{
		DrawFormatString((SCREEN_WIDIH / 2) - 80, (SCREEN_HEIGHT / 2) - 50, RED, "\"1P\"WIN!");
		DrawFormatString((SCREEN_WIDIH / 2) - 80, (SCREEN_HEIGHT / 2) + 32, RED, "ESC�ŏI��");
		for (int s = 0; s < E_SHOTS_MAX; ++s)
		{
			Enemy.flag = false;
			Eshot[s].flag = false;
			SE_Explosion();
			DrawGraph(Enemy.x, Enemy.y - Enemy.h, Bomb.Bomb_pic[(Bomb.animCnt / 4) % 9], TRUE);
		}
	}
	else if(Enemy.score >= 10)
	{
		DrawFormatString((SCREEN_WIDIH / 2) - 80, (SCREEN_HEIGHT / 2) - 50, Blue, "\"2P\"WIN!");
		DrawFormatString((SCREEN_WIDIH / 2) - 80, (SCREEN_HEIGHT / 2) + 32, Blue, "ESC�ŏI��");
		for (int s = 0; s < SHOTS_MAX; ++s)
		{
			me.flag = false;
			shot[s].flag = false;
			SE_Explosion();
			DrawGraph(me.x, me.y - me.h, Bomb.Bomb_pic[(Bomb.animCnt / 4) % 9], TRUE);
		}
	}
}
//�`��
void Obj_Draw()
{
	//�w�i
	DrawGraph(back.x, back.y, back.Handle, true);
	DrawGraph(back2.x, back2.y, back2.Handle, true);
	DrawGraph(back3.x, back3.y, back3.Handle, true);
	DrawGraph(back4.x, back4.y, back4.Handle, true);
	//����
	DrawFormatString(0, 0, RED, "\"1P\"SCORE %d", me.score);
	DrawFormatString(SCREEN_WIDIH - 220, 0, Blue, "\"2P\"SCORE %d", Enemy.score);
	SetFontSize(32);								//�T�C�Y��32�ɕύX
	SetFontThickness(8);							//������8�ɕύX
	ChangeFont("�l�r ���C���I");                     //��ނ����C���I�ɕύX
	ChangeFontType(DX_FONTTYPE_ANTIALIASING);		//�A���`�G�C���A�X�t�H���g�ɕύX
	//�e
	for (int s = 0; s < SHOTS_MAX; ++s)
	{
		//���@
		if (shot[s].flag == true)
		{
			DrawCircle(shot[s].x, shot[s].y, shot[s].r, shot[s].color, true);
		}
	}
	for (int s = 0; s < E_SHOTS_MAX; ++s)
	{
		//�G
		if (Eshot[s].flag == true)
		{
			DrawCircle(Eshot[s].x, Eshot[s].y, Eshot[s].r, Eshot[s].color, true);
		}
	}
	//�~���[�{�[��
	r += 7;
	g += 2;
	b += 7;
	if (r == 255 || g == 255 || b == 255)
	{
		r -= 1,
		g -= 1,
		b -= 1;
	}
	for (int b = 0; b < MIRRORBALL_MAX; ++b)
	{
		if (mirror_ball[b].flag == true)
		{
			DrawCircle(mirror_ball[b].x, mirror_ball[b].y, mirror_ball[b].r, mirror_ball[b].color = GetColor(r, g, b));
		}
	}
	//���@
	if (me.flag == true)
	{
	/*	DrawBox(me.x, me.y - me.h, me.x + me.w, me.y, me.color, false);*/
		DrawGraph(me.x, me.y - me.h, me.pic[(me.animCnt / 6) % 4], TRUE);
	}
	//�G
	if (Enemy.flag == true)
	{
	/*	DrawBox(Enemy.x, Enemy.y - Enemy.h, Enemy.x + Enemy.w, Enemy.y, Enemy.color, false);*/
		DrawGraph(Enemy.x, Enemy.y - Enemy.h, Enemy.pic[(Enemy.animCnt / 6) % 4], TRUE);
	}
}
//�G���f�B���O�̃L�����̓���
void End_Move()
{
	if (scene == End)
	{
		if (me.x + me.w >= SCREEN_WIDIH)
		{
			me.direction = -1;
		}
		if (me.x <= 0)
		{
			me.direction = 1;
		}
		if (Enemy.x + Enemy.w >= SCREEN_WIDIH)
		{
			Enemy.direction = -1;
		}
		if (Enemy.x <= 0)
		{
			Enemy.direction = 1;
		}
	}
	me.x += me.speed * me.direction;
	Enemy.x += Enemy.speed * Enemy.direction;
	++me.animCnt;
	++Enemy.animCnt;
}
//�Q�[���I�����ɕs�v�ȃf�[�^����������
void Game_Fin()
{
	InitGraph();							//���ׂẲ摜�f�[�^������
	for (int i = 0; i < 3; ++i)
	{
		DeleteSoundMem(SE[i]);				//�s�v��SE������
	}
	DeleteSoundMem(BGM, DX_PLAYTYPE_LOOP);	//�s�v��BGM������
}
//�V�[���̐؂�ւ�
void ChangeScene()
{
	switch (scene)
	{
	//�^�C�g���V�[��
	case Title:
		DrawGraph(0, 0, TitleGraph.Handle, true);
		if (key[KEY_INPUT_B] == 1)
		{
			scene = Game;
		}
		break;
	//�Q�[���{��
	case Game:
		BGM_Change(BGM);
		DeleteGraph(TitleGraph.Handle);				//�s�v�ȃ^�C�g���摜������
		Me_Update();
		MirrorBall_Update();
		Shot_Update();
		Enemy_Update();
		Eshot_Update();
		HitCheck();
		Back_Move();
		Bomb_Update();
		Obj_Draw();
		Win_Check();
		if (me.flag == false || Enemy.flag == false)
		{
			DeleteSoundMem(BGM, DX_PLAYTYPE_LOOP);	//�s�v��BGM������
			if (key[KEY_INPUT_ESCAPE] == 1)
			{
				scene = End;
				BGM_flag = false;		//���̃V�[����BGM��ς���̂�,�t���O���ăZ�b�g
			}
		}
		break;
	//�G���f�B���O�V�[��
	case End:
		DeleteGraph(back.Handle);
		DeleteGraph(back2.Handle);
		DeleteGraph(back3.Handle);
		DeleteGraph(back4.Handle);
		BGM_Change(BGM);
		End_Move();
		DrawGraph(0, 0, EndGraph.Handle, true);
		DrawFormatString(SCREEN_WIDIH - 600, SCREEN_HEIGHT / 2, RED, "R�Ń��g���C", me.score);
		DrawFormatString(SCREEN_WIDIH - 650, (SCREEN_HEIGHT / 2) + 32, Blue, "F�ŃV���b�g�_�E��", me.score);
		DrawGraph(me.x, me.y - me.h, me.pic[(me.animCnt / 6) % 4], TRUE);
		DrawGraph(Enemy.x, Enemy.y - Enemy.h, Enemy.pic[(Enemy.animCnt / 6) % 4], TRUE);
		if (key[KEY_INPUT_R] == 1)
		{
			//�^�C�g���V�[���Ɉڍs����O�ɏ�����������1�x�����s��
			DeleteSoundMem(BGM, DX_PLAYTYPE_LOOP);	//�s�v��BGM������
			BGM_flag = false;	//BGM�̍ăZ�b�g
			SE_flag = false;	//SE�̍ăZ�b�g
			SE_Ini();
			Graph_Ini();
			Bomb_Ini();
			MirrorBall_Ini();
			Me_Ini();
			Shot_Ini();
			Enemy_Ini();
			EnemyShot_Ini();
			scene = Title;
		}
		break;
	}
}
//�G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//�E�B���h�E���[�h�ύX�Ə������Ɨ���ʐݒ�
	ChangeWindowMode(TRUE), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);   
	//�E�C���h�E�T�C�Y(1000*900)
	SetGraphMode(SCREEN_WIDIH, SCREEN_HEIGHT, 32);
	//����������
	SE_Ini();
	Graph_Ini();
	Bomb_Ini();
	MirrorBall_Ini();
	Me_Ini();
	Shot_Ini();
	Enemy_Ini();
	EnemyShot_Ini();
	// while(����ʂ�\��ʂɔ��f�A���b�Z�[�W�����A��ʃN���A�A�L�[�̍X�V)
	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && gpUpdatekey() == 0)
	{
		//�����ŃQ�[���̏������s����-----------------------------------------------------
		ChangeScene();
		//�G���f�B���O����F�L�[�ŃQ�[���I��
		if (scene == End && key[KEY_INPUT_F] == 1)		
		{
			Game_Fin();
			break;
		}
		//------------------------------------------------------------------------------
	}
	DxLib_End();                            // DX���C�u�����I������
	return 0;
}
