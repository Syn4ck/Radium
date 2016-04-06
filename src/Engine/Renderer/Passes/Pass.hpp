#ifndef RADIUMENGINE_RENDERER_PASSCOMPONENT_HPP
#define RADIUMENGINE_RENDERER_PASSCOMPONENT_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra
{
    namespace Engine
    {


        class RA_ENGINE_API Pass
        {
        public:
            Pass(const std::string& name, int w, int h, int nTexIn, int nTexOut);
            virtual ~Pass() = 0;

            virtual std::string getPassName() const { return m_name; }

            virtual void renderPass() = 0;
            virtual void initFbo()    = 0;

            virtual void setIn(int slot, Texture* tex);
            virtual void setOut(int slot, Texture* tex);

            virtual std::shared_ptr<Texture> getIn(int slot);   // FIXME(Hugo) shared useful ? unique ?

        protected:
            std::string m_name;

            int m_width;
            int m_height;

            int m_texIn;
            int m_texOut;

            std::array<std::shared_ptr<Texture>, m_texIn>  m_texIn;
            std::array<std::shared_ptr<Texture>, m_texOut> m_texOut;

            std::unique_ptr<std::array<FBO,1>> m_fbo;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_PASS_HPP
