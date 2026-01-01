#pragma once

#include "shared/game/Types.hpp"

#include <SFML/System/Vector2.hpp>
#include <vector>

namespace cn::shared::game::placement
{
    struct Placement
    {
        sf::Vector2f pos{};
        float rot = 0.f;
    };

    Placement getDeck();
    Placement getDiscard();
    Placement getTable();
    std::vector<Placement> getParticipant(unsigned _participantNumber);

    Placement getButton(TableButtonType _type);

} // namespace cn::shared::game::placement
