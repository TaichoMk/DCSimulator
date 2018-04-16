#include "dcurling_simulator.h"

//#include <time.h>
#include <random>
//#include <cstdlib>
//#include <cstring>

// Include for debug TODO: Delete in Release build
#include <bitset>
#include <iostream>
#include <iomanip>

#include "Box2D/Box2D.h"

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
			// Copied from CurlingSimulator
			// TODO: Re-write
			b2Vec2	v_ret = vec;
			float v_length = vec.Length();

			if (v_length > friction) {
				b2Vec2 norm, force_vertical;

				// norm = normalized vector
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
		int MainLoop(const float time_step, const int loop_count, Board &board, float *trajectory, size_t traj_size) {
			int num_steps;

			// Add friction 0.5 step at first
			FrictionAll(kStoneFriction * time_step * 0.5f, board);

			for (num_steps = 0; num_steps < loop_count || num_steps == -1; num_steps++) {
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
				if (area ) {
					//  Destroy body if a stone is out from playarea
					board.world_.DestroyBody(board.body_[board.shot_num_]);
					board.body_[board.shot_num_] = nullptr;
				}
			}

			return num_steps;
		}

		// Simulation with Box2D (compatible with Simulation() in CurlingSimulator.h)
		int Simulation(
			GameState* const game_state, 
			ShotVec shot_vec, 
			float random_x, float random_y, 
			ShotVec* const run_shot, 
			float *trajectory, size_t traj_size) {

			// Add random number to shot
			AddRandom2Vec(random_x, random_y, &shot_vec);
			if (run_shot != nullptr) {
				// Copy random-added shot_vec to run_shot
				memcpy_s(run_shot, sizeof(ShotVec), &shot_vec, sizeof(ShotVec));
			}

			// Create board
			Board board(*game_state, shot_vec);

			// Run mainloop of simulation
			int steps = MainLoop(kTimeStep, (int)traj_size, board, trajectory, traj_size);

			std::cerr << "MainLoop() stopped in " << steps << " steps" << std::endl;

			// Check freeguard zone rule
			// TODO: implement, or do in mainloop?

			// Update game_state
			// TODO: implement

			// Delete board
			//delete &board;

			return steps;
		}

		// Add random number to ShotVec (normal distribution)
		void AddRandom2Vec(float random_x, float random_y, ShotVec* const vec) {
			// TODO: implement
		}

		// Set options for freeguard zone rule
		void SetOptions(unsigned int shot_num, StoneArea area) {
			num_freeguard = shot_num;
			area_freeguard = area;
		}
		// Reset
		void SetOptions() {
			num_freeguard = kNumFreeguard;
			area_freeguard = kAreaFreeguard;
		}
	}
}