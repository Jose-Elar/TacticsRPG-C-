#include "Battle.h"
#include "Constants.h"
#include "Pathfinding.h"
#include "raylib.h"

Battle::Battle()
    : m_grid(GameConfig::GRID_COLUMNS, GameConfig::GRID_ROWS),
      m_units(),
      m_currentTeam(Team::PLAYER),
      m_turnNumber(1),
      m_winner(),
      m_selectedUnit(),
      m_selectedAction(),
      m_validTargets(),
      m_hoveredTile(),
      m_unitsMoving(false)
{
    // Initialize units
    m_units.emplace_back("Knight", Team::PLAYER, GridPosition{2, 4}, UnitStats{.maxHealth = 30, .movementRange = 4, .attackPower = 8, .attackRange = 1});
    m_units.emplace_back("Archer", Team::PLAYER, GridPosition{2, 6}, UnitStats{.maxHealth = 20, .movementRange = 5, .attackPower = 6, .attackRange = 3});
    m_units.emplace_back("Goblin", Team::ENEMY, GridPosition{12, 4}, UnitStats{.maxHealth = 18, .movementRange = 4, .attackPower = 5, .attackRange = 1});
    m_units.emplace_back("Orc", Team::ENEMY, GridPosition{12, 6}, UnitStats{.maxHealth = 32, .movementRange = 3, .attackPower = 9, .attackRange = 1});
}

// FrameStages

void Battle::handleInput()
{
    m_hoveredTile = m_grid.tileAt(GetMousePosition());
    m_unitsMoving = isAnyUnitMoving();

    if (m_unitsMoving || m_winner.has_value())
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

    if(m_winner.has_value() || isAnyUnitMoving())
    {
        return;
    }

    checkForWinner();
    if(m_winner.has_value()){
        clearSelection();
        return;
    }

    if(hasTeamFinished(m_currentTeam)){
        endTurn();
    }

}

void Battle::draw() const
{
    drawWorld();
    drawUI();
    if(m_winner.has_value()){
        drawGameOver();
    }
}

bool Battle::isOver() const
{
    return m_winner.has_value();
}

// Used by Actions
const Grid &Battle::getGrid() const
{
    return m_grid;
}

size_t Battle::getUnitCount() const
{
    return m_units.size();
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
        if (unit.isAlive())
        {
            positions.push_back(unit.getPosition());
        }
    }
    return positions;
}

// Helper Functions
std::optional<size_t> Battle::findUnitAt(GridPosition position) const
{
    for (size_t i = 0; i < m_units.size(); ++i)
    {
        if ( m_units[i].isAlive() && m_units[i].getPosition() == position)
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
    return unit.isAlive() && unit.getTeam() == m_currentTeam && !unit.hasFinishedTurn();
}

bool Battle::canUseAction(size_t unitIndex, size_t actionIndex) const
{
    const Unit &unit = m_units[unitIndex];
    const Action &action = unit.getAction(actionIndex);
    return action.isAvailable(unit) && !action.getValidTargets(*this, unitIndex).empty();
}

bool Battle::hasTeamFinished(Team team) const
{
    for (const Unit &unit : m_units)
    {
        if (unit.isAlive() && unit.getTeam() == team && !unit.hasFinishedTurn())
        {
            return false;
        }
    }
    return true;
}

bool Battle::hasLivingUnits(Team team) const
{
    for (const Unit &unit : m_units)
    {
        if (unit.isAlive() && unit.getTeam() == team)
        {
            return true;
        }
    }
    return false;
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
        if (canUseAction(unitIndex, i))
        {
            chooseAction(i);
            return;
        }
    }
}

void Battle::chooseAction(size_t actionIndex)
{
    if (!canUseAction(*m_selectedUnit, actionIndex))
    {
        return;
    }

    const Action &action = m_units[*m_selectedUnit].getAction(actionIndex);
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

void Battle::checkForWinner()
{
    if (!hasLivingUnits(Team::ENEMY))
    {
        m_winner = Team::PLAYER;
    }
    else if (!hasLivingUnits(Team::PLAYER))
    {
        m_winner = Team::ENEMY;
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
        m_grid.drawHighlightedTile(m_units[*m_selectedUnit].getPosition(), YELLOW);
    }

    for (const Unit &unit : m_units)
    {
        if (unit.isAlive())
        {
            unit.draw(m_grid);
        }
    }

    if (m_selectedUnit.has_value() && m_selectedAction.has_value() && m_hoveredTile.has_value() && Pathfinding::containsPosition(m_validTargets, *m_hoveredTile))
    {
        const Action &action = m_units[*m_selectedUnit].getAction(*m_selectedAction);
        action.drawPreview(*this, *m_selectedUnit, *m_hoveredTile);
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
            DrawText(TextFormat("%s (%d/%d HP): %s", unit.getName().c_str(), unit.getHealth(), unit.getMaxHealth(), action.getHint().c_str()), 10, panelY + 40, 20, DARKGRAY);
        }
        else
        {
            DrawText(TextFormat("%s (%d/%d HP)", unit.getName().c_str(), unit.getHealth(), unit.getMaxHealth()), 10, panelY + 40, 20, DARKGRAY);
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
            if (!canUseAction(*m_selectedUnit, i))
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
        DrawText(TextFormat("Tile: %d, %d", m_hoveredTile->x, m_hoveredTile->y), rightColumnX, panelY + 10, 20, DARKGRAY);
    }
    else
    {
        DrawText("Tile: none", rightColumnX, panelY + 10, 20, DARKGRAY);
    }
}

void Battle::drawGameOver() const
{
    constexpr int mapHeight = GameConfig::GRID_ROWS * GameConfig::TILE_SIZE;

    DrawRectangle(0, 0, GameConfig::WINDOW_WIDTH, mapHeight, Fade(BLACK, 0.5f));

    const char *title = (*m_winner == Team::PLAYER) ? "Victory!" : "Defeat";
    Color titelColor = (*m_winner == Team::PLAYER) ? SKYBLUE : RED;
    int titleWidth = MeasureText(title, 60);
    DrawText(title, (GameConfig::WINDOW_WIDTH - titleWidth) / 2, mapHeight / 2 - 50, 60, titelColor);

    const char *hint = "Press R to play again";
    int hintWidth = MeasureText(hint, 24);
    DrawText(hint, (GameConfig::WINDOW_WIDTH - hintWidth) / 2, mapHeight / 2 + 20, 24, RAYWHITE);
}