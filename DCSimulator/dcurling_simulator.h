#pragma once

namespace digital_curling {
	namespace simulator {

		constexpr unsigned int kLastEndMax = 10;  // Maximum number of LastEnd

		// State of DigitalCurling game
		typedef struct GameState {
		public:
			GameState();
			GameState(unsigned int last_end);
			~GameState();

			//  Notice : member variables (and their name) are same as GAMESTATE in CurlingSimulator older ver2.x
			unsigned int ShotNum;    // Number of current Shot

			unsigned int CurEnd;     // Number of current End (0 to LastEnd - 1)
			unsigned int LastEnd;    // Number of last End    (up to kLastEndMax)

			int Score[kLastEndMax];  // Score of each End 
									 //       > 0  : first player scored (second scored 0)
									 //       < 0  : second player scored (first scored 0)
									 //       = 0  : blank end (both scored 0)

			bool WhiteToMove;        // which have next shot
									 //       true  : first
									 //       false : second

			float body[16][2];       // position of stones
			                         //       [n][0] : x coord of n th deliverd stone
			                         //       [n][1] : y coord
		} _GameState;

		// Position of Stone
		typedef struct ShotPos {
			ShotPos();
			ShotPos(float x, float y, bool angle);
			~ShotPos();

			float x;
			float y;
			bool angle;
		} _ShotPos;

		// Vector of Stone
		typedef struct ShotVec {
			ShotVec();
			ShotVec(float x, float y, bool angle);
			~ShotVec();

			float x;
			float y;
			bool angle;
		} _ShotVec;

		class BasicSimulator {

		};

		// Operators
		ShotPos operator+(ShotPos pos_l, ShotPos pos_r);
		ShotPos operator-(ShotPos pos_l, ShotPos pos_rs);
		ShotPos operator+=(ShotPos &pos_l, ShotPos pos_r);
		ShotPos operator-=(ShotPos &pos_l, ShotPos pos_r);
		ShotVec operator+(ShotVec pos_l, ShotVec pos_r);
		ShotVec operator-(ShotVec pos_l, ShotVec pos_rs);
		ShotVec operator+=(ShotVec &pos_l, ShotVec pos_r);
		ShotVec operator-=(ShotVec &pos_l, ShotVec pos_r);
	}
}