#include "PGSAA.h"

using namespace SparCraft;

PGSAA::PGSAA(const IDType & player, const IDType & enemyScript, const size_t & iter, const size_t & responses, const size_t & timeLimit)
	: _player(player)
	, _enemyScript(enemyScript)
	, _iterations(iter)
    , _responses(responses)
    , _totalEvals(0)
    , _timeLimit(timeLimit)
{
	_playerScriptPortfolio.push_back(PlayerModels::NOKDPS);
	_playerScriptPortfolio.push_back(PlayerModels::KiterDPS);
	//_playerScriptPortfolio.push_back(PlayerModels::Cluster);
}



std::vector<Action> PGSAA::search(const IDType & player, const GameState & state)
{
    Timer t;
    t.start();        
    
    if(lastTime > state.getTime()){
        _unitAbstraction.clear();
    }
    lastTime = state.getTime();
    
    //inicio a chamada para definir as unidades controladas
    GameState tempState(state);
    controlUnits(tempState);

    const IDType enemyPlayer(state.getEnemy(player));

    // calculate the seed scripts for each player
    // they will be used to seed the initial root search
    IDType seedScript = calculateInitialSeed(player, state);
    IDType enemySeedScript = calculateInitialSeed(enemyPlayer, state);

    // set up the root script data
    UnitScriptData originalScriptData;
    setAllScripts(player, state, originalScriptData, seedScript);
    setAllScripts(enemyPlayer, state, originalScriptData, enemySeedScript);

    double ms = t.getElapsedTimeInMilliSec();
    //printf("\nFirst Part %lf ms\n", ms);

    // do the initial root portfolio search for our player
    UnitScriptData currentScriptData(originalScriptData);
    std::map<Unit, Action> map_UnitAction;
    doPortfolioSearch(player, state, currentScriptData, t, map_UnitAction);

    // iterate as many times as required
    for (size_t i(0); i<_responses; ++i)
    {
        // do the portfolio search to improve the enemy's scripts
        doPortfolioSearch(enemyPlayer, state, currentScriptData, t, map_UnitAction);

        // then do portfolio search again for us to improve vs. enemy's update
        doPortfolioSearch(player, state, currentScriptData, t, map_UnitAction);
    }

    ms = t.getElapsedTimeInMilliSec();
/*
    _fileTime.open("PGS.txt", std::ostream::app);
    if (!_fileTime.is_open())
    {
    	std::cout << "ERROR Opening file" << std::endl;
    }
    _fileTime << ms << ", ";
    _fileTime.close();
*/
    // convert the script vector into a move vector and return it
	MoveArray moves;
	state.generateMoves(moves, player);
    std::vector<Action> moveVec;
    GameState copy(state);
    currentScriptData.calculateMovesAndActions(player, moves, copy, moveVec, map_UnitAction);

    ms = t.getElapsedTimeInMilliSec();
    //printf("\nMove PGS chosen in %lf ms\n", ms);

    _totalEvals = 0;

    return moveVec;
}

void PGSAA::doPortfolioSearch(const IDType & player, const GameState & state, UnitScriptData & currentScriptData, Timer & t, std::map<Unit, Action>& map_UnitAction)
{
  //  Timer t;
 //   t.start();


    // the enemy of this player
    const IDType enemyPlayer(state.getEnemy(player));
    
    while(t.getElapsedTimeInMilliSec() < _timeLimit)
    //for (size_t i(0); i<_iterations; ++i)
    {
        // set up data for best scripts
        IDType          bestScriptVec[Constants::Max_Units];
	    StateEvalScore  bestScoreVec[Constants::Max_Units];
            Action bestAction;
            StateEvalScore bestScoreAction;

        // for each unit that can move
        for (size_t unitIndex(0); unitIndex<state.numUnits(player); ++unitIndex)
        {
            if (_timeLimit > 0 && t.getElapsedTimeInMilliSec() > _timeLimit)
            {
                break;
            }

            const Unit & unit(state.getUnit(player, unitIndex));

            if (_unitAbstraction.find(unit) != _unitAbstraction.end() ) {
                
                //transforma moves em scripts
                std::vector<Action> moves;
                state.generateMovesForUnit(moves,_player, unit.ID());
                int counterMoves = 0;
                for (auto & mov : moves ) {                    
                    map_UnitAction[unit] = mov;
                    // evaluate the current state given a playout with these unit scripts
                    StateEvalScore score = evalAbstract(player, state, currentScriptData, map_UnitAction);

                    // if we have a better score, set it
                    //if (sIndex == 0 || score > bestScoreVec[unitIndex]) {
                    if (counterMoves == 0 || score > bestScoreAction) {
                        bestAction = mov;
                        bestScoreAction = score;
                    }
                    
                    counterMoves++;
                    
                }
                map_UnitAction[unit] = bestAction;
            } else {
                // iterate over each script move that it can execute
                for (size_t sIndex(0); sIndex < _playerScriptPortfolio.size(); ++sIndex) {
                    // set the current script for this unit
                    currentScriptData.setUnitScript(unit, _playerScriptPortfolio[sIndex]);

                    // evaluate the current state given a playout with these unit scripts
                    StateEvalScore score = eval(player, state, currentScriptData);

                    // if we have a better score, set it
                    if (sIndex == 0 || score > bestScoreVec[unitIndex]) {
                        bestScriptVec[unitIndex] = _playerScriptPortfolio[sIndex];
                        bestScoreVec[unitIndex] = score;
                    }
                }
                // set the current vector to the best move for use in future simulations
                currentScriptData.setUnitScript(unit, bestScriptVec[unitIndex]);
            }
        }   
    }   
}

