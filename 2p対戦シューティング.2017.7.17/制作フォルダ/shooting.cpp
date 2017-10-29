//-----全部1から作ったよ(´・ω・`)----------------------------------------------------------//
//-----DXライブラリで作ったよ(^q^)---------------------------------------------------------//
//-----作業開始日-------------------------------------------------------------------------//
//-----2017年/7月/10日(月)----------------------------------------------------------------//
//---------------------------------------------------------------------------------------//
//-----作業終了日-------------------------------------------------------------------------//
//-----2017年/7月/15日(土)----------------------------------------------------------------//
//---------------------------------------------------------------------------------------//
//-----更新履歴---------------------------------------------------------------------------//
//-----2017年/7月/17日(月)----------------------------------------------------------------//
//-----エンディング曲を追加----------------------------------------------------------------//
//-----BGM周りの処理を変更-----------------------------------------------------------------//
//----このBGMの管理方法だと配列を使わずに済む------------------------------------------------//
//----releaseで実行するとショットの効果音が消えるバグあり-------------------------------------//
//----------------------------------------------------------------------------------------//

#define _USE_MATH_DEFINES
#include "DxLib.h"
#include <math.h>
#define SCREEN_WIDIH 1000
#define SCREEN_HEIGHT 900
#define SHOTS_MAX 15
#define E_SHOTS_MAX 15
#define MIRRORBALL_MAX 4

