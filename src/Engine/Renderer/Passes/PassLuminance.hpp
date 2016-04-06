#ifndef RADIUMENGINE_RENDERER_PASS_LUM_HPP
#define RADIUMENGINE_RENDERER_PASS_LUM_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra
{
    namespace Engine
    {
        class RA_ENGINE_API PassLuminance : public Pass
        {
        public:
            PassLuminance(const std::string& name, int w, int h, int nTexIn, int nTexOut)
                : Pass(name, w, h, nTexIn, nTexOut)
            {}
            ~PassLuminance();
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_LUM_HPP
