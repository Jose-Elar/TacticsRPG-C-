#pragma once

#include "Action.h"

class AttackAction : public Action {
    public:
        std::string getName() const override;
        std::string getHint() const override;
        std::vector<GridPosition> getValidTargets(const Battle& battle, size_t actorIndex) const override;
        void execute(Battle& battle, size_t actorIndex, GridPosition target) const override;
        void drawPreview(const Battle& battle, size_t actorIndex, GridPosition hoveredTile)const override;

};