//キーの実装
int key[256];
int gpUpdatekey()
{
	char tmpkey[256];               //現在のキーの入力状態を格納
	GetHitKeyStateAll(tmpkey);      //すべてのキーの入力状態を得る
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
//シーンの切り替え用
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
	int		r;			//円の半径
	int     pic[4];		//分割画像の格納
	int		Bomb_pic[8];
	int		score;
	int		Handle;		//画像データのハンドル
	int		animCnt;
	int		direction;	//向き、1でプラス方向
	double	degree;		//弾の反射角度を格納
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
//使用する色の初期化
int RED     = GetColor(255, 0, 0);
int White   = GetColor(255, 255, 255);
int Blue    = GetColor(0, 100, 255);
int Green   = GetColor(0, 255, 0);
int SkyBlue = GetColor(0, 255, 255);
//色のデータ虹色用
int 	  r = 50,					
		  g = 0,
		  b = 50;
//ゲームに関するフラグ
bool BGM_flag = false;
bool SE_flag = false;
//BGMやSEのハンドル
int BGM;
int SE[3];
//音関連-----------------------------------------------------------------------------
//BGM初期化,シーンによってBGMを切り替える
void BGM_Change(int &BGM)
{
	//シーンが始まったら一度だけBGMをロード&再生し、切り替えられるようにする
	if (BGM_flag == false)
	{
		if (scene == Game)
		{
			BGM = LoadSoundMem("BGM/曲1.ogg");
			PlaySoundMem(BGM, DX_PLAYTYPE_LOOP);
			BGM_flag = true;		//すぐにtrueにすることで1度だけ処理を行う
		}
		if (scene == End)
		{
			BGM = LoadSoundMem("BGM/メロディ.ogg");
			PlaySoundMem(BGM, DX_PLAYTYPE_LOOP);
			BGM_flag = true;		//すぐにtrueにすることで1度だけ処理を行う
		}
	}
}
//SEのロード
void SE_Ini()
{
	SE[0] = LoadSoundMem("SE/爆発.ogg");
	SE[1] = LoadSoundMem("SE/kan.ogg");
	SE[2] = LoadSoundMem("SE/ダメージ.ogg");
	SE[3] = LoadSoundMem("SE/ショット.ogg");
}
//SEの再生,引数に再生したいSEを指定
void Play_SE(int &SE)
{
	if (SE_flag == false)
	{
		SE_flag = true;
		PlaySoundMem(SE, DX_PLAYTYPE_BACK);
	}
}
//爆発
void SE_Explosion()
{
	Play_SE(SE[0]);
}
//反射
void SE_Reflect()
{
	if (me.score < 10 && Enemy.score < 10)
	{
		Play_SE(SE[1]);
		SE_flag = false;	//SEの再生フラグを再セット
	}
}
//キャラクターに弾が当たったら
void SE_Hit()
{
	for (int s = 0; s < SHOTS_MAX; ++s)
	{
		//自機の弾と敵
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
		//敵の弾と自機
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
//ショット
void SE_Shot()
{
	//条件を限定的なものにしないと別のSE(ｱﾄﾞﾚス)に変わってしまうので注意
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
//画像の初期化
void Graph_Ini()
{
	//タイトル
	TitleGraph.Handle = LoadGraph("画像/タイトル.bmp");
	//エンド
	EndGraph.Handle = LoadGraph("画像/END.bmp");
	//背景
	back.Handle = LoadGraph("画像/背景_修正.bmp");
	back2.Handle = LoadGraph("画像/背景_修正.bmp");
	back3.Handle = LoadGraph("画像/背景2_修正.bmp");
	back4.Handle = LoadGraph("画像/背景2_修正.bmp");
	//キャラ
	LoadDivGraph("画像/Me.png",4, 4, 1, 32, 32, me.pic);
	LoadDivGraph("画像/Enemy.png",4, 4, 1, 32, 32, Enemy.pic);
	//爆発
	LoadDivGraph("画像/bomb.png",8, 8, 1, 32, 32, Bomb.Bomb_pic);
	//背景
	back.x = 0;
	back.y = 0;
	back2.x = 0;
	back2.y = SCREEN_HEIGHT;
	back3.x = 0;
	back3.y = 0;
	back4.x = 0;
	back4.y = SCREEN_HEIGHT;
}
//爆発初期化
void Bomb_Ini()
{
	Bomb.animCnt = 0;
}
//自機初期化
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
//自機の弾の初期化
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
//敵初期化
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
//敵の弾の初期化
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
//弾が当たると特定の角度に軌道を変えるミラーボールの初期化
void MirrorBall_Ini()
{
	//ソースが短くなるやり方by植山T
	//Obj data[4] =
	//{
	//	{ (SCREEN_WIDIH / 2  , _ , _, _ ,}



	//};
	//for (int b = 0; b < MIRRORBALL_MAX; ++b)
	//	mirror_ball[b] = data[b];

	//ソースが読みやすいやり方↓
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
//背景の動き
void Back_Move()
{
	//背景スクロール---------------------------------------
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
//爆発更新
void Bomb_Update()
{
	++Bomb.animCnt;
}
//自機更新
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
				//弾の再設定
				shot[s].flag = true;
				shot[s].x = me.x + (me.w / 2);
				shot[s].y = me.y;
				break;
			}
		}
	}
	++me.animCnt;
}
//弾の更新
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
//敵の動き
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
				//弾の再設定
				Eshot[e_s].flag = true;
				Eshot[e_s].x = Enemy.x + (Enemy.w / 2);
				Eshot[e_s].y = Enemy.y;
				break;
			}
		}
	}
	++Enemy.animCnt;
}
//ミラーボールの動き
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
//敵の弾の更新
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
//弾のあたり判定
void HitCheck()
{
	for (int s = 0; s < SHOTS_MAX; ++s)
	{
		//自機の弾と敵
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
		//敵の弾と自機
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
	//ミラーボールと自機の弾の判定
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
			//自機の弾と壁の反射
		if (shot[s].x >= SCREEN_WIDIH)
		{
			//右側
			SE_Reflect();
			shot[s].direction = -1;
			shot[s].degree = 35;
		}
		if (shot[s].x <= 0)
		{
			//左側
			SE_Reflect();
			shot[s].direction = 1;
			shot[s].degree = 35;
		}
		//座標の再設定
		else if(shot[s].flag == false)
		{
			shot[s].degree = 90;
		}
		//弾の軌道の更新
		shot[s].x += shot[s].speed * cos(shot[s].radian) * shot[s].direction;
		shot[s].radian = M_PI / 180.0f * shot[s].degree;
	}
	//ミラーボールと敵の弾の判定
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
		//敵の弾と壁の反射
		if (Eshot[s].x >= SCREEN_WIDIH)
		{
			//右側
			SE_Reflect();
			Eshot[s].direction = -1;
			Eshot[s].degree = -35;
		}
		if (Eshot[s].x <= 0)
		{
			//左側
			SE_Reflect();
			Eshot[s].direction = 1;
			Eshot[s].degree = -35;
		}
		//座標の再設定
		else if (Eshot[s].flag == false)
		{
			Eshot[s].degree = -90;
		}
		//弾の軌道の更新
		Eshot[s].x += shot[s].speed * cos(Eshot[s].radian) * Eshot[s].direction;
		Eshot[s].radian = M_PI / 180.0f * Eshot[s].degree;
	}
}
//勝敗判定とその時の描画
void Win_Check()
{
	if (me.score >= 10)
	{
		DrawFormatString((SCREEN_WIDIH / 2) - 80, (SCREEN_HEIGHT / 2) - 50, RED, "\"1P\"WIN!");
		DrawFormatString((SCREEN_WIDIH / 2) - 80, (SCREEN_HEIGHT / 2) + 32, RED, "ESCで終了");
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
		DrawFormatString((SCREEN_WIDIH / 2) - 80, (SCREEN_HEIGHT / 2) + 32, Blue, "ESCで終了");
		for (int s = 0; s < SHOTS_MAX; ++s)
		{
			me.flag = false;
			shot[s].flag = false;
			SE_Explosion();
			DrawGraph(me.x, me.y - me.h, Bomb.Bomb_pic[(Bomb.animCnt / 4) % 9], TRUE);
		}
	}
}
//描画
void Obj_Draw()
{
	//背景
	DrawGraph(back.x, back.y, back.Handle, true);
	DrawGraph(back2.x, back2.y, back2.Handle, true);
	DrawGraph(back3.x, back3.y, back3.Handle, true);
	DrawGraph(back4.x, back4.y, back4.Handle, true);
	//文字
	DrawFormatString(0, 0, RED, "\"1P\"SCORE %d", me.score);
	DrawFormatString(SCREEN_WIDIH - 220, 0, Blue, "\"2P\"SCORE %d", Enemy.score);
	SetFontSize(32);								//サイズを32に変更
	SetFontThickness(8);							//太さを8に変更
	ChangeFont("ＭＳ メイリオ");                     //種類をメイリオに変更
	ChangeFontType(DX_FONTTYPE_ANTIALIASING);		//アンチエイリアスフォントに変更
	//弾
	for (int s = 0; s < SHOTS_MAX; ++s)
	{
		//自機
		if (shot[s].flag == true)
		{
			DrawCircle(shot[s].x, shot[s].y, shot[s].r, shot[s].color, true);
		}
	}
	for (int s = 0; s < E_SHOTS_MAX; ++s)
	{
		//敵
		if (Eshot[s].flag == true)
		{
			DrawCircle(Eshot[s].x, Eshot[s].y, Eshot[s].r, Eshot[s].color, true);
		}
	}
	//ミラーボール
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
	//自機
	if (me.flag == true)
	{
	/*	DrawBox(me.x, me.y - me.h, me.x + me.w, me.y, me.color, false);*/
		DrawGraph(me.x, me.y - me.h, me.pic[(me.animCnt / 6) % 4], TRUE);
	}
	//敵
	if (Enemy.flag == true)
	{
	/*	DrawBox(Enemy.x, Enemy.y - Enemy.h, Enemy.x + Enemy.w, Enemy.y, Enemy.color, false);*/
		DrawGraph(Enemy.x, Enemy.y - Enemy.h, Enemy.pic[(Enemy.animCnt / 6) % 4], TRUE);
	}
}
//エンディングのキャラの動き
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
//ゲーム終了時に不要なデータを消去する
void Game_Fin()
{
	InitGraph();							//すべての画像データを消去
	for (int i = 0; i < 3; ++i)
	{
		DeleteSoundMem(SE[i]);				//不要なSEを消去
	}
	DeleteSoundMem(BGM, DX_PLAYTYPE_LOOP);	//不要なBGMを消去
}
//シーンの切り替え
void ChangeScene()
{
	switch (scene)
	{
	//タイトルシーン
	case Title:
		DrawGraph(0, 0, TitleGraph.Handle, true);
		if (key[KEY_INPUT_B] == 1)
		{
			scene = Game;
		}
		break;
	//ゲーム本編
	case Game:
		BGM_Change(BGM);
		DeleteGraph(TitleGraph.Handle);				//不要なタイトル画像を消去
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
			DeleteSoundMem(BGM, DX_PLAYTYPE_LOOP);	//不要なBGMを消去
			if (key[KEY_INPUT_ESCAPE] == 1)
			{
				scene = End;
				BGM_flag = false;		//次のシーンでBGMを変えるので,フラグを再セット
			}
		}
		break;
	//エンディングシーン
	case End:
		DeleteGraph(back.Handle);
		DeleteGraph(back2.Handle);
		DeleteGraph(back3.Handle);
		DeleteGraph(back4.Handle);
		BGM_Change(BGM);
		End_Move();
		DrawGraph(0, 0, EndGraph.Handle, true);
		DrawFormatString(SCREEN_WIDIH - 600, SCREEN_HEIGHT / 2, RED, "Rでリトライ", me.score);
		DrawFormatString(SCREEN_WIDIH - 650, (SCREEN_HEIGHT / 2) + 32, Blue, "Fでシャットダウン", me.score);
		DrawGraph(me.x, me.y - me.h, me.pic[(me.animCnt / 6) % 4], TRUE);
		DrawGraph(Enemy.x, Enemy.y - Enemy.h, Enemy.pic[(Enemy.animCnt / 6) % 4], TRUE);
		if (key[KEY_INPUT_R] == 1)
		{
			//タイトルシーンに移行する前に初期化処理を1度だけ行う
			DeleteSoundMem(BGM, DX_PLAYTYPE_LOOP);	//不要なBGMを消去
			BGM_flag = false;	//BGMの再セット
			SE_flag = false;	//SEの再セット
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
//エントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ウィンドウモード変更と初期化と裏画面設定
	ChangeWindowMode(TRUE), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK);   
	//ウインドウサイズ(1000*900)
	SetGraphMode(SCREEN_WIDIH, SCREEN_HEIGHT, 32);
	//初期化処理
	SE_Ini();
	Graph_Ini();
	Bomb_Ini();
	MirrorBall_Ini();
	Me_Ini();
	Shot_Ini();
	Enemy_Ini();
	EnemyShot_Ini();
	// while(裏画面を表画面に反映、メッセージ処理、画面クリア、キーの更新)
	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && gpUpdatekey() == 0)
	{
		//ここでゲームの処理が行われる-----------------------------------------------------
		ChangeScene();
		//エンディング時にFキーでゲーム終了
		if (scene == End && key[KEY_INPUT_F] == 1)		
		{
			Game_Fin();
			break;
		}
		//------------------------------------------------------------------------------
	}
	DxLib_End();                            // DXライブラリ終了処理
	return 0;
}
