#include "Battle.h"
#include "Constants.h"
#include "Pathfinding.h"
#include "raylib.h"

Battle::Battle()
    : m_grid(GameConfig::GRID_COLUMNS, GameConfig::GRID_ROWS), m_units(), m_currentTeam(Team::PLAYER), m_turnNumber(1),
      m_selectedUnit(), m_reachableTiles(), m_hoveredTile(), m_previewPath(), m_unitsMoving(false)
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

    if (IsKeyPressed(KEY_SPACE) && !m_unitsMoving)
    {
        endTurn();
        return;
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || m_unitsMoving || !m_hoveredTile.has_value())
    {
        return;
    }

    std::optional<size_t> clickedUnit = findUnitAt(m_hoveredTile.value());

    if (clickedUnit.has_value() && canSelect(clickedUnit.value()))
    {
        selectUnit(clickedUnit.value());
    }
    else if (m_selectedUnit.has_value() && Pathfinding::containsPosition(m_reachableTiles, *m_hoveredTile))
    {
        moveSelectedUnitTo(*m_hoveredTile);
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

    m_previewPath.clear();
    if (m_selectedUnit.has_value() && m_hoveredTile.has_value() && Pathfinding::containsPosition(m_reachableTiles, *m_hoveredTile))
    {
        m_previewPath = Pathfinding::findPath(m_grid, m_units[*m_selectedUnit].getPosition(), *m_hoveredTile, getUnitPositions());
    }
    else
    {
        m_previewPath.clear();
    }
}

void Battle::draw()
{
    drawWorld();
    drawUI();
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

std::vector<GridPosition> Battle::getUnitPositions() const
{
    std::vector<GridPosition> positions;
    for (const Unit &unit : m_units)
    {
        positions.push_back(unit.getPosition());
    }
    return positions;
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
    return unit.getTeam() == m_currentTeam && !unit.hasMoved();
}

bool Battle::hasTeamFinished(Team team) const
{
    for (const Unit &unit : m_units)
    {
        if (unit.getTeam() == team && !unit.hasMoved())
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
    const Unit &unit = m_units[*m_selectedUnit];
    m_reachableTiles = Pathfinding::findReachableTiles(m_grid, unit.getPosition(), unit.getMovementRange(), getUnitPositions());
}

void Battle::moveSelectedUnitTo(const GridPosition &targetPosition)
{
    if (!m_selectedUnit.has_value())
    {
        return;
    }

    Unit &unit = m_units[*m_selectedUnit];
    unit.startMoving(Pathfinding::findPath(m_grid, unit.getPosition(), targetPosition, getUnitPositions()));

    clearSelection();
}

void Battle::clearSelection()
{
    m_selectedUnit.reset();
    m_reachableTiles.clear();
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

void Battle::drawWorld()
{
    m_grid.draw();

    for (const GridPosition &tile : m_reachableTiles)
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

    if (!m_previewPath.empty())
    {
        Vector2 previous = m_grid.tileCenter(m_units[*m_selectedUnit].getPosition());
        for (const GridPosition &step : m_previewPath)
        {
            Vector2 current = m_grid.tileCenter(step);
            DrawLineEx(previous, current, 5.0f, PURPLE);
            previous = current;
        }
    }

    for (Unit &unit : m_units)
    {
        unit.draw(m_grid);
    }
}

void Battle::drawUI() const
{
    constexpr int panelY = GameConfig::GRID_ROWS * GameConfig::TILE_SIZE;

    const char *phaseName = (m_currentTeam == Team::PLAYER) ? "Player phase" : "Enemy phase";
    Color phaseColor = (m_currentTeam == Team::PLAYER) ? BLUE : MAROON;
    DrawText(TextFormat("Turn %d - %s", m_turnNumber, phaseName), 10, panelY + 10, 20, phaseColor);

    if (m_selectedUnit.has_value())
    {
        DrawText(TextFormat("Selected: %s", m_units[*m_selectedUnit].getName().c_str()),10, panelY + 40, 20, DARKGRAY);
    }
    else if (m_unitsMoving)
    {
        DrawText("Moving...", 10, panelY + 40, 20, DARKGRAY);
    }
    else
    {
        DrawText("Click one of your units to select it", 10, panelY + 40, 20, DARKGRAY);
    }

    DrawText("SPACE: end turn", 10, panelY + 70, 20, GRAY);

    if (m_hoveredTile.has_value())
    {
        DrawText(TextFormat("Tile: %d, %d", m_hoveredTile->x, m_hoveredTile->y),GameConfig::WINDOW_WIDTH - 160, panelY + 10, 20, DARKGRAY);
    }
    else
    {
        DrawText("Tile: none", GameConfig::WINDOW_WIDTH - 160, panelY + 10, 20, DARKGRAY);
    }
}