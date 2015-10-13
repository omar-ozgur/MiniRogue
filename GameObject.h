//
//  GameObject.h
//  minirogue
//
//  Created by Omar Ozgur on 5/12/15.
//  Copyright (c) 2015 Omar Ozgur. All rights reserved.
//

#ifndef __minirogue__GameObject__
#define __minirogue__GameObject__

#include <string>
using namespace std;

class Game;

class GameObject{
public:
    GameObject(int y, int x, int c, string name, string action = ""):m_posY(y), m_posX(x), m_c(c), m_name(name), m_action(action) {}
    virtual ~GameObject(){}
    
    // Set GameObject variables
    void setGame(Game* g){ m_game = g; }
    
    // Get GameObject variables
    int getX() const{ return m_posX; }
    int getY() const{ return m_posY; }
    char getC() const{ return m_c; }
    string getName() const{ return m_name; }
    string getAction() const{ return m_action; }
    Game* getGame() const{ return m_game; }
private:
    int m_posX, m_posY;
    char m_c;
    string m_name;
    string m_action;
    Game* m_game;
};

class Weapon: public GameObject{
public:
    Weapon(int y, int x, int dex, int dmg, string name = "", string action = ""):GameObject(y, x, ')', name, action), m_dexBonus(dex), m_damage(dmg){}
    virtual ~Weapon(){}

    // Get weapon-specific variables
    int getDexBonus() const{ return m_dexBonus; }
    int getDmg() const{ return m_damage; }
private:
    int m_dexBonus;
    int m_damage;
};

class Mace: public Weapon{
public:
    Mace(int y = -1, int x = -1):Weapon(y, x, 0, 2, "mace", "swings"){}
    ~Mace(){}
};

class ShortSword: public Weapon{
public:
    ShortSword(int y = -1, int x = -1):Weapon(y, x, 0, 2, "short sword", "slashes"){}
    ~ShortSword(){}
};

class LongSword: public Weapon{
public:
    LongSword(int y = -1, int x = -1):Weapon(y, x, 2, 4, "long sword", "swings"){}
    ~LongSword(){}
};

class MagicAxe: public Weapon{
public:
    MagicAxe(int y = -1, int x = -1):Weapon(y, x, 5, 5, "magic axe", "chops"){}
    ~MagicAxe(){}
};

class MagicFangs: public Weapon{
public:
    MagicFangs(int y = -1, int x = -1):Weapon(y, x, 3, 2, "magic fangs of sleep", "strikes"){}
    ~MagicFangs(){}
};

class Scroll: public GameObject{
public:
    Scroll(int y, int x, string name):GameObject(y, x, '?', name){}
    virtual ~Scroll(){}
    
    virtual void doEffect() = 0;
};

class TeleportScroll: public Scroll{
public:
    TeleportScroll(Game* g, int y = -1, int x = -1):Scroll(y, x, "scroll of teleportation"){
        setGame(g);
    }
    ~TeleportScroll(){}
    
    virtual void doEffect();
};

class ArmorScroll: public Scroll{
public:
    ArmorScroll(Game* g, int y = -1, int x = -1):Scroll(y, x, "scroll of improve armor"){
        setGame(g);
    }
    ~ArmorScroll(){}
    
    virtual void doEffect();
};

class StrengthScroll: public Scroll{
public:
    StrengthScroll(Game* g, int y = -1, int x = -1):Scroll(y, x, "scroll of raise strength"){
        setGame(g);
    }
    ~StrengthScroll(){}
    
    virtual void doEffect();
};

class HealthScroll: public Scroll{
public:
    HealthScroll(Game* g, int y = -1, int x = -1):Scroll(y, x, "scroll of enhance health"){
        setGame(g);
    }
    ~HealthScroll(){}
    
    virtual void doEffect();
};

class DexterityScroll: public Scroll{
public:
    DexterityScroll(Game* g, int y = -1, int x = -1):Scroll(y, x, "scroll of enhance dexterity"){
        setGame(g);
    }
    ~DexterityScroll(){}

    virtual void doEffect();
};

#endif /* defined(__minirogue__GameObject__) */
