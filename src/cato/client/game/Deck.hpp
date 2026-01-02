#pragma once

#include "shared/game/object/Deck.hpp"
#include "core/Context.hpp"
#include "core/Interpolator.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace cn::client::game
{

class Deck final : public shared::game::object::Deck
{
public:
    Deck(const core::Context& _context, shared::game::object::Id _id, unsigned _seed);

private:
    void onActivate() override;
    void onDraw(sf::RenderWindow& _window) override;

    void onShuffle() override;

    sf::Sprite m_sprite;

    sf::Sprite m_shuffleSprite1;
    sf::Sprite m_shuffleSprite2;

    core::Interpolator<float> m_interpolatedRot1;
    core::Interpolator<float> m_interpolatedRot2;
};

} // namespace cn::client::game
