#pragma once

#include <memory>
#include <string>
#include <vector>
#include "GridPosition.h"
#include "Action.h"

class Grid;

enum class Team {
    PLAYER,
    ENEMY
};

class Unit {
    public:
        Unit(std::string name, Team team, GridPosition position, int MovementRange);

        void update(float deltaTime);
        void draw(const Grid& grid) const ;

        void startMoving(std::vector<GridPosition> path);
        bool isMoving() const;

        bool hasMoved() const;
        bool hasFinishedTurn() const;
        void finishTurn();
        void resetTurn();

        size_t getActionCount() const;
        const Action& getAction(size_t index) const;

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

        std::vector<GridPosition> m_path;
        size_t m_pathStep;
        float m_moveProgress;

        bool m_hasMoved;
        bool m_hasActed;

        std::vector<std::unique_ptr<Action>> m_actions;
};