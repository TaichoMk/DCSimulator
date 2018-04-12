// Constructors and operators in dcurling_simulator.h
#include "dcurling_simulator.h"

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