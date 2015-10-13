//
//  Actor.cpp
//  minirogue
//
//  Created by Omar Ozgur on 5/12/15.
//  Copyright (c) 2015 Omar Ozgur. All rights reserved.
//

#include "utilities.h"
#include "Game.h"
#include "Actor.h"
#include "GameObject.h"
#include <cmath>
#include <iostream>
using namespace std;

// This is the main recursive function for the goblin's movement
// gY and gX specify the goblin's position, pY and pX specify the player's position, n is used to keep track of the current depth, and 'v' is used to keep track of places that were already checked.
// Calling example: shortestPath(y, x, pY, pX, getGame()->getSmellDistance(), checked); (where 'checked' is a 2D boolean array that is created earlier)
int Goblin::shortestPath(int gY, int gX, int pY, int pX, int n, bool v[DUNGEON_HEIGHT][DUNGEON_WIDTH]){
    
    // Return -1 if the goblin reaches the player by taking the current path
    if(gY == pY && gX == pX)
        return -1;
    
    // Return the smell distance if the move is invalid
    if(n <= 0)
        return getGame()->getSmellDistance();
    
    // Save the current position to a boolean array that is used to save the current path information
    v[gY][gX] = true;
    
    // Use integers to save the number of steps that each path takes (north, south, east, west)
    int a = 0, b = 0, c = 0, d = 0;
    
    // Set a, b, c, or d to the smell distance if the goblin cannot move in that direction
    if(v[gY + 1][gX] == true || !canMove(gY + 1, gX))
        a = getGame()->getSmellDistance();
    if(v[gY - 1][gX] == true || !canMove(gY - 1, gX))
        b = getGame()->getSmellDistance();
    if(v[gY][gX + 1] == true || !canMove(gY, gX + 1))
        c = getGame()->getSmellDistance();
    if(v[gY][gX - 1] == true|| !canMove(gY, gX - 1))
        d = getGame()->getSmellDistance();
    
    // If the goblin can move in a certain direction, find the shortest path to the player in that direction
    if(a < getGame()->getSmellDistance())
        a = 1 + shortestPath(gY + 1, gX, pY, pX, n - 1, v);
    if(b < getGame()->getSmellDistance())
        b = 1 + shortestPath(gY - 1, gX, pY, pX, n - 1, v);
    if(c < getGame()->getSmellDistance())
        c = 1 + shortestPath(gY, gX + 1, pY, pX, n - 1, v);
    if(d < getGame()->getSmellDistance())
        d = 1 + shortestPath(gY, gX - 1, pY, pX, n - 1, v);
    
    // Remove the current position from the visited array
    v[gY][gX] = false;
    
    // Return the shortest path, and move in the optimal direction if the goblin can smell the player
    if(a < getGame()->getSmellDistance() && a <= b && a <= c && a <= d){
        if(n == getGame()->getSmellDistance()){
            move(gY + 1, gX);
        }
        return a;
    }
    else if(b < getGame()->getSmellDistance() && b <= c && b <= d){
        if(n == getGame()->getSmellDistance()){
            move(gY - 1, gX);
        }
        return b;
    }
    else if(c < getGame()->getSmellDistance() && c <= d){
        if(n == getGame()->getSmellDistance()){
            move(gY, gX + 1);
        }
        return c;
    }
    else if(d < getGame()->getSmellDistance()){
        if(n == getGame()->getSmellDistance()){
            move(gY, gX - 1);
        }
        return d;
    }
    else{
        return getGame()->getSmellDistance();
    }
    
}

Actor::~Actor(){
    delete m_weapon;
}

bool Actor::move(int y, int x){
    
    // Check if the specified position is a wall
    if(getGame()->getFloor()->isWall(y, x))
        return false;
    
    // Check if there is a monster at the specified position
    for(int i = 0; i < getGame()->getFloor()->getMonster().size(); i++){
        if(getGame()->getFloor()->getMonster()[i]->getY() == y && getGame()->getFloor()->getMonster()[i]->getX() == x)
            return false;
    }
    
    // Move the player to the new position
    getGame()->getFloor()->setTile(getY(), getX(), ' ');
    setPos(y, x);
    return true;
}

