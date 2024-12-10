#include "MyPG.h"
#include "MyGameMain.h"

// ����
namespace Game
{
	// ���� ����
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
	GameState Map_CheckGameState(MapData& md_); // �Լ� ���� �߰�

	//-----------------------------------------------------------------------------
	// �ʱ�ȭ
	//-----------------------------------------------------------------------------
	void Initialize()
	{
		// ��� ���� �ʱ�ȭ
		ge->dgi->EffectState().param.bgColor = ML::Color(1, 0, 1, 0);

		imgMapChip = DG::Image::Create("./data/image/MaruBatu.bmp");
		font = DG::Font::Create("HG�������ë�M-PRO", 8, 16);

		// �� �ʱ�ȭ
		Map_Initialize(mapData);

		mapDrawOffset.x = (480 - 64 * 3) / 2;
		mapDrawOffset.y = (270 - 64 * 3) / 2;
		turn = Stone::Maru; // ù ���� o

		gameState = GameState::Normal;
	}
	//-----------------------------------------------------------------------------
	// ���� ó��
	//-----------------------------------------------------------------------------
	void Finalize()
	{
		imgMapChip.reset();
		font.reset();
	}
	//-----------------------------------------------------------------------------
	// ������Ʈ
	//-----------------------------------------------------------------------------
	TaskFlag UpDate()
	{
		auto inp = ge->in1->GetState();
		auto ms = ge->mouse->GetState();

		// Ŭ���� �����ϴ� ó��
		if (ms.LB.down) {
			ML::Point mp = ms.pos;
			// ȭ�鿡 ǥ�õǴ� ������
			ML::Rect sb = { mapDrawOffset.x, mapDrawOffset.y,
							mapDrawOffset.x + (64 * 3),
							mapDrawOffset.y + (64 * 3)
			};
			if (mp.x >= sb.left && mp.x < sb.right &&
				mp.y >= sb.top && mp.y < sb.bottom) {
				ML::Point mp2 = { mp.x - sb.left, mp.y - sb.top };
				// �Ž� ������ ��ǥ�� ���Ѵ�
				ML::Point mp3 = { mp2.x / 64, mp2.y / 64 };

				// �ʿ� �ٲ��� ��û
				bool set = Map_ChangeChip(mapData, mp3, turn);
				if (set == true) {
					// ���� ���� üũ
					gameState = Map_CheckGameState(mapData);
					if (turn == Stone::Maru) { turn = Stone::Batu; }
					else if (turn == Stone::Batu) { turn = Stone::Maru; }
				}
			}
		}

		TaskFlag rtv = TaskFlag::Game; // ���� ���� ����
		if (true == inp.ST.down) {
			rtv = TaskFlag::Title; // Ÿ��Ʋ�� �̵�
		}
		return rtv;
	}
	//-----------------------------------------------------------------------------
	// ������
	//-----------------------------------------------------------------------------
	void Render()
	{
		Map_Render(mapData);
		GameState_Render(); // ���� ���� ǥ��
	}
	//-----------------------------------------------------------------------------
	void Map_Initialize(MapData& md_)
	{
		// �迭 �ʱ�ȭ
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				md_.arr[y][x] = Stone::Non;
			}
		}
		// Ĩ ��ġ �ʱ�ȭ
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
				draw.Offset(mapDrawOffset.x + x * 64, mapDrawOffset.y + y * 64); // x�� y�� ���� ������ ���
				int num = int(md_.arr[y][x]); // ������ ����
				ML::Box2D src = md_.chip[num]; // ������ �̹��� �ҽ�
				imgMapChip->Draw(draw, src);
			}
		}
	}

	// �ʿ� Ĩ�� �����ϱ� �� ����Ǵ� ó��
	bool Map_ChangeChip(MapData& md_, ML::Point p_, Stone stone_)
	{
		// ���� üũ
		if (p_.x < 0 || p_.y < 0 || p_.x >= 3 || p_.y >= 3) {
			return false;
		}
		// ���� ��ǥ�� ��ĭ�� �ƴ�
		if (md_.arr[p_.y][p_.x] != Stone::Non) {
			return false;
		}
		md_.arr[p_.y][p_.x] = stone_;
		return true;
	}

	void GameState_Render()
	{
		ML::Box2D textBox(0, 0, 480, 32);

		// ������ ǥ��
		if (gameState == GameState::Normal) {
			if (turn == Stone::Maru) {
				font->Draw(textBox, "o turn", ML::Color(1, 0, 0, 1));
			}
			else {
				font->Draw(textBox, "x turn", ML::Color(1, 0, 0, 1));
			}
		}
		// ���� ǥ��
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
		p = md_.arr; // �������� ��� ������ ����
		//ox�� �¸�����
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
		
		// ���º� �Ǻ�
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				if (p[y][x] == Stone::Non) {
					return GameState::Normal; // ��ĭ�� ������ ���� ���� ��
				}
			}
		}

		return GameState::Draw; // ���º�
	}
} // namespace Game
