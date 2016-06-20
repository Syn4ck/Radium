#ifndef RADIUMENGINE_RENDERER_PASS_REGULAR_H
#define RADIUMENGINE_RENDERER_PASS_REGULAR_H

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Passes/Pass.hpp>

namespace Ra
{
    namespace Engine
    {

        /**
         * \class PassRegular
         *
         * @brief The Regular Pass that will apply its shader once.
         *
         * Basic post-processing pass who applies its shader one-shot
         * and output directly the textures associated with its FBO.
         *
         */

        class RA_ENGINE_API PassRegular : public Pass
        {
        public:
            PassRegular(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, std::string shader);
            ~PassRegular();

            virtual void init();

            virtual void renderPass( const RenderData& renderData );

            virtual void resizePass(uint w, uint h);

        protected:
            virtual void resizePass();

        public:
            std::string      m_shadername;
            const ShaderProgram* m_shader;

        private:
            std::unique_ptr<FBO> m_fbo;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_PASS_REGULAR_HPP
