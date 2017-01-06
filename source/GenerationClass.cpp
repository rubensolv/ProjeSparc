
#include "GenerationClass.h"

using namespace SparCraft;

GenerationClass::GenerationClass(const IDType& playerID) {
    _playerID = playerID;
    iniciarAlphaBeta();
}

void GenerationClass::getMoves(GameState& state, const MoveArray& moves, std::vector<Action>& moveVec) {
    state.print();
    //estado que será utilizado para simular as variações necessárias do AB
    GameState newState;
    //vetores ordenados por distancia que conterão as unidades
    std::vector<Unit> unidadesAliadas;
    std::vector<Unit> unidadesInimigas;
    moveVec.clear();
    _unAttack.clear();

    //obtem nossa unidade inicial.
    const Unit & ourUnit(state.getUnit(_playerID, (0)));

    listaOrdenadaForMoves(_playerID, ourUnit, state, unidadesAliadas, moves);

    int qtdUnPlayerAbstr = 4;
    int control = 0;
    
    for (auto & un : unidadesAliadas) {
        control++;
        //para mudanças na forma de escolha das unidades associadas
        if (newState.numUnits(_playerID) == 0) {
            listaOrdenada(state.getEnemy(_playerID), un, state, unidadesInimigas);
        }
        if(!(_UnReut.empty())){
            std::cout<<"()()()Adicionando unidades para reaproveitamento"<<std::endl;
            //adiciono as unidades à abstração
            for(auto & un : _UnReut){
                newState.addUnitWithID(un);
                un.print();
            }
            _UnReut.clear();
            std::cout<<"()()()FIM Adicionando unidades para reaproveitamento"<<std::endl;
        }
        if (qtdUnPlayerAbstr != newState.numUnits(_playerID)) {
            //adiciono o número de unidades que desejamos na abstração
            if(!newState.unitExist(_playerID, un.ID())){
                newState.addUnitWithID(un);
            }
            
            
        }
        if (qtdUnPlayerAbstr == newState.numUnits(_playerID)
                or unidadesAliadas.size() == control) {
            
            //newState.addUnitWithID(getEnemyClosestvalid(newState, unidadesInimigas));
            newState.addUnitWithID(getCalculateEnemy(newState, unidadesInimigas));
            
            doAlphaBeta(newState, moveVec, state);
            //função que analisa o moveVec em busca de ataques desnecessários
            removeLoseAttacks(newState, moveVec, state);
            
            copiarStateCleanUnit(state, newState);
        }
    }
    
    
    
     std::cout<<"##################################################"<<std::endl;
    std::cout<<"************* INICIO GenerationClass  **************"<<std::endl;
    for(auto & ac : moveVec){
        std::cout<<ac.debugString()<<std::endl;
    }
    std::cout<<"************* FIM GenerationClass  **************"<<std::endl;
    std::cout<<"##################################################"<<std::endl;
    /*
    printMapAttack()
    */
    
}


//função que analisa os movimentos sugeridos pelo AB e busca encontrar ataques perdidos
//funciona apenas com 1 unidade inimiga
void GenerationClass::removeLoseAttacks(GameState& newState, std::vector<Action>& moveVec, GameState & state){
    _UnReut.clear();
    Unit unAval;
    
    std::cout<<"************* INICIO Analise de atacks perdidos  **************"<<std::endl;
    
    //obtenho unidade que está sendo atacada  -- indice zero pois iremos obter a unica unidade inimiga na abstração.
    Unit & enemy = newState.getUnit(newState.getEnemy(_playerID), 0);
    std::cout<<"Unidadade inimiga= "; enemy.print();
    
    HealthType enemyHP =  enemy.currentHP();
    
    for(auto & mov : moveVec){
        if(mov.type() == ActionTypes::ATTACK
            and enemy.ID() == state.getUnit(newState.getEnemy(_playerID), mov.index()).ID() 
                ){
            std::cout<<"-- HP analisado= "<< enemyHP<< std::endl;
            //busco verificar se a unidade que foi atacada conseguiu ser eliminada sem necessitar de todas as ações do vetor
            std::cout<<"-- Movimento de ataque= "<< mov.debugString()<< std::endl;
            //obtenho a unidade relacionada ao movimento
            unAval = state.getUnit(_playerID, mov.unit()); 
            if(enemyHP >= 0 ){
                enemyHP = enemyHP - unAval.getDamageTo(enemy);
            }else{
                std::cout<<"--- Ação considerada desnecessária. HP Atual= "<<enemyHP <<" "<< std::endl;
                //caso o Hp seja zero, esta será uma ação perdida. Removo do vetor.
                removeActionInVector(mov,moveVec);
                //adiciono a unidade aliada para nova abstração
                _UnReut.push_back(unAval);
            }
        }
    }
    
    
    std::cout<<"************* FIM Analise de atacks perdidos  **************"<<std::endl;
}


