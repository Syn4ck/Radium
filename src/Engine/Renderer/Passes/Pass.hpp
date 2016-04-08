#ifndef RADIUMENGINE_RENDERER_PASSCOMPONENT_HPP
#define RADIUMENGINE_RENDERER_PASSCOMPONENT_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>

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

            virtual void renderPass(ShaderProgramManager* shaderMgr, Mesh* screen) = 0;
            virtual void initFbos() = 0;

            virtual void setIn(int slot, Texture* tex);
            virtual void setOut(int slot, Texture* tex);

            virtual std::shared_ptr<Texture> getInternTextures(int i) = 0;

        protected:
            std::string m_name;

            int m_width;
            int m_height;

            int m_nTexIn;
            int m_nTexOut;

            std::vector<std::shared_ptr<Texture>> m_texIn;
            std::vector<std::shared_ptr<Texture>> m_texOut;

            static const GLenum buffers[];
        };


    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_PASS_HPP
