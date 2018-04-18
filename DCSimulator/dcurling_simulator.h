#pragma once

#ifndef DLLEXP
#ifdef _WIN32
#define DLLEXP __declspec(dllexport)
#else // _WIN32
#define DLLEXP
#endif // _WIN32
#endif // _DLLEXP

namespace digital_curling {

		// Constant values
		constexpr unsigned int kLastEndMax = 10;  // Maximum number of LastEnd

		constexpr float kCenterX     =  2.375f;  // X coord of Center Line
		constexpr float kTeeY        =  4.880f;  // Y coord of Tee Line
		constexpr float kSideX       =  4.750f;  // X coord of Side Line
		constexpr float kHogY        = 11.280f;  // Y coord of Hog Line
		constexpr float kRinkHeight  = 42.500f;  // Height of Rink
		constexpr float kStoneR      =  0.145f;  // Radius of Stone
		constexpr float kHouseR      =  1.830f;  // Radius of House (12 foot circle)
		constexpr float kHouse8FootR =  1.220f;  // Radius of House ( 8 foot circle)
		constexpr float kHouse4FootR =  0.610f;  // Radius of House ( 4 foot circle)

		// State of DigitalCurling game
		class DLLEXP GameState {
		public:
			GameState();
			GameState(unsigned int last_end);
			~GameState();

			// Clear body and Set ShotNum = 0
			void Clear();

			// Set stone 
			void Set(unsigned int num, float x, float y);

			//  Note: Same member variables as GAMESTATE in CurlingSimulator older ver2.x
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
		};

		// Position of Stone
		class DLLEXP ShotPos {
		public:
			ShotPos();
			ShotPos(float x, float y, bool angle);
			~ShotPos();

			float x;
			float y;
			bool angle;
		};

		// Vector of Stone
		class DLLEXP ShotVec {
		public:
			ShotVec();
			ShotVec(float x, float y, bool angle);
			~ShotVec();

			float x;
			float y;
			bool angle;
		};

		// Simulator with Box2D 2.3.0 (http://box2d.org/)
		namespace b2simulator {

			// Simulation with Box2D (compatible with Simulation() in CurlingSimulator.h)
			//  returns number of steps taken
			DLLEXP int Simulation(
				GameState* const game_state, ShotVec shot_vec, 
				float random_x, float random_y, 
				ShotVec* const run_shot, float *trajectory, size_t traj_size);

			// Create ShotVec from ShotPos which a stone will stop at
			DLLEXP void CreateShot(ShotPos pos, ShotVec* const vec);

			// Add random number to ShotVec (normal distribution)
			DLLEXP void AddRandom2Vec(float random_x, float random_y, ShotVec* const vec);

			// Return score of second (which has last shot in this end)
			DLLEXP int GetScore(const GameState* const game_state);

			// Area of stone
			DLLEXP typedef enum {
				OUT_OF_RINK = 0x0000,
				IN_RINK = 0x0001,
				IN_PLAYAREA = IN_RINK << 1,
				IN_FREEGUARD = IN_PLAYAREA << 1,
				IN_HOUSE = IN_FREEGUARD << 1
			} StoneArea;

			// Set options for freeguard zone rule
			DLLEXP void SetOptions(unsigned int shot_num, StoneArea area);
			// Set options to default
			DLLEXP void SetOptions();
		}

		// Operators
		DLLEXP ShotPos operator+(ShotPos pos_l, ShotPos pos_r);
		DLLEXP ShotPos operator-(ShotPos pos_l, ShotPos pos_rs);
		DLLEXP ShotPos operator+=(ShotPos &pos_l, ShotPos pos_r);
		DLLEXP ShotPos operator-=(ShotPos &pos_l, ShotPos pos_r);
		DLLEXP ShotVec operator+(ShotVec pos_l, ShotVec pos_r);
		DLLEXP ShotVec operator-(ShotVec pos_l, ShotVec pos_r);
		DLLEXP ShotVec operator+=(ShotVec &pos_l, ShotVec pos_r);
		DLLEXP ShotVec operator-=(ShotVec &pos_l, ShotVec pos_r);
}