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
            PassT(const std::string& name, const T& val, paramType t);
            ~PassT();
            virtual void renderPass() override;
            virtual void resizePass(uint w, uint h) {}
            virtual void resizePass() {}
            virtual void init();

            virtual void* getDataPtr(paramType *t) override;

            virtual paramType generates() const override;

        public:
            T         m_value;
            paramType m_type;
        };

        template <typename T>
        PassT<T>::PassT(const std::string& name, const T& val, paramType t)
                : Pass(name, 1, 1, 0, 1)
                , m_value(val)
                , m_type ( t )
        {}

        template <typename T>
        PassT<T>::~PassT() {}

        template <typename T>
        void PassT<T>::init()
        {
            setupParamOut(0, "", m_type);
            m_paramOut.addParameter("", m_value);
        }

        template <typename T>
        void* PassT<T>::getDataPtr(paramType *t)
        {
            *t = m_type;
            return &m_value;
        }

        template <typename T>
        void PassT<T>::renderPass()
        {
            m_paramOut.updateParameter("", m_value);
        }

        template <typename T>
        paramType PassT<T>::generates() const
        {
            return m_type;
        }
    }
}


#endif // RADIUMENGINE_RENDERER_PASS_FLOAT_HPP
