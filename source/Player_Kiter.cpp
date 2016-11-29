#include "Player_Kiter.h"


using namespace SparCraft;

Player_Kiter::Player_Kiter (const IDType & playerID) 
{
	_playerID = playerID;
}
//############################################################################################
//  Início das funcionalidades relacionadas à classe Generation Abstract
//############################################################################################

void Player_Kiter::testarMovimentos(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec){
    
    //std::cout<< "Valores do estado "<< state.toString()<< "\n";
    //std::cout<< moves.numUnits() <<" total de unidades"<<std::endl;   
    
    for (IDType u(0); u < moves.numUnits(); u++) {        
        const Unit & ourUnit           (state.getUnit(_playerID,u));
        const Unit & closestOurUnit    (state.getClosestOurUnit(_playerID, u));
        
        const Unit & enemy (state.getClosestEnemyUnit(_playerID, ourUnit.ID(), false));
        
        std::vector<Unit> unidades;
        listaOrdenada(_playerID, ourUnit, state, unidades);
        
        //std::cout << "---------------------------\n";
        //for(auto& un : unidades){
        //    un.print();
        //}
        
        //efetua a cópia do estado 
        GameState newState;
        copiarStateCleanUnit(state, newState);
        //verificar a remoção das unidades não desejadas
        
        std::cout<<" Minha unidade :\n";
        ourUnit.print();
        std::cout<<" Unidade inimiga :\n";
        enemy.print();
        
        std::cout<<" Estado atual :";
        newState.print();
        
                
        std::cout<<" Estado após remoção :";
        newState.print();
        
        std::cout<<" Estado após inclusões :";
        newState.addUnit(ourUnit);
        newState.addUnit(enemy);
        newState.print();
        
        
        
        std::cout<<" Estado origial :";
        state.print();
        
    }
}

bool evalUnitDistance(const Unit& u1, const Unit& u2){
    return u1.position() < u2.position();
}

/*
 * Retorna todas as unidades de um player ordenados pela distância da unidade
 * passada como referência
 */
void Player_Kiter::listaOrdenada(const IDType& playerID, const Unit & unidade, GameState& state, std::vector<Unit> & unidades){
    //declaração
    Unit t;
    std::map<size_t, Unit> unitsOrder;
    std::map<size_t, Unit>::iterator myIt;
    
    //obtenho posição atual da unidade base
    Position currentPos = unidade.currentPosition(state.getTime());
        
    for(IDType u(0); u<state.numUnits(playerID);++u){        
        t = state.getUnit(playerID,u);        
        if(!unidade.equalsID( t ) ){
            size_t distSq(currentPos.getDistanceSq(t.currentPosition(state.getTime())));
            unitsOrder[distSq] = t;
        }
    }
    
    for(myIt = unitsOrder.begin(); myIt != unitsOrder.end(); myIt++){
        unidades.push_back(myIt->second);
    }   
}
/*
 * Função tem o objetivo de realizar a cópia do state e já devolver ela 
 * com as unidades zeradas, mantando assim as outras informações 
 * necessárias.
 */
void Player_Kiter::copiarStateCleanUnit(GameState& origState, GameState& copState){
    copState = origState;
    copState.cleanUpStateUnits();
}

void Player_Kiter::aplicarAlphaBeta(GameState& state, const MoveArray& moves, std::vector<Action>& moveVec){
    
}

//############################################################################################
//  Fim das funcionalidades relacionadas à classe Generation Abstract
//############################################################################################


void Player_Kiter::getMoves(GameState & state, const MoveArray & moves, std::vector<Action> & moveVec)
{     
    testarMovimentos(state, moves, moveVec);
    
    moveVec.clear();
	for (IDType u(0); u<moves.numUnits(); ++u)
	{
		bool foundAction						(false);
		IDType actionMoveIndex					(0);
		IDType furthestMoveIndex				(0);
		size_t furthestMoveDist					(0);
		IDType closestMoveIndex					(0);
		int actionDistance						(std::numeric_limits<int>::max());
		unsigned long long closestMoveDist		(std::numeric_limits<unsigned long long>::max());

		const Unit & ourUnit					(state.getUnit(_playerID, u));
		const Unit & closestUnit				(ourUnit.canHeal() ? state.getClosestOurUnit(_playerID, u) : state.getClosestEnemyUnit(_playerID, u));

		for (IDType m(0); m<moves.numMoves(u); ++m)
		{
			const Action move						(moves.getMove(u, m));
				
			if (move.type() == ActionTypes::ATTACK)
			{
				const Unit & target				(state.getUnit(state.getEnemy(move.player()), move.index()));
				PositionType dist				(ourUnit.getDistanceSqToUnit(target, state.getTime()));

				if (dist < actionDistance)
				{
					actionDistance = dist;
					actionMoveIndex = m;
					foundAction = true;
				}
			}
			else if (move.type() == ActionTypes::HEAL)
			{
				const Unit & target				(state.getUnit(move.player(), move.index()));
				PositionType dist				(ourUnit.getDistanceSqToUnit(target, state.getTime()));

				if (dist < actionDistance)
				{
					actionDistance = dist;
					actionMoveIndex = m;
					foundAction = true;
				}
			}
			else if (move.type() == ActionTypes::MOVE)
			{
				Position ourDest				(ourUnit.x() + Constants::Move_Dir[move.index()][0], 
												 ourUnit.y() + Constants::Move_Dir[move.index()][1]);
				size_t dist						(closestUnit.getDistanceSqToPosition(ourDest, state.getTime()));

				if (dist > furthestMoveDist)
				{
					furthestMoveDist = dist;
					furthestMoveIndex = m;
				}

				if (dist < closestMoveDist)
				{
					closestMoveDist = dist;
					closestMoveIndex = m;
				}
			}
		}

		// the move we will be returning
		size_t bestMoveIndex(0);

		// if we have an attack move we will use that one
		if (foundAction)
		{
			bestMoveIndex = actionMoveIndex;
		}
		// otherwise use the closest move to the opponent
		else
		{
			// if we are in attack range of the unit, back up
			if (ourUnit.canAttackTarget(closestUnit, state.getTime()))
			{
				bestMoveIndex = furthestMoveIndex;
			}
			// otherwise get back into the fight
			else
			{
				bestMoveIndex = closestMoveIndex;
			}
		}
			
		moveVec.push_back(moves.getMove(u, bestMoveIndex));
	}
}
