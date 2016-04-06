//#include <Engine/Renderer/Passes/Pass.hpp>
//#include <Engine/Renderer/OpenGL/FBO.hpp>

//namespace Ra
//{
//    namespace Engine {

//        // COMPONENT
//        PassComponent::PassComponent(const std::string& name)
//            : m_name(name)
//            , m_texIn(nullptr)
//            , m_texOut(nullptr)
//        {
//        }

//        // LEAF
//        Pass::Pass(const std::string& name, int w, int h, int ntex=0)
//            : PassComponent(name)
//            , m_requiredTextures(ntex)
//            , m_width(w)
//            , m_height(h)
//        {
//            m_fbo.reset( new FBO( FBO::Components( FBO::COLOR ), m_width, m_height ) );
//        }


//        virtual void Pass::initFbo()
//        {
//            m_fbo->bind();
//            m_fbo->setSize( m_width, m_height );
//            m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_texIn);
//            m_fbo->attachTexture(GL_COLOR_ATTACHMENT1, m_texOut);
//            m_fbo->check();
//            m_fbo->unbind( true );
//        }



////        // COMPOSITE/NODE
////        PassComposite::PassComposite(const std::string& name)
////            : PassComponent(name)
////        {
////        }

////        PassComposite::add(PassComponent& p)
////        {
////            m_childrens->push_back(p);
////        }

////        PassComposite::remove()
////        {
////            m_childrens->pop_back();
////        }

////        PassComposite::remove(const int i)
////        {
////            m_childrens->erase(m_childrens->begin() + i)
////        }

////        PassComposite::remove(const std::string& name)
////        {
////            std::vector<PassComponent>::iterator elem;
////            elem = std::find(m_childrens->begin(), m_childrens->end(), name);

////            if (elem != m_childrens->end())
////            {
////                m_childrens->erase(elem);
////            }
////        }

////        PassComposite::operator[](const int i)
////        {
////            return m_childrens.get()[i];
////        }

//    }
//}
