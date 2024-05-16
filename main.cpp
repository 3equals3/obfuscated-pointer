#include <iostream>
#include "pointer.h"

class Player {
public:
	int health;
	int armor;
public:
	Player(int health, int armor) : health(health), armor(armor) {};
	void hurt() {
		health--;
	};
	void heal() {
		health++;
	};
};

class GameData;
eq3::encrypted_pointer<GameData> game_data;

class GameData {
public:
	eq3::encrypted_pointer<Player> local_player;
public:
	static eq3::encrypted_pointer<GameData> getGameData();
	inline eq3::encrypted_pointer<Player> getLocalPlayer() {
	    return local_player;
	}
};
eq3::encrypted_pointer<GameData> GameData::getGameData() { // цю хуйню надо отдельно выделять потому что компилятор оптимизирует и все идет по бороде и inline ruining
	static volatile bool flag = false;
	if (!flag || game_data == nullptr) {
		flag = true;
		game_data = new GameData();
	}
	return game_data;
}

void idontgiveafuck(eq3::encrypted_pointer<GameData> pon) {
  auto g = GameData::getGameData();
	if (g == pon.get()) {
		auto player = g->getLocalPlayer();
		if (player->health < 10)
	        player->heal(); // never die lol
	}
}

int main() {
	auto da = GameData::getGameData(); // this f****t perceived it as inline simply because the definition of return was in the class and not separately
	auto p = new Player(100, 100);
  game_data->local_player = p;
	idontgiveafuck(da);
	while (game_data->getLocalPlayer()->health > 0) {
		std::cout << "Health: " << game_data->getLocalPlayer()->health << std::endl;
		game_data->getLocalPlayer()->hurt();
		idontgiveafuck(da);
		Sleep(50);
	}
	std::cout << "Player died!";
	return 0;
}
