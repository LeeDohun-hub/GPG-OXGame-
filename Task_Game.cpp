#include "MyPG.h"
#include "MyGameMain.h"

// 게임
namespace Game
{
	// 게임 정보
	DG::Image::SP imgMapChip;
	DG::Font::SP font;
	ML::Point mPos;
	ML::Point mapDrawOffset;

	enum class Stone : int { Non, Maru, Batu };
	enum class GameState { Normal, MaruWin, BatuWin, Draw };
	GameState gameState;

	struct MapData {
		Stone arr[3][3];
		ML::Box2D chip[3];
	};

	MapData mapData;
	void Map_Initialize(MapData& md_);
	void Map_Render(MapData& md_);
	bool Map_ChangeChip(MapData& md_, ML::Point p_, Stone stone_);
	void GameState_Render();
	Stone turn;
	GameState Map_CheckGameState(MapData& md_); // 함수 선언 추가

	//-----------------------------------------------------------------------------
	// 초기화
	//-----------------------------------------------------------------------------
	void Initialize()
	{
		// 배경 색상 초기화
		ge->dgi->EffectState().param.bgColor = ML::Color(1, 0, 1, 0);

		imgMapChip = DG::Image::Create("./data/image/MaruBatu.bmp");
		font = DG::Font::Create("HG丸ゴシックM-PRO", 8, 16);

		// 맵 초기화
		Map_Initialize(mapData);

		mapDrawOffset.x = (480 - 64 * 3) / 2;
		mapDrawOffset.y = (270 - 64 * 3) / 2;
		turn = Stone::Maru; // 첫 수는 o

		gameState = GameState::Normal;
	}
	//-----------------------------------------------------------------------------
	// 종료 처리
	//-----------------------------------------------------------------------------
	void Finalize()
	{
		imgMapChip.reset();
		font.reset();
	}
	//-----------------------------------------------------------------------------
	// 업데이트
	//-----------------------------------------------------------------------------
	TaskFlag UpDate()
	{
		auto inp = ge->in1->GetState();
		auto ms = ge->mouse->GetState();

		// 클릭에 대응하는 처리
		if (ms.LB.down) {
			ML::Point mp = ms.pos;
			// 화면에 표시되는 게임판
			ML::Rect sb = { mapDrawOffset.x, mapDrawOffset.y,
							mapDrawOffset.x + (64 * 3),
							mapDrawOffset.y + (64 * 3)
			};
			if (mp.x >= sb.left && mp.x < sb.right &&
				mp.y >= sb.top && mp.y < sb.bottom) {
				ML::Point mp2 = { mp.x - sb.left, mp.y - sb.top };
				// 매스 단위의 좌표를 구한다
				ML::Point mp3 = { mp2.x / 64, mp2.y / 64 };

				// 맵에 바꿈을 요청
				bool set = Map_ChangeChip(mapData, mp3, turn);
				if (set == true) {
					// 게임 상태 체크
					gameState = Map_CheckGameState(mapData);
					if (turn == Stone::Maru) { turn = Stone::Batu; }
					else if (turn == Stone::Batu) { turn = Stone::Maru; }
				}
			}
		}

		TaskFlag rtv = TaskFlag::Game; // 게임 상태 유지
		if (true == inp.ST.down) {
			rtv = TaskFlag::Title; // 타이틀로 이동
		}
		return rtv;
	}
	//-----------------------------------------------------------------------------
	// 렌더링
	//-----------------------------------------------------------------------------
	void Render()
	{
		Map_Render(mapData);
		GameState_Render(); // 게임 상태 표시
	}
	//-----------------------------------------------------------------------------
	void Map_Initialize(MapData& md_)
	{
		// 배열 초기화
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				md_.arr[y][x] = Stone::Non;
			}
		}
		// 칩 위치 초기화
		for (int c = 0; c < 3; ++c) {
			md_.chip[c] = ML::Box2D(c * 64, 0, 64, 64);
		}
	}
	//-----------------------------------------------------------------------------
	void Map_Render(MapData& md_)
	{
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				ML::Box2D draw(0, 0, 64, 64);
				draw.Offset(mapDrawOffset.x + x * 64, mapDrawOffset.y + y * 64); // x와 y에 따라 오프셋 계산
				int num = int(md_.arr[y][x]); // 스톤의 종류
				ML::Box2D src = md_.chip[num]; // 스톤의 이미지 소스
				imgMapChip->Draw(draw, src);
			}
		}
	}

	// 맵에 칩을 변경하기 전 실행되는 처리
	bool Map_ChangeChip(MapData& md_, ML::Point p_, Stone stone_)
	{
		// 범위 체크
		if (p_.x < 0 || p_.y < 0 || p_.x >= 3 || p_.y >= 3) {
			return false;
		}
		// 지정 좌표가 빈칸이 아님
		if (md_.arr[p_.y][p_.x] != Stone::Non) {
			return false;
		}
		md_.arr[p_.y][p_.x] = stone_;
		return true;
	}

	void GameState_Render()
	{
		ML::Box2D textBox(0, 0, 480, 32);

		// 차례의 표시
		if (gameState == GameState::Normal) {
			if (turn == Stone::Maru) {
				font->Draw(textBox, "o turn", ML::Color(1, 0, 0, 1));
			}
			else {
				font->Draw(textBox, "x turn", ML::Color(1, 0, 0, 1));
			}
		}
		// 승패 표시
		else if (gameState == GameState::MaruWin) {
			font->Draw(textBox, "o win", ML::Color(1, 1, 0, 1));
		}
		else if (gameState == GameState::BatuWin) {
			font->Draw(textBox, "x win", ML::Color(1, 1, 0, 1));
		}
		else {
			font->Draw(textBox, "draw", ML::Color(1, 1, 0, 1));
		}
	}

	GameState Map_CheckGameState(MapData& md_)
	{
		struct sJug { Stone s; GameState gs; };
		sJug jug[2] = {
			{ Stone::Maru, GameState::MaruWin},
			{ Stone::Batu, GameState::BatuWin}
		};

		struct Lins { int y1; int x1; int y2; int x2; int y3; int x3;};
		Lins lines[8] = {
			{0,0,0,1,0,2}, {1,0,1,1,1,2}, {2,0,2,1,2,2},
			{0,0,1,0,2,0}, {0,1,1,1,2,1}, {0,2,1,2,2,2},
			{0,0,1,1,2,2}, {0,2,1,1,2,0},
		};

		Stone(*p)[3];
		p = md_.arr; // 변수명이 길기 때문에 생략
		//ox의 승리판정
		for (int i = 0; i < 2; i++)
		{
			Stone s = jug[i].s;
			GameState rtv = jug[i].gs;
			for (int j = 0; j < 8; j++) {
				if (p[lines[j].y1][lines[j].x1] == s &&
					p[lines[j].y2][lines[j].x2] == s &&
					p[lines[j].y3][lines[j].x3] == s) {
					return rtv;
				}
			}
		}
		
		// 무승부 판별
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				if (p[y][x] == Stone::Non) {
					return GameState::Normal; // 빈칸이 있으면 게임 진행 중
				}
			}
		}

		return GameState::Draw; // 무승부
	}
} // namespace Game
