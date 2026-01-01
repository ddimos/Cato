#pragma once

#include "shared/game/object/PrivateZone.hpp"
#include "core/Context.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace cn::client::game
{

class PrivateZone final : public shared::game::object::PrivateZone
{
public:
    PrivateZone(const core::Context& _context, shared::game::object::Id _id, PlayerId _ownerId, const std::string& _name);

private:
    void onActivate() override;
    void onDraw(sf::RenderWindow& _window) override;

    void onEntered() override;
    void onExited() override;

    sf::Sprite m_sprite;
    sf::Text m_name;

    unsigned m_enteredCount = 0;
};

} // namespace cn::client::game
