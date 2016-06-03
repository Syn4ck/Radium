#include <algorithm>

#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Passes/Pass.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>


namespace Ra
{
    namespace Engine {

        // a static buffer for every passes to use in glDrawBuffers
        const GLenum Pass::buffers[] = {
                    GL_COLOR_ATTACHMENT0,
                    GL_COLOR_ATTACHMENT1,
                    GL_COLOR_ATTACHMENT2,
                    GL_COLOR_ATTACHMENT3,
                    GL_COLOR_ATTACHMENT4,
                    GL_COLOR_ATTACHMENT5,
                    GL_COLOR_ATTACHMENT6,
                    GL_COLOR_ATTACHMENT7
                };

        Pass::Pass(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut)
            : m_name     ( name )
            , m_width    ( w )
            , m_height   ( h )
            , m_wModifier( 1.f )
            , m_hModifier( 1.f )
            , m_canvas   ( nullptr )
        {
            // resize vectors of textures if necessary
            if (m_nTexIn > 1)
            {
                m_texIn.resize(m_nTexIn, nullptr);  // <<<<<<<<<<<< HERE TODO TODO TODO
                //m_texNames.resize(m_nTexIn);
            }

            if (m_nTexOut > 1)
            {
                m_texOut.resize(m_nTexOut, nullptr);
            }
        }

        Pass::~Pass() {}



        void Pass::setIn(const char* name, Texture* tex, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, tex);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_TEX));
        }

        void Pass::setIn(const char* name, int value, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, value);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_INT));
        }

        void Pass::setIn(const char* name, uint value, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, value);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_UINT));
        }

        void Pass::setIn(const char* name, Scalar value, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, value);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_SCALAR));
        }

        void Pass::setIn(const char* name, const Core::Vector2& value, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, value);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_VEC2));
        }

        void Pass::setIn(const char* name, const Core::Vector3& value, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, value);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_VEC3));
        }

        void Pass::setIn(const char* name, const Core::Vector4& value, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, value);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_VEC4));
        }

        void Pass::setIn(const char* name, const Core::Matrix2& value, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, value);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_MAT2));
        }

        void Pass::setIn(const char* name, const Core::Matrix3& value, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, value);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_MAT3));
        }

        void Pass::setIn(const char* name, const Core::Matrix4& value, uint slot/*, paramType p*/)
        {
            m_RPin.addParameter(name, value);
            m_paramIn[slot](std::pair<std::string, paramType>(name, PARAM_MAT4));
        }



        Texture* Pass::getTex(const char* name)
        {
            return m_RPout.getTexParameter( name );
        }

        int Pass::getInt(const char* name)
        {
            return m_RPout.getIntParameter( name );
        }

        uint Pass::getUint(const char* name)
        {
            return m_RPout.getUintParameter( name );
        }

        Scalar Pass::getScalar(const char* name)
        {
            return m_RPout.getScalarParameter( name );
        }

        Core::Vector2 Pass::getVec2(const char* name)
        {
            return m_RPout.getVec2Parameter( name );
        }

        Core::Vector3 Pass::getVec3(const char* name)
        {
            return m_RPout.getVec3Parameter( name );
        }

        Core::Vector4 Pass::getVec4(const char* name)
        {
            return m_RPout.getVec4Parameter( name );
        }

        Core::Matrix2 Pass::getMat2(const char* name)
        {
            return m_RPout.getMat2Parameter( name );
        }

        Core::Matrix3 Pass::getMat3(const char* name)
        {
            return m_RPout.getMat3Parameter( name );
        }

        Core::Matrix4 Pass::getMat4(const char* name)
        {
            return m_RPout.getMat4Parameter( name );
        }



        void Pass::setCanvas(Mesh* canvas)
        {
            m_canvas = canvas;
        }

        void Pass::setSizeModifier(Scalar w, Scalar h)
        {
            m_wModifier = w;
            m_hModifier = h;
        }

        uint Pass::getId()   const { return m_id;      }

        std::string Pass::getName() const { return m_name; }

    }
}
