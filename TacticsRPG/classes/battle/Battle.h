#pragma once

#include <optional>
#include <vector>
#include "Grid.h"
#include "Unit.h"
#include "GridPosition.h"

class Battle {
    public:
        Battle();

        void handleInput();
        void update(float deltaTime);
        void draw() const ;

        bool isOver() const;

        //Used by Actions
        const Grid& getGrid() const ;
        size_t getUnitCount() const;

        const Unit& getUnit(size_t unitIndex) const;
        Unit& getUnit(size_t unitIndex);

        std::vector<GridPosition> getUnitPositions() const;
        std::optional<size_t> findUnitAt(GridPosition position) const; //Changed for AttackAction Needs
    
    private:
        // Helper functions

        bool isAnyUnitMoving() const;
        bool canSelect(size_t unitIndex) const;
        bool canUseAction(size_t unitIndex, size_t actionIndex) const;
        bool hasTeamFinished(Team team) const;
        bool hasLivingUnits(Team team) const;

        //Actions
        void selectUnit(size_t unitIndex);
        void chooseAction(size_t actionIndex);
        void executeSelectedAction(GridPosition target);
        void clearSelection();
        void endTurn();
        void checkForWinner();

        //Draw
        void drawWorld() const;
        void drawUI() const;
        void drawGameOver() const; 

        Grid m_grid;
        std::vector<Unit> m_units;

        Team m_currentTeam;
        int m_turnNumber;
        std::optional<Team> m_winner;

        std::optional<size_t> m_selectedUnit;
        std::optional<size_t> m_selectedAction;
        std::vector<GridPosition> m_validTargets;

        std::optional<GridPosition> m_hoveredTile;
        bool m_unitsMoving;
};