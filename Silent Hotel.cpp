#include <bangtal>
#include <random>
#include <string>
#include "hotel.h"

using namespace bangtal;
using namespace std;

ScenePtr startScene;
ScenePtr counterScene;
ScenePtr callScene;
ScenePtr questionScene;
ScenePtr elevatorScene;
ScenePtr roomScene;
ScenePtr reportScene;
ScenePtr infoScene1;
ScenePtr infoScene2;
ScenePtr reasonScene;
ScenePtr gameoverScene;

ObjectPtr startButton;
ObjectPtr exitButton;
ObjectPtr title;
ObjectPtr win;
ObjectPtr lose;
ObjectPtr killer;

ObjectPtr manager;
ObjectPtr phone;
ObjectPtr elevator;
ObjectPtr desk;
ObjectPtr callingText;
ObjectPtr visitText;
ObjectPtr reasoningText;

ObjectPtr floorQuestion;
ObjectPtr numberQuestion;
ObjectPtr personaQuestion;
ObjectPtr call_check;
ObjectPtr room_check;
ObjectPtr answer;

ObjectPtr room[5][5];
ObjectPtr f[5];
ObjectPtr num[5];

ObjectPtr tomorrow;
ObjectPtr deadReport;
ObjectPtr buttonReturn;
ObjectPtr info;
ObjectPtr nextInfo;
ObjectPtr prevInfo;

bool isWin = false;

void RoomInit() {
    for (int i = 0; i < 5; i++) {
        for (int k = 0; k < 5; k++) {
            room[i][k] = Object::create("Images/Door.png", elevatorScene, 300 + (k * 130), 100 + (i * 115));
            f[k] = Object::create("Images/" + to_string(k + 1) + "F.png", elevatorScene, 160, 120 + (k * 115));
            num[k] = Object::create("Images/" + to_string(k + 1) + "호.png", elevatorScene, 320 + (k * 130), 700);
        }
    }
}

void Init() {
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
    setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

    startScene = Scene::create("", "Images/Hotel.png");
    counterScene = Scene::create("", "Images/Counter.png");
    callScene = Scene::create("", "Images/Counter.png");
    questionScene = Scene::create("", "Images/Counter.png");
    reportScene = Scene::create("", "Images/Counter.png");
    infoScene1 = Scene::create("", "Images/InfoPaper1.png");
    infoScene2 = Scene::create("", "Images/InfoPaper2.png");
    reasonScene = Scene::create("", "Images/Counter.png");
    elevatorScene = Scene::create("", "Images/Background.png");
    roomScene = Scene::create("", "Images/Room.png");
    gameoverScene = Scene::create("", "Images/NightHotel.png");

    startButton = Object::create("Images/Start.png", startScene, 500, 320);
    exitButton = Object::create("Images/Exit.png", startScene, 560, 200);
    title = Object::create("Images/Title.png", startScene, 260, 420);
    buttonReturn = Object::create("Images/Return.png", reportScene, 50, 50);

    manager = Object::create("Images/Manager.png", counterScene, 620, 250);
    phone = Object::create("Images/Phone.png", counterScene, 570, 380);
    elevator = Object::create("Images/Elevator.png", counterScene, -100, 0);
    desk = Object::create("Images/Desk.png", counterScene, 330, 0);
    callingText = Object::create("Images/Calling.png", counterScene, 580, 370);
    visitText = Object::create("Images/Visit.png", counterScene, 95, 320);
    reasoningText = Object::create("Images/Reasoning.png", counterScene, 825, 650);
    deadReport = Object::create("Images/DeadReport.png", counterScene, 1160, 80);
    tomorrow = Object::create("Images/다음날로.png", counterScene, 60, 700, false);
    info = Object::create("Images/Info.png", counterScene, 1160, 680);

    nextInfo = Object::create("Images/NextInfo.png", infoScene1, 800, 640);
    prevInfo = Object::create("Images/PrevInfo.png", infoScene2, 800, 640);

    floorQuestion = Object::create("Images/FloorQuestion.png", questionScene, 50, 600);
    numberQuestion = Object::create("Images/NumberQuestion.png", questionScene, 50, 450);
    personaQuestion = Object::create("Images/PersonaQuestion.png", questionScene, 50, 300);
    answer = Object::create("Images/1.png", questionScene, 950, 570, false);
    call_check = Object::create("Images/Check.png", questionScene, 50, 100, false);
    room_check = Object::create("Images/Check.png", roomScene, 1150, 300, false);

    RoomInit();
}

