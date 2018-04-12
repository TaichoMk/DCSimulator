//#include "Box2D/Box2D.h"
#include "dcurling_simulator.h"

using digital_curling::simulator::ShotPos;

#include <iostream>

int  main(void) {

	ShotPos pos1(5.0f, 1.0f, false);
	ShotPos pos2(2.0f, -2.0f, false);

	ShotPos pos_add = pos1 + pos2;

	std::cout << "pos_add.x = " << pos_add.x << std::endl;
	std::cout << "pos_add.y = " << pos_add.y << std::endl;
	std::cout << "pos_add.angle = " << pos_add.angle << std::endl;

	ShotPos pos_sub = pos1 - pos2;
	std::cout << "pos_sub.x = " << pos_sub.x << std::endl;
	std::cout << "pos_sub.y = " << pos_sub.y << std::endl;
	std::cout << "pos_sub.angle = " << pos_sub.angle << std::endl;

	pos_add += pos1;
	std::cout << "pos_add.x = " << pos_add.x << std::endl;
	std::cout << "pos_add.y = " << pos_add.y << std::endl;
	std::cout << "pos_add.angle = " << pos_add.angle << std::endl;

	return 0;
}