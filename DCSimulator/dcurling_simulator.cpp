#include "dcurling_simulator.h"

#include <random>
#include <cmath>

// Include for debug TODO: Delete in Release build
#include <bitset>
#include <iostream>
#include <iomanip>

#include "Box2D/Box2D.h"

#ifdef _WIN32
#define DLLEXP __declspec(dllexport)
#else // _WIN32
#define DLLEXP
#endif // _WIN32

#ifdef _DEBUG
#pragma comment( lib, "Box2D/bin/Box2D_d.lib" )
#endif
#ifndef _DEBUG
#pragma comment( lib, "Box2D/bin/Box2D.lib" )
#endif

namespace digital_curling {

	namespace b2simulator {

		// Constant values for Stone
		constexpr float kStoneDensity    = 10.0f;
		constexpr float kStoneResitution = 1.0f;
		constexpr float kStoneFriction   = 12.009216f;  // NOTE: shoud NOT be constant?
		constexpr float kStandardAngle   = 0.066696f;
		//constexpr float kForceVerticalBase = kStandardAngle * kStoneFriction;

		// Constant values for Rink
		constexpr float kPlayAreaXLeft   = 0.000f + kStoneR;
		constexpr float kPlayAreaXRight  = kSideX - kStoneR;
		constexpr float kPlayAreaYTop    = 3.050f + kStoneR;
		constexpr float kPlayAreaYBottom = kHogY - kStoneR;
		constexpr float kRinkYTop        = 0.000f + kStoneR;
		constexpr float kRinkYBottom     = 3.050f + kRinkHeight - kStoneR;
		constexpr float kHackY           = 41.280f;     // Y coord of Hack?

		// Constant values for simulation
		constexpr int kVelocityIterations = 10;        // Iteration?
		constexpr int kPositionIterations = 10;        // Iteration?
		constexpr float kTimeStep = (1.0f / 1000.0f);  // Flame rate

		// Options
		const unsigned int kNumFreeguard = 4;     // Num of shot for freeguard rule
		StoneArea kAreaFreeguard = IN_FREEGUARD;  // Area of unremoval stones
		unsigned int num_freeguard = kNumFreeguard;
		StoneArea area_freeguard = kAreaFreeguard;

		// Create body (= stone)
		b2Body *CreateBody(float x, float y, b2World &world) {
			b2BodyDef body_def;

			body_def.type = b2_dynamicBody;  // set body type as dynamic
			body_def.position.Set(x, y);     // set position (x, y)
			body_def.angle = 0.0f;           // set angle  0 (not affected by angle?)

			// Create body
			b2Body *body = world.CreateBody(&body_def);

			// Set CircleShape as Stone
			b2CircleShape dynamic_ball;
			dynamic_ball.m_radius = kStoneR;

			// Set FixtureDef
			b2FixtureDef fixture_def;
			fixture_def.shape       = &dynamic_ball;
			fixture_def.density     = kStoneDensity;
			fixture_def.restitution = kStoneResitution;
			fixture_def.friction    = kStoneFriction;

			// Create Fixture
			body->CreateFixture(&fixture_def);

			return body;
		}

		// State of Board for b2d simulator
		class Board {
		public:
			// Set stones into board
			Board(GameState const &gs, ShotVec const &vec) : world_(b2Vec2(0, 0)) {
				// Set shot_num_
				shot_num_ = gs.ShotNum;
				// Create bodies by positions of stone in GameState
				for (unsigned int i = 0; i < gs.ShotNum; i++) {
					body_[i] = CreateBody(gs.body[i][0], gs.body[i][1], world_);
				}

				// Set ShotVec
				assert(shot_num_ < 16);
				// Create body
				body_[shot_num_] = CreateBody(kCenterX, kHackY, world_);
				// Set verocity
				body_[shot_num_]->SetLinearVelocity(b2Vec2(vec.x, vec.y));
				if (vec.angle) {
					body_[shot_num_]->SetAngularVelocity(-1 * kStandardAngle);
				}
				else {
					body_[shot_num_]->SetAngularVelocity(kStandardAngle);
				}
			}
			~Board() {
				for (unsigned int i = 0; i < 16; i++) {
					if (body_[i] != nullptr) {
						world_.DestroyBody(body_[i]);
					}
				}
			}

			b2World world_;
			b2Body *body_[16];
			unsigned int shot_num_;
		};

