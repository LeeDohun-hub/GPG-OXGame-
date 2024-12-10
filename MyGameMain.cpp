#include "MyGameMain.h"

//ゲーム情報
TaskFlag			actTask, nextTask;

//-----------------------------------------------------------------------------
//初期化処理
//機能概要：プログラム起動時に１回実行される（素材などの準備を行う）
//-----------------------------------------------------------------------------
void  MyGameMain_Initialize()
{
	//キーボードの入力を受け取るオブジェクトを生成する
	//アナログスティックL
	XI::AnalogAxisKB	ls = { DIK_LEFT, DIK_RIGHT, DIK_UP, DIK_DOWN };
	XI::AnalogAxisKB	rs = { DIK_NUMPAD4, DIK_NUMPAD6, DIK_NUMPAD8, DIK_NUMPAD2 };
	XI::AnalogTriggerKB  tg = { DIK_E, DIK_R };
	XI::KeyDatas_KB	 key = {
		{ DIK_Z, XI::VGP::B1 },{ DIK_X, XI::VGP::B2 },{ DIK_C, XI::VGP::B3 },
	{ DIK_V, XI::VGP::B4 },{ DIK_A, XI::VGP::SE },{ DIK_S, XI::VGP::ST },
	{ DIK_Q, XI::VGP::L1 },{ DIK_W, XI::VGP::R1 },
	{ DIK_D, XI::VGP::L3 },{ DIK_NUMPAD5, XI::VGP::R3 },
	};
	ge->in1 = XI::GamePad::CreateKB(ls, rs, tg, key);

	//ゲームパッドの入力を受け取るオブジェクトを生成する
	XI::KeyDatas_GP	but = {
		{ XI::RGP::B01, XI::VGP::B1 },{ XI::RGP::B02, XI::VGP::B2 },
	{ XI::RGP::B03, XI::VGP::B3 },{ XI::RGP::B04, XI::VGP::B4 },
	{ XI::RGP::B05, XI::VGP::L1 },{ XI::RGP::B06, XI::VGP::R1 },
	{ XI::RGP::B07, XI::VGP::SE },{ XI::RGP::B08, XI::VGP::ST },
	{ XI::RGP::B09, XI::VGP::L3 },{ XI::RGP::B10, XI::VGP::R3 },
	};
	auto wgp = XI::GamePad::CreateGP(0, but);
	//キーボードとゲームパッド統合
	XI::GamePad::Link(ge->in1, wgp);

	ge->in2 = XI::GamePad::CreateGP(1, but);
	ge->in3 = XI::GamePad::CreateGP(2, but);
	ge->in4 = XI::GamePad::CreateGP(3, but);
	ge->mouse = XI::Mouse::Create(ge->viewScaleW, ge->viewScaleH);

	actTask = TaskFlag::Non;	//初期状態は実行中のタスクを無効にしておく
	nextTask = TaskFlag::Title;	//次のタスクをタイトルにすることで最初にタイトルが実行される
}
//-----------------------------------------------------------------------------
//外部ファイルより
namespace  Title {//タイトルタスク
	extern  void  Initialize();
	extern  TaskFlag  UpDate();
	extern  void  Render();
	extern  void  Finalize();
}
namespace  Game {//ゲーム本編タスク
	extern  void  Initialize();
	extern  TaskFlag  UpDate();
	extern  void  Render();
	extern  void  Finalize();
}
namespace  Ending {//エンディングタスク
	extern  void  Initialize();
	extern  TaskFlag  UpDate();
	extern  void  Render();
	extern  void  Finalize();
}
//以下必要に応じて追加

//-----------------------------------------------------------------------------
//解放処理
//機能概要：プログラム終了時に１回実行される（素材などの解放を行う）
//-----------------------------------------------------------------------------
void  MyGameMain_Finalize()
{
	//途中で終了要求された場合の対策
	//今実行中のタスクの終了処理を呼びだす
	switch (actTask) {
	case TaskFlag::Title:	Title::Finalize();	break;
	case TaskFlag::Game:		Game::Finalize();	break;
	case TaskFlag::Ending:	Ending::Finalize();	break;
		//以下必要に応じて追加
	}

	ge->in1.reset();
	ge->in2.reset();
	ge->in3.reset();
	ge->in4.reset();
	ge->mouse.reset();
}
//-----------------------------------------------------------------------------
//更新処理
//機能概要：ゲームの１フレームに当たる処理
//-----------------------------------------------------------------------------
void  MyGameMain_UpDate()
{
	//次に実行するタスクが、今のタスクと一致しない
	if (actTask != nextTask) {
		//今実行中のタスクの終了処理を呼びだす
		switch (actTask) {
		case TaskFlag::Title:	Title::Finalize();	break;
		case TaskFlag::Game:		Game::Finalize();	break;
		case TaskFlag::Ending:	Ending::Finalize();	break;
			//以下必要に応じて追加
		}
		//次のタスクに移る
		actTask = nextTask;
		//次のタスクの初期化処理を呼びだす
		switch (actTask) {
		case TaskFlag::Title:	Title::Initialize();	break;
		case TaskFlag::Game:		Game::Initialize();		break;
		case TaskFlag::Ending:	Ending::Initialize();	break;
			//以下必要に応じて追加
		}
	}
	//現在のタスクの実行・更新処理を呼びだす
	switch (actTask) {
	case TaskFlag::Title:	nextTask = Title::UpDate();		break;
	case TaskFlag::Game:		nextTask = Game::UpDate();		break;
	case TaskFlag::Ending:	nextTask = Ending::UpDate();	break;
		//以下必要に応じて追加
	}
}
//-----------------------------------------------------------------------------
//描画処理
//機能概要：ゲームの１フレームに当たる表示処理 ２Ｄ
//-----------------------------------------------------------------------------
void  MyGameMain_Render2D()
{
	switch (actTask) {
	case TaskFlag::Title:	Title::Render();	break;
	case TaskFlag::Game:		Game::Render();		break;
	case TaskFlag::Ending:	Ending::Render();	break;
		//以下必要に応じて追加
	}
}
