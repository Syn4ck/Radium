#ifndef RADIUMENGINE_RENDERER_PASS_HPP
#define RADIUMENGINE_RENDERER_PASS_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace Ra
{
    namespace Engine
    {

        /**
         * \class Pass
         *
         * @brief The class representing the post-processing effects
         * @note Deserve a better name (TODO hugo)
         *
         * The pass class is the base class to deal with post-processing
         * effects driven by shaders.
         *
         * Parameters are managed via RenderParameters, and are set/accessed
         * by their names. Names are associated with types (enum paramType).
         *
         * This class manages the memory for textures and the FBO required,
         * has a quad mesh to be able to render, and automatically deduces
         * input parameters' name and type from the shader (glGetProgramiv).
         *
         * The class defines a connect() function to define the behavior
         * when connecting two nodes.
         *
         */

        class RA_ENGINE_API Pass
        {
        public:
            Pass(const std::string& name, uint w, uint h, uint nbIn, uint nbOut);
            virtual ~Pass() = 0;

            virtual void init() = 0;

            virtual void renderPass( const RenderData& renderData ) = 0;

            /// Function to be called by the renderer when resizing.
            /// Basically what really modify m_width and m_height.
            virtual void resizePass( uint w, uint h ) = 0;

            ///{@
            /// Set of functions used to set RenderParameter value to a shader.
            /// Will add a new parameter to m_paramIn <b>without</b>
            /// type-checking.
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
            ///@}

            ///{@
            /// Get output of a pass by name. No type-checking.
            Texture*      getTex   (const std::string& name);

            int           getInt   (const std::string& name);
            uint          getUint  (const std::string& name);
            Scalar        getScalar(const std::string& name);

            Core::Vector2 getVec2  (const std::string& name);
            Core::Vector3 getVec3  (const std::string& name);
            Core::Vector4 getVec4  (const std::string& name);

            Core::Matrix2 getMat2  (const std::string& name);
            Core::Matrix3 getMat3  (const std::string& name);
            Core::Matrix4 getMat4  (const std::string& name);
            ///@}

            /// Set a geometry to render on. Doesn't manage memory.
            void setCanvas(Mesh* canvas);

            /// Apply an aspect ratio to the dimensions.
            void setSizeModifier(Scalar w = 1.0, Scalar h = 1.0);

            ///{@
            /// Set type and name of a slot.
            void setupParamIn  (uint slot, const std::string& name, paramType t);
            void setupParamOut (uint slot, const std::string& name, paramType t);
            ///@}

            uint getId() const { return m_id; }
            std::string getName() const { return m_name; }

            uint getNbIn () const { return m_nbin;  }
            uint getNbOut() const { return m_nbout; }

            /// Perform introspection on a shader program
            /// and set inputs/outputs names.
            void paramNamesFromShaderProgram(const ShaderProgram* prog);

            /// Tells if the pass generate data from nothing, and give type.
            virtual paramType generates() const;

            /// Return nothing by default, used only for data generators.
            virtual void* getDataPtr(paramType* t);

            /// Function pointer to be passed to a container that would need
            /// to connect one pass to another with slot considerations.
            static bool connect( Pass* a, uint ia, Pass* b, uint ib );

            /// Function pointer to get a parameter name.
            static const std::string& getParamNameIn( Pass* p, uint slot );

            /// Function pointer to get a parameter name.
            static const std::string& getParamNameOut( Pass* p, uint slot );

        protected:
            /// Will reinitialize every textures and reattach to fbo.
            /// Called by resizePass(uint w, uint h).
            virtual void resizePass() = 0;



        public:
            RenderParameters m_paramIn;   ///< input  render parameters
            RenderParameters m_paramOut;  ///< output render parameters

        protected:
            /// Mapping of names to inputs slots.
            std::vector<std::pair<std::string, paramType>> m_nameIn;
            /// Mapping of names to outputs slots.
            std::vector<std::pair<std::string, paramType>> m_nameOut;

            std::string m_name;
            uint        m_id;

            uint m_nbin;
            uint m_nbout;

            uint m_width;
            uint m_height;

            Scalar m_wModifier; ///< Modifier (factor) for the width.
            Scalar m_hModifier; ///< Modifier (factor) for the height.

            /// Physical space for textures.
            std::vector<std::unique_ptr<Texture>> m_outputs;

            /// Geometry for GL to render the fragment shader on.
            Mesh* m_canvas;

            static const GLenum buffers[];
        };


    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_PASS_HPP