void Actor::attack(Actor *a, Actor *b, Weapon *w){
    
    // Calculate attack and defense variables
    int attackerPoints = a->getDex() + w->getDexBonus();
    int defenderPoints = b->getDex() + b->getArmor();
    int damagePoints = randInt(a->getStr() + w->getDmg());
    
    // Check if actor 'a' hits actor 'b'
    if (randInt(attackerPoints) >= randInt(defenderPoints)){
        b->setHp(b->getHp() - damagePoints);
        
        // Check if the attack kills actor 'b'
        if(b->getHp() <= 0)
            m_game->setText(a->m_name + " " + w->getAction() + " " + w->getName() + " at " + b->m_name + " dealing a final blow.");
        
        // Check if attacker 'a' hits with magic fangs of sleep
        else if(m_weapon->getName() == "magic fangs of sleep"){
            if(trueWithProbability(0.2)){
                if(b->getSleep() <= 0)
                    b->setSleep(2 + randInt(5));
                else{
                    int x = b->getSleep();
                    int y = 2 + randInt(5);
                    if(y > x)
                        b->setSleep(y);
                }
                m_game->setText(a->m_name + " " + w->getAction() + " " + w->getName() + " at " + b->m_name + " and hits, putting " + b->m_name + " to sleep.");
            }
            m_game->setText(a->m_name + " " + w->getAction() + " " + w->getName() + " at " + b->m_name + " and hits.");
        }
        
        // Check if actor 'b' is hit and doesn't die
        else
            m_game->setText(a->m_name + " " + w->getAction() + " " + w->getName() + " at " + b->m_name + " and hits.");
    }
    
    // Set text if actor 'a' misses
    else
        m_game->setText(a->m_name + " " + w->getAction() + " " + w->getName() + " at " + b->m_name + " and misses.");
}

void Actor::teleport(){
    
    // Loop until a valid position is found
    while(1){
        int randX = 1 + randInt(DUNGEON_WIDTH - 2);
        int randY = 1 + randInt(DUNGEON_HEIGHT - 2);
        
        // Check if the random position is a wall
        if(m_game->getFloor()->
           isWall(randY, randX))
            continue;
        
        // Move the player to the random position if there is no monster at the position
        for(int i = 0; i < m_game->getFloor()->getMonster().size(); i++){
            if(m_game->getFloor()->getMonster()[i]->getY() == randY && m_game->getFloor()->getMonster()[i]->getX() == randX)
                break;
            if(i == m_game->getFloor()->getMonster().size() - 1){
                setPos(randY, randX);
                return;
            }
            
        }
    }
}

Player::Player()
:m_floor(0), Actor('@', 0, 0, 20, 2, 2, 2, 0, new ShortSword, "Player"){
    m_inventory.push_back(getWeapon());
}

Player::~Player(){
    for(int i = 0; i < m_inventory.size(); i++){
        delete m_inventory[i];
    }
}

char Player::viewInventory(){
    clearScreen();
    cout << "Inventory:" << endl;
    char c = 'a';
    
    // Check each item in the inventory
    for(int i = 0; i < m_inventory.size(); i++){
        Weapon* w = dynamic_cast<Weapon*>(m_inventory[i]);
        
        // Set certain text if the item is a weapoon
        if (w != nullptr){
            cout << " " << c << ". " << m_inventory[i]->getName() << endl;
        }
        
        // Set certain text if the item is a scroll
        else{
            cout << " " << c << ". " << "a " << m_inventory[i]->getName() << endl;
        }
        
        // Increment 'c', which keeps track of which letter to display
        c++;
    }
    return getCharacter();
}

void Player::wield(){
    char choice = viewInventory();
    char c = 'a';
    for(int i = 0; i < m_inventory.size(); i++){
        
        // See if the user entered the character corresponding to a specific item
        if(choice == c){
            Weapon* w = dynamic_cast<Weapon*>(m_inventory[i]);
            
            // If the item is a weapon, wield it
            if (w != nullptr){
                setWeapon(w);
                getGame()->setText("You are wielding " + m_inventory[i]->getName());
                return;
            }
            
            // If the item is not a weapon, set error text
            else{
                getGame()->setText("You can't wield " + m_inventory[i]->getName());
                return;
            }
            
        }
        c++;
    }
}

void Player::read(){
    char choice = viewInventory();
    char c = 'a';
    for(int i = 0; i < m_inventory.size(); i++){
        
        // See if the user entered a character corresponding to a specific item
        if(choice == c){
            Scroll* s = dynamic_cast<Scroll*>(m_inventory[i]);
            
            // If the item is a scroll, read the scroll
            if (s != nullptr){
                getGame()->setText("You read the scroll called " + m_inventory[i]->getName());
                s->doEffect();
                delete m_inventory[i];
                m_inventory.erase(m_inventory.begin() + i);
                return;
            }
            
            // If the item is not a scroll, set error text
            else{
                getGame()->setText("You can't read a " + m_inventory[i]->getName());
                return;
            }
        }
        c++;
    }
}

