#include "Observer.hpp"
#include <iostream>

int main() {

	UI* uiRef = new UI();
	Achievement* achievementRef = new Achievement();

	Player player;
	player.AddObserver(uiRef);
	player.AddObserver(achievementRef);

	player.Damage(5);
	player.Damage(3);

	player.RemoveObserver(uiRef);
	player.RemoveObserver(achievementRef);
	player.RemoveObserver(uiRef);

	delete uiRef;
	delete achievementRef;
	return 0;
}