///* This is ugly and should be splitted into more files, maybe */

//#ifndef RADIUMENGINE_FORWARDRENDERER_PASSCOMPONENT_HPP
//#define RADIUMENGINE_FORWARDRENDERER_PASSCOMPONENT_HPP

//#include <Engine/RadiumEngine.hpp>
//#include <Engine/Renderer/Renderer.hpp>

//namespace Ra
//{
//    namespace Engine
//    {


//        class RA_ENGINE_API PassComponent
//        {
//        public:
//            PassComponent(const std::string& name);
//            virtual ~PassComponent() = 0;

//            virtual std::string getPassName() const { return m_name; }

//            virtual void renderPass() = 0;

//        protected:
//            std::string m_name;
//            Texture    *m_texIn;
//            Texture    *m_texOut;
//        };

//#endif // RADIUMENGINE_FORWARDRENDERER_PASSCOMPONENT_HPP



//// #ifndef RADIUMENGINE_FORWARDRENDERER_PASS_HPP
//// #define RADIUMENGINE_FORWARDRENDERER_PASS_HPP

//        class RA_ENGINE_API Pass : public PassComponent
//        {
//        public:
//            Pass(const std::string& name, int w, int h, int ntex);
//            virtual ~Pass() = 0;

//            virtual void render() = 0;
//            virtual void initFbo() = 0;

//        private:
//            std::unique_ptr<FBO> m_fbo;
//            int m_requiredTextures;
//            int m_width;
//            int m_height;
//        };

//// #endif // RADIUMENGINE_FORWARDRENDERER_PASS_HPP



//// #ifndef RADIUMENGINE_FORWARDRENDERER_PASS_LUM_HP
//// #define RADIUMENGINE_FORWARDRENDERER_PASS_LUM_HP

//        class

//// #endif // RADIUMENGINE_FORWARDRENDERER_PASS_LUM_HP


////#ifndef RADIUMENGINE_FORWARDRENDERER_PASSCOMPOSITE_HPP
////#define RADIUMENGINE_FORWARDRENDERER_PASSCOMPOSITE_HPP

////        class RA_ENGINE_API PassComposite : public PassComponent
////        {
////        public:
////            PassComposite(const std::string& name) : PassComponent(name) {}
////            virtual ~PassComposite();

////            virtual void add(PassComponent& p);
////            virtual void remove(const int i);
////            virtual void remove(const std::string& name);
////            virtual PassComponent operator[](const int i) const;

////            void renderPass();  // actually call renderPass on every child node

////        private:
////            std::unique_ptr<std::vector<PassComponent>> m_childrens;  // unique_ptr<T> ?
////        };

////#endif // RADIUMENGINE_FORWARDRENDERER_PASSCOMPOSITE_HPP



//    } // namespace Engine
//} // namespace Ra
