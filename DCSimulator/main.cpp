//#include "Box2D/Box2D.h"
#include "dcurling_simulator.h"

#include <fstream>
#include <iostream>
#include <iomanip>

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
	ShotVec vec(-0.99074f, -29.559774f, false);

	const size_t traj_size = 4096;
	float trajectory[32 * traj_size];
	memset(trajectory, 0, sizeof(float) * 32 * traj_size);

	int steps = 
		digital_curling::b2simulator::Simulation(&gs, vec, 0, 0, nullptr, trajectory, traj_size);

	std::ofstream ofs("trajectory_log.txt");
	for (int i = 0; i < steps; i++) {
		for (unsigned int j = 0; j < 16; j++) {
			ofs <<
				std::fixed << std::setprecision(6) << trajectory[32 * i + 2 * j] << "," <<
				std::fixed << std::setprecision(6) << trajectory[32 * i + 2 * j + 1];
			if (j < 15) {
				ofs << ",";
			}
		}
		ofs << endl;
	}
}

int  main(void) {

	//operator_test();
	simuration_test();

	return 0;
}