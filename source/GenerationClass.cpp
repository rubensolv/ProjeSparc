
#include "GenerationClass.h"

using namespace SparCraft;

GenerationClass::GenerationClass(const IDType& playerID) {
    _playerID = playerID;
    iniciarAlphaBeta();
}

void GenerationClass::getMoves(GameState& state, const MoveArray& moves, std::vector<Action>& moveVec) {
    moveVec.clear();
    _unAttack.clear();
    //qtd que controla o quantitativo de unidades na abstração
    int qtdUnPlayerAbstr = 4;
    //estado que será utilizado para simular as variações necessárias do AB
    GameState newState;    
        
    //obtem nossa unidade inicial.
    const Unit & ourUnit           (state.getUnit(_playerID,(0)));        
    
    //vetores ordenados por distancia que conterão as unidades
    std::vector<Unit> unidadesAliadas ;    
    std::vector<Unit> unidadesInimigas;
    
    listaOrdenadaForMoves(_playerID, ourUnit, state, unidadesAliadas, moves);
    //listaOrdenada(state.getEnemy(_playerID), ourUnit, state, unidadesInimigas);
    
    copiarStateCleanUnit(state, newState);
    for(auto & un : unidadesAliadas){
        //para mudanças na forma de escolha das unidades associadas
        if(newState.numUnits(_playerID) == 0){
            listaOrdenada(state.getEnemy(_playerID), un, state, unidadesInimigas);
        }
        if(qtdUnPlayerAbstr != newState.numUnits(_playerID)){
            //adiciono o número de unidades que desejamos na abstração
            newState.addUnitWithID(un);
            //listaOrdenada(state.getEnemy(_playerID), un, state, unidadesInimigas);
            newState.addUnitWithID(getEnemyClosestvalid(newState, unidadesInimigas));
        }
        if(qtdUnPlayerAbstr == newState.numUnits(_playerID)
                or newState.numUnits(_playerID) == moves.numUnits()){
            doAlphaBeta(newState, moveVec, state);
                        
            copiarStateCleanUnit(state, newState);
        }
    }
    
}

//função para rodar a abstração que será testada.
//entrada: Novo GameState com as unidades testadas. Vetor de ações que será retornado. GameState original.
void GenerationClass::doAlphaBeta(GameState & newState, std::vector<Action> & moveVec, GameState & state){
    //executa a busca
            alphaBeta->doSearch(newState);
            for(auto & mov : alphaBeta->getResults().bestMoves){
                if(mov.type() == ActionTypes::ATTACK){
                    moveVec.push_back( Action( state.getIndexUnit(_playerID, newState.getUnit(_playerID, mov.unit()).ID() ),
                                                mov.player(), 
                                                mov.type(), 
                                                state.getIndexUnit(state.getEnemy(_playerID), 
                                                newState.getUnit(state.getEnemy(_playerID), mov.unit()).ID()), 
                                              mov.pos() ));
                    //insere no vetor qual unidade aliada está atacando
                    addAttack(newState.getUnit(state.getEnemy(_playerID), mov.unit()), newState.getUnit(_playerID, mov.unit()));
                }else{
                    moveVec.push_back( Action(state.getIndexUnit(_playerID, newState.getUnit(_playerID, mov.unit()).ID() ),
                                                mov.player(), 
                                                mov.type(), 
                                                mov.index(), 
                                            mov.pos() ));
                }    
            }
}

//adicionar uma unidade no vetor de controle de ataque de unidades.
void GenerationClass::addAttack(const Unit& unitEnemy, const Unit& unitAttack){
    if(_unAttack.find(unitEnemy) == _unAttack.end()){
        //não foi encontrado. Insere no map
        std::vector<Unit> unitsAttack;
        unitsAttack.push_back(unitAttack);
        _unAttack[unitEnemy] = unitsAttack;
    }else{
        //apenas atualiza as unidades atacantes
        _unAttack.find(unitEnemy)->second.push_back(unitAttack);
    }
}

//utilizada para debug e verificação do map de controle de ataques.
void GenerationClass::printMapAttack(){
    std::cout<<" ********************************** "<<std::endl;        
    std::cout<<" Relatório de unidades atacadas "<<std::endl;        
    for (std::map<Unit, std::vector<Unit> >::const_iterator it= _unAttack.begin(); it!= _unAttack.end(); ++it){
        std::cout<<" Unidade atacada= "<<std::endl;
        it->first.print();
        std::cout<<" Unidades atacantes= "<<std::endl;
        for(auto & un : it->second){
            un.print();
        }
        std::cout<<" "<<std::endl;
    }
    std::cout<<" ********************************** "<<std::endl;            
}

//Verifica qual a unidade válida para inclusão 
Unit GenerationClass::getEnemyClosestvalid(GameState& state, std::vector<Unit> unidadesInimigas){
    for(auto & un : unidadesInimigas){
        if( !state.unitExist(un.player(), un.ID())){
            if(unitNeedMoreAttackForKilled(un)){
                return un;
            }
        }
    }
}



