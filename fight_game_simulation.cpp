#include <iostream>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <string>
#include <ctime>


enum class Status { Dead, Alive };


class Item {
protected:
    enum class Condition { Broken, New };
    enum class ItemType { None, Gun, ColdWeapon, Armour };

    Condition condition{ Condition::New };
    ItemType type{ ItemType::None };

public:
    Item() = default;
    ~Item() = default;

    [[nodiscard]] Condition GetCondition() const noexcept {
        return condition;
    }

    [[nodiscard]] std::string GetType() const noexcept {
        std::string typeArray[] = { "None", "Gun", "ColdWeapon", "Armour" };
        return typeArray[static_cast<int>(type)];
    }
};


class Armour : public Item {
public:
    explicit Armour(size_t _durability = 0) : durability(_durability) { type = ItemType::Armour; }
    Armour(Armour const& other) : durability(other.durability) { type = ItemType::Armour; }

    [[nodiscard]] constexpr size_t GetDurability() const noexcept {
        return durability;
    }
    void Damage(size_t amount) noexcept {
        if (condition != Condition::Broken) {
            durability -= amount;
        }
        if (durability <= 0) {
            condition = Condition::Broken;
        }
    }
private:
    size_t durability;
};


class Weapon : public Item {
public:
    Weapon() = default; virtual ~Weapon() = default;
    Weapon(size_t _damage, size_t _accuracy) : damage(_damage), accuracy(_accuracy) { }

    [[nodiscard]] constexpr size_t GetAccuracy() const noexcept {
        return accuracy;
    }
    static constexpr size_t GetMaxAccuracy() noexcept {
        return maxWeaponAccuracy;
    }

    virtual bool DoAttack() = 0;
    virtual void Upgrade(size_t) noexcept = 0;
    [[nodiscard]] virtual size_t GetDamage() const noexcept = 0;

protected:
    size_t damage{};
    size_t accuracy{};

    static constexpr int16_t maxWeaponAccuracy = 100;
};


class Entity {
public:
    Entity() = delete;

    virtual ~Entity() {
        delete weapon;
        weapon = nullptr;
    }

    explicit Entity(size_t _health, Weapon* _weapon = nullptr) :
        health(_health), weapon(_weapon) {
        entitiesCreated++;
    }

    Entity(Entity const& other) :
        health(other.health), weapon(other.weapon), status(other.status) {
        entitiesCreated++;
    }

    [[nodiscard]] Status GetStatus() const noexcept {
        return status;
    }

    [[nodiscard]] bool IsAlive() const noexcept {
        return this->status == Status::Alive;
    }

    [[nodiscard]] constexpr size_t GetHealth() const noexcept {
        return health;
    }

    [[nodiscard]] Weapon* GetWeapon() const noexcept {
        return weapon;
    }

    void SetWeapon(Weapon* const _weapon) noexcept {
        this->weapon = _weapon;
    }

    void SetHealth(size_t _health) noexcept {
        this->health = _health;
    }

    virtual void Damage(size_t amount) = 0;

    size_t Attack(Entity* enemy) {
        if (this == enemy) {
            throw std::logic_error("Entity cannot attack oneself");
        }
        if (this->status == Status::Dead || enemy->status == Status::Dead) {
            throw std::logic_error("Fight cannot involve dead entities");
        }

        const size_t weaponDamage = weapon->GetDamage();
        bool doHit = rand() % Weapon::GetMaxAccuracy() <= weapon->GetAccuracy();

        if (doHit) {
            if (weapon->DoAttack())
                enemy->Damage(weaponDamage);
        }
        return weaponDamage;
    }

    static size_t GetDeadEntitiesNumber() noexcept {
        return entitiesDied;
    }

    static size_t GetCreatedEntitiesNumber() noexcept {
        return entitiesCreated;
    }


protected:
    int health;
    Weapon* weapon;
    Status status{ Status::Alive };

    void KillEntity() noexcept {
        health = 0; status = Status::Dead;
        entitiesDied++;
    }

    static size_t entitiesDied;
    static size_t entitiesCreated;
};
size_t Entity::entitiesDied = 0;
size_t Entity::entitiesCreated = 0;


class Gun : public Weapon {
public:
    Gun() = delete;
    ~Gun() override = default;

    Gun(size_t _damage, size_t _accuracy,
        size_t critChance = 0, int critMultiplier = 1) :
            Weapon(_damage, _accuracy),
            criticalHitChance(critChance),
            criticalHitMultiplier(critMultiplier) {
        type = ItemType::Gun;
    }

    Gun(Gun const& other) :
            Weapon(other.damage, other.accuracy),
            criticalHitChance(other.criticalHitChance),
            criticalHitMultiplier(other.criticalHitMultiplier) {
        type = ItemType::Gun;
    }

