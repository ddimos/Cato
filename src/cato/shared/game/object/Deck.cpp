#include "shared/game/object/Deck.hpp"
#include "shared/game/object/Card.hpp"

#include "core/Assert.hpp"
#include "core/Log.hpp"

#include <numeric>

namespace cn::shared::game::object
{

Deck::Deck(object::Id _id, unsigned _seed)
    : Object(_id)
{
    m_randomizer.init(_seed);
}

void Deck::setCards(const std::vector<Card*>& _cards)
{
    m_cards = _cards;

    for (auto* card : m_cards)
    {
        if (card->isInDeck())
            continue;
        card->addToDeck();
        card->setPosition(getPosition());
    }
}

void Deck::add(Card& _card)
{
    CN_LOG_I_FRM("Add to deck {}, size {}", _card.getId().value(), m_cards.size());
    m_cards.push_back(&_card);
    _card.addToDeck();
}

// TODO
// void Deck::shuffleInCard(Card& _card)
// {
//     int offset = 0;
//     if (m_cards.size() >= 1)
//         offset = m_randomizer.rand(0, m_cards.size());
//     CN_LOG_I_FRM("Shuffle in card {}, offset {}, size {}", _card.getId().value(), offset, m_cards.size());
//     m_cards.insert(m_cards.begin() + offset, &_card);
//     _card.addToDeck(getPosition());
// }

void Deck::shuffle()
{
    for (int i = m_cards.size() - 1; i > 0; --i)
    {
        int j = m_randomizer.rand(0, i + 1);
        std::swap(m_cards[j], m_cards[i]);
    }

#ifndef CN_DISABLE_LOGGING
    uint8_t arr[5];
    for (unsigned i = 0; i < 5; ++i)
        arr[i] = m_cards[i]->getId().value();
    CN_LOG_I_FRM("Cards {},{},{},{},{}", arr[0], arr[1], arr[2], arr[3], arr[4]);
#endif

    onShuffle();
}

Card* Deck::getNextCard()
{
    CN_ASSERT(!m_cards.empty());
    Card* card = m_cards.back();
    m_cards.pop_back(); // TODO can I do this without popping

    CN_LOG_I_FRM("Get Next Card {}", card->getId().value());

    return card;
}

void Deck::visit(std::function<void(Card&)> _visitor) const
{
    for (auto* card : m_cards)
        _visitor(*card);
}

void Deck::onUpdate(sf::Time)
{
    m_cards.erase(
        std::remove_if(m_cards.begin(), m_cards.end(),
            [this](Card* _card){
                bool remove = !_card->contains(getPosition());
                if (remove)
                {
                    _card->removeFromDeck();
                    CN_LOG_I_FRM("Remove deck {}", _card->getId().value());
                }
                return remove;
            }
        ),
        m_cards.end()
    );
}

} // namespace cn::shared::game::object