		// Get which area stone is in
		int GetStoneArea(const b2Vec2 &pos) {
			int ret = 0;

			// Check stone is in Rink and Playarea
			if (kPlayAreaXLeft < pos.x && pos.x < kPlayAreaXRight) {
				if (kRinkYTop < pos.y && pos.y < kRinkYBottom) {
					ret |= IN_RINK;
				}
				if (kPlayAreaYTop < pos.y && pos.y < kPlayAreaYBottom) {
					ret |= IN_PLAYAREA;
					
					// Calculate distance from center of House
					float distance = b2Vec2(pos.x - kCenterX, pos.y - kTeeY).Length();
					// Check stone is in house
					if (distance < kHouseR + kStoneR) {
						ret |= IN_HOUSE;
					}
					else if (kTeeY + kStoneR < pos.y) {
						ret |= IN_FREEGUARD;
					}
				}
			}

			return ret;
		}

		// Add friction to single stone
		b2Vec2 FrictionStep(float friction, b2Vec2 vec, float angle)
		{
			b2Vec2	v_ret = vec;
			float v_length = vec.Length();

			if (v_length > friction) {
				b2Vec2 norm, force_vertical;

				// norm = normalized vector from vec
				norm = vec;
				norm.x = vec.x / v_length;
				norm.y = vec.y / v_length;

				// force_vertical = force which is applied vertically to vec
				force_vertical.x = norm.y;
				force_vertical.y = -1 * norm.y;

				norm *= friction;
				v_ret -= norm;

				// Add vertical force
				if (angle != 0.0f) {  // Only delivered shot has angle != 0
					force_vertical *= 
						((angle > 0) ? 
						-friction * kStandardAngle:
						friction * kStandardAngle);
					v_length = v_ret.Length();
					// Add vertical force
					v_ret += force_vertical;
					// Normalize again
					v_ret.Normalize();
					// Reform vec_ret
					v_ret *= v_length;
				}

				return v_ret;
			}
			else {
				return b2Vec2(0.0f, 0.0f);
			}
		}

		// Add friction to all stones
		void FrictionAll(float friction, Board &board) {
			b2Vec2 vec;

			// Add friction to each stones in board
			for (unsigned int i = 0; i < board.shot_num_ + 1; i++) {
				if (board.body_[i] != nullptr) {
					vec = FrictionStep(
						friction, 
						board.body_[i]->GetLinearVelocity(), 
						board.body_[i]->GetAngularVelocity());
					board.body_[i]->SetLinearVelocity(vec);
					if (vec.Length() == 0) {
						board.body_[i]->SetAngularVelocity(0.0f);
					}
				}
			}
		}

		// Main loop for simulation
		int MainLoop(const float time_step, const int loop_count, Board &board) {
			int num_steps;

			// Add friction 0.5 step at first
			FrictionAll(kStoneFriction * time_step * 0.5f, board);

			for (num_steps = 0; num_steps < loop_count || loop_count == -1; num_steps++) {
				// Calclate friction
				board.world_.Step(time_step, kVelocityIterations, kPositionIterations);
				FrictionAll(kStoneFriction * time_step, board);

				// Check state of each stone
				for (unsigned int i = 0; i < board.shot_num_ + 1; i++) {
					if (board.body_[i] != nullptr) {
						b2Vec2 vec = board.body_[i]->GetLinearVelocity();
						// Get area of stone
						int area = GetStoneArea(board.body_[i]->GetPosition());
						if (area == OUT_OF_RINK) {
							//  Destroy body if a stone is out from Rink
							board.world_.DestroyBody(board.body_[i]);
							board.body_[i] = nullptr;
						}
						else if (vec.x != 0.0f || vec.y != 0.0f) {
							// Continue first loop if a stone is awake
							break;
						}
					}
					if (i == board.shot_num_) {
						// Break first loop if all stone is stopped
						goto LOOP_END;
					}
				}
			}

			LOOP_END:

			// Remove delivered stone if not in playarea
			if (board.body_[board.shot_num_] != nullptr) {
				// Get area of stone
				int area = GetStoneArea(board.body_[board.shot_num_]->GetPosition());
				if (!(area & IN_PLAYAREA)) {
					//  Destroy body if a stone is out from playarea
					board.world_.DestroyBody(board.body_[board.shot_num_]);
					board.body_[board.shot_num_] = nullptr;
				}
			}

			return num_steps;
		}

