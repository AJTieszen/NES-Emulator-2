#pragma once

class MemMap {
	// Singleton Class
	static MemMap* instance;
	MemMap() {}

private:

public:
	// Singleton Class
	static MemMap* getInstance() {
		if (!instance) instance = new MemMap;
		return instance;
	}

	void test() {
		std::cout << "Memory Map OK\n";
	}
};
