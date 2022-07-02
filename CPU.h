#pragma once

class CPU {
	// Singleton Class
	static CPU* instance;
	CPU() {}

private:

public:
	// Singleton Class
	static CPU* getInstance() {
		if (!instance) instance = new CPU;
		return instance;
	}

	void test() {
		std::cout << "CPU OK\n";
	}

};
