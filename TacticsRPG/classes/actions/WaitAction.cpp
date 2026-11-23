#include "WaitAction.h"
#include "Battle.h"
#include "Unit.h"

std::string WaitAction::getName() const{
    return "Wait";
}

std::vector<GridPosition> WaitAction::getValidTargets(const Battle& battle, size_t actorIndex) const {
    return std::vector<GridPosition> {battle.getUnit(actorIndex).getPosition()};
}

void WaitAction::execute(Battle& battle, size_t actorIndex, GridPosition) const{
    battle.getUnit(actorIndex).finishTurn();
}

std::string WaitAction::getHint() const {
    return "Click the unit to end its turn";
}
