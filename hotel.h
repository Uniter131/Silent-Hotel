#include <bangtal>
#include <random>
#include <string>

using namespace bangtal;
using namespace std;

enum Name {
    PUMPKIN = 1,
    GENTLEMAN,
    LADY,
    OLDMAN,
    CULTIST
};

enum Personality {
    사이코패스 = 1,
    거짓말쟁이,
    치밀한자,
    시체처리반,
    평화주의자
};

class CharacterManager {
private:
    int killerGuest;
    int x[10] = { 100, 500, 900, 250, 750, 170, 510, 930, 310, 800 };
    int y[10] = { 450, 450, 450, 150, 150, 460, 460, 460, 140, 140 };
    string imgName[5] = { "Pumpkin", "Gentleman", "Lady", "OldMan", "Cultist" };
    string persona[5] = { "사이코패스", "거짓말쟁이", "치밀한 자", "시체처리반", "평화주의자" };
public:
    int getX(int i) { return x[i]; };
    int getY(int i) { return y[i]; };
    string getImgName(int i) { return imgName[i]; };
    string getPersona(int i) { return persona[i]; };
    int getKillerGuest() { return killerGuest; };
    void setKillerGuest(int n) { killerGuest = n; };
} cm;

class Hotel {
public:
    int rooms[5][5] = { 0 };
};

class Guest : public Hotel {
private:
    int x, y, persona = 0;
    int roomNum;
    string imgName;
    bool isDead = false;
    bool isKiller = false;
public:
    ObjectPtr object;

    void Init(ScenePtr scene, int i);
    void PersonaInit(Guest g[], int index);
    void Receipt(Hotel** h, int gn);

    string getImgName() { return imgName; };
    int getRoomNum() { return this->roomNum; };
    int getPersona() { return this->persona; };
    bool getKiller() { return this->isKiller; };
    bool getIsDead() { return this->isDead; };

    void setRoomNum(int i) { roomNum = i; };
    void setKiller() { isKiller = true; };
    void setDead() { isDead = true; };
};

void Guest::Init(ScenePtr scene, int i) {
    this->x = cm.getX(i);
    this->y = cm.getY(i);
    this->imgName = cm.getImgName(i);
    this->object = Object::create("Images/" + imgName + ".png", scene, x, y);
}

void Guest::PersonaInit(Guest g[], int index) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 5);

    while (1) {
        int randNum = dis(gen);
        for (int i = 0; i < 5; i++) {
            if (g[i].persona == randNum)
                break;
            else if (g[i].persona != randNum && i == 4) {
                g[index].persona = randNum;
                printf("g[%d]: %d\n", index, randNum);
                return;
            }
        }
    }
}

void Guest::Receipt(Hotel** h, int gn) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 24);

    while (1) {
        int randNum = dis(gen);
        if ((**h).rooms[randNum / 5][randNum % 5] == 0) {
            (**h).rooms[randNum / 5][randNum % 5] = gn;
            printf("rooms[%d][%d]: %d\n", randNum / 5, randNum % 5, (**h).rooms[randNum / 5][randNum % 5]);
            this->roomNum = (randNum / 5 + 1) * 100 + (randNum % 5 + 1);
            break;
        }
    }
}

class GuestName {
private:
    int x, y;
    string imgName;
public:
    ObjectPtr objName;
    void Init(ScenePtr scene, int i);
};

void GuestName::Init(ScenePtr scene, int i) {
    this->x = cm.getX(i + 5);
    this->y = cm.getY(i + 5);
    this->imgName = cm.getImgName(i);
    this->objName = Object::create("Images/Name_" + imgName + ".png", scene, x, y);
}