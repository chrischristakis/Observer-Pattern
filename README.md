## Motivation

This is my take on the observer pattern in C++ which allows events to be sent from subject->observer and supports generic payloads to send context alongside events.

Here's an example of how to use it in a program:
```cpp
int main() {
  // ...

	UI* uiRef = new UI();
	Achievement* achievementRef = new Achievement();

	Player player;
	player.AddObserver(uiRef);
	player.AddObserver(achievementRef);

  // Observers will now respond to player notifications
	player.Damage(5);
	player.Damage(3);

	player.RemoveObserver(uiRef);
	player.RemoveObserver(achievementRef);
	player.RemoveObserver(uiRef); // error!

	// ...
}
```
Output:
```
[UI]: You have -5 health!
[ACHIEVEMENT]: You unlocked the 'Take damage' achievement!
[UI]: You have -8 health!
 * [INFO] Removed an observer
 * [INFO] Removed an observer
 * [WARNING] Cannot find observer to remove!
```

## The problem

In a game there is often a tight coupling between various systems that do not need to know about eachother. 

Consider this example:
```c++
class Player {
	void OnDamage(int dmg) {
		health -= dmg;
	
		audioRef->PlaySound("ouch.wav");
		cameraRef->Shake();
		uiRef->SetHealth(health);
		achievementsRef->UnlockDamagedAchivement();
		// ... etc...
	}
};
```
As you can see, the player taking damage needs to be aware of multiple systems that are unrelated to it (audio, camera, ui, achievements, etc)
Cluttering code with various unrelated systems can really reduce readability and lead into spaghetti as we pass in dependencies where they don't belong.

The observer pattern instead lets the player (subject) fire notifications that gets relayed to observers watching the subject, and decide whether they care about the event or not, and what to do with it.

By extending the Subject interface, this allows seperation of concerns, and turns the `OnDamage` method into:
```cpp
class Player: public Subject {
  void OnDamage(int dmg) {
      health -= dmg;
  		Notify(PlayerDamageEvent{ health, dmg });
  }
}
```
Which is much more readable and decouples components.

We can choose how an observer responds to this event by extending the Observer interface and choosing which events it cares about:
```cpp
public UI: public Observer {
  // Handle notification from subject
	void OnNotify(Event& e) override {
		switch (e.GetType()) {
			case EventType::PLAYER_DAMAGE_EVENT:

        // Cast to access generic payload
				if (PlayerDamageEvent* casted = EventCast<PlayerDamageEvent>(e))
					HealthPopUp(casted->health);

				break;
			case EventType::COIN_COLLECT_EVENT:

				if (CoinCollectEvent* casted = EventCast<CoinCollectEvent>(e))
					CoinPopUp(casted->coins);

				break;
		}
	}
}
```

Where events are simply delcared as:
```cpp
struct PlayerDamageEvent : public Event {
	int health;
	int damage;

	PlayerDamageEvent(int health, int damage): health(health), damage(damage) { }

	EventType GetType() const override {
		return EventType::PLAYER_DAMAGE_EVENT;
	}
};
```

Now, systems are isolated and manage their own concerns, communicating via notifications with data associated with them. This makes things like tutorials and achievements much easier and scalable
