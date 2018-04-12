//#include "Box2D/Box2D.h"
#include "dcurling_simulator.h"

#include <iostream>

using digital_curling::GameState;
using digital_curling::ShotPos;
using digital_curling::ShotVec;

using std::cout;
using std::endl;

void operator_test() {
	ShotPos pos1(5.0f, 1.0f, false);
	ShotPos pos2(2.0f, -2.0f, false);

	ShotPos pos_add = pos1 + pos2;

	cout << "pos_add.x = " << pos_add.x << endl;
	cout << "pos_add.y = " << pos_add.y << endl;
	cout << "pos_add.angle = " << pos_add.angle << endl;

	ShotPos pos_sub = pos1 - pos2;
	cout << "pos_sub.x = " << pos_sub.x << endl;
	cout << "pos_sub.y = " << pos_sub.y << endl;
	cout << "pos_sub.angle = " << pos_sub.angle << endl;

	pos_add += pos1;
	cout << "pos_add.x = " << pos_add.x << endl;
	cout << "pos_add.y = " << pos_add.y << endl;
	cout << "pos_add.angle = " << pos_add.angle << endl;
}

void simuration_test() {
	GameState gs(8);
	ShotVec vec(0, 0, false);


}

int  main(void) {

	operator_test();

	return 0;
}