void KillerSetting(Guest g[]) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 4);

    while (1) {
        int randNum = dis(gen);
        if (g[randNum].getPersona() != 평화주의자) {
            cm.setKillerGuest(randNum);
            g[randNum].setKiller();
            break;
        }
    }
}

void GuestInit(Hotel* h, Guest g[], GuestName gn[]) {
    for (int i = PUMPKIN; i <= CULTIST; i++) {
        g[i - 1].Init(callScene, i - 1);
        g[i - 1].PersonaInit(g, i - 1);
        g[i - 1].Receipt(&h, i);
        gn[i - 1].Init(callScene, i - 1);
    }
}

int CheckRoom(Hotel* h, Guest g[], int f, int num, bool isReason) {
    int guestNum = h->rooms[f][num];
    if (isReason) {
        if (guestNum != cm.getKillerGuest() + 1) {
            isWin = false;
            showMessage(to_string((f + 1) * 100 + (num + 1)) + "호에는 범인이 묵고있지 않습니다. 호텔 규정에 따라 당신의 출입이 거부됩니다.");
            gameoverScene->enter();
        }
        else {
            isWin = true;
            g[guestNum - 1].object->locate(roomScene, 100, 100);
            g[guestNum - 1].object->setScale(2.f);
            showMessage("범인을 찾았습니다! 호텔 규정에 따라 범인은 제거될 것입니다.");
            gameoverScene->enter();
        }
    }
    else if (guestNum != 0) {
        g[guestNum - 1].object->locate(roomScene, 100, 100);
        g[guestNum - 1].object->setScale(2.f);
        showMessage(to_string((f + 1) * 100 + (num + 1)) + "호에는 " + g[guestNum - 1].getImgName() + "(이)가 묵고있습니다.");
    }
    else {
        showMessage(to_string((f + 1) * 100 + (num + 1)) + "호에는 아무도 묵고있지 않습니다.");
    }
    return guestNum;
}

Guest NightKill(Hotel* h, Guest g[], GuestName gn[]) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 4);
    int randNum;
    while (1) {
        randNum = dis(gen);
        if (!g[randNum].getKiller() && !g[randNum].getIsDead()) {
            g[randNum].object->setImage("Images/Dead" + g[randNum].getImgName() + ".png");
            g[randNum].object->locate(reportScene, cm.getX(randNum), cm.getY(randNum));
            gn[randNum].objName->locate(reportScene, cm.getX(randNum + 5), cm.getY(randNum + 5));
            g[randNum].setDead();
            return g[randNum];
        }
    }
}

void Lie(Guest* g, int qn) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 5);
    int rn = g->getRoomNum();

    while (1) {
        int randNum = dis(gen);
        switch (qn)
        {
        case 1:
            if (rn /= 100 != randNum) {
                answer->setImage("Images/" + to_string(randNum) + ".png");
                return;
            }
            break;
        case 2:
            if (rn %= 100 != randNum) {
                answer->setImage("Images/" + to_string(randNum) + ".png");
                return;
            }
            break;
        case 3:
            if (g->getPersona() != randNum) {
                answer->setImage("Images/" + cm.getPersona(randNum - 1) + ".png");
                return;
            }
            break;
        }
    }
}

void disposal(Guest* g, int p) {
    if (p == 치밀한자) {
        (g->getRoomNum() % 100 != 5 ? showMessage(to_string(g->getRoomNum() + 1) + "호에서 발견됨. 성격은 " + cm.getPersona(g->getPersona() - 1) + ".")
            : showMessage(to_string(g->getRoomNum() - 1) + "호에서 발견됨. 성격은 " + cm.getPersona(g->getPersona() - 1) + "."));
    }
    else {
        (g->getRoomNum() / 100 != 5 ? showMessage(to_string(g->getRoomNum() + 100) + "호에서 발견됨. 성격은 " + cm.getPersona(g->getPersona() - 1) + ".")
            : showMessage(to_string(g->getRoomNum() - 100) + "호에서 발견됨. 성격은 " + cm.getPersona(g->getPersona() - 1) + "."));
    }
}

