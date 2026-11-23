#include "MoveAction.h"
#include "Battle.h"
#include "Grid.h"
#include "Pathfinding.h"
#include "Unit.h"
#include "raylib.h"

std::string MoveAction::getName() const {
    return "Move"; //HardCoded for the moment
}

bool MoveAction:: isAvailable(const Unit& unit) const {
    return !unit.hasMoved();
}

std::vector<GridPosition> MoveAction::getValidTargets(const Battle& battle, size_t actorIndex) const {
    const Unit& unit = battle.getUnit(actorIndex);
    return Pathfinding::findReachableTiles(battle.getGrid(),unit.getPosition(),unit.getMovementRange(),battle.getUnitPositions());
}

void MoveAction::execute(Battle& battle, size_t actorIndex, GridPosition target) const {
    Unit& unit = battle.getUnit(actorIndex);
    unit.startMoving(Pathfinding::findPath(battle.getGrid(),unit.getPosition(),target,battle.getUnitPositions()));
}

void MoveAction::drawPreview(const Battle& battle, size_t actorIndex, GridPosition hoveredTile) const{
    const Unit& unit = battle.getUnit(actorIndex);
    const Grid& grid = battle.getGrid();

    std::vector<GridPosition> path = Pathfinding::findPath(grid, unit.getPosition(),hoveredTile,battle.getUnitPositions());

    if(path.empty()) {return;}

    Vector2 previous = grid.tileCenter(unit.getPosition());
    for(const GridPosition& step : path){
        Vector2 current = grid.tileCenter(step);
        DrawLineEx(previous,current,6.0f,DARKBLUE);
        previous = current;
    }
    DrawCircleV(previous,8.0f,DARKBLUE);
}

std::string MoveAction::getHint() const{
    return "Click a blue tile to move there";
}