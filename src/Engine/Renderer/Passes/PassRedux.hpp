//#ifndef RADIUMENGINE_RENDERER_PASS_REDUX_HPP
//#define RADIUMENGINE_RENDERER_PASS_REDUX_HPP

//#include <Engine/RadiumEngine.hpp>
//#include <Engine/Renderer/Renderer.hpp>

//#include <Engine/Renderer/Passes/Pass.hpp>

//namespace Ra
//{
//    namespace Engine
//    {

//        class ReduxPass : public Pass
//        {
//        public:
//            ReduxPass(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut, uint priority);
//            virtual ~ReduxPass();

//            void renderPass();
//            void resizePass(uint w, uint h);
//            void resizePass();
//            void init();

//            virtual std::shared_ptr<Texture> getInternTextures(uint i);

//        private:
//            enum FboTags
//            {
//                FBO_MAIN = 0,
//                FBO_COUNT,
//            };
///*
//            enum TextureInternalTags
//            {
//                TEX_BLUR_PING = 0,
//                TEX_BLUR_PONG,
//                TEX_INTERNAL_COUNT,
//            };

//            enum TextureInTags
//            {
//                TEX_COLOR = 0,
//                TEX_IN_COUNT,
//            };

//            enum TextureOutTags
//            {
//                TEX_BLURRED = 0,
//                TEX_OUT_COUNT,
//            };

//            uint m_amount;
//            uint m_boxfactor;
//            uint m_pingPongSize;
//            std::array<std::unique_ptr<FBO>,FBO_COUNT> m_fbo;
//            std::array<std::shared_ptr<Texture>,TEX_INTERNAL_COUNT> m_texIntern;

//            enum Shaders
//            {
//                SHADER_DRAWSCREEN = 0,
//                SHADER_BLUR,
//                SHADER_COUNT,
//            };

//            const ShaderProgram *m_shader[SHADER_COUNT];*/
//        };

//    }
//}

//#endif // RADIUMENGINE_RENDERER_PASS_REDUX_HPP
