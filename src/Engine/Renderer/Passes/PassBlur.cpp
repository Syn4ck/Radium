#include "PassBlur.hpp"

#include <iostream>

namespace Ra {
    namespace Engine {

        PassBlur::PassBlur(const std::string& name, uint w, uint h, uint nTexIn, uint nTexOut,
                           Mesh* canvas, uint priority, uint amount, uint box)
            : Pass(name, w, h, nTexIn, nTexOut, canvas, priority)
            , m_boxfactor(box)
            , m_pingpong("blur2", w/box, h/box, nTexIn, nTexOut, canvas, priority, amount)
        {
            // parameters to set as uniforms to SHADER_PINGPONG
            m_params[0].addParameter("offset", Core::Vector2(1.0 / m_width,  0.0));
            m_params[1].addParameter("offset", Core::Vector2(0.0, 1.0 / m_height));

            m_params[0].addParameter("view_width", (int) m_width);
            m_params[1].addParameter("view_width", (int) m_width);

            m_params[0].addParameter("view_height", (int) m_height);
            m_params[1].addParameter("view_height", (int) m_height);
        }

        PassBlur::~PassBlur()
        {
        }

        void PassBlur::init()
        {
            // the init stuff
            m_pingpong.setCanvas(m_canvas);
            m_pingpong.setIn(0, m_texIn[0]);
            m_pingpong.init();

            // load shader
            ShaderProgramManager* shaderMgr = ShaderProgramManager::getInstance();
            m_shader[SHADER_BLUR] = shaderMgr->addShaderProgram("Blur", "../Shaders/Basic2D.vert.glsl", "../Shaders/Blur.frag.glsl");

            // and tell the pingpong passe to use parameters
            m_pingpong.setPingPongShader(m_shader[SHADER_BLUR], m_params+0);
            m_pingpong.setPongPingShader(m_shader[SHADER_BLUR], m_params+1);
        }

        void PassBlur::resizePass(uint w, uint h)
        {
            m_width  = w;
            m_height = h;
            resizePass();
        }

        void PassBlur::resizePass()
        {
            // tell the ping-pong object to resize()
            m_pingpong.resizePass(m_width/m_boxfactor, m_height/m_boxfactor);
        }

        void PassBlur::renderPass()
        {
            // just render the ping-pong pass, which in this case is the only thing to do
            m_pingpong.renderPass();
        }

        std::shared_ptr<Texture> PassBlur::getInternTextures(uint i)
        {
            return m_pingpong.getInternTextures(i);
        }

        Texture* PassBlur::getOut(uint slot)
        {
            return m_pingpong.getOut(slot);
        }

    }
}