//função utilizada para remoção das ações de dentro do vetor moveVec
void GenerationClass::removeActionInVector(Action& action, std::vector<Action>& moveVec){
    std::vector<Action> newMoveVec;
    for(auto & mov : moveVec){
        if(!(mov == action)){
            newMoveVec.push_back(mov);
        }
    }
    moveVec = newMoveVec;
}



//Esta função tem como objetivo definir e escolher a melhor unidade a ser atacada com base na abstração informada
//Comportamento inicial: Será criada a média da distância Euclidiana de todas as unidades aliadas em relação
//às unidades inimigas. Será escolhida a unidade inimiga que tiver a menor distância.
Unit& GenerationClass::getCalculateEnemy(GameState& state, std::vector<Unit> unidadesInimigas){
    std::map<Unit, PositionType> unDistance;
    std::map<Unit, PositionType>::iterator myIt;
    PositionType sum;
    //Utilziados na definição da melhor unidade
    Unit & bestUnit = unidadesInimigas[0];
    PositionType bestPosition = 999999;
    //std::cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<std::endl;
    //std::cout<<"Unidade mais proxima no vetor:";
    //bestUnit.print();
    
    
    for(auto & un : unidadesInimigas){
        sum = 0;
        for(int u(0);u < state.numUnits(_playerID); ++u){
            sum += getDistEuclidiana(state.getUnit(_playerID, u).position(), un.position());
        }
        unDistance[un] = sum/state.numUnits(_playerID);
    }
    //seleciono como base de analise a unidade mais próxima
    //std::cout<<"__Listagem de unidades:"<<std::endl;
    for(myIt = unDistance.begin(); myIt != unDistance.end(); myIt++){
        //std::cout<<"Distancia:"<<myIt->second<<"  ";
        //myIt->first.print();
        if(myIt->second < bestPosition){
            bestPosition = myIt->second;
            bestUnit = myIt->first;
        }
    }
    
    //std::cout<<"Unidade mais proxima por distancia euclidiana:";
    //bestUnit.print();
    
    //aplico outras análises
    bestPosition = 9999999;
    for(myIt = unDistance.begin(); myIt != unDistance.end(); myIt++){
        
        if(myIt->second < bestPosition
                and myIt->first.currentHP() < bestUnit.currentHP()
                and unitNeedMoreAttackForKilled( myIt->first )
          ){
           bestPosition = myIt->second;
           bestUnit = myIt->first;
        }
    }
    //std::cout<<"Unidade mais proxima Escolhida:";
    //bestUnit.print();
    return bestUnit;
    
}