void Player::randomHealth(){
    
    // Increment the player's health with a probability of 1/10
    if(trueWithProbability(0.1)){
        if(getHp() < getMaxHp())
            setHp(getHp() + 1);
    }
}

Bogeyman::Bogeyman(int y, int x)
:Monster('B', y, x, 5 + randInt(6), 2, 2 + randInt(2), 2 + randInt(2), 0, new ShortSword, "the Bogeyman"){}

void Bogeyman::takeTurn(){
    if(getSleep() <= 0){
        int pY = getGame()->getPlayer()->getY();
        int pX = getGame()->getPlayer()->getX();
        
        // If the Bogeyman is next to the player, attack the player
        if((getY() == pY - 1 && getX() == pX) || (getY() == pY + 1 && getX() == pX) || (getY() == pY && getX() == pX - 1) || (getY() == pY && getX() == pX + 1)){
            attack(this, getGame()->getPlayer(), getWeapon());
            return;
        }
        
        // If the Bogeyman can smell the player, move one step closer to the player if possible
        if((abs(getY() - pY) + abs(getX() - pX)) <= 5)
        {
            if(getY() - pY > 0){
                if(move(getY() - 1, getX()))
                    return;
            }
            if(pX - getX() > 0){
                if(move(getY(), getX() + 1))
                    return;
            }
            if(pY - getY() > 0){
                if(move(getY() + 1, getX()))
                    return;
            }
            if(getX() - pX > 0){
                if(move(getY(), getX() - 1))
                    return;
            }
        }
    }
    else
        setSleep(getSleep() - 1);
}

void Bogeyman::drop(){
    
    // Check if there is already an object at the Bogeyman's position
    for(int i = 0; i < getGame()->getFloor()->getObj().size(); i++){
        if(getGame()->getFloor()->getObj()[i]->getY() == getY() && getGame()->getFloor()->getObj()[i]->getX() == getX())
            return;
    }
    
    // Randomly drop a magic axe with a probability of 1/10
    if(trueWithProbability(0.1)){
        getGame()->getFloor()->addObj(new MagicAxe(getY(), getX()));
        getGame()->getFloor()->setTile(getY(), getX(), ')');
    }
}

Snakewoman::Snakewoman(int y, int x)
:Monster('S', y, x, 3 + randInt(4), 3, 2, 3, 0, new MagicFangs, "the Snakewoman"){}

void Snakewoman::takeTurn(){
    if(getSleep() <= 0){
        int pY = getGame()->getPlayer()->getY();
        int pX = getGame()->getPlayer()->getX();
        
        // If the Snakewoman is next to the player, attack the player
        if((getY() == pY - 1 && getX() == pX) || (getY() == pY + 1 && getX() == pX) || (getY() == pY && getX() == pX - 1) || (getY() == pY && getX() == pX + 1)){
            attack(this, getGame()->getPlayer(), getWeapon());
            return;
        }
        
        // If the Snakewoman can smell the player, move one step closer to the player
        if((abs(getY() - pY) + abs(getX() - pX)) <= 3)
        {
            if(getY() - pY > 0){
                if(move(getY() - 1, getX()))
                    return;
            }
            if(pX - getX() > 0){
                if(move(getY(), getX() + 1))
                    return;
            }
            if(pY - getY() > 0){
                if(move(getY() + 1, getX()))
                    return;
            }
            if(getX() - pX > 0){
                if(move(getY(), getX() - 1))
                    return;
            }
        }
    }
    else
        setSleep(getSleep() - 1);
}

void Snakewoman::drop(){
    
    // Check if there is already an object at the Snakewoman's position
    for(int i = 0; i < getGame()->getFloor()->getObj().size(); i++){
        if(getGame()->getFloor()->getObj()[i]->getY() == getY() && getGame()->getFloor()->getObj()[i]->getX() == getX())
            return;
    }
    
    // Randomly drop magic fangs of sleep with a probability of 1/3
    if(trueWithProbability(1.0/3.0)){
        getGame()->getFloor()->addObj(new MagicFangs(getY(), getX()));
        getGame()->getFloor()->setTile(getY(), getX(), ')');
    }
}

