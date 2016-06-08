#include <algorithm>

#include <iostream>

#include <cstring>

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



        void Pass::setIn(const char* name, Texture* tex)
        {
            m_paramIn.addParameter(name, tex);
        }

        void Pass::setIn(const char* name, int value)
        {
            m_paramIn.addParameter(name, value);
        }

        void Pass::setIn(const char* name, uint value)
        {
            m_paramIn.addParameter(name, value);
        }

        void Pass::setIn(const char* name, Scalar value)
        {
            m_paramIn.addParameter(name, value);
        }

        void Pass::setIn(const char* name, const Core::Vector2& value)
        {
            m_paramIn.addParameter(name, value);
        }

        void Pass::setIn(const char* name, const Core::Vector3& value)
        {
            m_paramIn.addParameter(name, value);
        }

        void Pass::setIn(const char* name, const Core::Vector4& value)
        {
            m_paramIn.addParameter(name, value);
        }

        void Pass::setIn(const char* name, const Core::Matrix2& value)
        {
            m_paramIn.addParameter(name, value);
        }

        void Pass::setIn(const char* name, const Core::Matrix3& value)
        {
            m_paramIn.addParameter(name, value);
        }

        void Pass::setIn(const char* name, const Core::Matrix4& value)
        {
            m_paramIn.addParameter(name, value);
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


        void Pass::setupParamIn(uint slot, const std::string& name, paramType t)
        {
            m_nameIn[slot].first  = name;
            m_nameIn[slot].second = t;
        }

        void Pass::setupParamOut(uint slot, const std::string& name, paramType t)
        {
            m_nameOut[slot].first  = name;
            m_nameOut[slot].second = t;
        }


        uint Pass::getId()   const { return m_id; }

        std::string Pass::getName() const { return m_name; }

        void Pass::paramNamesFromShaderProgram(const ShaderProgram* prog)
        {
            // perform introspection on the shader progam to tell which are the parameters
            GLint i;
            GLint count;

            GLint  size;
            GLenum type;

            const GLsizei bufsize = 16;
            GLchar  name[bufsize];
            GLsizei length;

            // query the number of uniform
            glGetProgramiv(prog->getId(), GL_ACTIVE_UNIFORMS, &count);

            for (i = 0; i < count; ++ i)
            {
                glGetActiveUniform(prog->getId(), (GLuint)i, bufsize, &length, &size, &type, name);
                setupParamIn((GLuint)i, name, (type == 35665) ? PARAM_VEC3 : PARAM_TEX);
            }
        }


        bool Pass::connect(Pass* a, uint ia, Pass* b, uint ib)
        {
            std::string& name_out = a->m_nameOut[ia].first;
            std::string& name_in  = b->m_nameIn [ib].first;

            paramType type_out = a->m_nameOut[ia].second;
            paramType type_in  = b->m_nameIn [ib].second;

            if (type_out == type_in)
            {
                switch (type_in)
                {
                case PARAM_TEX:
                    b->setIn( name_in.c_str(), a->getTex(name_out.c_str()) );
                    break;
                case PARAM_INT:
                    b->setIn( name_in.c_str(), a->getInt(name_out.c_str()) );
                    break;
                case PARAM_UINT:
                    b->setIn( name_in.c_str(), a->getUint(name_out.c_str()) );
                    break;
                case PARAM_SCALAR:
                    b->setIn( name_in.c_str(), a->getScalar(name_out.c_str()) );
                    break;
                case PARAM_VEC2:
                    b->setIn( name_in.c_str(), a->getVec2(name_out.c_str()) );
                    break;
                case PARAM_VEC3:
                    b->setIn( name_in.c_str(), a->getVec3(name_out.c_str()) );
                    break;
                case PARAM_VEC4:
                    b->setIn( name_in.c_str(), a->getVec4(name_out.c_str()) );
                    break;
                case PARAM_MAT2:
                    b->setIn( name_in.c_str(), a->getMat2(name_out.c_str()) );
                    break;
                case PARAM_MAT3:
                    b->setIn( name_in.c_str(), a->getMat3(name_out.c_str()) );
                    break;
                case PARAM_MAT4:
                    b->setIn( name_in.c_str(), a->getMat4(name_out.c_str()) );
                    break;
                default:
                    break;
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        const char* Pass::getParamNameIn  ( Pass* p, uint slot )
        {
            return p->m_nameIn[slot].first.c_str();
        }

        const char* Pass::getParamNameOut ( Pass* p, uint slot )
        {
            return p->m_nameOut[slot].first.c_str();
        }

    }
}
