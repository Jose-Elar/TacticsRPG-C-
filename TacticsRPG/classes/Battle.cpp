#include "Battle.h"
#include "Constants.h"
#include "Pathfinding.h"
#include "raylib.h"

Battle::Battle()
    : m_grid(GameConfig::GRID_COLUMNS, GameConfig::GRID_ROWS),
      m_units(),
      m_currentTeam(Team::PLAYER),
      m_turnNumber(1),
      m_selectedUnit(),
      m_hoveredTile(),
      m_previewPath(),
      m_unitsMoving(false)
{
    // Initialize units
    m_units.emplace_back("Knight", Team::PLAYER, GridPosition{2, 4}, 4);
    m_units.emplace_back("Archer", Team::PLAYER, GridPosition{2, 6}, 5);
    m_units.emplace_back("Goblin", Team::ENEMY, GridPosition{12, 4}, 4);
    m_units.emplace_back("Orc", Team::ENEMY, GridPosition{12, 6}, 3);
}

// FrameStages

void Battle::handleInput()
{
    m_hoveredTile = m_grid.tileAt(GetMousePosition());
    m_unitsMoving = isAnyUnitMoving();

    if (m_unitsMoving)
    {
        return;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        endTurn();
        return;
    }

    if (m_selectedUnit.has_value())
    {
        const Unit &unit = m_units[*m_selectedUnit];
        for (size_t i = 0; i < unit.getActionCount() && i < 9; i++)
        {
            if (IsKeyPressed(KEY_ONE + static_cast<int>(i)))
            {
                chooseAction(i);
            }
        }
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || !m_hoveredTile.has_value())
    {
        return;
    }

    if (m_selectedAction.has_value() && Pathfinding::containsPosition(m_validTargets, *m_hoveredTile))
    {
        executeSelectedAction(*m_hoveredTile);
        return;
    }

    std::optional<size_t> clickedUnit = findUnitAt(*m_hoveredTile);
    if (clickedUnit.has_value() && canSelect(*clickedUnit))
    {
        selectUnit(*clickedUnit);
    }
    else
    {
        clearSelection();
    }
}

void Battle::update(float deltaTime)
{
    for (Unit &unit : m_units)
    {
        unit.update(deltaTime);
    }

    if (!isAnyUnitMoving() && hasTeamFinished(m_currentTeam))
    {
        endTurn();
    }
    /*
        m_previewPath.clear();
        if (m_selectedUnit.has_value() && m_hoveredTile.has_value() && Pathfinding::containsPosition(m_reachableTiles, *m_hoveredTile))
        {
            m_previewPath = Pathfinding::findPath(m_grid, m_units[*m_selectedUnit].getPosition(), *m_hoveredTile, getUnitPositions());
        }
        else
        {
            m_previewPath.clear();
        }
            */
}

void Battle::draw() const
{
    drawWorld();
    drawUI();
}

// Used by Actions
const Grid &Battle::getGrid() const
{
    return m_grid;
}

const Unit &Battle::getUnit(size_t unitIndex) const
{
    return m_units[unitIndex];
}

Unit &Battle::getUnit(size_t unitIndex)
{
    return m_units[unitIndex];
}

std::vector<GridPosition> Battle::getUnitPositions() const
{
    std::vector<GridPosition> positions;
    for (const Unit &unit : m_units)
    {
        positions.push_back(unit.getPosition());
    }
    return positions;
}

// Helper Functions
std::optional<size_t> Battle::findUnitAt(GridPosition position) const
{
    for (size_t i = 0; i < m_units.size(); ++i)
    {
        if (m_units[i].getPosition() == position)
        {
            return i;
        }
    }
    return std::nullopt;
}

bool Battle::isAnyUnitMoving() const
{
    for (const Unit &unit : m_units)
    {
        if (unit.isMoving())
        {
            return true;
        }
    }
    return false;
}

bool Battle::canSelect(size_t unitIndex) const
{
    const Unit &unit = m_units[unitIndex];
    return unit.getTeam() == m_currentTeam && !unit.hasFinishedTurn();
}

bool Battle::hasTeamFinished(Team team) const
{
    for (const Unit &unit : m_units)
    {
        if (unit.getTeam() == team && !unit.hasFinishedTurn())
        {
            return false;
        }
    }
    return true;
}

// Actions

void Battle::selectUnit(size_t unitIndex)
{
    m_selectedUnit = unitIndex;
    m_selectedAction.reset();
    m_validTargets.clear();

    const Unit &unit = m_units[unitIndex];
    for (size_t i = 0; i < unit.getActionCount(); i++)
    {
        if (unit.getAction(i).isAvailable(unit))
        {
            chooseAction(i);
            return;
        }
    }
}

