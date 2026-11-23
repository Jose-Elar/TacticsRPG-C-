#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "GridPosition.h"

class Battle;
class Unit;

class Action {
    public:
        virtual ~Action() = default;

        virtual std::string getName() const = 0;
        virtual bool isAvailable(const Unit& actor) const;
        virtual std::vector<GridPosition> getValidTargets(const Battle& battle, size_t actorIndex) const = 0;
        virtual void execute(Battle& battle, size_t actorIndex, GridPosition target) const = 0;
        virtual void drawPreview(const Battle& battle, size_t actorIndex, GridPosition hoveredTile) const;

        virtual std::string getHint() const;
};