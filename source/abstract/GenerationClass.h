#pragma once

/* 
 * File:   GenerationClass.h
 * Author: rubens
 *
 * Created on 19 de Novembro de 2016, 14:35
 */

#include "Common.h"
#include "Player.h"

namespace SparCraft {

    class GenerationClass : public Player {
    public:
        GenerationClass(const IDType & playerID);
        void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
        IDType getType() { return PlayerModels::Kiter; }
    private:

    };
}


