#pragma once

#include "Common.h"
#include "GameState.h"
#include "Player.h"
#include "Game.h"
#include "Action.h"
#include "UnitScriptData.h"
#include "AbstractScript.h"
#include <memory>

namespace SparCraft {

    typedef std::shared_ptr<Player> PlayerPtr;
    struct compare {

        bool operator()(const Unit & lUn, const Unit & rUn) const {
            return lUn < rUn;
        }
    };

    class PGSAA {
    protected:

        const IDType _player;
        const IDType _enemyScript;
        const size_t _iterations;
        const size_t _responses;
        std::vector<IDType> _playerScriptPortfolio;
        size_t _totalEvals;
        size_t _timeLimit;
        std::ofstream _fileTime;
        
        
        std::set<Unit, compare> _unitAbstraction;
        TimeType lastTime;

        void doPortfolioSearch(const IDType & player, const GameState & state, UnitScriptData & currentData, Timer & t, std::map<Unit, Action>& map_UnitAction);
        std::vector<Action> getMoveVec(const IDType & player, const GameState & state, const std::vector<IDType> & playerScripts);
        StateEvalScore eval(const IDType & player, const GameState & state, UnitScriptData & playerScriptsChosen);
        StateEvalScore evalAbstract(const IDType & player, const GameState & state, UnitScriptData & playerScriptsChosen, std::map<Unit,Action> & map_abs_action);
        IDType calculateInitialSeed(const IDType & player, const GameState & state);
        void setAllScripts(const IDType & player, const GameState & state, UnitScriptData & data, const IDType & script);

    public:

        PGSAA(const IDType & player, const IDType & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit);
        std::vector<Action> search(const IDType & player, const GameState & state);
    
    private:
        void controlUnits(GameState & state);
    
    };

}
