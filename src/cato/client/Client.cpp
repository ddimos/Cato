#include "client/Client.hpp"

#include "core/event/Dispatcher.hpp"
#include "core/Log.hpp"
#include "core/Path.hpp"

#include "shared/events/InputEvents.hpp"

#include "client/state/States.hpp"

#include "Constants.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Clipboard.hpp>


namespace
{

std::string getResourcesPath()
{
#if defined(__APPLE__)
    return std::string(cn::core::getExecutablePath().parent_path().parent_path().c_str()) + "/Resources/";
#else
    return "res/";
#endif
}

} // namespace

namespace cn::client
{

Client::Client()
    :// m_window(sf::VideoMode(1600, 960), "Cato", sf::Style::Close)
    m_window(sf::VideoMode::getFullscreenModes().front(), "Cato", sf::Style::Fullscreen)
    , m_netManager(m_context, false), m_stateManager(m_context), m_playerManager(m_context)
{
    m_context.insert(m_window);
    m_context.insert(m_textureHolder);
    m_context.insert(m_fontHolder);
    m_context.insert(m_saveHolder);
    m_context.insert(m_netManager);
    m_context.insert(m_eventManager.getDispatcher());
    m_context.insert(m_playerManager);
    m_context.insert(m_systemClock);
    m_context.insert(m_seed);

    m_window.setKeyRepeatEnabled(false);
    m_window.setFramerateLimit(60);

    m_netManager.init();
}

void Client::start()
{
    CN_LOG_I("Start Client Cato");

    m_isRunning = true;
    m_systemClock.restart();
    
    init();
    
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;

    while (m_isRunning)
    {
        sf::Time elapsed = clock.restart();
        accumulator += elapsed;

        m_netManager.updateReceive();
        handleEvents();

        update(elapsed);
        while (accumulator >= TimePerFrame)
        {
            fixedUpdate(TimePerFrame);
            accumulator -= TimePerFrame;
        }

        m_window.clear(sf::Color(128, 198, 134));

        draw();

        m_window.display();

        m_netManager.updateSend();
    }

    deinit();
}

void Client::init()
{
    auto resourcePath = getResourcesPath();
    m_fontHolder.load(FontIds::Main,                        resourcePath + "fonts/times_new_roman.ttf");
    m_textureHolder.load(TextureIds::BackButton,            resourcePath + "textures/back_button.png");
    m_textureHolder.load(TextureIds::Background,            resourcePath + "textures/background.png");
    m_textureHolder.load(TextureIds::Cards,                 resourcePath + "textures/cards.png");
    m_textureHolder.load(TextureIds::Deck,                  resourcePath + "textures/deck.png");
    m_textureHolder.load(TextureIds::Discard,               resourcePath + "textures/discard.png");
    m_textureHolder.load(TextureIds::Field,                 resourcePath + "textures/field.png");
    m_textureHolder.load(TextureIds::JoinButton,            resourcePath + "textures/join_button.png");
    m_textureHolder.load(TextureIds::Paw,                   resourcePath + "textures/paw.png");
    m_textureHolder.load(TextureIds::PrivateZone,           resourcePath + "textures/private_zone.png");
    m_textureHolder.load(TextureIds::ReadyButton,           resourcePath + "textures/ready_button.png");
    m_textureHolder.load(TextureIds::SettingsButton,        resourcePath + "textures/settings_button.png");
    m_textureHolder.load(TextureIds::Table,                 resourcePath + "textures/table.png");
    m_textureHolder.load(TextureIds::TableButtons,          resourcePath + "textures/table_icons.png");

    m_saveHolder.load(SaveIds::PlayerName,                  resourcePath + "save/player_name");
    m_saveHolder.load(SaveIds::ServerAddress,               resourcePath + "save/server_address");

// TODO move states
    m_stateManager.registerState<states::TestState>(states::id::Test);
    m_stateManager.registerState<states::TitleState>(states::id::Title);
    m_stateManager.registerState<states::EnteringState>(states::id::Entering);
    m_stateManager.registerState<states::MainMenuState>(states::id::MainMenu);
    m_stateManager.registerState<states::JoiningState>(states::id::Joining);
    m_stateManager.registerState<states::LobbyState>(states::id::Lobby);
    m_stateManager.registerState<states::GameState>(states::id::Game);
    m_stateManager.registerState<states::FinishState>(states::id::Finish);

    m_stateManager.pushState(states::id::MainMenu);

    m_playerManager.registerEvents(m_eventManager.getDispatcher(), true);
}

void Client::deinit()
{
    m_playerManager.registerEvents(m_eventManager.getDispatcher(), false);
}

void Client::handleEvents()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            m_isRunning = false;
            break;
        case sf::Event::KeyReleased:
            if (event.key.code  == sf::Keyboard::V && (event.key.control || event.key.system))
                m_eventManager.getDispatcher().send<events::TextPastedEvent>(sf::Clipboard::getString());
            else
                m_eventManager.getDispatcher().send<events::KeyReleasedEvent>(event.key);
            break;
        case sf::Event::MouseButtonPressed:
            m_eventManager.getDispatcher().send<events::MouseButtonPressedEvent>(event.mouseButton);
            break;
        case sf::Event::MouseButtonReleased:
            m_eventManager.getDispatcher().send<events::MouseButtonReleasedEvent>(event.mouseButton);
            break;
        case sf::Event::MouseMoved:
            m_eventManager.getDispatcher().send<events::MouseMovedEvent>(event.mouseMove);
            break;
        case sf::Event::TextEntered:
            m_eventManager.getDispatcher().send<events::TextEnteredEvent>(event.text);
            break;
        default:
            break;
        }
    }
    m_eventManager.process();
}

void Client::update(sf::Time _dt)
{
    m_stateManager.update(_dt);
}

void Client::fixedUpdate(sf::Time _dt)
{
    (void)_dt;
}

void Client::draw()
{
    m_stateManager.draw();
}

} // namespace cn::client
