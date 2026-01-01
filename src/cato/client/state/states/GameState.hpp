#pragma once

#include "core/event/Types.hpp"
#include "core/state/State.hpp"

#include "shared/game/Board.hpp"
#include "client/game/InputController.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace cn::client::states
{

class GameState final : public core::state::State
{
public:
    GameState(core::state::Manager& _stateManagerRef);

private:
    void onRegisterEvents(core::event::Dispatcher& _dispatcher, bool _isBeingRegistered) override;
    core::state::Return onUpdate(sf::Time _dt) override;
    void onDraw() override;

    std::unique_ptr<game::InputController> m_inputController;
    std::unique_ptr<shared::game::Board> m_board;
    sf::Sprite m_tableSprite;

    core::event::ListenerId m_listenerId{};
};

} // namespace cn::client::states
