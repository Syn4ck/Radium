#ifndef RADIUMENGINE_RENDERER_PASS_BLUR_HPP
#define RADIUMENGINE_RENDERER_PASS_BLUR_HPP

#include <Engine/Renderer/Passes/PassPingPong.hpp>


namespace Ra
{
    namespace Engine
    {

        /**
         * \class PassBlur
         *
         * @brief The PassBlur class is a specialization of PassPingPong
         * to achieve blur.
         *
         * This pass, <b>not fully working</b>, applies the X blur on
         * ping->pong and the Y blur on pong->ping.
         *
         * The shader applied is "Blur.frag.glsl".
         *
         */

        class RA_ENGINE_API PassBlur : public PassPingPong
        {
        public:
            PassBlur(const std::string& name, uint w, uint h, uint loop)
                : PassPingPong( name, w, h, 2, 1, loop, "Blur" )
            {
            }

            virtual ~PassBlur() {}

            virtual void renderPass( const RenderData& renderData ) override;
        };

    }
}

#endif // RADIUMENGINE_RENDERER_PASS_PINGPONG_HPP

