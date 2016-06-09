#ifndef RADIUMENGINE_RENDERER_PASS_T_H
#define RADIUMENGINE_RENDERER_PASS_T_H

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Engine/Renderer/Passes/Pass.hpp>

namespace Ra
{
    namespace Engine
    {

        template <typename T>
        class PassT : public Pass
        {
        public:
            PassT(const std::string& name, uint nbOut, const T& val, paramType t);
            ~PassT();
            virtual void renderPass() {}
            virtual void resizePass(uint w, uint h) {}
            virtual void resizePass() {}
            virtual void init();

        public:
            T         m_value;
            paramType m_type;
        };

        template <typename T>
        PassT<T>::PassT(const std::string& name, uint nbOut, const T& val, paramType t)
                : Pass(name, 1, 1, 0, nbOut)
                , m_value(val)
                , m_type ( t )
        {}

        template <typename T>
        PassT<T>::~PassT() {}

        template <typename T>
        void PassT<T>::init()
        {
            // TODO (hugo) add support for multiple outputs (for loop and distinct names)
            setupParamOut(0, "", m_type);
            m_paramOut.addParameter("", m_value);
        }
    }
}


#endif // RADIUMENGINE_RENDERER_PASS_FLOAT_HPP
