// Constructors, operators and setter/getter or something
#include "dcurling_simulator.h"

#include <string>
#include <cassert>

namespace digital_curling {
	// Constructors
	GameState::GameState() :
		ShotNum(0),
		CurEnd(0),
		LastEnd(kLastEndMax),
		Score(),
		WhiteToMove(true),
		body() {}
	GameState::GameState(unsigned int last_end) :
		ShotNum(0),
		CurEnd(0),
		LastEnd(last_end),
		Score(),
		WhiteToMove(true),
		body() {}
	GameState::~GameState() {}

	// Clear body and Set ShotNum = 0
	void GameState::Clear() {
		ShotNum = 0;
		memset(body, 0x00, 2 * 16 * sizeof(float));
	}

	// Set stone 
	void GameState::Set(unsigned int num, float x, float y) {
		assert(num < 16);
		body[num][0] = x;
		body[num][1] = y;
		if (num >= ShotNum) {
			ShotNum++;
		}
	}

	ShotPos::ShotPos() :
		x(0.0f),
		y(0.0f),
		angle(false) {}
	ShotPos::ShotPos(float x, float y, bool angle) :
		x(x),
		y(y),
		angle(angle) {}
	ShotPos::~ShotPos() {}

	ShotVec::ShotVec() :
		x(0.0f),
		y(0.0f),
		angle(false) {}
	ShotVec::ShotVec(float x, float y, bool angle) :
		x(x),
		y(y),
		angle(angle) {}
	ShotVec::~ShotVec() {}

	// Operators
	ShotPos operator+(ShotPos pos_l, ShotPos pos_r) {
		return ShotPos(
			pos_l.x + pos_r.x,
			pos_l.y + pos_r.y,
			pos_l.angle & pos_r.angle
		);
	}
	ShotPos operator-(ShotPos pos_l, ShotPos pos_r) {
		return ShotPos(
			pos_l.x - pos_r.x,
			pos_l.y - pos_r.y,
			pos_l.angle & pos_r.angle
		);
	}
	ShotPos operator+=(ShotPos &pos_l, ShotPos pos_r) {
		pos_l.x += pos_r.x;
		pos_l.y += pos_r.y;
		pos_l.angle &= pos_r.angle;
		return pos_l;
	}
	ShotPos operator-=(ShotPos &pos_l, ShotPos pos_r) {
		pos_l.x -= pos_r.x;
		pos_l.y -= pos_r.y;
		pos_l.angle &= pos_r.angle;
		return pos_l;
	}

	ShotVec operator+(ShotVec pos_l, ShotVec pos_r) {
		return ShotVec(
			pos_l.x + pos_r.x,
			pos_l.y + pos_r.y,
			pos_l.angle & pos_r.angle
		);
	}
	ShotVec operator-(ShotVec pos_l, ShotVec pos_r) {
		return ShotVec(
			pos_l.x - pos_r.x,
			pos_l.y - pos_r.y,
			pos_l.angle & pos_r.angle
		);
	}
	ShotVec operator+=(ShotVec &pos_l, ShotVec pos_r) {
		pos_l.x += pos_r.x;
		pos_l.y += pos_r.y;
		pos_l.angle &= pos_r.angle;
		return pos_l;
	}
	ShotVec operator-=(ShotVec &pos_l, ShotVec pos_r) {
		pos_l.x -= pos_r.x;
		pos_l.y -= pos_r.y;
		pos_l.angle &= pos_r.angle;
		return pos_l;
	}
}