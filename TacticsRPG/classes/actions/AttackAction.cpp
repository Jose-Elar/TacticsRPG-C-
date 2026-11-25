#include "AttackAction.h"
#include "Battle.h"
#include "Constants.h"
#include "Grid.h"
#include "Unit.h"
#include "raylib.h"
#include <algorithm>
#include <optional>

std::string AttackAction:: getName() const {
    return "Attack";
}

std::string AttackAction:: getHint() const {
    return "Click an enemy in range to attack";
}

std::vector<GridPosition> AttackAction::getValidTargets(const Battle& battle, size_t actorIndex ) const {
    const Unit& attacker = battle.getUnit(actorIndex);
    std::vector<GridPosition> targets;

    for (size_t i = 0; i < battle.getUnitCount(); i ++){
        const Unit& other = battle.getUnit(i);

        if( !other.isAlive() || other.getTeam() == attacker.getTeam()){
            continue;
        }

        if ( attacker.getPosition().distanceTo(other.getPosition())  <= attacker.getAttackRange() ){
            targets.push_back(other.getPosition());
        }
    }


    return targets;
} 

void AttackAction::execute(Battle& battle, size_t actorIndex, GridPosition target) const {
    std::optional<size_t> targetIndex = battle.findUnitAt(target);

    if(!targetIndex.has_value()){
        return;
    }

    Unit& attacker = battle.getUnit(actorIndex);
    Unit& defender = battle.getUnit(*targetIndex);

    defender.takeDamage(attacker.getAttackPower());
    attacker.finishTurn();
}

void AttackAction::drawPreview (const Battle& battle, size_t actorIndex, GridPosition hoveredTile) const {
    std::optional<size_t> targetIndex = battle.findUnitAt(hoveredTile);
    if (!targetIndex.has_value()){
        return;
    }

    const Unit& attacker = battle.getUnit(actorIndex);
    const Unit& defender = battle.getUnit(*targetIndex);

    int damage = attacker.getAttackRange();
    int remaining = std::max(0, defender.getHealth() - damage);

    const char* text = TextFormat("-%d  (%d left )", damage,remaining);
    int textWidth = MeasureText(text, 18);
    Vector2 center = battle.getGrid().tileCenter(hoveredTile);
    DrawText(text, static_cast<int>(center.x) - textWidth / 2, static_cast<int>(center.y) - GameConfig::TILE_SIZE, 18, RED);
}