void GenerationClass::getMoves2(GameState& state, const MoveArray& moves, std::vector<Action>& moveVec) {
    
    //vetor de controle de abstrações a serem escolhidas
    std::vector< std::vector<Action> > vecMoves;
    //estado que será utilizado para simular as variações necessárias do AB
    GameState newState;    
    
    //vetores ordenados por distancia que conterão as unidades
    std::vector<Unit> unidadesAliadas;
    std::vector<Unit> unidadesInimigas;
        
    for (int qtdUnPlayerAbstr = 20; qtdUnPlayerAbstr <= moves.numUnits(); ++qtdUnPlayerAbstr) {
        moveVec.clear();
        _unAttack.clear();
        
        //obtem nossa unidade inicial.
        const Unit & ourUnit(state.getUnit(_playerID, (0)));

        listaOrdenadaForMoves(_playerID, ourUnit, state, unidadesAliadas, moves);
        //listaOrdenada(state.getEnemy(_playerID), ourUnit, state, unidadesInimigas);

        int control = 0;
        for (auto & un : unidadesAliadas) {
            control++;
            //para mudanças na forma de escolha das unidades associadas
            if (newState.numUnits(_playerID) == 0) {
                listaOrdenada(state.getEnemy(_playerID), un, state, unidadesInimigas);
            }
            if (qtdUnPlayerAbstr != newState.numUnits(_playerID)) {
                //adiciono o número de unidades que desejamos na abstração
                newState.addUnitWithID(un);
                //listaOrdenada(state.getEnemy(_playerID), un, state, unidadesInimigas);
                newState.addUnitWithID(getEnemyClosestvalid(newState, unidadesInimigas));
            }
            if (qtdUnPlayerAbstr == newState.numUnits(_playerID)
                    or unidadesAliadas.size() == control ) {
                //fim validando
                doAlphaBeta(newState, moveVec, state);
                copiarStateCleanUnit(state, newState);
            }
        }
        vecMoves.push_back(moveVec);
    }
    
    //inicia o processo para escolher qual abstração será utilizada
    GameState evalGame;
    ScoreType ltd2max = 999999;
    std::vector<Action> & moveVecSelected = vecMoves[0];
    
    for(auto & mov : vecMoves){
        evalGame = state;
        evalGame.makeMoves(mov);
        //if(ltd2max > evalGame.LTD2(state.getEnemy(_playerID))){  //diminuir o ltd2 do meu adversário
        //if(ltd2max < evalGame.LTD2_playerID)){  //jogadas que melhorem (aumentem) o meu ltd2
        //if(ltd2max < evalGame.evalLTD2(_playerID)){  //jogadas que melhorem (aumentem) o meu ltd2
        //if(ltd2max < evalGame.eval(_playerID, PlayerModels::NOKDPS, PlayerModels::KiterDPS).val()){  //avaliação utilizando playout
        if(ltd2max > evalGame.eval(state.getEnemy(_playerID), PlayerModels::NOKDPS, PlayerModels::KiterDPS).val()){  //avaliação utilizando playout
            ltd2max = evalGame.evalLTD2(_playerID);
            moveVecSelected = mov;
        }
    }
    moveVec = moveVecSelected;
    
    //std::cout<<"##################################################"<<std::endl;
    //std::cout<<"************* INICIO GenerationClass  **************"<<std::endl;
    //for(auto & ac : moveVec){
    //    std::cout<<ac.debugString()<<std::endl;
    //}
    //std::cout<<"************* FIM GenerationClass  **************"<<std::endl;
    
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
                                                    newState.getUnit(state.getEnemy(_playerID), mov.index()).ID()), 
                                              mov.pos() ));
                    //insere no vetor qual unidade aliada está atacando
                    addAttack(newState.getUnit(state.getEnemy(_playerID), mov.index()), newState.getUnit(_playerID, mov.unit()));
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
    
    unidades.push_back(unidade);
    
    //obtenho posição atual da unidade base
    //Position currentPos = unidade.currentPosition(state.getTime());
        
    for(IDType u(0); u<moves.numUnits();++u){        
        t = state.getUnit(playerID,u);            
        if(!unidade.equalsID( t ) ){
            unidades.push_back(t);
            //size_t distSq(currentPos.getDistanceSq(t.currentPosition(state.getTime())));
            //size_t distSq(getDistManhantan(currentPos, t.currentPosition(state.getTime())) );
            //size_t distSq(getDistEuclidiana(currentPos, t.currentPosition(state.getTime())) );
        }
    }
    sortUnit(unidades, unidade, state);
}

void GenerationClass::sortUnit(std::vector<Unit>& unidades, const Unit& base, GameState & state){
    for(int i = 1; i < unidades.size(); i++) {
		Unit key = unidades[i];
		int j = i - 1;
		while((j >= 0) && ( getDistManhantan(base.currentPosition(state.getTime()),unidades[j].currentPosition(state.getTime()))  
                                    > getDistManhantan(base.currentPosition(state.getTime()), key.currentPosition(state.getTime())))
                               && ( unidades[j].currentHP() >= key.currentHP())
                        ) {
			unidades[j + 1] = unidades[j];
			j--;
		}
		unidades[j + 1] = key;
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
    
    if(unidade.player() == playerID){
        unidades.push_back(unidade);
    }
    //obtenho posição atual da unidade base
    Position currentPos = unidade.currentPosition(state.getTime());
        
    for(IDType u(0); u<state.numUnits(playerID);++u){        
        t = state.getUnit(playerID,u);            
        if(!unidade.equalsID( t ) ){
            unidades.push_back(t);
        }
    }
    sortUnit(unidades, unidade, state);
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
    int evalMethodID = 2;  //testar com 1
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
    params.setTimeLimit(20);
    params.setMaxChildren(0);
    params.setMoveOrderingMethod(moveOrderingID);
    params.setEvalMethod(evalMethodID);
    params.setSimScripts(playoutScriptID1, playoutScriptID2);
    params.setPlayerToMoveMethod(playerToMoveID);

    // add scripts for move ordering
    //if (moveOrderingID == MoveOrderMethod::ScriptFirst) {
        //params.addOrderedMoveScript(PlayerModels::NOKDPS);
        //params.addOrderedMoveScript(PlayerModels::KiterDPS);
        //params.addOrderedMoveScript(PlayerModels::Cluster);
        //params.addOrderedMoveScript(PlayerModels::AttackWeakest);
    //}

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
const bool GenerationClass::unitNeedMoreAttackForKilled(Unit un) {
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







