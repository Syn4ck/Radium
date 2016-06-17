#ifndef RADIUMENGINE_RENDERER_PASS_BLUR_HPP
#define RADIUMENGINE_RENDERER_PASS_BLUR_HPP

#include <Engine/Renderer/Passes/PassPingPong.hpp>


namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API PassBlur : public PassPingPong
        {
        public:
            PassBlur(const std::string& name, uint w, uint h, uint loop)
                : PassPingPong( name, w, h, 2, 1, loop, "Blur" )
            {
            }

            virtual ~PassBlur() {}

            virtual void renderPass() override;
        };

    }
}

#endif // RADIUMENGINE_RENDERER_PASS_PINGPONG_HPP