void Battle::chooseAction(size_t actionIndex)
{
    const Unit &unit = m_units[*m_selectedUnit];
    const Action &action = unit.getAction(actionIndex);

    if (!action.isAvailable(unit))
    {
        return;
    }

    m_selectedAction = actionIndex;
    m_validTargets = action.getValidTargets(*this, *m_selectedUnit);
}

void Battle::executeSelectedAction(GridPosition target)
{
    size_t unitIndex = *m_selectedUnit;
    const Action &action = m_units[unitIndex].getAction(*m_selectedAction);

    clearSelection();
    action.execute(*this, unitIndex, target);
}

void Battle::clearSelection()
{
    m_selectedUnit.reset();
    m_selectedAction.reset();
    m_validTargets.clear();
}

void Battle::endTurn()
{
    clearSelection();

    for (Unit &unit : m_units)
    {
        unit.resetTurn();
    }

    if (m_currentTeam == Team::PLAYER)
    {
        m_currentTeam = Team::ENEMY;
    }
    else
    {
        m_currentTeam = Team::PLAYER;
        m_turnNumber++;
    }
}

// Draw

void Battle::drawWorld() const
{
    m_grid.draw();

    for (const GridPosition &tile : m_validTargets)
    {
        m_grid.drawHighlightedTile(tile, SKYBLUE);
    }

    if (m_hoveredTile.has_value())
    {
        Color highlightColor = m_grid.isWalkable(*m_hoveredTile) ? YELLOW : RED;
        m_grid.drawHighlightedTile(*m_hoveredTile, highlightColor);
    }

    if (m_selectedUnit.has_value())
    {
        m_grid.drawHighlightedTile(m_units[*m_selectedUnit].getPosition(), GREEN);
    }

    if (m_selectedUnit.has_value() && m_selectedAction.has_value() && m_hoveredTile.has_value() && Pathfinding::containsPosition(m_validTargets, *m_hoveredTile))
    {
        const Action &action = m_units[*m_selectedUnit].getAction(*m_selectedAction);
        action.drawPreview(*this, *m_selectedUnit, *m_hoveredTile);
    }

    for (const Unit &unit : m_units)
    {
        unit.draw(m_grid);
    }
}

void Battle::drawUI() const
{
    constexpr int panelY = GameConfig::GRID_ROWS * GameConfig::TILE_SIZE;
    constexpr int rightColumnX = GameConfig::WINDOW_WIDTH - 200;

    const char *phaseName = (m_currentTeam == Team::PLAYER) ? "Player phase" : "Enemy phase";
    Color phaseColor = (m_currentTeam == Team::PLAYER) ? BLUE : MAROON;
    DrawText(TextFormat("Turn %d - %s", m_turnNumber, phaseName), 10, panelY + 10, 20, phaseColor);

    if (m_selectedUnit.has_value())
    {
        const Unit &unit = m_units[*m_selectedUnit];

        if (m_selectedAction.has_value())
        {
            const Action &action = unit.getAction(*m_selectedAction);
            DrawText(TextFormat("%s: %s", unit.getName().c_str(), action.getHint().c_str()), 10, panelY + 40, 20, DARKGRAY);
        }
        else
        {
            DrawText(TextFormat("Selected: %s", unit.getName().c_str()), 10, panelY + 40, 20, DARKGRAY);
        }
    }
    else if (m_unitsMoving)
    {
        DrawText("Moving...", 10, panelY + 40, 20, DARKGRAY);
    }
    else
    {
        DrawText("Click one of your units to select it", 10, panelY + 40, 20, DARKGRAY);
    }

    if (m_selectedUnit.has_value())
    {
        const Unit &unit = m_units[*m_selectedUnit];
        int x = 10;
        for (size_t i = 0; i < unit.getActionCount(); i++)
        {
            const Action &action = unit.getAction(i);

            Color color = DARKGRAY;
            if (!action.isAvailable(unit))
            {
                color = LIGHTGRAY;
            }
            else if (m_selectedAction == i)
            {
                color = DARKBLUE;
            }

            const char *label = TextFormat("%d: %s", static_cast<int>(i + 1), action.getName().c_str());
            DrawText(label, x, panelY + 70, 20, color);
            x += MeasureText(label, 20) + 30;
        }
    }

    DrawText("SPACE: end turn", rightColumnX, panelY + 70, 20, GRAY);

    if (m_hoveredTile.has_value())
    {
        DrawText(TextFormat("Tile: %d, %d", m_hoveredTile->x, m_hoveredTile->y),
                 rightColumnX, panelY + 10, 20, DARKGRAY);
    }
    else
    {
        DrawText("Tile: none", rightColumnX, panelY + 10, 20, DARKGRAY);
    }
}