#pragma once

namespace digital_curling {

		// Constant values
		constexpr unsigned int kLastEndMax = 10;  // Maximum number of LastEnd

		constexpr float kCenterX     =  2.375f;  // X coord of Center Line
		constexpr float kTeeY        =  4.880f;  // Y coord of Tee Line
		constexpr float kSideX       =  4.750f;  // X coord of Side Line
		constexpr float kHogY        = 11.280f;  // Y coord of Hog Line
		constexpr float kRinkHeight  = 42.500f;  // Height of Rink
		constexpr float kHackY       = 41.280f;  // Y coord of Hack? (Shot starts here)
		constexpr float kStoneR      =  0.145f;  // Radius of Stone
		constexpr float kHouseR      =  1.830f;  // Radius of House (12 foot circle)
		constexpr float kHouse8FootR =  1.220f;  // Radius of House ( 8 foot circle)
		constexpr float kHouse4FootR =  0.610f;  // Radius of House ( 4 foot circle)

		constexpr float kRinkInfoX = 0.000f;
		constexpr float kRinkInfoY = 3.050f;

		// State of DigitalCurling game
		typedef struct GameState {
		public:
			GameState();
			GameState(unsigned int last_end);
			~GameState();

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

		// Operators
		ShotPos operator+(ShotPos pos_l, ShotPos pos_r);
		ShotPos operator-(ShotPos pos_l, ShotPos pos_rs);
		ShotPos operator+=(ShotPos &pos_l, ShotPos pos_r);
		ShotPos operator-=(ShotPos &pos_l, ShotPos pos_r);
		ShotVec operator+(ShotVec pos_l, ShotVec pos_r);
		ShotVec operator-(ShotVec pos_l, ShotVec pos_r);
		ShotVec operator+=(ShotVec &pos_l, ShotVec pos_r);
		ShotVec operator-=(ShotVec &pos_l, ShotVec pos_r);

		// Simulator with Box2D 2.3.0 (http://box2d.org/)
		namespace b2simulator {
			// Area of stone
			typedef enum {
				IN_RINK = 0x0000,
				IN_PLAYAREA = IN_RINK << 1,
				IN_FREEGUARD = IN_PLAYAREA << 1,
				IN_HOUSE = IN_FREEGUARD << 1
			} StoneArea;

			// Simulation with Box2D (compatible with Simulation() in CurlingSimulator.h)
			void Simulation(GameState* const game_state, ShotVec shot_vec, float random_x, float random_y, ShotVec* const run_shot, float *trajectory, size_t traj_size);

			// Add random number to ShotVec (normal distribution)
			void AddRandom2Vec(float random_x, float random_y, ShotVec* const vec);

			// Set options for freeguard zone rule
			void SetOptions(unsigned int shot_num, StoneArea area);
			// Reset options to default
			void SetOptions();
		}
}