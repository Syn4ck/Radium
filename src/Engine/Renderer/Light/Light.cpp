#include <Engine/Renderer/Light/Light.hpp>

#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra
{

    Engine::Light::Light(const LightType& type)
        : m_color(1.0, 1.0, 1.0, 1.0)
        , m_position(0.0, 0.0, 0.0)
        , m_direction(0.0, -1.0, 0.0)
        , m_attenuation()
        , m_innerAngle(Core::Math::Pi / 6.0)
        , m_outerAngle(Core::Math::Pi / 4.0)
        , m_type(type)
    {
    }

    Engine::Light::~Light()
    {
    }

    void Engine::Light::getRenderParameters(RenderParameters& params)
    {
        params.addParameter("light.color", m_color);
        params.addParameter("light.type", m_type);

        switch (m_type)
        {
            case LightType_Directional:
            {
                params.addParameter("light.direction", m_direction);
            }
            break;

            case LightType_Point:
            {
                params.addParameter("light.position", m_position);
                params.addParameter("light.attenuation.constant", m_attenuation.constant);
                params.addParameter("light.attenuation.linear", m_attenuation.linear);
                params.addParameter("light.attenuation.quadratic", m_attenuation.quadratic);
            }
            break;

            case LightType_Spot:
            {
                params.addParameter("light.position", m_position);
                params.addParameter("light.direction", m_direction);
                params.addParameter("light.innerAngle", m_innerAngle);
                params.addParameter("light.outerAngle", m_outerAngle);
                params.addParameter("light.attenuation.constant", m_attenuation.constant);
                params.addParameter("light.attenuation.linear", m_attenuation.linear);
                params.addParameter("light.attenuation.quadratic", m_attenuation.quadratic);
            }
            break;
        }
    }
}