		// Main loop for simulation (with recording trajectory)
		int MainLoop_Trajectory(const float time_step, const int loop_count, Board &board, float *trajectory, size_t traj_size) {
			int num_steps;

			// Add friction 0.5 step at first
			FrictionAll(kStoneFriction * time_step * 0.5f, board);

			for (num_steps = 0; num_steps < loop_count || loop_count == -1; num_steps++) {
				// Calclate friction
				board.world_.Step(time_step, kVelocityIterations, kPositionIterations);
				FrictionAll(kStoneFriction * time_step, board);

				// Record to trajectory array
				if (trajectory != nullptr) {
					b2Vec2 vec;
					for (unsigned int i = 0; i < board.shot_num_ + 1 && i < traj_size; i++) {
						if (board.body_[i] != nullptr) {
							vec = board.body_[i]->GetPosition();
							trajectory[num_steps * 32 + i] = vec.x;
							trajectory[num_steps * 32 + i + 1] = vec.y;
						}
						else {
							trajectory[num_steps * 32 + i] = vec.x;
							trajectory[num_steps * 32 + i + 1] = vec.y;
						}
					}
				}

				// Check state of each stone
				for (unsigned int i = 0; i < board.shot_num_ + 1; i++) {
					if (board.body_[i] != nullptr) {
						b2Vec2 vec = board.body_[i]->GetLinearVelocity();
						// Get area of stone
						int area = GetStoneArea(board.body_[i]->GetPosition());
						if (area == OUT_OF_RINK) {
							//  Destroy body if a stone is out from Rink
							board.world_.DestroyBody(board.body_[i]);
							board.body_[i] = nullptr;
						}
						else if (vec.x != 0.0f || vec.y != 0.0f) {
							// Continue first loop if a stone is awake
							break;
						}
					}
					if (i == board.shot_num_) {
						// Break first loop if all stone is stopped
						goto LOOP_END;
					}
				}
			}

		LOOP_END:

			// Remove delivered stone if not in playarea
			if (board.body_[board.shot_num_] != nullptr) {
				// Get area of stone
				int area = GetStoneArea(board.body_[board.shot_num_]->GetPosition());
				if (!(area & IN_PLAYAREA)) {
					//  Destroy body if a stone is out from playarea
					board.world_.DestroyBody(board.body_[board.shot_num_]);
					board.body_[board.shot_num_] = nullptr;
				}
			}

			return num_steps;
		}

		// Check Freeguard rule
		//  returns true if stone is removed in freeguard
		bool IsFreeguardFoul(
			const Board &board,        // Board after simulation
			const GameState* const gs  // GameState before simulation
		) {
			if (gs->ShotNum < kNumFreeguard) {
				int area_before;
				int area_after;
				for (unsigned int i = 0; i < gs->ShotNum; i++) {
					// Get stone area before and after Simulation
					area_before =
						GetStoneArea(b2Vec2(gs->body[i][0], gs->body[i][1]));
					area_after = (board.body_[i] != nullptr) ?
						GetStoneArea(board.body_[i]->GetPosition()) :
						OUT_OF_RINK;
					if ((area_before & area_freeguard) && !(area_after & IN_PLAYAREA)) {
						// if area b4 sim is PLAYAREA and area after is NOT PLAYAREA
						return true;
					}
				}
			}
			return false;
		}

		// Get distance from (0,0)
		inline float GetDistance(float x, float y) {
			return sqrt(pow(x, 2) + pow(y, 2));
		}

		// Return score of second (which has last shot in this end)
		int GetScore(const GameState* const game_state) {

			struct Stone {
				unsigned int shot_num = 0;   // Number of shot
				float distance = 9999.0f;    // Distance from center of House
			} stone[16];

			// Get Number and Distance
			for (unsigned int i = 0; i < game_state->ShotNum; i++) {
				stone[i].shot_num = i;
				stone[i].distance = GetDistance(
					game_state->body[i][0] - kCenterX, 
					game_state->body[i][1] - kTeeY);
			}

			// Sort by distance
			std::sort(stone, stone + game_state->ShotNum, 
				[](const Stone& s1, const Stone s2) {return s1.distance < s2.distance; });

			// Calculate score
			int score = 0;                    // Score
			int mod = stone[0].shot_num % 2;  // Player which has no.1 stone
			int base = (mod == 0) ? -1 : 1;   // Score per 1 stone
			for (unsigned int i = 0; i < game_state->ShotNum; i++) {
				if ((stone[i].distance < kHouseR + kStoneR) &&
					(stone[i].shot_num % 2 == mod)) {
					score += base;
				}
				else {
					break;
				}
			}
			
			return score;
		}

