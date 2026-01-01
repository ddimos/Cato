#pragma once

#include "shared/game/Types.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace cn::client::game::spriteSheet
{

enum class Hover
{
    Yes,
    No
};

sf::IntRect getCardTextureRect(shared::game::Rank _rank, shared::game::Suit _suit);
sf::IntRect getBlurredCardTextureRect();
sf::IntRect getCardBackTextureRect(Hover _hover = Hover::No);
sf::IntRect getDiscardTextureRect(Hover _hover = Hover::No);

sf::IntRect getShuffleButton(Hover _hover = Hover::No);
sf::IntRect getResetButton(Hover _hover = Hover::No);
sf::IntRect getFromDisacrdButton(Hover _hover = Hover::No);

sf::IntRect getPrivateZone(Hover _hover = Hover::No);

} // namespace cn::client::game::spriteSheet
