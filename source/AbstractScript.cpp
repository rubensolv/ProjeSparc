#include "AbstractScript.h"


using namespace SparCraft;

AbstractScript::AbstractScript (const IDType & playerID) 
{
	_playerID = playerID;
}


AbstractScript::AbstractScript(const IDType& playerID, Action action) {
    	_playerID = playerID;
        _singleAction = action;
}

void AbstractScript::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
    moveVec.clear();
    moveVec.push_back(_singleAction);
    
}

void AbstractScript::setMoviment(Action moviment){
    _singleAction = moviment;
}
