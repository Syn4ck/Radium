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
            void setIn(const char* name, Texture* tex,               uint slot = 0, paramType p = PARAM_TEX);
            void setIn(const char* name, int      value,             uint slot = 0, paramType p = PARAM_TEX);
            void setIn(const char* name, uint     value,             uint slot = 0, paramType p = PARAM_TEX);
            void setIn(const char* name, Scalar   value,             uint slot = 0, paramType p = PARAM_TEX);
            void setIn(const char* name, const Core::Vector2& value, uint slot = 0, paramType p = PARAM_TEX);
            void setIn(const char* name, const Core::Vector3& value, uint slot = 0, paramType p = PARAM_TEX);
            void setIn(const char* name, const Core::Vector4& value, uint slot = 0, paramType p = PARAM_TEX);
            void setIn(const char* name, const Core::Matrix2& value, uint slot = 0, paramType p = PARAM_TEX);
            void setIn(const char* name, const Core::Matrix3& value, uint slot = 0, paramType p = PARAM_TEX);
            void setIn(const char* name, const Core::Matrix4& value, uint slot = 0, paramType p = PARAM_TEX);

            /// @brief output of a pass
            Texture* getTex(const char* name);

            int    getInt   (const char* name);
            uint   getUint  (const char* name);
            Scalar getScalar(const char* name);

            Core::Vector2 getVec2(const char* name);
            Core::Vector3 getVec3(const char* name);
            Core::Vector4 getVec4(const char* name);

            Core::Matrix2 getMat2(const char* name);
            Core::Matrix3 getMat3(const char* name);
            Core::Matrix4 getMat4(const char* name);

            void setCanvas(Mesh* canvas);
            void setSizeModifier(Scalar w = 1.0, Scalar h = 1.0);

            uint getId() const;
            std::string getName() const;

            virtual std::shared_ptr<Texture> getInternTextures(uint i) const = 0;

        public:
            RenderParameters m_RPin;   /// input  render parameters
            RenderParameters m_RPout;  /// output render parameters

            std::vector<std::pair<std::string, paramType>> m_paramIn;  /// mapping of names to inputs slots
            std::vector<std::pair<std::string, paramType>> m_paramOut; /// mapping of names to outputs slots

        protected:
            std::string m_name;
            uint m_id;

            uint m_width;
            uint m_height;

            Scalar m_wModifier; /// modifier (factor) for the width
            Scalar m_hModifier; /// modifier (factor) for the height

            /// geometry for GL to render the fragment shader to
            Mesh* m_canvas;

            static const GLenum buffers[];
        };


    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_PASS_HPP
