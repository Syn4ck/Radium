#ifndef RADIUMENGINE_RENDERER_PASS_REDUX_HPP
#define RADIUMENGINE_RENDERER_PASS_REDUX_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Passes/Pass.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

namespace Ra
{
    namespace Engine
    {

        /**
         * \class PassRedux
         *
         * @brief The PassRedux class iteratively applies the shader on a
         * reduction of the precedent texture.
         *
         * The Redux Pass provides the mechanism to draw a smaller-and-smaller
         * texture. The resize divisor is m_ratio which default to 2.
         *
         * The pass computes the number of required steps before size is 1, and
         * outputs the good textures at '0' and the other at '1'.
         *
         */

        class RA_ENGINE_API PassRedux : public Pass
        {
        public:
            PassRedux(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                      const std::string& shader, uint ratio = 2);
            virtual ~PassRedux();

            virtual void init();

            virtual void renderPass( const RenderData& renderData );

            virtual void resizePass(uint w, uint h);

        protected:
            virtual void resizePass();

            /// Compute the number of steps required to get to size 1.
            uint nbResizements() const;

        private:
            enum TextureOutTags
            {
                TEX_PING = 0,    // both are equally prone to be the output texture
                TEX_PONG,
                TEX_OUT_COUNT,
            };

            /// internal fbo.
            std::unique_ptr<FBO> m_fbo;

            /// Parameters for redux to know what to do.
            std::string          m_shadername;
            const ShaderProgram* m_shader;

            uint m_ratio;    ///< The ratio by which the size is divided each iteration.

            /// Size of the drawn textures.
            uint m_pingPongSize;

            /// Be able to know where to pick textures in the FBO.
            uint m_pingpong;
        };

    }
}


#endif // RADIUMENGINE_RENDERER_REDUX_HPP

