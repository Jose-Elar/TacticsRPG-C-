#include "Unit.h"
#include "Grid.h"
#include "Constants.h"
#include "raylib.h"
#include <utility>


Unit::Unit(std::string name, Team team, GridPosition position, int MovementRange)
    : m_name(std::move(name)), m_team(team), m_position(position), movementRange(MovementRange)
    {}


void Unit::draw(const Grid& grid) {
    Vector2 center = grid.gridToScreenPosition(m_position);
    center.x += GameConfig::TILE_SIZE / 2.0f;
    center.y += GameConfig::TILE_SIZE / 2.0f;

    float radius = GameConfig::TILE_SIZE * 0.35f;
    Color color = (m_team == Team::PLAYER) ? BLUE : RED;

    DrawCircleV(center, radius, color);
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, BLACK);
}

const std::string& Unit::getName() const {
    return m_name;
}

Team Unit::getTeam() const {
    return m_team;
}

const GridPosition& Unit::getPosition() const {
    return m_position;
}

int Unit::getMovementRange() const {
    return movementRange;
}

void Unit::setPosition(GridPosition newPosition) {
    m_position = newPosition;
}
