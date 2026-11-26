#include "Unit.h"
#include "Grid.h"
#include "Constants.h"
#include "MoveAction.h"
#include "AttackAction.h"
#include "WaitAction.h"
#include "raylib.h"
#include <utility>
#include <algorithm>

static void drawHealthBar(Vector2 center, int health, int maxHealth){
    const float width = GameConfig::TILE_SIZE * 0.8f;
    const float height = 5.0f;
    const float x = center.x - width / 2.0f;
    const float y = center.y - GameConfig::TILE_SIZE / 2.0f + 2.0f;

    float ratio = static_cast<float>(health) / static_cast<float>(maxHealth);

    DrawRectangleV(Vector2{x,y},Vector2{width,height}, DARKGRAY);
    DrawRectangleV(Vector2{x,y},Vector2{width * ratio, height}, GREEN);
}

Unit::Unit(std::string name, Team team, GridPosition position, UnitStats stats) :
    m_name(std::move(name)),
    m_team(team),
    m_position(position),
    m_stats(stats),
    m_health(stats.maxHealth),
    m_path(),
    m_pathStep(0),
    m_moveProgress(0.0f),
    m_hasMoved(false),
    m_hasActed(false),
    m_actions() {
        m_actions.push_back(std::make_unique<MoveAction>());
        m_actions.push_back(std::make_unique<AttackAction>());
        m_actions.push_back(std::make_unique<WaitAction>());
    }


void Unit::startMoving(std::vector<GridPosition> path) {
    m_path = std::move(path);
    m_pathStep = 0;
    m_moveProgress = 0.0f;
    m_hasMoved = true;
}

bool Unit::isMoving() const {
    return m_pathStep < m_path.size();
}

bool Unit::hasMoved() const {
    return m_hasMoved;
}

bool Unit::hasFinishedTurn() const {
    return m_hasActed;
}

void Unit::finishTurn() {
    m_hasActed = true;
}

void Unit::resetTurn(){
    m_hasMoved = false;
    m_hasActed = false;
}

void Unit::takeDamage(int amount){
    m_health = std::max(0 , m_health - amount);
}

bool Unit::isAlive() const {
    return m_health > 0;
}

int Unit::getHealth() const {
    return m_health;
}

int Unit::getMaxHealth() const{
    return m_stats.maxHealth;
}


size_t Unit::getActionCount() const{
    return m_actions.size();
}

const Action& Unit::getAction(size_t index) const {
    return *m_actions[index];
}


void Unit::update(float deltaTime){
    if(!isMoving()){
        return;
    }

    m_moveProgress += GameConfig::UNIT_MOVE_SPEED * deltaTime;

    while(m_moveProgress >= 1.0f && isMoving()){
        m_moveProgress -= 1.0f;        
        m_position = m_path[m_pathStep];
        m_pathStep++;
    }

    if(!isMoving()){
        m_path.clear();
        m_pathStep = 0;
        m_moveProgress = 0.0f;
    }
}

void Unit::draw(const Grid& grid) const {
    Vector2 center = grid.tileCenter(m_position);

    if(isMoving()){
        Vector2 nextTileCenter = grid.tileCenter(m_path[m_pathStep]);
        center.x = center.x + (nextTileCenter.x - center.x) * m_moveProgress;
        center.y = center.y + (nextTileCenter.y - center.y) * m_moveProgress;
    }

    float radius = GameConfig::TILE_SIZE * 0.35f;
    Color color = (m_team == Team::PLAYER) ? BLUE : RED;

    if (hasFinishedTurn() && !isMoving()) {
        color = Fade(color, 0.4f);
    }

    DrawCircleV(center, radius, color);
    DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, BLACK);

    drawHealthBar(center, m_health, m_stats.maxHealth);
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
    return m_stats.movementRange;
}

int Unit::getAttackPower() const {
    return m_stats.attackPower;
}

int Unit::getAttackRange() const {
    return m_stats.attackRange;
}

void Unit::setPosition(GridPosition newPosition) {
    m_position = newPosition;
}