void Reasoning(int gn) {
    if (gn != (cm.getKillerGuest() + 1)) {
        isWin = false;
        showMessage(cm.getImgName(gn - 1) + "(은)는 범인이 아닙니다. 호텔 규정에 따라, 당신의 출입이 거부됩니다.");
        gameoverScene->enter();
    }
    else {
        elevatorScene->enter();
        showMessage(cm.getImgName(gn - 1) + "(이)가 묵고있는 방을 고르십시오.\n만약 모른다면, 규정에 따라 호텔 출입이 거부됩니다.");
    }
}

int main()
{
    ScenePtr prevScene;
    int selectedObj = 0, day = 1, index;
    bool isCalled = false, isVisited = false, isReasoning = false, isReported = false;
    Init();
    Hotel hotel;
    Guest guests[5];
    GuestName guestNames[5];
    GuestInit(&hotel, guests, guestNames);
    KillerSetting(guests);

    guests[PUMPKIN - 1].object->setOnMouseCallback([&](auto obj, auto x, auto y, auto action)->bool {
        if (prevScene == callScene || isReported || isReasoning) {
            if (isReasoning) {
                Reasoning(PUMPKIN);
            }
            else if (!guests[PUMPKIN - 1].getIsDead()) {
                questionScene->enter();
                selectedObj = PUMPKIN;
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 치밀한자 || guests[cm.getKillerGuest()].getPersona() == 시체처리반) {
                disposal(&guests[PUMPKIN - 1], guests[cm.getKillerGuest()].getPersona());
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 사이코패스) {
                int temp = guests[cm.getKillerGuest()].getRoomNum();
                guests[cm.getKillerGuest()].setRoomNum(guests[PUMPKIN - 1].getRoomNum());
                showMessage(to_string(temp) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[PUMPKIN - 1].getPersona() - 1) + ".");
            }
            else {
                if (guests[PUMPKIN - 1].getIsDead()) {
                    obj->locate(reportScene, cm.getX(PUMPKIN - 1), cm.getY(PUMPKIN - 1));
                }
                showMessage(to_string(guests[PUMPKIN - 1].getRoomNum()) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[PUMPKIN - 1].getPersona() - 1) + ".");
            }
            return true;
        }
    });

    guests[GENTLEMAN - 1].object->setOnMouseCallback([&](auto obj, auto x, auto y, auto action)->bool {
        if (prevScene == callScene || isReported || isReasoning) {
            if (isReasoning) {
                Reasoning(GENTLEMAN);
            }
            else if (!guests[GENTLEMAN - 1].getIsDead()) {
                questionScene->enter();
                selectedObj = GENTLEMAN;
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 치밀한자 || guests[cm.getKillerGuest()].getPersona() == 시체처리반) {
                disposal(&guests[GENTLEMAN - 1], guests[cm.getKillerGuest()].getPersona());
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 사이코패스) {
                int temp = guests[cm.getKillerGuest()].getRoomNum();
                guests[cm.getKillerGuest()].setRoomNum(guests[GENTLEMAN - 1].getRoomNum());
                showMessage(to_string(temp) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[GENTLEMAN - 1].getPersona() - 1) + ".");
            }
            else {
                if (guests[GENTLEMAN - 1].getIsDead()) {
                    obj->locate(reportScene, cm.getX(GENTLEMAN - 1), cm.getY(GENTLEMAN - 1));
                }
                showMessage(to_string(guests[GENTLEMAN - 1].getRoomNum()) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[GENTLEMAN - 1].getPersona() - 1) + ".");
            }
            return true;
        }
    });

    guests[LADY - 1].object->setOnMouseCallback([&](auto obj, auto x, auto y, auto action)->bool {
        if (prevScene == callScene || isReported || isReasoning) {
            if (isReasoning) {
                Reasoning(LADY);
            }
            else if (!guests[LADY - 1].getIsDead()) {
                questionScene->enter();
                selectedObj = LADY;
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 치밀한자 || guests[cm.getKillerGuest()].getPersona() == 시체처리반) {
                disposal(&guests[LADY - 1], guests[cm.getKillerGuest()].getPersona());
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 사이코패스) {
                int temp = guests[cm.getKillerGuest()].getRoomNum();
                guests[cm.getKillerGuest()].setRoomNum(guests[LADY - 1].getRoomNum());
                showMessage(to_string(temp) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[LADY - 1].getPersona() - 1) + ".");
            }
            else {
                if (guests[LADY - 1].getIsDead()) {
                    obj->locate(reportScene, cm.getX(LADY - 1), cm.getY(LADY - 1));
                }
                showMessage(to_string(guests[LADY - 1].getRoomNum()) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[LADY - 1].getPersona() - 1) + ".");
            }
            return true;
        }
    });

    guests[OLDMAN - 1].object->setOnMouseCallback([&](auto obj, auto x, auto y, auto action)->bool {
        if (prevScene == callScene || isReported || isReasoning) {
            if (isReasoning) {
                Reasoning(OLDMAN);
            }
            else if (!guests[OLDMAN - 1].getIsDead()) {
                questionScene->enter();
                selectedObj = OLDMAN;
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 치밀한자 || guests[cm.getKillerGuest()].getPersona() == 시체처리반) {
                disposal(&guests[OLDMAN - 1], guests[cm.getKillerGuest()].getPersona());
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 사이코패스) {
                int temp = guests[cm.getKillerGuest()].getRoomNum();
                guests[cm.getKillerGuest()].setRoomNum(guests[OLDMAN - 1].getRoomNum());
                showMessage(to_string(temp) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[OLDMAN - 1].getPersona() - 1) + ".");
            }
            else {
                if (guests[OLDMAN - 1].getIsDead()) {
                    obj->locate(reportScene, cm.getX(OLDMAN - 1), cm.getY(OLDMAN - 1));
                }
                showMessage(to_string(guests[OLDMAN - 1].getRoomNum()) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[OLDMAN - 1].getPersona() - 1) + ".");
            }
            return true;
        }
    });

    guests[CULTIST - 1].object->setOnMouseCallback([&](auto obj, auto x, auto y, auto action)->bool {
        if (prevScene == callScene || isReported || isReasoning) {
            if (isReasoning) {
                Reasoning(CULTIST);
            }
            else if (!guests[CULTIST - 1].getIsDead()) {
                questionScene->enter();
                selectedObj = CULTIST;
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 치밀한자 || guests[cm.getKillerGuest()].getPersona() == 시체처리반) {
                disposal(&guests[CULTIST - 1], guests[cm.getKillerGuest()].getPersona());
            }
            else if (guests[cm.getKillerGuest()].getPersona() == 사이코패스) {
                int temp = guests[cm.getKillerGuest()].getRoomNum();
                guests[cm.getKillerGuest()].setRoomNum(guests[CULTIST - 1].getRoomNum());
                showMessage(to_string(temp) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[CULTIST - 1].getPersona() - 1) + ".");
            }
            else {
                if (guests[CULTIST - 1].getIsDead()) {
                    obj->locate(reportScene, cm.getX(CULTIST - 1), cm.getY(CULTIST - 1));
                }
                showMessage(to_string(guests[CULTIST - 1].getRoomNum()) + "호에서 발견됨. 성격은 " + cm.getPersona(guests[CULTIST - 1].getPersona() - 1) + ".");
            }
            return true;
        }
        });

    room[0][0]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 0, 0, isReasoning);
        return true;
        });

    room[0][1]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 0, 1, isReasoning);
        return true;
        });

    room[0][2]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 0, 2, isReasoning);
        return true;
        });

    room[0][3]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 0, 3, isReasoning);
        return true;
        });

    room[0][4]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 0, 4, isReasoning);
        return true;
        });

    room[1][0]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 1, 0, isReasoning);
        return true;
        });

    room[1][1]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 1, 1, isReasoning);
        return true;
        });

    room[1][2]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 1, 2, isReasoning);
        return true;
        });

    room[1][3]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 1, 3, isReasoning);
        return true;
        });

    room[1][4]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 1, 4, isReasoning);
        return true;
        });

    room[2][0]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 2, 0, isReasoning);
        return true;
        });

    room[2][1]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 2, 1, isReasoning);
        return true;
        });

    room[2][2]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 2, 2, isReasoning);
        return true;
        });

    room[2][3]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 2, 3, isReasoning);
        return true;
        });

    room[2][4]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 2, 4, isReasoning);
        return true;
        });

    room[3][0]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 3, 0, isReasoning);
        return true;
        });

    room[3][1]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 3, 1, isReasoning);
        return true;
        });

    room[3][2]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 3, 2, isReasoning);
        return true;
        });

    room[3][3]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 3, 3, isReasoning);
        return true;
        });

    room[3][4]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 3, 4, isReasoning);
        return true;
        });

    room[4][0]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 4, 0, isReasoning);
        return true;
        });

    room[4][1]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 4, 1, isReasoning);
        return true;
        });

    room[4][2]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 4, 2, isReasoning);
        return true;
        });

    room[4][3]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 4, 3, isReasoning);
        return true;
        });

    room[4][4]->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        roomScene->enter();
        index = CheckRoom(&hotel, guests, 4, 4, isReasoning);
        return true;
        });

    startButton->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        Guest killedGuest = NightKill(&hotel, guests, guestNames);
        showMessage(killedGuest.getImgName() + "(이)가 살해당했습니다.\n리포트를 확인하세요.");
        counterScene->enter();
        return true;
        });

    exitButton->setOnMouseCallback([](auto object, auto x, auto y, auto action)->bool {
        endGame();
        return true;
        });

    manager->setOnMouseCallback([](auto object, auto x, auto y, auto action)->bool {
        reasonScene->enter();
        return true;
        });

    phone->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        if (!isCalled) {
            callScene->enter();
        }
        else showMessage("전화면담은 하루에 한 번만 가능합니다.");
        return true;
        });

    elevator->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        if (!isVisited) {
            elevatorScene->enter();
        }
        return true;
        });

    floorQuestion->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        numberQuestion->hide();
        personaQuestion->hide();
        if (guests[selectedObj - 1].getPersona() != 거짓말쟁이) {
            answer->setImage("Images/" + to_string(guests[selectedObj - 1].getRoomNum() / 100) + ".png");
        }
        else Lie(&guests[selectedObj - 1], 1);
        answer->show();
        call_check->show();
        return true;
        });

    numberQuestion->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        floorQuestion->hide();
        personaQuestion->hide();
        if (guests[selectedObj - 1].getPersona() != 거짓말쟁이) {
            answer->setImage("Images/" + to_string(guests[selectedObj - 1].getRoomNum() % 100) + ".png");
        }
        else Lie(&guests[selectedObj - 1], 2);
        answer->show();
        call_check->show();
        return true;
        });

    personaQuestion->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        floorQuestion->hide();
        numberQuestion->hide();
        if (guests[selectedObj - 1].getPersona() != 거짓말쟁이) {
            answer->setImage("Images/" + cm.getPersona(guests[selectedObj - 1].getPersona() - 1) + ".png");
        }
        else Lie(&guests[selectedObj - 1], 3);
        answer->show();
        call_check->show();
        return true;
        });

    call_check->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        floorQuestion->show();
        numberQuestion->show();
        personaQuestion->show();
        answer->hide();
        counterScene->enter();
        return true;
        });

    room_check->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        counterScene->enter();
        if (index != 0) {
            guests[index - 1].object->setScale(1.f);
            if (!guests[index - 1].getIsDead()) {
                guests[index - 1].object->locate(callScene, cm.getX(index - 1), cm.getY(index - 1));
            }
            else {
                guests[index - 1].object->locate(reportScene, cm.getX(index - 1), cm.getY(index - 1));
            }
        }
        return true;
        });

    tomorrow->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        isCalled = false; isVisited = false;
        tomorrow->hide();

        if (++day >= 7) {
            gameoverScene->enter();
            showMessage("범인을 제외한 모든 게스트가 살해당했습니다.\n호텔 규정에 따라 당신의 출입이 거부됩니다.");
        }
        else if (day % 2) {
            Guest killedGuest = NightKill(&hotel, guests, guestNames);
            counterScene->enter();
            showMessage(killedGuest.getImgName() + "(이)가 살해당했습니다.\n리포트를 확인하세요.");
        }
        else {
            counterScene->enter();
            showMessage("오늘은 아무도 죽지 않았습니다. 조사를 시작하세요.");
        }
        return true;
        });

    deadReport->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        reportScene->enter();
        return true;
        });

    info->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        infoScene1->enter();
        return true;
        });

    nextInfo->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        infoScene2->enter();
        return true;
        });

    prevInfo->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        infoScene1->enter();
        return true;
        });

    buttonReturn->setOnMouseCallback([&](auto object, auto x, auto y, auto action)->bool {
        isReasoning = false; isReported = false;
        for (int i = PUMPKIN; i <= CULTIST; i++) {
            if (!guests[i - 1].getIsDead()) {
                guests[i - 1].object->locate(callScene, cm.getX(i - 1), cm.getY(i - 1));
                guestNames[i - 1].objName->locate(callScene, cm.getX(i + 4), cm.getY(i + 4));
            }
        }
        prevScene->enter();
        return true;
        });

    counterScene->setOnEnterCallback([&](auto scene)->bool {
        prevScene = scene;
        deadReport->locate(scene, 1160, 80);
        manager->locate(scene, 620, 250);
        phone->locate(scene, 570, 380);
        elevator->locate(scene, -100, 0);
        desk->locate(scene, 330, 0);

        if (isCalled && isVisited) {
            showMessage("오늘의 조사를 마쳤습니다. 밤을 기다리세요.");
            tomorrow->show();
        }
        else if (isCalled) {
            showMessage("오늘은 더이상 전화할 수 없습니다.\n 호텔 방을 직접 방문하여 정보를 얻으세요.");
        }
        else if (isVisited) {
            showMessage("오늘은 더이상 방문할 수 없습니다.\n 손님에게 전화하여 정보를 얻으세요.");
        }
        return true;
        });

    callScene->setOnEnterCallback([&](auto scene)->bool {
        prevScene = scene;
        deadReport->locate(scene, 1160, 80);
        isCalled = true;
        showMessage("전화를 통해 손님의 정보를 알 수 있습니다.\n질문할 손님을 고르세요.");
        return true;
        });

    questionScene->setOnEnterCallback([&](auto scene)->bool {
        prevScene = scene;
        deadReport->locate(scene, 1160, 80);
        manager->locate(questionScene, 620, 250);
        desk->locate(questionScene, 330, 0);
        phone->locate(questionScene, 570, 380);
        showMessage("호텔 규정상 지배인은 손님의 정보를 많이 제공할 수 없습니다.\n신중하게 질문을 고르세요.");
        return true;
        });

    elevatorScene->setOnEnterCallback([&](auto scene)->bool {
        prevScene = scene;
        deadReport->locate(scene, 1160, 80);
        isVisited = true;
        showMessage("방문을 클릭해 방을 조사할 수 있습니다.");
        return true;
        });

    roomScene->setOnEnterCallback([&](auto scene)->bool {
        prevScene = scene;
        deadReport->locate(scene, 1160, 80);
        room_check->show();
        return true;
        });

    reportScene->setOnEnterCallback([&](auto scene)->bool {
        isReported = true;
        buttonReturn->locate(scene, 50, 50);
        return true;
        });

    infoScene1->setOnEnterCallback([&](auto scene)->bool {
        buttonReturn->locate(scene, 50, 50);
        return true;
        });

    infoScene2->setOnEnterCallback([&](auto scene)->bool {
        buttonReturn->locate(scene, 50, 50);
        return true;
        });

    reasonScene->setOnEnterCallback([&](auto scene)->bool {
        isReasoning = true;
        deadReport->locate(scene, 1160, 80);
        buttonReturn->locate(scene, 50, 50);

        for (int i = PUMPKIN; i <= CULTIST; i++) {
            if (!guests[i - 1].getIsDead()) {
                guests[i - 1].object->locate(reasonScene, cm.getX(i - 1), cm.getY(i - 1));
                guestNames[i - 1].objName->locate(reasonScene, cm.getX(i + 4), cm.getY(i + 4));
            }
        }

        showMessage("누가 범인인지 고르세요.");
        return true;
        });

    gameoverScene->setOnEnterCallback([&](auto scene)->bool {
        if (isWin) {
            win = Object::create("Images/Win.png", scene, 450, 350);
        }
        else {
            lose = Object::create("Images/Lose.png", scene, 420, 500);
            killer = Object::create("Images/Killer.png", scene, 480, 400);
            auto killer = Object::create("Images/" + cm.getImgName(cm.getKillerGuest()) + ".png", scene, 540, 130);
        }
        return true;
    });

    startGame(startScene);
}