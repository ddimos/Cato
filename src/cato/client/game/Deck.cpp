#include "client/game/Deck.hpp"
#include "client/game/SpriteSheet.hpp"
#include "client/ResourceIds.hpp"

namespace cn::client::game
{

Deck::Deck(const core::Context& _context, shared::game::object::Id _id, unsigned _seed)
    : shared::game::object::Deck(_id, _seed)
    , m_interpolatedRot1(sf::seconds(0.5f), core::Easing::easeOutBack)
    , m_interpolatedRot2(sf::seconds(0.7f), core::Easing::linear)
{
    m_sprite.setTexture(_context.get<TextureHolder>().get(TextureIds::Deck));
    m_sprite.setOrigin(m_sprite.getLocalBounds().getSize() / 2.f);

    m_shuffleSprite1.setTexture(_context.get<TextureHolder>().get(TextureIds::Cards));
    m_shuffleSprite1.setTextureRect(game::spriteSheet::getCardBackTextureRect());
    m_shuffleSprite1.setOrigin(m_shuffleSprite1.getLocalBounds().getSize() / 2.f);

    m_shuffleSprite2.setTexture(_context.get<TextureHolder>().get(TextureIds::Cards));
    m_shuffleSprite2.setTextureRect(game::spriteSheet::getCardBackTextureRect());
    m_shuffleSprite2.setOrigin(m_shuffleSprite2.getLocalBounds().getSize() / 2.f);
}

void Deck::onActivate()
{
    m_sprite.setPosition(getPosition());
    m_shuffleSprite1.setPosition(getPosition());
    m_shuffleSprite2.setPosition(getPosition());
}

void Deck::onDraw(sf::RenderWindow& _window)
{
    _window.draw(m_sprite);
    if (m_interpolatedRot1.doesInterpolate())
    {
        m_shuffleSprite1.setRotation(m_interpolatedRot1.get());
        _window.draw(m_shuffleSprite1);
    }
    if (m_interpolatedRot2.doesInterpolate())
    {
        m_shuffleSprite2.setRotation(m_interpolatedRot2.get());
        _window.draw(m_shuffleSprite2);
    }
}

void Deck::onShuffle()
{
    m_interpolatedRot1.start(0, 360);
    m_interpolatedRot2.start(0, 360);
}

} // namespace cn::client::game
