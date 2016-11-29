/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GenerationClass.cpp
 * Author: rubens
 * 
 * Created on 19 de Novembro de 2016, 14:35
 */

#include "GenerationClass.h"

using namespace SparCraft;

GenerationClass::GenerationClass(const IDType& playerID) {
    _playerID = playerID;
}

GenerationClass::getMoves(GameState& state, const MoveArray& moves, std::vector<Action>& moveVec) {
    moveVec.clear();

    for (IDType u(0); u < moves.numUnits(); u++) {
        
        std::cout<< "Valores do estado "<< state.toString();
                
        const Unit & ourUnit           (state.getUnit(_playerID,u));
        const Unit & closestOurUnit    (state.getClosestOurUnit(_playerID, u));
        const Unit & closestEnemyUnit  (state.getEnemy(_playerID, u));
        
    }
}




