#pragma once 

#include "render/actor.h"
#include "render/light.h"

inline void Grafkit::HasLightsRole::AddLight(ActorRef& actor, LightRef& light) { m_lights->AddEntiy(actor, light); }

inline void Grafkit::HasLightsRole::InitializeLights() {
    m_lights = new EntityContainer();
}

inline void Grafkit::HasLightsRole::ShutdownLights() {
    delete m_lights;
    m_lights = nullptr;

}

inline void Grafkit::HasLightsRole::UpdateLight(Renderer& render, size_t id) {
    m_lights->Calculate(render, id);
}

inline void Grafkit::HasLightsRole::UpdateLights(Renderer& render) {
    m_lights->CalculateAll(render);
}