IDType PGSAA::calculateInitialSeed(const IDType & player, const GameState & state)
{
    IDType bestScript;
    StateEvalScore bestScriptScore;
    const IDType enemyPlayer(state.getEnemy(player));
    
    // try each script in the portfolio for each unit as an initial seed
    for (size_t sIndex(0); sIndex<_playerScriptPortfolio.size(); ++sIndex)
    {
        UnitScriptData currentScriptData;
    
        // set the player's chosen script initially to the seed choice
        for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
        {
            currentScriptData.setUnitScript(state.getUnit(player, unitIndex), _playerScriptPortfolio[sIndex]);
        }

        // set the enemy units script choice to NOKDPS
        for (size_t unitIndex(0); unitIndex < state.numUnits(enemyPlayer); ++unitIndex)
        {
            currentScriptData.setUnitScript(state.getUnit(enemyPlayer, unitIndex), _enemyScript);
        }

        // evaluate the current state given a playout with these unit scripts
        StateEvalScore score = eval(player, state, currentScriptData);

        if (sIndex == 0 || score > bestScriptScore)
        {
            bestScriptScore = score;
            bestScript = _playerScriptPortfolio[sIndex];
        }
    }

    return bestScript;
}


StateEvalScore PGSAA::evalAbstract(const IDType& player, const GameState& state, UnitScriptData& playerScriptsChosen, std::map<Unit, Action>& map_abs_action) {
    
    Game g(state, 100);

    _totalEvals++;
    
    return g.playIndividualScriptsAndActions(player, playerScriptsChosen, map_abs_action, 100);
}


StateEvalScore PGSAA::eval(const IDType & player, const GameState & state, UnitScriptData & playerScriptsChosen)
{
    const IDType enemyPlayer(state.getEnemy(player));

	Game g(state, 100);

	_totalEvals++;

    g.playIndividualScripts(playerScriptsChosen);
	return g.getState().eval(player, SparCraft::EvaluationMethods::LTD2);
}

void  PGSAA::setAllScripts(const IDType & player, const GameState & state, UnitScriptData & data, const IDType & script)
{
    for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
    {
        data.setUnitScript(state.getUnit(player, unitIndex), script);
    }
}


//separo as unidades que serão utilizadas para compor a abstração que será utilizada no AB
//e faço controle e manutenção destas

void PGSAA::controlUnits(GameState & state) {
    int numUnits = 10;
    //verifico se as unidades não foram mortas
    std::set<Unit, compare> tempUnitAbsAB;
    for (auto & un : _unitAbstraction) {
        if (state.unitExist(_player, un.ID()))  {
            tempUnitAbsAB.insert(un);
        }
    }
    _unitAbstraction = tempUnitAbsAB;
    
    if (state.numUnits(_player) <= numUnits) {
        _unitAbstraction.clear();
        //adiciono todas as unidades para serem controladas pelo AB
        for (int u(0); u < state.numUnits(_player); ++u) {
            _unitAbstraction.insert(state.getUnit(_player, u));
        }
    } else if (!(_unitAbstraction.size() == numUnits)) {
        
        if ((state.numUnits(_player) < 2 )
                and _unitAbstraction.size() == 0) {
            _unitAbstraction.insert(state.getUnit(_player, 0));
        } else {
            int control = 0;
            while (_unitAbstraction.size() < numUnits and control < 20) {
                _unitAbstraction.insert(state.getUnit(_player, rand() % state.numUnits(_player)));
                control++;
            }
        }
    }

}





