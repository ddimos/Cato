#include "shared/game/Placement.hpp"
#include "core/Assert.hpp"

namespace
{
    constexpr float EDGE_OFFSET = 30.f;
    constexpr float OFFSET_1 = 150.f;
    constexpr float POS_X_1 = 960.f - 540.f + EDGE_OFFSET;
    constexpr float POS_X_2 = 960.f - OFFSET_1;
    constexpr float POS_X_3 = 960.f;
    constexpr float POS_X_4 = 960.f + OFFSET_1;
    constexpr float POS_X_5 = 960.f + 540.f - EDGE_OFFSET;
    constexpr float POS_Y_1 = EDGE_OFFSET;
    constexpr float POS_Y_2 = 540.f - OFFSET_1;
    constexpr float POS_Y_3 = 540.f;
    constexpr float POS_Y_4 = 540.f + OFFSET_1;
    constexpr float POS_Y_5 = 1080.f - EDGE_OFFSET;
}

namespace cn::shared::game::placement
{

Placement getDeck()
{
    return { sf::Vector2f(810.f, 540.f), 0.f };
}

Placement getDiscard()
{
    return { sf::Vector2f(1110.f, 540.f), 0.f };
}

Placement getTable()
{
    return { sf::Vector2f(960.f, 540.f), 0.f };
}

std::vector<Placement> getParticipant(unsigned _participantNumber)
{
    switch (_participantNumber)
    {
    case 1:
        return {
            {{ POS_X_3, POS_Y_5 }, 0.f}
        };
    case 2:
        return {
            {{ POS_X_3, POS_Y_5 }, 0.f},
            {{ POS_X_3, POS_Y_1 }, 180.f}
        };
    case 3:
        return {
            {{ POS_X_3, POS_Y_5 }, 0.f},
            {{ POS_X_5, POS_Y_3 }, -90.f}, {{ POS_X_1, POS_Y_3 }, 90.f}
        };
    case 4:
        return {
            {{ POS_X_3, POS_Y_5 }, 0.f},   {{ POS_X_5, POS_Y_3 }, -90.f},
            {{ POS_X_3, POS_Y_1 }, 180.f}, {{ POS_X_1, POS_Y_3 }, 90.f}
        };
    case 5:
        return {
            {{ POS_X_3, POS_Y_5 }, 0.f},   {{ POS_X_5, POS_Y_3 }, -90.f},
            {{ POS_X_2, POS_Y_1 }, 180.f}, {{ POS_X_4, POS_Y_1 }, 180.f},
            {{ POS_X_1, POS_Y_3 }, 90.f},
        };
    case 6:
        return {
            {{ POS_X_2, POS_Y_5 }, 0.f},   {{ POS_X_4, POS_Y_5 }, 0.f},
            {{ POS_X_5, POS_Y_3 }, -90.f}, {{ POS_X_2, POS_Y_1 }, 180.f},
            {{ POS_X_4, POS_Y_1 }, 180.f}, {{ POS_X_1, POS_Y_3 }, 90.f},
        };
    case 7:
        return {
            {{ POS_X_2, POS_Y_5 }, 0.f},   {{ POS_X_4, POS_Y_5 }, 0.f},
            {{ POS_X_5, POS_Y_4 }, -90.f}, {{ POS_X_5, POS_Y_2 }, -90.f},
            {{ POS_X_2, POS_Y_1 }, 180.f}, {{ POS_X_4, POS_Y_1 }, 180.f},
            {{ POS_X_1, POS_Y_3 }, 90.f}
        };
    case 8:
        return {
            {{ POS_X_2, POS_Y_5 }, 0.f},   {{ POS_X_4, POS_Y_5 }, 0.f},
            {{ POS_X_5, POS_Y_4 }, -90.f}, {{ POS_X_5, POS_Y_2 }, -90.f},
            {{ POS_X_2, POS_Y_1 }, 180.f}, {{ POS_X_4, POS_Y_1 }, 180.f},
            {{ POS_X_1, POS_Y_2 }, 90.f},  {{ POS_X_1, POS_Y_4 }, 90.f}
        };
    
    default:
        CN_ASSERT(false);
        return {};
    }
}

Placement getButton(TableButtonType _type)
{
    float y = 0;
    switch (_type)
    {
    case TableButtonType::Shuffle:
        y = 540.f - 40.f;
        break;
    case TableButtonType::Reset:
        y = 540.f;
        break;
    case TableButtonType::FromDiscard:
        y = 540.f + 40.f;
        break;
    }
    return {{ 960.f, y }, 0.f };
}

} // namespace cn::shared::game::placement
