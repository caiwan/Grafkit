#pragma once 

void HasLightsRole::AddLight(ActorRef& actor, LightRef& light) { m_lights->AddEntiy(actor, light); }

void Grafkit::HasLightsRole::InitializeLights() {
    m_lights = new EntityContainer();
}

void Grafkit::HasLightsRole::ShutdownLights() {
    delete m_lights;
    m_lights = nullptr;

}

void Grafkit::HasLightsRole::UpdateLight(Renderer& render, size_t id) {
    m_lights->Calculate(render, id);
}

void Grafkit::HasLightsRole::UpdateLights(Renderer& render) {
    m_lights->CalculateAll(render);
}