    [[nodiscard]] size_t GetDamage() const noexcept override {
        bool isCritical = rand() % maxWeaponAccuracy <= criticalHitChance;
        return (isCritical) ? damage * criticalHitMultiplier : damage;
    }

    void Upgrade(size_t magazineCap) noexcept override {
        int newMagCap = this->magazineCapacity + magazineCap;
        if (newMagCap > maxMagazineCapacity) newMagCap = maxMagazineCapacity;
        this->magazineCapacity = newMagCap;
    }

    bool DoAttack() override {
        if (!this->bulletsInMagazine) {
            this->Reload();
            return false;
        }
        this->bulletsInMagazine--;
        return true;
    }

    void Reload() {
        bulletsInMagazine = magazineCapacity;
    }

protected:
    size_t criticalHitChance;
    int16_t criticalHitMultiplier;
    int16_t magazineCapacity{ 8 };
    int16_t bulletsInMagazine{ magazineCapacity };

    static constexpr int16_t maxMagazineCapacity = 24;
};


class ColdWeapon : public Weapon {
public:
    ColdWeapon() = delete;
    ~ColdWeapon() override = default;

    explicit ColdWeapon(size_t _damage) :
        Weapon(_damage, maxWeaponAccuracy) {
        type = ItemType::ColdWeapon;
    }

    ColdWeapon(ColdWeapon const& other) :
        Weapon(other.damage, maxWeaponAccuracy) {
        type = ItemType::ColdWeapon;
    }

    [[nodiscard]] constexpr size_t GetDamage() const noexcept override {
        return damage;
    }

    void Upgrade(size_t damageAmount) noexcept override {
        this->damage += damageAmount;
    }

    bool DoAttack() override {
        return true;
    }
};


class Mutant : public Entity {
public:
    Mutant() = delete;

    Mutant(int _health, Weapon* _weapon) :
        Entity(_health, _weapon) { }

    Mutant(Mutant const& other) :
        Entity(other.health, other.weapon) { }

    void Damage(size_t amount) override {
        if (status == Status::Alive) {
            health -= amount;
            if (health <= 0) KillEntity();
        }
    }
};


class Player : public Entity {
public:
    Player() = delete;

    Player(size_t _health, Weapon* _weapon, const Armour& _armor) :
        Entity(_health, _weapon), Armour(_armor) { }

    Player(size_t _health, Weapon* _weapon, size_t armorDurability) :
        Entity(_health, _weapon), Armour(armorDurability) { }

    Player(Player const& other) :
        Entity(other.health, other.weapon), Armour(other.Armour) { }

    void Damage(size_t damageAmount) override {
        if (status == Status::Alive) {

            if (Armour.GetDurability() >= damageAmount) {
                Armour.Damage(damageAmount);

            } else {
                size_t damageToPlayer = damageAmount - Armour.GetDurability();
                Armour.Damage(damageAmount - damageToPlayer);

                health -= damageToPlayer;
                if (health <= 0) KillEntity();
            }
        }
    }

private:
    Armour Armour;
};




std::atomic_bool menuThreadKillFlag = false;

class Fight {
private:
    struct FighterInfo {
        std::string name;
        Entity* participant;
        size_t dealtDamage{ 0 };
        size_t receivedDamage{ 0 };
        int16_t numberOfTurns{ 0 };

        FighterInfo() = default;
        ~FighterInfo() = default;

        explicit FighterInfo(Entity* entity) : participant(entity) { }

        friend std::ostream& operator<< (std::ostream& out, FighterInfo const& fightInfo) {
            out << "Name is: " << fightInfo.name << std::endl;
            out << "Dealt damage is: " << fightInfo.dealtDamage << std::endl;
            out << "Received damage is: " << fightInfo.receivedDamage << std::endl;
            out << "Hp left: " << fightInfo.participant->GetHealth() << std::endl;
            out << "Won by round: " << fightInfo.numberOfTurns << std::endl;
            out << "Fought with: " << (fightInfo.participant)->GetWeapon()->GetType() << std::endl;
            return out;
        }

        friend std::ifstream& operator>> (std::ifstream& in, FighterInfo& fighter) {
            std::string charInfo;
            std::getline(in, charInfo); EraseSubStr(charInfo, "Name is: ");
            fighter.name = charInfo;
            std::getline(in, charInfo); EraseSubStr(charInfo, "Dealt damage is: ");
            fighter.dealtDamage = std::stoi(charInfo);
            std::getline(in, charInfo); EraseSubStr(charInfo, "Received damage is: ");
            fighter.receivedDamage = std::stoi(charInfo);
            std::getline(in, charInfo); EraseSubStr(charInfo, "Hp left: ");
            fighter.participant->SetHealth(std::stoi(charInfo));
            return in;
        }

