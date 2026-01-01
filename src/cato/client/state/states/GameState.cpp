#include "client/state/states/GameState.hpp"
#include "client/state/StateIds.hpp"

#include "client/player/Manager.hpp"

#include "client/game/Anchor.hpp"
#include "client/game/Card.hpp"
#include "client/game/CountableButton.hpp"
#include "client/game/Deck.hpp"
#include "client/game/Discard.hpp"
#include "client/game/Participant.hpp"
#include "client/game/PrivateZone.hpp"
#include "client/game/SpriteSheet.hpp"

#include "client/ResourceIds.hpp"

#include "core/event/Dispatcher.hpp"

#include "shared/events/GameEvents.hpp"
#include "shared/events/NetworkEvents.hpp"
#include "shared/Types.hpp"
#include "shared/game/Placement.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include "core/Log.hpp"

namespace
{
constexpr cn::core::object::Container::Id GameContainerId = 1;
constexpr cn::core::object::Container::Id MenuContainerId = 2;
} // namespace

namespace cn::client::states
{

GameState::GameState(core::state::Manager& _stateManagerRef)
    : State(_stateManagerRef)
{
    createContainer(MenuContainerId);
    createSortedContainer(GameContainerId, 
        [](const core::object::Object& _left, const core::object::Object& _right){
            const auto& left = static_cast<const shared::game::object::Object&>(_left);
            const auto& right = static_cast<const shared::game::object::Object&>(_right);
            return left.getZIndex() < right.getZIndex();
        }
    );

    auto& playerManagerRef = getContext().get<player::Manager>();

    m_inputController = std::make_unique<game::InputController>(getContext(), 
        [this, &playerManagerRef](sf::Vector2f _pos){
            m_board->participantMoves(playerManagerRef.getLocalPlayerId(), _pos);
        },
        [this, &playerManagerRef](sf::Vector2f _pos){
            // pre release the object to avoid visual gliches with high ping
            m_board->getParticipant(playerManagerRef.getLocalPlayerId())->setObject(nullptr);
        },
        [this, &playerManagerRef](sf::Vector2f _pos){
            // Only find an object to start the animation
            // TODO to think how to revert the change if a desync happens
            auto* component = m_board->findObjectToFlip(_pos);
            if (!component)
                return;
            static_cast<game::Card&>(component->getParent()).startFlipping(!component->isFaceUp());
        }
    );

    m_board = std::make_unique<shared::game::Board>(playerManagerRef.getPlayers(),
        [this](shared::game::object::Id _id){
            auto card = std::make_shared<game::Card>(getContext(), _id);
            getContainer(GameContainerId).add(card);
            return card.get();
        },
        [this](shared::game::object::Id _id){
            auto deck = std::make_shared<game::Deck>(getContext(), _id, getContext().get<shared::Seed>().seed);
            getContainer(GameContainerId).add(deck);
            return deck.get();
        },
        [this](shared::game::object::Id _id){
            auto discard = std::make_shared<game::Discard>(getContext(), _id);
            getContainer(GameContainerId).add(discard);
            return discard.get();
        },
        [&playerManagerRef, this](shared::game::object::Id _id, PlayerId _playerId){
            auto participant = std::make_shared<game::Participant>(getContext(), _id, _playerId, _playerId == playerManagerRef.getLocalPlayerId());
            getContainer(GameContainerId).add(participant);
            return participant.get();
        },
        [&playerManagerRef, this](shared::game::object::Id _id, PlayerId _playerId){
            auto zone = std::make_shared<game::PrivateZone>(getContext(), _id, _playerId, playerManagerRef.getPlayer(_playerId)->name);
            getContainer(GameContainerId).add(zone);
            return zone.get();
        },
        [this](shared::game::object::Id _id, shared::game::TableButtonType _type, unsigned _numberOfOPlayersToClick){
            auto button = std::make_shared<game::CountableButton>(getContext(), _id, _type, _numberOfOPlayersToClick);
            getContainer(GameContainerId).add(button);
            return button.get();
        },
        [](shared::game::object::Card&){}
    );

    {
        m_tableSprite.setTexture(getContext().get<TextureHolder>().get(TextureIds::Table));
        m_tableSprite.setOrigin(m_tableSprite.getLocalBounds().getSize() / 2.f);
        m_tableSprite.setPosition(shared::game::placement::getTable().pos);
    }


    m_listenerId = core::event::getNewListenerId();

    // We don't know the values on the client yet
    std::vector<game::Card::Value> cardValues;
    m_board->start(cardValues);

    auto& window = getContext().get<sf::RenderWindow>();
    sf::View view = window.getDefaultView();
    view.setCenter(sf::Vector2f(960.f, 540.f));
    view.setRotation(m_board->getParticipant(playerManagerRef.getLocalPlayerId())->getRotation());
    window.setView(view);
}

void GameState::onRegisterEvents(core::event::Dispatcher& _dispatcher, bool _isBeingRegistered)
{
    if (_isBeingRegistered)
    {
        _dispatcher.registerEvent<events::FinishGameNetEvent>(m_listenerId,
            [this](const events::FinishGameNetEvent& _event){
                pop();
                push(id::Finish);
            }
        );
        _dispatcher.registerEvent<events::ServerCommandNetEvent>(m_listenerId, // TODO I probably should move this code somewhere else 
            [this](const events::ServerCommandNetEvent& _event){
                if (_event.m_type == shared::game::ServerCommandType::PlayerClicksOnButton)
                {
                    const auto& data = std::get<shared::game::PlayerClicksOnButtonData>(_event.m_data);
                    m_board->participantClicks(data.playerId, data.id);
                }
                else if (_event.m_type == shared::game::ServerCommandType::PlayerInteractsWithCard)
                {
                    const auto& data = std::get<shared::game::PlayerInteractsWithCardData>(_event.m_data);
                    if (data.type == shared::game::PlayerInteractsWithCardData::Type::Grabs)
                    {
                        auto* card = m_board->getCard(data.cardId);
                        m_board->participantGrabs(data.playerId, data.cardId, data.pos);
                        if (data.playerId != getContext().get<player::Manager>().getLocalPlayerId())
                            m_board->participantMoves(data.playerId, data.pos);
                    }
                    else if (data.type == shared::game::PlayerInteractsWithCardData::Type::Releases)
                    {
                        auto* card = m_board->getCard(data.cardId);
                        m_board->participantReleases(data.playerId, data.cardId, data.pos);
                    }
                    else if (data.type == shared::game::PlayerInteractsWithCardData::Type::TurnsDown)
                    {
                        if (data.playerId != getContext().get<player::Manager>().getLocalPlayerId())
                            m_board->participantMoves(data.playerId, data.pos);
                        m_board->participantTurnsDown(data.playerId, data.cardId);
                    }
                    else if (data.type == shared::game::PlayerInteractsWithCardData::Type::TurnsUp)
                    {
                        if (data.playerId != getContext().get<player::Manager>().getLocalPlayerId())
                            m_board->participantMoves(data.playerId, data.pos);
                        m_board->participantTurnsUp(data.playerId, data.cardId);
                    }
                }
                else if (_event.m_type == shared::game::ServerCommandType::PlayerMoves)
                {
                    const auto& data = std::get<shared::game::PlayerMovesData>(_event.m_data);
                    m_board->participantMoves(data.playerId, data.pos);
                }
                else if (_event.m_type == shared::game::ServerCommandType::ProvideCardValue)
                {
                    const auto& data = std::get<shared::game::ProvideCardValueData>(_event.m_data);
                    m_board->getCard(data.cardId)->setValue(data.value);
                    CN_LOG_I_FRM("Card {} value {}", data.cardId.value(), data.value.value());
                }
            }
        );
    }
    else
    {
        _dispatcher.unregisterEvent<events::FinishGameNetEvent>(m_listenerId);
        _dispatcher.unregisterEvent<events::ServerCommandNetEvent>(m_listenerId);
    }

    m_inputController->registerEvents(_dispatcher, _isBeingRegistered);
    // m_board->registerEvents(_dispatcher, _isBeingRegistered);
}

core::state::Return GameState::onUpdate(sf::Time _dt)
{
    m_board->update(_dt);
    return core::state::Return::Break;
}

void GameState::onDraw()
{
    auto& window = getContext().get<sf::RenderWindow>();
    window.draw(m_tableSprite);
}

} // namespace cn::client::states
