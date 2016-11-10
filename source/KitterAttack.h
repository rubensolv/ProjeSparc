/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#pragma once

#include "Common.h"
#include "Player.h"

/* 
 * File:   KitterAttack.h
 * Author: rubens
 *
 * Created on 8 de Novembro de 2016, 18:04
 */

namespace SparCraft {

    class KitterAttack : public Player{
    public:
        KitterAttack(const IDType & playerID);
        void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);

        IDType getType() {
            return PlayerModels::Kiter;
        }
    private:

    };
}