        static void EraseSubStr(std::string& str, std::string const& subStr) noexcept {
            size_t pos = str.find(subStr);
            if (pos != std::string::npos) {
                str.erase(pos, subStr.length());
            }
        }
    };

    void SimulateRound(int curRound) {
        size_t firstParticipantDamage =
                first.participant->Attack(second.participant);

        size_t secondParticipantDamage =
                second.participant->Attack(first.participant);

        first.dealtDamage += firstParticipantDamage;
        first.receivedDamage += secondParticipantDamage;

        second.dealtDamage += secondParticipantDamage;
        second.receivedDamage += firstParticipantDamage;

        first.numberOfTurns = second.numberOfTurns = curRound;
    }

    FighterInfo first;
    FighterInfo second;

    std::mutex mutex;
    std::condition_variable cv;

    bool isPaused{ false };
    bool isStopped{ false };

public:
    Fight() = delete;
    ~Fight() = default;

    Fight(Entity* _first, Entity* _second)
    : first(_first), second(_second) {
        first.name = "first"; second.name = "second";
    }

    void Resume() noexcept {
        std::unique_lock<std::mutex> uLock{ mutex };
        this->isPaused = false;
        cv.notify_one();
    }

    void Pause() noexcept {
        this->isPaused = true;
    }

    void Stop() noexcept {
        this->isStopped = true;
    }

    void ResumeFromFile(std::string const& filePath = "fight.log") noexcept(false) {
        std::ifstream fightLog{ filePath, std::ios_base::in };

        fightLog >> first;
        for (int i = 0; i < 3; ++i)
            fightLog.ignore(10000, '\n');
        fightLog >> second;

        fightLog.close();
        this->Resume();
    }

    void SaveToFile(std::string const& filePath = "fight.log") const noexcept(false) {
        std::ofstream fightLog{ filePath, std::ios_base::out | std::ios_base::trunc };
        fightLog << first << std::endl << second;
        fightLog.close();
    }

    void InitiateFight() {
        std::ofstream fightLog{ "fighter.log",
                                std::ios_base::out | std::ios_base::trunc };

        for (int curRound = 1; first.participant->IsAlive()
                                && second.participant->IsAlive(); ++curRound) {

            if (this->isStopped) break;

            if (this->isPaused) {
                std::ofstream _fightLog{ "fight.log",
                                        std::ios_base::out | std::ios_base::trunc };
                _fightLog << first << std::endl << second;
                _fightLog.close();
            }

            std::unique_lock<std::mutex> uLock{ mutex };
            cv.wait(uLock, [this] { return !isPaused; });

            this->SimulateRound(curRound);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
        fightLog << (first.participant->GetHealth() >=
            second.participant->GetHealth() ? first : second);

        menuThreadKillFlag = true;
        fightLog.close();
    }
};


void FightMenu(Fight* fight) {
    int choice = 0;
    bool hasStarted = false;
    std::thread* thread = nullptr;

    if (!fight)
        throw std::invalid_argument("No fight was engaged");

    do {
        if (menuThreadKillFlag) {
            if (thread) thread->detach();
            break;
        }

        std::cout << "1 - Start, 2 - Pause, 3 - Resume, 4 - Resume from File, 5 - Save to File, 0 - Stop: ";
        std::cin >> choice;

        if (!hasStarted && choice != 1) {
            std::cout << "Fight hasn't been started yet" << std::endl;
            continue;
        }

        switch (choice) {
            case 1:
                thread = new std::thread{ &Fight::InitiateFight, std::ref(*fight) };
                hasStarted = true;
                break;
            case 2:
                fight->Pause();
                std::cout << "Fight Paused" << std::endl;
                break;
            case 3:
                fight->Resume();
                std::cout << "Fight Resumed" << std::endl;
                break;
            case 4:
                fight->ResumeFromFile();
                std::cout << "Fight Resumed" << std::endl;
                break;
            case 5:
                fight->SaveToFile();
                std::cout << "Fight data succesfully saved" << std::endl;
                choice = 0;
            case 0:
                fight->Stop();
                if (thread) thread->detach();
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                std::cout << "Fight Ended" << std::endl;
                break;
            default:
                break;
        }

    } while (choice);

    delete thread;
}


int main(int argc, char** argv) {

    srand(time(NULL));

    Entity* player = new Player{ 500, new ColdWeapon(60), Armour(45) };
    Entity* mutant = new Mutant{ 500, new Gun(100, 75) };

    std::thread fMenu{ FightMenu, new Fight{player, mutant} };
    fMenu.join();

    delete player;
    delete mutant;

    std::cin.get();
    return 0;
}