bool evalUnitDistance(const Unit& u1, const Unit& u2){
    return u1.position() < u2.position();
}


std::vector<Unit> GenerationClass::retornaQtdUnidadesMaisProximas(const IDType& playerId, const Unit& unitRef, GameState& state, int qtdUnidades){
    std::vector<Unit> unidades ;    
    std::vector<Unit> unidadesRetorno ;
    listaOrdenada(playerId, unitRef, state, unidades);
    for(int x = 0;x < qtdUnidades; x++){
        unidadesRetorno.push_back(unidades[x]);
    }
}

/*
 * Retorna todas as unidades de um player ordenados pela distância da unidade
 * passada como referência (inclusive a unidade passada como ponto de referencia)
 * levando em consideração as unidades que tem movimento válido
 */
void GenerationClass::listaOrdenadaForMoves(const IDType& playerID, const Unit& unidade, GameState& state, std::vector<Unit>& unidades, const MoveArray& moves) {
    unidades.clear();
    //declaração
    Unit t;
    std::map<size_t, Unit> unitsOrder;
    std::map<size_t, Unit>::iterator myIt;
    
    //obtenho posição atual da unidade base
    Position currentPos = unidade.currentPosition(state.getTime());
        
    for(IDType u(0); u<moves.numUnits();++u){        
        t = state.getUnit(playerID,u);        
        if(!unidade.equalsID( t ) ){
            //size_t distSq(currentPos.getDistanceSq(t.currentPosition(state.getTime())));
            size_t distSq(getDistManhantan(currentPos, t.currentPosition(state.getTime())) );
            //size_t distSq(getDistEuclidiana(currentPos, t.currentPosition(state.getTime())) );
            unitsOrder[distSq] = t;
        }
    }
    if(unidade.player() == playerID){
        unidades.push_back(unidade);
    }
    for(myIt = unitsOrder.begin(); myIt != unitsOrder.end(); myIt++){
        unidades.push_back(myIt->second);
    }  
}


/*
 * Retorna todas as unidades de um player ordenados pela distância da unidade
 * passada como referência (inclusive a unidade passada como ponto de referencia)
 */
void GenerationClass::listaOrdenada(const IDType& playerID, const Unit & unidade, GameState& state, std::vector<Unit> & unidades){
    unidades.clear();
    //declaração
    Unit t;
    std::map<size_t, Unit> unitsOrder;
    std::map<size_t, Unit>::iterator myIt;
    //obtenho posição atual da unidade base
    Position currentPos = unidade.currentPosition(state.getTime());
        
    for(IDType u(0); u<state.numUnits(playerID);++u){        
        t = state.getUnit(playerID,u);        
        if(!unidade.equalsID( t ) ){
            //size_t distSq(currentPos.getDistanceSq(t.currentPosition(state.getTime())));
            size_t distSq(getDistManhantan(currentPos, t.currentPosition(state.getTime())) );
            //size_t distSq(getDistEuclidiana(currentPos, t.currentPosition(state.getTime())) );
            unitsOrder[distSq] = t;
        }
    }
    if(unidade.player() == playerID){
        unidades.push_back(unidade);
    }
    //std::cout<<"***************** Distâncias *****************"<<std::endl;
    //unidade.print();
    for(myIt = unitsOrder.begin(); myIt != unitsOrder.end(); myIt++){
        
        //std::cout<<"Dist: "<<myIt->first<<std::endl;
        //myIt->second.print();
        unidades.push_back(myIt->second);
        
    }   
    //std::cout<<"***************** FIM Distâncias *****************"<<std::endl;
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
        //params.addOrderedMoveScript(PlayerModels::Cluster);
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

//função para cálculo da distância baseada na fórmula de cálculo da Distância Manhantan
const PositionType GenerationClass::getDistManhantan(const Position& pInicial, const Position& pFinal) {
    return  abs(pInicial.x() - pFinal.x()) + abs(pInicial.y() - pFinal.y());
}

//função para cálculo da distância baseada na fórmua de cálculo utilizando a Distância Euclidiana
const PositionType GenerationClass::getDistEuclidiana(const Position& pInicial, const Position& pFinal) {
    return sqrt(( (pInicial.x() - pFinal.x())*(pInicial.x() - pFinal.x()) +
                  (pInicial.y() - pFinal.y())*(pInicial.y() - pFinal.y() ) 
                ));
}

//função utilizada para validar se existe a necessidade de mais um ataque para
//uma unidade inimiga ou se ela já irá morrer com os ataques existentes.
const bool GenerationClass::unitNeedMoreAttackForKilled(Unit& un) {
    if( _unAttack.find(un) == _unAttack.end()) {
        return true;
    }
    HealthType HpAtual = un.currentHP();
    for(auto & unAttack : _unAttack.find(un)->second){
        HpAtual -= unAttack.damage();
        if(HpAtual <= 0){
            return false;
        }
    }
    
    return true;
}








