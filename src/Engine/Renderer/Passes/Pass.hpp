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
            Pass(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut);
            virtual ~Pass() = 0;

            virtual std::string getPassName() const { return m_name; }

            virtual void renderPass() = 0;
            virtual void resizePass() = 0;
            virtual void resizePass( uint w, uint h ) = 0;
            virtual void init() = 0;

            /// @brief set of functions used to set RenderParameters to a shader
            /// @param paramSet is used when a pass has multiple sets of parameters (like ping-pong)
            void setIn(                  Texture* tex,               uint slot = 0, uint paramSet = 0);
            void setIn(const char* name, int      value,             uint slot = 0, uint paramSet = 0);
            void setIn(const char* name, uint     value,             uint slot = 0, uint paramSet = 0);
            void setIn(const char* name, Scalar   value,             uint slot = 0, uint paramSet = 0);
            void setIn(const char* name, const Core::Vector2& value, uint slot = 0, uint paramSet = 0);
            void setIn(const char* name, const Core::Vector3& value, uint slot = 0, uint paramSet = 0);
            void setIn(const char* name, const Core::Vector4& value, uint slot = 0, uint paramSet = 0);
            void setIn(const char* name, const Core::Matrix2& value, uint slot = 0, uint paramSet = 0);
            void setIn(const char* name, const Core::Matrix3& value, uint slot = 0, uint paramSet = 0);
            void setIn(const char* name, const Core::Matrix4& value, uint slot = 0, uint paramSet = 0);

            void setCanvas(Mesh* canvas);
            void setSizeModifier(Scalar w = 1.0, Scalar h = 1.0);

            virtual Texture* getIn(uint slot)  const;
            virtual Texture* getOut(uint slot) const;

            uint getNIn() const;
            uint getNOut() const;
            uint getId() const;
            std::string getName() const;

            virtual std::shared_ptr<Texture> getInternTextures(uint i) const = 0;

        public:
            RenderParameters m_params[2];           // ← this two things are to be improved
            std::array<std::string,2> m_texNames;  //  ← this thing in particular

        protected:
            std::string m_name;
            uint m_id;

            uint m_width;
            uint m_height;

            /// modifiers for width and height of the pass FBO (factor)
            Scalar m_wModifier;
            Scalar m_hModifier;

            uint m_nTexIn;
            uint m_nTexOut;

            std::vector<Texture*> m_texIn;
            std::vector<std::shared_ptr<Texture>> m_texOut;

            Mesh* m_canvas;

            static const GLenum buffers[];
        };


    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_PASS_HPP
