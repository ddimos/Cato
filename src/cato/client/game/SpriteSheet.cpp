#include "client/game/SpriteSheet.hpp"

namespace
{

constexpr short unsigned CardWidth = 2*70;
constexpr short unsigned CardHeight = 2*100;

constexpr short unsigned TableButtonWidth = 50;
constexpr short unsigned TableButtonHeight = 30;

constexpr short unsigned PrivateZoneHeight = 50;
constexpr short unsigned PrivateZoneWidth = 200;

} // namespace

namespace cn::client::game::spriteSheet
{

sf::IntRect getCardTextureRect(shared::game::Rank _rank, shared::game::Suit _suit)
{
    int rankNum = static_cast<int>(_rank) + 1;
    int suitNum = static_cast<int>(_suit);
    return sf::IntRect{ CardWidth * rankNum , CardHeight * suitNum, CardWidth, CardHeight };
}

sf::IntRect getBlurredCardTextureRect()
{
    // TODO real blurred card
    return sf::IntRect{ 0, 2 * CardHeight, CardWidth, CardHeight };
}

sf::IntRect getCardBackTextureRect(Hover _hover)
{
    return _hover == Hover::No 
        ? sf::IntRect{ 0,          0, CardWidth, CardHeight }
        : sf::IntRect{ 0, CardHeight, CardWidth, CardHeight };
}

sf::IntRect getDiscardTextureRect(Hover _hover)
{
    return _hover == Hover::No 
        ? sf::IntRect{ 0, 2 * CardHeight, CardWidth, CardHeight }
        : sf::IntRect{ 0, 3 * CardHeight, CardWidth, CardHeight };
}

sf::IntRect getShuffleButton(Hover _hover)
{
    return _hover == Hover::No 
        ? sf::IntRect{ 0,                 0, TableButtonWidth, TableButtonHeight }
        : sf::IntRect{ 0, TableButtonHeight, TableButtonWidth, TableButtonHeight };
}

sf::IntRect getResetButton(Hover _hover)
{
    return _hover == Hover::No 
        ? sf::IntRect{ TableButtonWidth,                 0, TableButtonWidth, TableButtonHeight }
        : sf::IntRect{ TableButtonWidth, TableButtonHeight, TableButtonWidth, TableButtonHeight };
}

sf::IntRect getFromDisacrdButton(Hover _hover)
{
    return _hover == Hover::No 
        ? sf::IntRect{ 2 * TableButtonWidth,                 0, TableButtonWidth, TableButtonHeight }
        : sf::IntRect{ 2 * TableButtonWidth, TableButtonHeight, TableButtonWidth, TableButtonHeight };
}

sf::IntRect getPrivateZone(Hover _hover)
{
    return _hover == Hover::No 
        ? sf::IntRect{ 0,                 0, PrivateZoneWidth, PrivateZoneHeight }
        : sf::IntRect{ 0, PrivateZoneHeight, PrivateZoneWidth, PrivateZoneHeight };
}

} // namespace cn::client::game::spriteSheet