
#include "GenerationClass.h"

using namespace SparCraft;

GenerationClass::GenerationClass(const IDType& playerID) {
    _playerID = playerID;
    iniciarAlphaBeta();
}

void GenerationClass::getMoves(GameState& state, const MoveArray& moves, std::vector<Action>& moveVec) {
    moveVec.clear();
    //estado que será utilizado para simular as variações necessárias do AB
    GameState newState;
    //obtem nossa unidade inicial.
    const Unit & ourUnit           (state.getUnit(_playerID,(0)));        
    const Unit & enemy (state.getClosestEnemyUnit(_playerID, ourUnit.ID(), false));
    
    std::vector<Unit> unidadesAliadas ;
    std::vector<Unit> unidadesInimigas;
    
    listaOrdenada(_playerID, ourUnit, state, unidadesAliadas);
    listaOrdenada(enemy.player(), enemy, state, unidadesInimigas);
            
    for(auto & un : unidadesAliadas){
        copiarStateCleanUnit(state, newState);
        //adiciono o número de unidades que desejamos na abstração
        newState.addUnitWithID(un);
        const Unit & enemy2 (state.getClosestEnemyUnit(_playerID, un.ID(), false));
        newState.addUnitWithID(enemy2);
        
        //executa a busca
        alphaBeta->doSearch(newState);
        Action mov = alphaBeta->getResults().bestMoves[0];
        if(mov.type() == ActionTypes::ATTACK){
            moveVec.push_back( Action( state.getIndexUnit(un.player(), un.ID()) ,mov.player(), mov.type(), state.getIndexUnit(enemy.player(), enemy.ID()), mov.pos() ));
        }else{
            moveVec.push_back( Action(state.getIndexUnit(un.player(), un.ID()),mov.player(), mov.type(), mov.index(), mov.pos() ));
        }
        
    }
    
    std::cout<<"************* INICIO Generator  **************"<<std::endl;
    for(auto & ac : moveVec){
        std::cout<<ac.debugString()<<std::endl;
    }
    std::cout<<"************* FIM Generator  **************"<<std::endl;
    
}

void GenerationClass::getMoves2(GameState& state, const MoveArray& moves, std::vector<Action>& moveVec) {
    moveVec.clear();
    //std::cout<< "Valores do estado "<< state.toString()<< "\n";
    //std::cout<< moves.numUnits() <<" total de unidades"<<std::endl;   
    GameState newState;
   // std::cout << "Original state:" << std::endl;
   // state.print();
    for (IDType u(0); u < moves.numUnits(); u++) {        
        
        const Unit & ourUnit           (state.getUnit(_playerID,u));        
        const Unit & ourUnit2          (state.getClosestOurUnit(_playerID, ourUnit.ID()));
        
        const Unit & enemy (state.getClosestEnemyUnit(_playerID, ourUnit.ID(), false));
        const Unit & enemy2 (state.getClosestOurUnit(enemy.player(), enemy.ID()));
        
        //efetua a cópia do estado 
        copiarStateCleanUnit(state, newState);
        

        //adiciona as unidades da abstração
        newState.addUnitWithID(ourUnit);       
        newState.addUnitWithID(enemy);
        

       
        alphaBeta->doSearch(newState);
     
        //moveVec.assign(alphaBeta->getResults().bestMoves.begin(), alphaBeta->getResults().bestMoves.end());
        //moveVec.insert(moveVec.cend(),alphaBeta->getResults().bestMoves.begin(), alphaBeta->getResults().bestMoves.end());        
        Action mov = alphaBeta->getResults().bestMoves[0];
        moveVec.push_back( Action((int)u,mov.player(), mov.type(), mov.index(), mov.pos() ));
        
    }   
    
    //std::cout<<"************* INICIO Generator  **************"<<std::endl;
    //for(auto & ac : moveVec){
    //    std::cout<<ac.debugString()<<std::endl;
    //}
    //std::cout<<"************* FIM Generator  **************"<<std::endl;
}

bool evalUnitDistance(const Unit& u1, const Unit& u2){
    return u1.position() < u2.position();
}

/*
 * Retorna todas as unidades de um player ordenados pela distância da unidade
 * passada como referência
 */
void GenerationClass::listaOrdenada(const IDType& playerID, const Unit & unidade, GameState& state, std::vector<Unit> & unidades){
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
    unidades.push_back(unidade);
    for(myIt = unitsOrder.begin(); myIt != unitsOrder.end(); myIt++){
        unidades.push_back(myIt->second);
    }   
}
/*
 * Função tem o objetivo de realizar a cópia do state e já devolver ela 
 * com as unidades zeradas, mantando assim as outras informações 
 * necessárias.
 */
void GenerationClass::copiarStateCleanUnit(GameState& origState, GameState& copState){
    copState = origState;
    copState.cleanUpStateUnits();
}

void GenerationClass::aplicarAlphaBeta(GameState& state, const MoveArray& moves, std::vector<Action>& moveVec){
    //inicia o player alpha beta para utilização
    
}

//inicializa o player alpha beta com as configurações necessárias para executar
//os testes relacionados à classe

void GenerationClass::iniciarAlphaBeta() {
    
    // convert them to the proper enum types
    int moveOrderingID = 0;
    int evalMethodID = 2;
    int playoutScriptID1 = 7;
    int playoutScriptID2 = 7;
    int playerToMoveID = 1;
    int opponentModelID = 12;

    // construct the parameter object
    AlphaBetaSearchParameters params;

    // give the default parameters we can't set via options
    params.setMaxDepth(50);
    params.setSearchMethod(SearchMethods::IDAlphaBeta);

    // set the parameters from the options in the file
    params.setMaxPlayer(_playerID);
    params.setTimeLimit(40);
    params.setMaxChildren(20);
    params.setMoveOrderingMethod(moveOrderingID);
    params.setEvalMethod(evalMethodID);
    params.setSimScripts(playoutScriptID1, playoutScriptID2);
    params.setPlayerToMoveMethod(playerToMoveID);

    // add scripts for move ordering
    if (moveOrderingID == MoveOrderMethod::ScriptFirst) {
        params.addOrderedMoveScript(PlayerModels::NOKDPS);
        params.addOrderedMoveScript(PlayerModels::KiterDPS);
        params.addOrderedMoveScript(PlayerModels::Cluster);
        //params.addOrderedMoveScript(PlayerModels::AttackWeakest);
    }

    // set opponent modeling if it's not none
    if (opponentModelID != PlayerModels::None) {
        if (_playerID == 0) {
            params.setSimScripts(playoutScriptID1, opponentModelID);
            params.setPlayerModel(1, playoutScriptID2);
        } else {
            params.setSimScripts(opponentModelID, playoutScriptID2);
            params.setPlayerModel(0, playoutScriptID1);
        }
    }

    //PlayerPtr abPlayer(new Player_AlphaBeta(_playerID, params, TTPtr((TranspositionTable *) NULL)));
    alphaBeta = new AlphaBetaSearchAbstract(params, TTPtr((TranspositionTable *) NULL));
    
}





