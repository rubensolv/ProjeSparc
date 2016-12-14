#pragma once

/* 
 * File:   GenerationClass.h
 * Author: rubens
 *
 * Created on 19 de Novembro de 2016, 14:35
 */


#include "Player.h"
#include "AlphaBetaSearchParameters.hpp"
#include "AlphaBetaSearchAbstract.h"

class TranspositionTable;

namespace SparCraft {
    class AlphaBetaSearchAbstract;
    
    class GenerationClass : public Player {
        AlphaBetaSearchAbstract * alphaBeta;
    public:

        GenerationClass(const IDType & playerID);
        void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
        void getMoves2(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
        IDType getType() { return PlayerModels::Kiter; }
        void listaOrdenada(const IDType & playerID, const Unit & unidade, GameState & state, std::vector<Unit> & unidades);
        //void aplicarAbstracao(GameState & origState);
        void copiarStateCleanUnit(GameState & origState, GameState & copState);
        void aplicarAlphaBeta(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
        void iniciarAlphaBeta();
    private:        
        std::vector<Unit> retornaQtdUnidadesMaisProximas(const IDType & playerId, const Unit & unitRef, GameState & state, int qtdUnidades);
    };
}


