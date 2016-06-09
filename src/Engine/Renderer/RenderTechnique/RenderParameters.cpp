#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace Engine
    {
        void RenderParameters::bind(const ShaderProgram* shader) const
        {
            m_intParamsVector.bind( shader );
            m_uintParamsVector.bind( shader );
            m_scalarParamsVector.bind( shader );

            m_vec2ParamsVector.bind( shader );
            m_vec3ParamsVector.bind( shader );
            m_vec4ParamsVector.bind( shader );

            m_mat2ParamsVector.bind( shader );
            m_mat3ParamsVector.bind( shader );
            m_mat4ParamsVector.bind( shader );

            m_texParamsVector.bind( shader );
        }



        void RenderParameters::addParameter( const char* name, int value )
        {
            m_intParamsVector[name] = IntParameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, uint value )
        {
            m_uintParamsVector[name] = UIntParameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, Scalar value )
        {
            m_scalarParamsVector[name] = ScalarParameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Vector2& value )
        {
            m_vec2ParamsVector[name] = Vec2Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Vector3& value )
        {
            m_vec3ParamsVector[std::string(name)] = Vec3Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Vector4& value )
        {
            m_vec4ParamsVector[name] = Vec4Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Matrix2& value )
        {
            m_mat2ParamsVector[name] = Mat2Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Matrix3& value )
        {
            m_mat3ParamsVector[name] = Mat3Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, const Core::Matrix4& value )
        {
            m_mat4ParamsVector[name] = Mat4Parameter( name, value );
        }

        void RenderParameters::addParameter( const char* name, Texture* tex )
        {
            m_texParamsVector[name] = TextureParameter( name, tex );
        }



        void RenderParameters::updateParameter( const char* name, int    value )
        {
            m_intParamsVector[name] = IntParameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, uint   value )
        {
            m_uintParamsVector[name] = UIntParameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, Scalar value )
        {
            m_scalarParamsVector[name] = ScalarParameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Vector2& value )
        {
            m_vec2ParamsVector[name] = Vec2Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Vector3& value )
        {
            m_vec3ParamsVector[name] = Vec3Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Vector4& value )
        {
            m_vec4ParamsVector[name] = Vec4Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Matrix2& value )
        {
            m_mat2ParamsVector[name] = Mat2Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Matrix3& value )
        {
            m_mat3ParamsVector[name] = Mat3Parameter( name, value );
        }

        void RenderParameters::updateParameter( const char* name, const Core::Matrix4& value )
        {
            m_mat4ParamsVector[name] = Mat4Parameter( name, value );
        }



        Texture* RenderParameters::getTexParameter( const char* name )
        {
            return m_texParamsVector[name].m_texture;
        }

        int RenderParameters::getIntParameter ( const char* name )
        {
            return m_intParamsVector[name].m_value;
        }

        uint RenderParameters::getUintParameter ( const char* name )
        {
            return m_uintParamsVector[name].m_value;
        }

        Scalar RenderParameters::getScalarParameter( const char* name )
        {
            return m_scalarParamsVector[name].m_value;
        }

        Core::Vector2& RenderParameters::getVec2Parameter( const char* name )
        {
            return m_vec2ParamsVector[name].m_value;
        }

        Core::Vector3& RenderParameters::getVec3Parameter( const char* name )
        {
            return m_vec3ParamsVector[name].m_value;
        }

        Core::Vector4& RenderParameters::getVec4Parameter( const char* name )
        {
            return m_vec4ParamsVector[name].m_value;
        }

        Core::Matrix2& RenderParameters::getMat2Parameter( const char* name )
        {
            return m_mat2ParamsVector[name].m_value;
        }

        Core::Matrix3& RenderParameters::getMat3Parameter( const char* name )
        {
            return m_mat3ParamsVector[name].m_value;
        }

        Core::Matrix4& RenderParameters::getMat4Parameter( const char* name )
        {
            return m_mat4ParamsVector[name].m_value;
        }



        void RenderParameters::concatParameters( const RenderParameters &params )
        {
            for (const auto& param : params.m_intParamsVector)
            {
                m_intParamsVector.insert( param );
            }

            for (const auto& param : params.m_uintParamsVector)
            {
                m_uintParamsVector.insert( param );
            }

            for (const auto& param : params.m_scalarParamsVector)
            {
                m_scalarParamsVector.insert( param );
            }

            for (const auto& param : params.m_vec2ParamsVector)
            {
                m_vec2ParamsVector.insert( param );
            }

            for (const auto& param : params.m_vec3ParamsVector)
            {
                m_vec3ParamsVector.insert( param );
            }

            for (const auto& param : params.m_vec4ParamsVector)
            {
                m_vec4ParamsVector.insert( param );
            }

            for (const auto& param : params.m_mat2ParamsVector)
            {
                m_mat2ParamsVector.insert( param );
            }

            for (const auto& param : params.m_mat3ParamsVector)
            {
                m_mat3ParamsVector.insert( param );
            }

            for (const auto& param : params.m_mat4ParamsVector)
            {
                m_mat4ParamsVector.insert( param );
            }

            for (const auto& param : params.m_texParamsVector)
            {
                m_texParamsVector.insert( param );
            }
        }

    } // namespace Engine
} // namespace Ra
