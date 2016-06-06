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


        Pass::Pass(const std::string& name, uint w, uint h, uint nbIn, uint nbOut)
            : m_name     ( name )
            , m_nbin     ( nbIn )
            , m_nbout    ( nbOut )
            , m_width    ( w )
            , m_height   ( h )
            , m_wModifier( 1.f )
            , m_hModifier( 1.f )
            , m_canvas   ( nullptr )
        {
            // resize vectors of textures if necessary
            m_nameIn.resize  ( m_nbin,  std::pair<std::string, paramType>("", PARAM_TEX) );
            m_nameOut.resize ( m_nbout, std::pair<std::string, paramType>("", PARAM_TEX) );
        }

        Pass::~Pass() {}



        void Pass::setIn(const char* name, Texture* tex, uint slot)
        {
            m_paramIn.addParameter(name, tex);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_TEX;
        }

        void Pass::setIn(const char* name, int value, uint slot)
        {
            m_paramIn.addParameter(name, value);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_INT;
        }

        void Pass::setIn(const char* name, uint value, uint slot)
        {
            m_paramIn.addParameter(name, value);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_UINT;
        }

        void Pass::setIn(const char* name, Scalar value, uint slot)
        {
            m_paramIn.addParameter(name, value);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_SCALAR;
        }

        void Pass::setIn(const char* name, const Core::Vector2& value, uint slot)
        {
            m_paramIn.addParameter(name, value);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_VEC2;
        }

        void Pass::setIn(const char* name, const Core::Vector3& value, uint slot)
        {
            m_paramIn.addParameter(name, value);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_VEC3;
        }

        void Pass::setIn(const char* name, const Core::Vector4& value, uint slot)
        {
            m_paramIn.addParameter(name, value);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_VEC4;
        }

        void Pass::setIn(const char* name, const Core::Matrix2& value, uint slot)
        {
            m_paramIn.addParameter(name, value);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_MAT2;
        }

        void Pass::setIn(const char* name, const Core::Matrix3& value, uint slot)
        {
            m_paramIn.addParameter(name, value);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_MAT3;
        }

        void Pass::setIn(const char* name, const Core::Matrix4& value, uint slot)
        {
            m_paramIn.addParameter(name, value);
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = PARAM_MAT4;
        }



        Texture* Pass::getTex(const char* name)
        {
            return m_paramOut.getTexParameter( name );
        }

        int Pass::getInt(const char* name)
        {
            return m_paramOut.getIntParameter( name );
        }

        uint Pass::getUint(const char* name)
        {
            return m_paramOut.getUintParameter( name );
        }

        Scalar Pass::getScalar(const char* name)
        {
            return m_paramOut.getScalarParameter( name );
        }

        Core::Vector2 Pass::getVec2(const char* name)
        {
            return m_paramOut.getVec2Parameter( name );
        }

        Core::Vector3 Pass::getVec3(const char* name)
        {
            return m_paramOut.getVec3Parameter( name );
        }

        Core::Vector4 Pass::getVec4(const char* name)
        {
            return m_paramOut.getVec4Parameter( name );
        }

        Core::Matrix2 Pass::getMat2(const char* name)
        {
            return m_paramOut.getMat2Parameter( name );
        }

        Core::Matrix3 Pass::getMat3(const char* name)
        {
            return m_paramOut.getMat3Parameter( name );
        }

        Core::Matrix4 Pass::getMat4(const char* name)
        {
            return m_paramOut.getMat4Parameter( name );
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