Dragon::Dragon(int y, int x)
:Monster('D', y, x, 20 + randInt(6), 4, 4, 4, 0, new LongSword, "the Dragon"){}

void Dragon::takeTurn(){
    
    // Randomly increment the dragon's health with a probability of 1/10
    if(getHp() < getMaxHp()){
        if(trueWithProbability(0.1)){
            setHp(getHp() + 1);
        }
    }
    if(getSleep() <= 0){
        int pY = getGame()->getPlayer()->getY();
        int pX = getGame()->getPlayer()->getX();
        
        // If the Dragon is next to the player, attack the player
        if((getY() == pY - 1 && getX() == pX) || (getY() == pY + 1 && getX() == pX) || (getY() == pY && getX() == pX - 1) || (getY() == pY && getX() == pX + 1))
            attack(this, getGame()->getPlayer(), getWeapon());
    }
    else
        setSleep(getSleep() - 1);
}

void Dragon::drop(){
    
    // Check if there is already an object at the Dragon's position
    for(int i = 0; i < getGame()->getFloor()->getObj().size(); i++){
        if(getGame()->getFloor()->getObj()[i]->getY() == getY() && getGame()->getFloor()->getObj()[i]->getX() == getX())
            return;
    }
    
    // Randomly choose a scroll to drop with a probability of 100%
    int choice = randInt(5);
    switch (choice) {
        case 0:
            getGame()->getFloor()->setObj(new ArmorScroll(getGame(), getY(), getX()));
            getGame()->getFloor()->setTile(getY(), getX(), '?');
            break;
        case 1:
            getGame()->getFloor()->setObj(new HealthScroll(getGame(), getY(), getX()));
            getGame()->getFloor()->setTile(getY(), getX(), '?');
            break;
        case 2:
            getGame()->getFloor()->setObj(new DexterityScroll(getGame(), getY(), getX()));
            getGame()->getFloor()->setTile(getY(), getX(), '?');
            break;
        case 3:
            getGame()->getFloor()->setObj(new StrengthScroll(getGame(), getY(), getX()));
            getGame()->getFloor()->setTile(getY(), getX(), '?');
            break;
        case 4:
            getGame()->getFloor()->setObj(new TeleportScroll(getGame(), getY(), getX()));
            getGame()->getFloor()->setTile(getY(), getX(), '?');
            break;
    }
}

Goblin::Goblin(int y, int x)
:Monster('G', y, x, 15 + randInt(6), 1, 3, 1, 0, new ShortSword, "the Goblin"){}

void Goblin::takeTurn(){
    if(getSleep() <= 0){
        int pY = getGame()->getPlayer()->getY();
        int pX = getGame()->getPlayer()->getX();
        
        // If the goblin is next to the player, attack the player
        if((getY() == pY - 1 && getX() == pX) || (getY() == pY + 1 && getX() == pX) || (getY() == pY && getX() == pX - 1) || (getY() == pY && getX() == pX + 1)){
            attack(this, getGame()->getPlayer(), getWeapon());
            return;
        }
        
        int y = getY();
        int x = getX();
        
        bool v[DUNGEON_HEIGHT][DUNGEON_WIDTH] = {0};
        shortestPath(y, x, pY, pX, getGame()->getSmellDistance(), v);
    }
    else
        setSleep(getSleep() - 1);
    
}

bool Goblin::canMove(int y, int x){
    char tile = getGame()->getFloor()->getTile(y, x);
    
    // Return false if there is a wall or a monster at the specified position
    if(tile != 'B' && tile != 'S' && tile != 'G' && tile != 'D' && tile != '#')
        return true;
    return false;
}

void Goblin::drop(){
    
    // Check if there is already an object at the goblin's position
    for(int i = 0; i < getGame()->getFloor()->getObj().size(); i++){
        if(getGame()->getFloor()->getObj()[i]->getY() == getY() && getGame()->getFloor()->getObj()[i]->getX() == getX())
            return;
    }
    
    // Randomly drop a magic axe or magic fangs of sleep with a probability of 1/3
    if(trueWithProbability(1.0/3.0)){
        int choice = randInt(2);
        switch (choice) {
            case 0:
                getGame()->getFloor()->setObj(new MagicAxe(getY(), getX()));
                getGame()->getFloor()->setTile(getY(), getX(), ')');
                break;
            case 1:
                getGame()->getFloor()->setObj(new MagicFangs(getY(), getX()));
                getGame()->getFloor()->setTile(getY(), getX(), ')');
                break;
        }
    }
}