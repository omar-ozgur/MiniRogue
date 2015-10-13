//
//  GameObject.cpp
//  minirogue
//
//  Created by Omar Ozgur on 5/18/15.
//  Copyright (c) 2015 Omar Ozgur. All rights reserved.
//

#include "GameObject.h"
#include "utilities.h"
#include "Game.h"
#include "Actor.h"
using namespace std;

void TeleportScroll::doEffect(){
    
    // Set the effect text, and teleport the player
    getGame()->setText("You feel your body wrenched in space and time.");
    getGame()->getPlayer()->teleport();
}

void ArmorScroll::doEffect(){
    
    // Set the effect text, and randomly increase the player's armor value
    getGame()->setText("Your armor glows blue.");
    getGame()->getPlayer()->setArmor(getGame()->getPlayer()->getArmor() + (1 + randInt(3)));
    if(getGame()->getPlayer()->getArmor() > 99){
        getGame()->getPlayer()->setArmor(99);
    }
}

void StrengthScroll::doEffect(){
    
    // Set the effect text, and randomly increase the player's strength value
    getGame()->setText("Your muscles bulge.");
    getGame()->getPlayer()->setStr(getGame()->getPlayer()->getStr() + (1 + randInt(3)));
    if(getGame()->getPlayer()->getStr() > 99){
        getGame()->getPlayer()->setStr(99);
    }
}

void HealthScroll::doEffect(){
    
    // Set the effect text, and randomly increase the player's maximum health value
    getGame()->setText("You feel your heart beating stronger.");
    getGame()->getPlayer()->setMaxHp(getGame()->getPlayer()->getMaxHp()+(3 + randInt(6)));
    if(getGame()->getPlayer()->getMaxHp() > 99){
        getGame()->getPlayer()->setMaxHp(99);
    }
}

void DexterityScroll::doEffect(){
    
    // Set the effect text, and randomly increase the player's dexterity value
    getGame()->setText("You feel like less of a klutz.");
    getGame()->getPlayer()->setDex(getGame()->getPlayer()->getDex()+(1));
    if(getGame()->getPlayer()->getDex() > 99){
        getGame()->getPlayer()->setDex(99);
    }
}
