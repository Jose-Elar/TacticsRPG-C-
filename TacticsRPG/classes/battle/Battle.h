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

        //Used by Actions
        const Grid& getGrid() const ;
        const Unit& getUnit(size_t unitIndex) const;
        Unit& getUnit(size_t unitIndex);
        std::vector<GridPosition> getUnitPositions() const;
    
    private:
        // Helper functions
        std::optional<size_t> findUnitAt(GridPosition position) const;
        bool isAnyUnitMoving() const;
        bool canSelect(size_t unitIndex) const;
        bool hasTeamFinished(Team team) const;

        //Actions
        void selectUnit(size_t unitIndex);
        void chooseAction(size_t actionIndex);
        void executeSelectedAction(GridPosition target);
        void clearSelection();
        void endTurn();

        //Draw
        void drawWorld() const;
        void drawUI() const;

        Grid m_grid;
        std::vector<Unit> m_units;

        Team m_currentTeam;
        int m_turnNumber;

        std::optional<size_t> m_selectedUnit;
        std::optional<size_t> m_selectedAction;
        std::vector<GridPosition> m_validTargets;
        //std::vector<GridPosition> m_reachableTiles;

        std::optional<GridPosition> m_hoveredTile;
        std::vector<GridPosition> m_previewPath;
        bool m_unitsMoving;
};