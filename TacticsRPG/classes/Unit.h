#pragma once

#include <string>
#include "GridPosition.h"

class Grid;

enum class Team {
    PLAYER,
    ENEMY
};

class Unit {
    public:
        Unit(std::string name, Team team, GridPosition position, int MovementRange);

        void draw(const Grid& grid);

        const std::string& getName() const;
        Team getTeam() const;
        const GridPosition& getPosition() const;
        int getMovementRange() const;

        void setPosition(GridPosition newPosition);

    private:
        std::string m_name;
        Team m_team;
        GridPosition m_position;
        int movementRange;
};