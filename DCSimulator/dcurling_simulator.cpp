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

		// Constant values for CreateBody()
		constexpr float kStoneDensity    = 10.0f;
		constexpr float kStoneResitution = 1.0f;
		constexpr float kStoneFriction   = 12.009216f;  // NOTE: shoud not be constant?

		// Options
		const unsigned int kNumFreeguard = 4;     // Num of shot for freeguard rule
		StoneArea kAreaFreeguard = IN_FREEGUARD;  // Area of unremoval stones
		unsigned int num_freeguard = kNumFreeguard;
		StoneArea area_freeguard = kAreaFreeguard;

		// Create body (= stone)
		b2Body *CreateBody(float x, float y, b2World* const world) {
			b2BodyDef body_def;

			body_def.type = b2_dynamicBody;  // set body type as dynamic
			body_def.position.Set(x, y);     // set position (x, y)
			body_def.angle = 0.0f;           // set angle  0 (not affected by angle?)

			// Create body
			b2Body *body = world->CreateBody(&body_def);

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
			Board(const GameState* const gs) : world(b2Vec2(0, 0)) {
				for (unsigned int i = 0; i < gs->ShotNum; i++) {
					body[i] = CreateBody(gs->body[i][0], gs->body[i][1], &world);
				}
			}
			~Board() {
				for (unsigned int i = 0; i < 16; i++) {
					if (body[i] != nullptr) {
						world.DestroyBody(body[i]);
					}
				}
			}

			b2World world;
			b2Body *body[16];
		};

		// Simulation (this function is compatible with Simulation() in CurlingSimulator.h)
		void Simulation(GameState *game_state, ShotVec shot_vec, float random_x, float random_y, ShotVec *run_shot) {

			// Create board
			Board *board = new Board(game_state);

			// Add random number to shot
			AddRandom2Vec(random_x, random_y, &shot_vec);
			if (run_shot != nullptr) {
				// Copy random-added shot_vec to run_shot
				memcpy_s(run_shot, sizeof(ShotVec), &shot_vec, sizeof(ShotVec));
			}

			// Set shot_vec to WorkVec
			// TODO: implement

			// Run mainloop of simulation
			// TODO: implement

			// Check freeguard zone rule
			// TODO: implement

			// Set game_state
			// TODO: implement

			// Delet board
			delete board;
		}

		// Add random number to ShotVec (normal distribution)
		void AddRandom2Vec(float random_x, float random_y, ShotVec *vec) {
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