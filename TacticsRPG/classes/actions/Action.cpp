#include "Action.h"

bool Action::isAvailable(const Unit&) const{
    return true;
}

void Action::drawPreview(const Battle&, size_t, GridPosition) const{
    
}

std::string Action::getHint() const{
    return "Click a highlighted tile";
}