#ifndef RADIUMENGINE_RENDERPARAMETERS_HPP
#define RADIUMENGINE_RENDERPARAMETERS_HPP

#include <Engine/RaEngine.hpp>

#include <vector>
#include <set>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace Engine
    {
        class Texture;
        class ShaderProgram;
    }
}

namespace Ra
{
    namespace Engine
    {

        enum paramType
        {
            PARAM_INT=0,
            PARAM_UINT,
            PARAM_SCALAR,
            PARAM_VEC2,
            PARAM_VEC3,
            PARAM_VEC4,
            PARAM_MAT2,
            PARAM_MAT3,
            PARAM_MAT4,
            PARAM_TEX,
            PARAM_COUNT
        };


        class RA_ENGINE_API RenderParameters
        {
        public:

            class Parameter
            {
            public:
                Parameter() = default;
                Parameter( const char* name ) : m_name( name ) {}
                virtual void bind(const ShaderProgram* shader ) const = 0;
                
                const char* m_name;
            };

            template <typename T>
            class TParameter : public Parameter
            {
            public:
                TParameter() = default;
                TParameter( const char* name, const T& value )
                    : Parameter( name ), m_value( value ) {}
                virtual void bind(const ShaderProgram* shader ) const override;

                T m_value;
            };

            class TextureParameter : public Parameter
            {
            public:
                TextureParameter() = default;
                TextureParameter( const char* name, Texture* tex, int texUnit = -1 )
                    :  Parameter( name ), m_texture( tex ), m_texUnit( texUnit ) {}
                virtual void bind( const ShaderProgram* shader ) const override;

                Texture* m_texture;
                int m_texUnit;
            };

            template <typename T> class UniformBindableVector : public std::map<std::string, T, std::less<std::string>, Core::AlignedAllocator<T,16> >
            {
            public:
                void bind(const ShaderProgram* shader ) const;
            };

            typedef TParameter<int>    IntParameter;
            typedef TParameter<uint>   UIntParameter;
            typedef TParameter<Scalar> ScalarParameter;

            typedef TParameter<Core::Vector2> Vec2Parameter;
            typedef TParameter<Core::Vector3> Vec3Parameter;
            typedef TParameter<Core::Vector4> Vec4Parameter;

            typedef TParameter<Core::Matrix2> Mat2Parameter;
            typedef TParameter<Core::Matrix3> Mat3Parameter;
            typedef TParameter<Core::Matrix4> Mat4Parameter;

        public:
            void addParameter( const char* name, Texture* tex );

            void addParameter( const char* name, int    value );
            void addParameter( const char* name, uint   value );
            void addParameter( const char* name, Scalar value );

            void addParameter( const char* name, const Core::Vector2& value );
            void addParameter( const char* name, const Core::Vector3& value );
            void addParameter( const char* name, const Core::Vector4& value );

            void addParameter( const char* name, const Core::Matrix2& value );
            void addParameter( const char* name, const Core::Matrix3& value );
            void addParameter( const char* name, const Core::Matrix4& value );


            void updateParameter( const char* name, int    value );
            void updateParameter( const char* name, uint   value );
            void updateParameter( const char* name, Scalar value );

            void updateParameter( const char* name, const Core::Vector2& value );
            void updateParameter( const char* name, const Core::Vector3& value );
            void updateParameter( const char* name, const Core::Vector4& value );

            void updateParameter( const char* name, const Core::Matrix2& value );
            void updateParameter( const char* name, const Core::Matrix3& value );
            void updateParameter( const char* name, const Core::Matrix4& value );


            Texture* getTexParameter( const char* name );

            int    getIntParameter   ( const char* name );
            uint   getUintParameter  ( const char* name );
            Scalar getScalarParameter( const char* name );

            Core::Vector2& getVec2Parameter( const char* name );
            Core::Vector3& getVec3Parameter( const char* name );
            Core::Vector4& getVec4Parameter( const char* name );

            Core::Matrix2& getMat2Parameter( const char* name );
            Core::Matrix3& getMat3Parameter( const char* name );
            Core::Matrix4& getMat4Parameter( const char* name );


            void concatParameters( const RenderParameters& params );

            void bind(const ShaderProgram* shader ) const;

            void print() const
            {
                for (const auto& p : m_scalarParamsVector)
                {
                    LOG( logDEBUG ) << "  " << p.first << " : " << p.second.m_name;
                }
            }

        private:
            // FIXME(Charly): Any way to simplify this a bit ?
            UniformBindableVector<IntParameter>     m_intParamsVector;
            UniformBindableVector<UIntParameter>    m_uintParamsVector;
            UniformBindableVector<ScalarParameter>  m_scalarParamsVector;

            UniformBindableVector<Vec2Parameter>    m_vec2ParamsVector;
            UniformBindableVector<Vec3Parameter>    m_vec3ParamsVector;
            UniformBindableVector<Vec4Parameter>    m_vec4ParamsVector;

            UniformBindableVector<Mat2Parameter>    m_mat2ParamsVector;
            UniformBindableVector<Mat3Parameter>    m_mat3ParamsVector;
            UniformBindableVector<Mat4Parameter>    m_mat4ParamsVector;

            UniformBindableVector<TextureParameter> m_texParamsVector;
        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderTechnique/RenderParameters.inl>

#endif // RADIUMENGINE_RENDERPARAMETERS_HPP
