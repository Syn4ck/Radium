#ifndef RADIUMENGINE_LIGHT_HPP
#define RADIUMENGINE_LIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Engine
    {
        class RenderParameters;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API Light
        {
        public:
            enum LightType
            {
                LightType_Point = 0,
                LightType_Directional,
                LightType_Spot
            };

            struct Attenuation
            {
                Scalar constant  = 1.0;
                Scalar linear    = 1.0;
                Scalar quadratic = 1.0;
            };

        public:
            RA_CORE_ALIGNED_NEW

            explicit Light(const LightType& type);
            virtual ~Light();

            static Light DirLight()
            {
                return Light(LightType_Directional);
            }

            static Light PointLight()
            {
                return Light(LightType_Point);
            }

            static Light SpotLight()
            {
                return Light(LightType_Spot);
            }

            inline const LightType& getType() const { return m_type; }

            virtual void getRenderParameters(RenderParameters& params);

        public:
            Core::Color m_color;
            Core::Vector3 m_position;
            Core::Vector3 m_direction;

            Attenuation m_attenuation;

            Scalar m_innerAngle;
            Scalar m_outerAngle;

        private:
            LightType m_type;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_LIGHT_HPP
