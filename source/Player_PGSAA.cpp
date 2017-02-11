#include "Player_PGSAA.h"

using namespace SparCraft;

Player_PGSAA::Player_PGSAA (const IDType & playerID) 
{
	_playerID = playerID;
	_iterations = 1;
    _responses = 0;
	_seed = PlayerModels::NOKDPS;
}

Player_PGSAA::Player_PGSAA (const IDType & playerID, const IDType & seed, const size_t & iter, const size_t & responses, const size_t & timeLimit)
{
	_playerID = playerID;
	_iterations = iter;
    _responses = responses;
	_seed = seed;
    _timeLimit = timeLimit;
}

void Player_PGSAA::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{
    moveVec.clear();
	PGSAA pgs(_playerID, _seed, _iterations, _responses, _timeLimit);

	moveVec = pgs.search(_playerID, state);
}
