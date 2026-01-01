#include "shared/game/Board.hpp"
#include "shared/game/Constants.hpp"
#include "shared/game/Placement.hpp"
#include "shared/Types.hpp"

#include "core/math/Math.hpp"
#include "core/Assert.hpp"
#include "core/Log.hpp"

namespace cn::shared::game
{

Board::Board(
    const std::vector<Player>& _players,
    std::function<object::Card*(object::Id)> _createCardFunc,
    std::function<object::Deck*(object::Id)> _createDeckFunc,
    std::function<object::Discard*(object::Id)> _createDiscardFunc,
    std::function<object::Participant*(object::Id, PlayerId)> _createParticipantFunc,
    std::function<object::PrivateZone*(object::Id, PlayerId)> _createPrivateZoneFunc,
    std::function<object::CountableButton*(object::Id, TableButtonType, unsigned)> _createButtonFunc,
    std::function<void(object::Card&)> _provideCardValue
)
    : m_layerController({
            { layer::Cards, StandartDeckSize },
            { layer::GrabbedCards, MaxNumberOfPlayers * 2},
            { layer::Players, MaxNumberOfPlayers}
        })
    , m_privateZoneViewableController(
        [this](shared::game::component::PrivateZoneViewable& _component){
            auto& card = static_cast<shared::game::object::Card&>(_component.getParent());
            if (!_component.isHidden())
            {
                m_flipController.turnDown(card.getFlippableComponent());
                card.flip(false);
            }
        })
    , m_provideCardValueCallback(_provideCardValue)
{
    {
        m_deck = _createDeckFunc(generateNextOjectId());
        m_deck->setPosition(placement::getDeck().pos);
        m_deck->setSize({80.f, 110.f});
    }
    {
        m_discard = _createDiscardFunc(generateNextOjectId());
        m_discard->setPosition(placement::getDiscard().pos);
        m_discard->setSize({80.f, 110.f});
    }
    {
        unsigned short deckSize = game::StandartDeckSize;
        for (unsigned short i = 0; i < deckSize; ++i)
        {
            auto* card = _createCardFunc(generateNextOjectId());
            card->setSize({70.f, 100.f});
            m_cards.push_back(card);
            m_flipController.add(card->getFlippableComponent());
            m_grabController.add(card->getGrabbableComponent());
            m_privateZoneViewableController.add(card->getPrivateZoneViewableComponent());
        }
        m_deck->setCards(m_cards);
    }

    // TODO auto points = core::math::generatePointOnEllipse(900, 480, _players.size(), {960.f, 540.f});
    
    auto points = placement::getParticipant(_players.size());
    unsigned i = 0;
    for (const Player& player : _players)
    {
        auto [it, inserted] = m_participants.try_emplace(player.id, _createParticipantFunc(generateNextOjectId(), player.id));
        CN_ASSERT(inserted);
        it->second->setPosition(points.at(i).pos);
        it->second->setRotation(points.at(i).rot);
        m_layerController.addTolayer(layer::Players, it->second->getLayerableComponent());

        auto* zone = m_privateZones.emplace_back(_createPrivateZoneFunc(generateNextOjectId(), player.id));
        zone->setPosition(points.at(i).pos);
        zone->setRotation(points.at(i).rot);
        zone->setSize(sf::Vector2f(200.f, 50.f + 20.f)); // The private zone bounderies are slightly bigger
        m_privateZoneViewableController.addPrivateZone(*zone);

        ++i;
    }

    {
        unsigned numberOfPlayerToClick = (_players.size() == 1) ? 1 : 2;

        auto* button = _createButtonFunc(generateNextOjectId(), TableButtonType::Shuffle, numberOfPlayerToClick);
        button->setPosition(placement::getButton(TableButtonType::Shuffle).pos);
        button->setSize(sf::Vector2f(50.f, 30.f));
        m_clickCountableController.add(button->getClickCountableComponent());

        button = _createButtonFunc(generateNextOjectId(), TableButtonType::Reset, numberOfPlayerToClick);
        button->setPosition(placement::getButton(TableButtonType::Reset).pos);
        button->setSize(sf::Vector2f(50.f, 30.f));
        m_clickCountableController.add(button->getClickCountableComponent());
        
        button = _createButtonFunc(generateNextOjectId(), TableButtonType::FromDiscard, numberOfPlayerToClick);
        button->setPosition(placement::getButton(TableButtonType::FromDiscard).pos);
        button->setSize(sf::Vector2f(50.f, 30.f));
        m_clickCountableController.add(button->getClickCountableComponent());
    }
}

void Board::start(const std::vector<object::Card::Value>& _cardValues)
{
    m_deck->shuffle();
    m_deck->visit(
        [this](object::Card& _card){
            m_layerController.addTolayer(layer::Cards, _card.getLayerableComponent());
        }
    );

    CN_ASSERT(_cardValues.empty() || _cardValues.size() == m_cards.size());
    for (size_t i = 0; i < _cardValues.size(); ++i)
        m_cards.at(i)->setValue(_cardValues.at(i));
}

object::Id Board::generateNextOjectId()
{
    object::Id newId(m_objectIdGenerator++);
    CN_ASSERT(newId.isValid());
    return newId;
}

object::Card* Board::getCard(object::Id _id)
{
    for (auto* card : m_cards)
    {
        if (card->getId() == _id)
            return card;
    }
    CN_ASSERT_FRM(false, "No card with id: {}", _id.value());
    return nullptr;
}

object::Participant* Board::getParticipant(PlayerId _playerId)
{
    CN_ASSERT_FRM(m_participants.contains(_playerId), "No participant with id: {}", _playerId.value());
    return m_participants.at(_playerId);
}

void Board::update(sf::Time _dt)
{
    m_clickCountableController.update(_dt);
    m_privateZoneViewableController.update();

    if (m_cardToCheckAfterReleased)
    {
        if (m_cardToCheckAfterReleased->getPrivateZoneViewableComponent().isHidden())
        {
            m_flipController.turnUp(m_cardToCheckAfterReleased->getFlippableComponent());
            m_cardToCheckAfterReleased->flip(true);

            m_provideCardValueCallback(*m_cardToCheckAfterReleased);
        }
        m_cardToCheckAfterReleased = nullptr;
    }
}

component::Grabbable* Board::findObjectToGrab(PlayerId _playerId, sf::Vector2f _position)
{
    return m_grabController.findObjectToGrab(_playerId, _position);
}

component::Grabbable* Board::findObjectToRelease(PlayerId _playerId, sf::Vector2f _position)
{
    return m_grabController.findObjectToRelease(_playerId, _position);
}

void Board::participantGrabs(PlayerId _playerId, object::Id _id, sf::Vector2f _position)
{
    CN_LOG_I_FRM("Grabs {} {}", _playerId.value(), _id.value());
    auto* card = getCard(_id);
    m_grabController.grabObject(_playerId, card->getGrabbableComponent());

    auto* part = m_participants.at(_playerId);
    card->move(_position);
    part->setObject(card);
    m_layerController.removeFromLayer(layer::Cards, card->getLayerableComponent());
    m_layerController.addTolayer(layer::GrabbedCards, card->getLayerableComponent());
    card->rotate(part->getRotation());
}

void Board::participantReleases(PlayerId _playerId, object::Id _id, sf::Vector2f _position)
{
    CN_LOG_I_FRM("Releases {} {}", _playerId.value(), _id.value());
    auto* card = getCard(_id);
    m_grabController.releaseObject(_playerId, card->getGrabbableComponent());

    m_participants.at(_playerId)->setObject(nullptr);
    m_layerController.removeFromLayer(layer::GrabbedCards, card->getLayerableComponent());
    m_layerController.addTolayer(layer::Cards, card->getLayerableComponent());
    sf::Vector2f pos = _position;
    if (m_discard->contains(_position) && !card->isInDiscard())
    {
        m_discard->discard(card);
        pos = m_discard->getPosition();
        m_flipController.turnUp(card->getFlippableComponent());
        card->flip(true);
        card->rotate(m_discard->getRotation());

        m_provideCardValueCallback(*card);
    }
    if (m_deck->contains(_position) && !card->isInDeck())
    {
        m_deck->add(*card);
        pos = m_deck->getPosition();
        m_flipController.turnDown(card->getFlippableComponent());
        card->flip(false);
        card->rotate(m_deck->getRotation());
    }
    card->move(pos);

    // TODO This is a hack
    // there might be a desync between peers because not all peers have updated position at this point
    // so the controller will have outdated info, so we need to wait for it to run the update
    m_cardToCheckAfterReleased = card;
}

object::Object* Board::participantClicks(PlayerId _playerId, sf::Vector2f _position)
{
    auto* component = m_clickCountableController.findObjectToClick(_playerId, _position);
    if (!component)
        return nullptr;
    m_clickCountableController.clickObject(_playerId, *component);
    CN_LOG_I_FRM("Clicks {} {}", _playerId.value(), component->getParent().getId().value());
    if (component->isClicked())
        performClick(component->getParent());
    return &(component->getParent());
}

void Board::participantClicks(PlayerId _playerId, object::Id _id)
{
    auto* component = m_clickCountableController.clickObject(_playerId, _id);
    CN_LOG_I_FRM("Clicks {} {}", _playerId.value(), _id.value());
    if (component->isClicked())
        performClick(component->getParent());
}

void Board::performClick(object::Object& _object)
{
    auto& object = static_cast<object::CountableButton&>(_object);
    if (object.getType() == TableButtonType::Shuffle)
    {
        // TODO optimize this piece
        m_deck->visit(
            [this](object::Card& _card){
                m_layerController.removeFromLayer(layer::Cards, _card.getLayerableComponent());
            }
        );
        m_deck->shuffle();
        m_deck->visit(
            [this](object::Card& _card){
                m_layerController.addTolayer(layer::Cards, _card.getLayerableComponent());
            }
        );
    }
    else if (object.getType() == TableButtonType::Reset)
    {
        for (auto* card : m_cards)
        {
            card->move(m_deck->getPosition());
            card->rotate(m_deck->getRotation());
            m_flipController.turnDown(card->getFlippableComponent());
            card->flip(false);
        }
        m_deck->setCards(m_cards);
    }
    else if (object.getType() == TableButtonType::FromDiscard)
    {
        m_discard->visit(
            [this](object::Card& _card){
                m_deck->add(_card);
                _card.move(m_deck->getPosition());
                _card.rotate(m_deck->getRotation());
                m_flipController.turnDown(_card.getFlippableComponent());
                _card.flip(false);
            }
        );
    }
}

component::Flippable* Board::findObjectToFlip(sf::Vector2f _position)
{
    return m_flipController.findObjectToFlip(_position);
}

void Board::participantTurnsUp(PlayerId _playerId, object::Id _id)
{
    auto* card = getCard(_id);
    m_flipController.turnUp(card->getFlippableComponent());

    m_layerController.promoteInLayer(
        card->getGrabbableComponent().isGrabbed() ? layer::GrabbedCards : layer::Cards,
        card->getLayerableComponent()
    );

    m_provideCardValueCallback(*card);

    card->flip(card->getFlippableComponent().isFaceUp());

    CN_LOG_I_FRM("Player {} turns up card {}", _playerId.value(), _id.value());
}

void Board::participantTurnsDown(PlayerId _playerId, object::Id _id)
{
    auto* card = getCard(_id);
    m_flipController.turnDown(card->getFlippableComponent());

    m_layerController.promoteInLayer(
        card->getGrabbableComponent().isGrabbed() ? layer::GrabbedCards : layer::Cards, 
        card->getLayerableComponent()
    );

    card->flip(card->getFlippableComponent().isFaceUp());

    CN_LOG_I_FRM("Player {} turns down card {}", _playerId.value(), _id.value());
}

void Board::participantMoves(PlayerId _playerId, sf::Vector2f _position)
{
    CN_LOG_D_FRM("Moves {} {}:{}", _playerId.value(), _position.x, _position.y);
    m_participants.at(_playerId)->setMousePosition(_position);
}

} // namespace cn::shared::game
