#include "shared/game/controller/PrivateZoneViewable.hpp"

namespace cn::shared::game::controller
{

PrivateZoneViewable::PrivateZoneViewable(OnZoneChanged _onZoneChanged)
    : m_onZoneChanged(_onZoneChanged)
{
}

void PrivateZoneViewable::addPrivateZone(object::PrivateZone& _zone)
{
    m_privateZones.push_back(&_zone);
}

void PrivateZoneViewable::update()
{
    // TODO to optimize this
    for (auto* component : m_components)
    {
        for (auto* zone : m_privateZones)
        {
            if (zone->contains(component->getParent().getPosition()))
            {
                if (!component->isHidden())
                {
                    component->hide(*zone);
                    zone->onEntered();
                    m_onZoneChanged(*component);
                }
            }
            else
            {
                if (component->isHiddenInZone(*zone))
                {
                    component->show(*zone);
                    zone->onExited();
                    m_onZoneChanged(*component);
                }
            }
        }
    }
}

} // namespace cn::shared::game::controller
