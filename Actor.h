//
//  Actor.h
//  minirogue
//
//  Created by Omar Ozgur on 5/12/15.
//  Copyright (c) 2015 Omar Ozgur. All rights reserved.
//

#ifndef __minirogue__Actor__
#define __minirogue__Actor__

#include <string>
#include <vector>
#include <stack>
#include <iostream>
using namespace std;

class Game;
class Weapon;
class GameObject;

class Actor{
public:
    Actor(char c, int y, int x, int hp, int armor, int str, int dex, int sleep, Weapon* w, string name)
    :m_char(c), m_posY(y), m_posX(x), m_hp(hp), m_armor(armor), m_str(str), m_dex(dex), m_sleep(sleep), m_weapon(w), m_maxHp(hp), m_name(name){}
    virtual ~Actor();
    
    // Print the common actor stats
    virtual void printStats() const{
        cout << "Hit points: " << m_hp << ", Armor: " << m_armor << ", Strength: " << m_str << ", Dexterity: " << m_dex << endl;
    }
    
    // Move the actor to the specified position if possible
    bool move(int y, int x);

    // Have one actor attack another actor with a specified weapon
    void attack(Actor* a, Actor* b, Weapon* w);
    
    // Activate cheat mode
    void cheat(){ m_str = 9; m_maxHp = 50; m_hp = 50; }
    
    // Teleport the actor to a random valid position
    void teleport();
    
    int getX() const{ return m_posX; }
    int getY() const{ return m_posY; }
    int getC() const{ return m_char; }
    int getHp() const{ return m_hp; }
    int getMaxHp() const{ return m_maxHp; }
    int getDex() const{ return m_dex; }
    int getStr() const{ return m_str; }
    int getArmor() const{ return m_armor; }
    int getSleep() const{ return m_sleep; }
    Weapon* getWeapon() const{ return m_weapon; }
    Game* getGame() const{ return m_game; }
    
    void setPos(int y, int x){ m_posY = y; m_posX = x; }
    void setHp(int x){ m_hp = x; }
    void setMaxHp(int x){ m_maxHp = x; }
    void setArmor(int x){ m_armor = x; }
    void setStr(int x){ m_str = x; }
    void setDex(int x){ m_dex = x; }
    void setSleep(int x){ m_sleep = x; }
    void setGame(Game* g){ m_game = g; }
    void setWeapon(Weapon* w){ m_weapon = w; }
private:
    int m_posX, m_posY;
    int m_hp, m_maxHp;
    int m_armor, m_str, m_dex;
    int m_sleep;
    char m_char;
    string m_name;
    Weapon* m_weapon;
    Game* m_game;
};

class Player: public Actor{
public:
    Player();
    ~Player();
    
    virtual void printStats() const{
        cout << "Dungeon Level: " << m_floor << ", ";
        Actor::printStats();
    }
    
    void randomHealth();
    
    // Manipulate inventory
    void addItem(GameObject* i){ m_inventory.push_back(i); }
    char viewInventory();
    void wield();
    void read();
    
    // Get player-specific variables
    int getFloor() const{ return m_floor; }
    vector<GameObject*> getInventory() const{ return m_inventory; }

    // Set player-specific variables
    void setFloor(int floor) { m_floor = floor; }
private:
    int m_floor;
    vector<GameObject*> m_inventory;
};

class Monster: public Actor{
public:
    Monster(char c, int y, int x, int hp, int armor, int str, int dex, int sleep, Weapon* w, string name)
    :Actor(c, y, x, hp, armor, str, dex, sleep, w, name){}
    ~Monster(){}
    virtual void takeTurn() = 0;
    virtual void drop() = 0;
};

class Bogeyman: public Monster{
public:
    Bogeyman(int y, int x);
    ~Bogeyman(){}
    virtual void takeTurn();
    virtual void drop();
};

class Snakewoman: public Monster{
public:
    Snakewoman(int y, int x);
    ~Snakewoman(){}
    virtual void takeTurn();
    virtual void drop();
};

class Dragon: public Monster{
public:
    Dragon(int y, int x);
    ~Dragon(){}
    virtual void takeTurn();
    virtual void drop();
};

class Goblin: public Monster{
public:
    Goblin(int y, int x);
    ~Goblin(){}
    bool canMove(int y, int x);
    int shortestPath(int gY, int gX, int pY, int pX, int n, bool v[DUNGEON_HEIGHT][DUNGEON_WIDTH]);
    virtual void takeTurn();
    virtual void drop();
};

#endif /* defined(__minirogue__Actor__) */
