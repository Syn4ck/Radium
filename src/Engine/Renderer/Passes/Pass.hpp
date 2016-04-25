#ifndef RADIUMENGINE_RENDERER_PASS_HPP
#define RADIUMENGINE_RENDERER_PASS_HPP

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
            Pass(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint priority);
            virtual ~Pass() = 0;

            virtual std::string getPassName() const { return m_name; }

            virtual void renderPass() = 0;
            virtual void resizePass( uint w, uint h ) = 0;
            virtual void init() = 0;

            virtual void setIn(uint slot, Texture* tex);
            virtual void setOut(uint slot, Texture* tex);

            void setCanvas(Mesh* canvas);

            virtual Texture* getIn(uint slot)  const;
            virtual Texture* getOut(uint slot) const;

            uint getNIn() const;
            uint getNOut() const;
            uint getId() const;
            std::string getName() const;

            virtual std::shared_ptr<Texture> getInternTextures(uint i) = 0;

        public:
            uint m_priority;

        protected:
            std::string m_name;
            uint m_id;

            uint m_width;
            uint m_height;

            uint m_nTexIn;
            uint m_nTexOut;

            std::vector<Texture*> m_texIn;
            std::vector<std::shared_ptr<Texture>> m_texOut;  // useful ?

            Mesh* m_canvas;

            // should this structure be implemented separately ?
            // Pass* m_parent;
            // std::vector<Pass*> m_siblings;
            // std::vector<Pass*> m_childs;

            static const GLenum buffers[];
            static       uint   population;
        };


    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_PASS_HPP
