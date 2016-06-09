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
            Pass(const std::string& name, uint w, uint h, uint nbIn, uint nbOut);
            virtual ~Pass() = 0;

            virtual void renderPass() = 0;
            virtual void resizePass() = 0;
            virtual void resizePass( uint w, uint h ) = 0;
            virtual void init() = 0;

            /// @brief set of functions used to set RenderParameters to a shader
            void setIn(const std::string& name, Texture* tex              );
            void setIn(const std::string& name, int      value            );
            void setIn(const std::string& name, uint     value            );
            void setIn(const std::string& name, Scalar   value            );
            void setIn(const std::string& name, const Core::Vector2& value);
            void setIn(const std::string& name, const Core::Vector3& value);
            void setIn(const std::string& name, const Core::Vector4& value);
            void setIn(const std::string& name, const Core::Matrix2& value);
            void setIn(const std::string& name, const Core::Matrix3& value);
            void setIn(const std::string& name, const Core::Matrix4& value);

            /// @brief output of a pass
            Texture* getTex(const std::string& name);

            int    getInt   (const std::string& name);
            uint   getUint  (const std::string& name);
            Scalar getScalar(const std::string& name);

            Core::Vector2 getVec2(const std::string& name);
            Core::Vector3 getVec3(const std::string& name);
            Core::Vector4 getVec4(const std::string& name);

            Core::Matrix2 getMat2(const std::string& name);
            Core::Matrix3 getMat3(const std::string& name);
            Core::Matrix4 getMat4(const std::string& name);

            void setCanvas(Mesh* canvas);
            void setSizeModifier(Scalar w = 1.0, Scalar h = 1.0);

            void setupParamIn  (uint slot, const std::string& name, paramType t);
            void setupParamOut (uint slot, const std::string& name, paramType t);

            uint getId() const;
            std::string getName() const;

            /// @brief perform introspection on a shader program
            /// and set inputs/outputs names
            void paramNamesFromShaderProgram(const ShaderProgram* prog);

            static bool connect (Pass* a, uint ia, Pass* b, uint ib );
            static const char* getParamNameIn  ( Pass* p, uint slot );
            static const char* getParamNameOut ( Pass* p, uint slot );

        public:
            RenderParameters m_paramIn;   /// input  render parameters
            RenderParameters m_paramOut;  /// output render parameters

        protected:
            /// mapping of names to inputs slots
            std::vector<std::pair<std::string, paramType>> m_nameIn;
            /// mapping of names to outputs slots
            std::vector<std::pair<std::string, paramType>> m_nameOut;

            std::string m_name;
            uint        m_id;

            uint m_nbin;
            uint m_nbout;

            uint m_width;
            uint m_height;

            Scalar m_wModifier; /// modifier (factor) for the width
            Scalar m_hModifier; /// modifier (factor) for the height

            /// physical space for textures
            std::vector<std::unique_ptr<Texture>> m_outputs;

            /// geometry for GL to render the fragment shader to
            Mesh* m_canvas;

            static const GLenum buffers[];
        };


    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_PASS_HPP
