#include "passblur2.h"

namespace Ra {
    namespace Engine {

        PassBlur2::PassBlur2(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                           Mesh* canvas, uint priority, uint amount, uint box)
            : Pass(name, w, h, nTexIn, nTexOut, canvas, priority)
            , m_boxfactor(box)
            , m_pingpong("blur2", w, h, nTexIn, nTexOut, canvas, priority, amount)
        {
//            // parameters to set as uniforms to SHADER_PINGPONG
//            m_parameters[0].addParameter("offset", Core::Vector2(1.0 / m_width, 0.0));
//            m_parameters[0].addParameter("wView", m_width );
//            m_parameters[0].addParameter("hView", m_height);

//            // parameters to set as uniforms to SHADER_PONGPING
//            m_parameters[1].addParameter("offset", Core::Vector2(0.0, 1.0 / m_height));
//            m_parameters[1].addParameter("wView", m_width );
//            m_parameters[1].addParameter("hView", m_height);
        }

        PassBlur2::~PassBlur2()
        {
        }

        void PassBlur2::init()
        {
            // the init stuff
            m_pingpong.setCanvas(m_canvas);
            m_pingpong.setIn(0, m_texIn[0]);
            m_pingpong.init();

            // load shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader[SHADER_BLUR] = shaderMgr->addShaderProgram("Blur", "../Shaders/Basic2D.vert.glsl", "../Shaders/Blur.frag.glsl");

            // and tell the pingpong passe to use parameters
            m_pingpong.setPingPongShader(m_shader[SHADER_BLUR]);
            m_pingpong.setPongPingShader(m_shader[SHADER_BLUR]);
        }

        void PassBlur2::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassBlur2::resizePass()
        {
            m_pingpong.resizePass(m_width, m_height);
        }

        void PassBlur2::renderPass()
        {
            // just render the ping-pong pass, which in this case is the only thing to do
            m_pingpong.renderPass();
        }

        std::shared_ptr<Texture> PassBlur2::getInternTextures(uint i)
        {
            return m_pingpong.getInternTextures(i);
        }

        Texture* PassBlur2::getOut(uint slot)
        {
            return m_pingpong.getOut(slot);
        }

    }
}
