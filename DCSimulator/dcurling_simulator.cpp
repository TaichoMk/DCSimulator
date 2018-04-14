#include "dcurling_simulator.h"

//#include <time.h>
#include <random>
//#include <cstdlib>
//#include <cstring>

#include "Box2D/Box2D.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/bin/Box2D_d.lib" )
#endif
#ifndef _DEBUG
#pragma comment( lib, "Box2D/bin/Box2D.lib" )
#endif

namespace digital_curling {

	namespace b2simulator {

		// Constant values for simulation
		constexpr float kStoneDensity    = 10.0f;
		constexpr float kStoneResitution = 1.0f;
		constexpr float kStoneFriction   = 12.009216f;  // NOTE: shoud NOT be constant?

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
			Board(GameState const &gs) : world_(b2Vec2(0, 0)) {
				// Set shot_num_
				shot_num_ = gs.ShotNum;
				// Create bodies by positions of stone in GameState
				for (unsigned int i = 0; i < gs.ShotNum; i++) {
					body_[i] = CreateBody(gs.body[i][0], gs.body[i][1], world_);
				}
			}
			~Board() {
				for (unsigned int i = 0; i < 16; i++) {
					if (body_[i] != nullptr) {
						world_.DestroyBody(body_[i]);
					}
				}
			}

			// Set a Shot into Board
			void SetShot(ShotVec const &vec) {
				assert(shot_num_ < 16);
				// Create body
				body_[shot_num_] = CreateBody(kCenterX, kHackY, world_);
				// Set verocity
				body_[shot_num_]->SetLinearVelocity(b2Vec2(vec.x, vec.y));
				body_[shot_num_]->SetAngularVelocity(0.0f);  // should set number > 0 ?

				shot_num_++;
			}

			b2World world_;
			b2Body *body_[16];
			unsigned int shot_num_;
		};

		void FrictionAll(float friction, Board &board) {

		}

		// Main loop for simulation
		int MainLoop(const float time_step, const int loop_count, Board &board, float *trajectory, size_t traj_size) {
			int ret;

			// Friction 0.5 step at first
			FrictionAll(kStoneFriction * time_step * 0.5f, board);

			for (ret = 0; ret < loop_count || ret == -1; ret++) {
				// Calclate friction
				board.world_.Step(time_step, kVelocityIterations, kPositionIterations);
				FrictionAll(kStoneFriction * time_step, board);

				// Record to trajectory array
				if (trajectory != nullptr) {
					b2Vec2 vec;
					for (unsigned int i = 0; i < board.shot_num_ && i < traj_size; i++) {
						vec = board.body_[i]->GetPosition();
						trajectory[ret * 32 + i] = vec.x;
						trajectory[ret * 32 + i + 1] = vec.y;
					}
				}

				// Check state of each stone
				for (unsigned int i = 0; i < board.shot_num_; i++) {
					if (0 /* TODO:check stone is in playarea */) {
						//  if a stone is out from playarea: DestroyBody() ?
					}
					else if (0 /* TODO:check freeguard zone rule */) {
						//  if freeguard zone rule: break ?
					}
					//  if all stone is stopped: break
					else if (board.body_[i]->IsAwake()) {
						break;
					}
					if (i == board.shot_num_ - 1) {
						goto LOOP_END;
					}
				}
			}

			LOOP_END:

			return ret;
		}

		// Simulation with Box2D (compatible with Simulation() in CurlingSimulator.h)
		void Simulation(
			GameState* const game_state, 
			ShotVec shot_vec, 
			float random_x, float random_y, 
			ShotVec* const run_shot, 
			float *trajectory, size_t traj_size) {

			// Create board
			Board board(*game_state);

			// Add random number to shot
			AddRandom2Vec(random_x, random_y, &shot_vec);
			if (run_shot != nullptr) {
				// Copy random-added shot_vec to run_shot
				memcpy_s(run_shot, sizeof(ShotVec), &shot_vec, sizeof(ShotVec));
			}

			// Set shot_vec to WorkVec
			board.SetShot(shot_vec);

			// Run mainloop of simulation
			MainLoop(kTimeStep, traj_size, board, trajectory, traj_size);

			// Check freeguard zone rule
			// TODO: implement, or do in mainloop?

			// Update game_state
			// TODO: implement

			// Delete board
			//delete &board;
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