		// Update game_state from board
		void UpdateState(const Board &board, GameState* const game_state) {
			// Update ShotNum (ShotNum can be 16, not reset to 0)
			game_state->ShotNum++;

			// Update positions
			for (unsigned int i = 0; i < game_state->ShotNum; i++) {
				if (board.body_[i] != nullptr) {
					b2Vec2 pos = board.body_[i]->GetPosition();
					game_state->body[i][0] = pos.x;
					game_state->body[i][1] = pos.y;
				}
				else {
					game_state->body[i][0] = 0.0f;
					game_state->body[i][1] = 0.0f;
				}
			}

			// Update Score if ShotNum == 16
			if (game_state->ShotNum == 16) {
				// Calculate Socre
				int score = GetScore(game_state);
				game_state->Score[game_state->CurEnd] = 
					(game_state->WhiteToMove) ? -score : score;
				// Update WhiteToMove
				if (score == 0) {
					game_state->WhiteToMove ^= true;
				}
				else {
					game_state->WhiteToMove = (score < 0);
				}
			}
			else {
				// Update WhiteToMove
				game_state->WhiteToMove ^= true;
			}
		}

		// Simulation with Box2D (compatible with Simulation() in CurlingSimulator.h)
		int Simulation(
			GameState* const game_state, 
			ShotVec shot_vec, 
			float random_x, float random_y, 
			ShotVec* const run_shot, 
			float *trajectory, size_t traj_size) {

			if (game_state->ShotNum > 15) {
				return -1;
			}

			// Add random number to shot
			AddRandom2Vec(random_x, random_y, &shot_vec);
			if (run_shot != nullptr) {
				// Copy random-added shot_vec to run_shot
				memcpy_s(run_shot, sizeof(ShotVec), &shot_vec, sizeof(ShotVec));
			}

			// Create board
			Board board(*game_state, shot_vec);

			// Run mainloop of simulation
			int steps;
			if (trajectory != nullptr) {
				steps = MainLoop_Trajectory(kTimeStep, -1, board, trajectory, traj_size);
			}
			else {
				steps = MainLoop(kTimeStep, -1, board);
			}

			// Check freeguard zone rule
			if (IsFreeguardFoul(board, game_state)) {
				game_state->ShotNum++;
				game_state->WhiteToMove ^= 1;
				return 0;
			}

			// Update game_state
			UpdateState(board, game_state);

			return steps;
		}

		// ?
		b2Vec2 CreateShot(float x, float y)
		{
			b2Vec2 Shot;
			float len;

			Shot.Set(x - kCenterX, y - kHackY);
			len = Shot.Length();
			Shot.Normalize();
			Shot.operator *=(sqrt(len * 2.0f * kStoneFriction));

			return Shot;
		}

		// Create ShotVec from ShotPos which a stone will stop at
		void CreateShot(ShotPos pos, ShotVec* const vec)
		{
			float tt = 0.0335f;
			float x_base = 1.22f;
			b2Vec2 vec_tmp;

			if (pos.angle == true) {
				vec_tmp = CreateShot(
					x_base + pos.x - tt*(pos.y - kTeeY), 
					tt*(pos.x - kCenterX) + pos.y);
			}
			else {
				vec_tmp = CreateShot(
					-x_base + pos.x + tt*(pos.y - kTeeY), 
					-tt*(pos.x - kCenterX) + pos.y);
			}

			vec->x = vec_tmp.x;
			vec->y = vec_tmp.y;
			vec->angle = pos.angle;
		}

		// Add random number to ShotVec (normal distribution)
		void AddRandom2Vec(float random_x, float random_y, ShotVec* const vec) {
			// Prepare random
			std::random_device seed_gen;
			std::default_random_engine engine(seed_gen());

			ShotPos tee_pos(kCenterX, kTeeY, vec->angle);
			ShotVec tee_shot, add_rand_tee_shot;

			// Create shot to center of house
			CreateShot(tee_pos, &tee_shot);

			// Add random to tee_pos (normal distribution)
			if (random_x != 0.0f) {
				std::normal_distribution<float> dist_x(0, random_x);
				tee_pos.x += dist_x(engine);
			}
			if (random_y != 0.0f) {
				std::normal_distribution<float> distY(0, random_y);
				tee_pos.y += distY(engine);
			}
			CreateShot(tee_pos, &add_rand_tee_shot);

			// Add random to vec
			*vec += tee_shot - add_rand_tee_shot;
		}

		// Set options for freeguard zone rule
		void SetOptions(unsigned int shot_num, StoneArea area) {
			num_freeguard = shot_num;
			area_freeguard = area;
		}
		// Set options to default
		void SetOptions() {
			num_freeguard = kNumFreeguard;
			area_freeguard = kAreaFreeguard;
		}
	}
}