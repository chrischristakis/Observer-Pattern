#pragma once
#include <vector>
#include <iostream>
#include <algorithm>

#define LOG_WARN(msg) std::cout << " * [WARNING] " << msg << std::endl;
#define LOG_INFO(msg) std::cout << " * [INFO] " << msg << std::endl;

enum class EventType {
	PLAYER_DAMAGE_EVENT,
	COIN_COLLECT_EVENT
};

class Event {
public:
	virtual EventType GetType() const = 0;
	virtual ~Event() = default;
};

struct PlayerDamageEvent : public Event {
	int health;
	int damage;

	PlayerDamageEvent(int health, int damage): health(health), damage(damage) { }

	EventType GetType() const override {
		return EventType::PLAYER_DAMAGE_EVENT;
	}
};

struct CoinCollectEvent : public Event {
	int coins;

	CoinCollectEvent(int coins): coins(coins) { }

	EventType GetType() const override {
		return EventType::COIN_COLLECT_EVENT;
	}
};

class Observer {
public:
	virtual void OnNotify(Event& e) = 0;
	virtual ~Observer() = default;

	template <typename T>
	static T* EventCast(Event& e) {
		T* eventPtr = dynamic_cast<T*>(&e);

		// Log failure here.
		if (!eventPtr)
			LOG_WARN("Event cast failed for event");

		return eventPtr;
	}
};

class Subject {
public:

	void Notify(Event&& e) {
		for (Observer* observer : m_observers)
			observer->OnNotify(e); // Notify all observers of the event that occurred
	}

	void AddObserver(Observer* observer) {
		m_observers.push_back(observer);
	}

	void RemoveObserver(Observer* observer) {
		auto found = std::find(m_observers.begin(), m_observers.end(), observer);
		if (found != m_observers.end()) {
			m_observers.erase(found);
			LOG_INFO("Removed an observer");
		}
		else
			LOG_WARN("Cannot find observer to remove!");
	}

protected:

	Subject() = default;

private:

	std::vector<Observer*> m_observers;

};

// ----------- Implementation subject/observer test -------------- //

// Subject
class Player: public Subject {
public:

	int health;
	int coins;
	

	void Damage(int dmg) {
		health -= dmg;
		Notify(PlayerDamageEvent{ health, dmg });
	}

	void CollectCoins(int amt) {
		coins += amt;
		Notify(CoinCollectEvent{ amt });
	}

};

// Observer
class UI: public Observer {
public:

	void HealthPopUp(int health) {
		std::cout << "[UI]: You have " << health << " health!" << std::endl;
	}

	void CoinPopUp(int amount) {
		std::cout << "[UI]: You got " << amount << " coins!" << std::endl;
	}

	// Handle notification from subject
	void OnNotify(Event& e) override {
		switch (e.GetType()) {
			case EventType::PLAYER_DAMAGE_EVENT:

				if (PlayerDamageEvent* casted = EventCast<PlayerDamageEvent>(e))
					HealthPopUp(casted->health);

				break;
			case EventType::COIN_COLLECT_EVENT:

				if (CoinCollectEvent* casted = EventCast<CoinCollectEvent>(e))
					CoinPopUp(casted->coins);

				break;
		}
	}
};

class Achievement: public Observer {

	bool unlockedDamageAchievement = false;

	void OnNotify(Event& e) override {
		switch (e.GetType()) {
			case EventType::PLAYER_DAMAGE_EVENT:
				if (unlockedDamageAchievement)
					return;

				// We don't need to get achievement data with cast here, simply checking for existence is good enough!
				std::cout << "[ACHIEVEMENT]: You unlocked the 'Take damage' achievement!" << std::endl;
				unlockedDamageAchievement = true;

				break;
		}
	}
};