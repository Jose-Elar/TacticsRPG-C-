#pragma once

#include "Action.h"

class MoveAction: public Action{
    public:
        std::string getName() const override;
        bool isAvailable(const Unit& actor) const override;
        std::vector<GridPosition> getValidTargets(const Battle& battle, size_t actorIndex) const override;
        void execute(Battle& battle, size_t actorIndex, GridPosition target) const override;
        void drawPreview(const Battle& battle, size_t actorIndex, GridPosition hoveredTile) const override;

        std::string getHint() const override;
};