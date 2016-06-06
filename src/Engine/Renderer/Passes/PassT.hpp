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
            PassT(const std::string& name, uint nbIn, uint nbOut, T val);
            ~PassT();
            virtual void renderPass() {}
            virtual void resizePass(uint w, uint h) {}
            virtual void resizePass() {}
            virtual void init();

        public:
            T m_value;
        };

        template <typename T>
        PassT<T>::PassT(const std::string& name, uint nbIn, uint nbOut, T val)
                : Pass(name, 1, 1, nbIn, nbOut)
                , m_value(val)
        {}

        template <typename T>
        PassT<T>::~PassT() {}

        template <typename T>
        void PassT<T>::init()
        {
            m_paramOut.addParameter(m_name.c_str(), m_value);
        }
    }
}


#endif // RADIUMENGINE_RENDERER_PASS_FLOAT_HPP
