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
        std::map<Unit, std::vector<Unit>> _unAttack;
        std::vector<Unit> _UnReut;
    public:
        GenerationClass(const IDType & playerID);
        void getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
        void getMoves2(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec);
        IDType getType() { return PlayerModels::Kiter; }
        void listaOrdenada(const IDType & playerID, const Unit & unidade, GameState & state, std::vector<Unit> & unidades);
        void listaOrdenadaForMoves(const IDType & playerID, const Unit & unidade, GameState & state, std::vector<Unit> & unidades, const MoveArray & moves);
        //void aplicarAbstracao(GameState & origState);
        void copiarStateCleanUnit(GameState & origState, GameState & copState);
        void iniciarAlphaBeta();
    private:        
        std::vector<Unit> retornaQtdUnidadesMaisProximas(const IDType & playerId, const Unit & unitRef, GameState & state, int qtdUnidades);
        Unit getEnemyClosestvalid(GameState & state, std::vector<Unit> unidadesInimigas);
        //manipulação do controle de atacantes
        void addAttack(const Unit & unitEnemy,  const Unit & unitAttack);
        void printMapAttack();
        const PositionType getDistManhantan(const Position & pInicial, const Position & pFinal);
        const PositionType getDistEuclidiana(const Position & pInicial, const Position & pFinal);
        const bool unitNeedMoreAttackForKilled(Unit un);
        void doAlphaBeta(GameState & newState, std::vector<Action> & moveVec, GameState& state);
        void sortUnit(std::vector<Unit> & unidades, const Unit & unit, GameState & state);
        Unit& getCalculateEnemy(GameState & state, std::vector<Unit> unidadesInimigas); 
        void removeLoseAttacks(GameState & newState, std::vector<Action> & moveVec, GameState & state);
        void removeActionInVector(Action & action, std::vector<Action>& moveVec);
    };
}


