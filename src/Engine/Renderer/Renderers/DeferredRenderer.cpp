#include <Engine/Renderer/Renderers/DeferredRenderer.hpp>

#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Renderers/DebugRender.hpp>

namespace Ra
{
    namespace Engine
    {
        static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };

        DeferredRenderer::DeferredRenderer(uint width, uint height)
            : Renderer(width, height)
            , m_fbo(nullptr)
            , m_postprocessFbo(nullptr)
        {
        }

        DeferredRenderer::~DeferredRenderer()
        {
        }

        void DeferredRenderer::initializeInternal()
        {
            initShaders();
            initBuffers();
        }

        void DeferredRenderer::initShaders()
        {
            // TODO(charly): Add shaders
            m_shaderMgr->addShaderProgram("GeometryPass", "../Shaders/Deferred/GeometryPass.vert.glsl", "../Shaders/Deferred/GeometryPass.frag.glsl");
            m_shaderMgr->addShaderProgram("LightPass", "../Shaders/Deferred/LightPass.vert.glsl", "../Shaders/Deferred/LightPass_Point.frag.glsl");
        }

        void DeferredRenderer::initBuffers()
        {
            m_fbo.reset(new FBO(FBO::Components(FBO::COLOR | FBO::DEPTH), m_width, m_height));
            m_postprocessFbo.reset(new FBO(FBO::Components(FBO::COLOR), m_width, m_height));

            m_textures[TEX_DEPTH].reset(new Texture("TEX_DEPTH", GL_TEXTURE_2D));
            m_textures[TEX_POSITION].reset(new Texture("TEX_POSITION", GL_TEXTURE_2D));
            m_textures[TEX_NORMAL].reset(new Texture("TEX_NORMAL", GL_TEXTURE_2D));
            m_textures[TEX_ALBEDO].reset(new Texture("TEX_ALBEDO", GL_TEXTURE_2D));
            m_textures[TEX_SPECULAR].reset(new Texture("TEX_SPECULAR", GL_TEXTURE_2D));
            m_textures[TEX_LIT].reset(new Texture("TEX_LIT", GL_TEXTURE_2D));

            for (const auto& tex : m_textures)
            {
                std::string name                               = tex->getName();
                m_secondaryTextures["Deferred Renderer"][name] = tex.get();
            }
        }

        void DeferredRenderer::updateStepInternal(const RenderData& renderData)
        {
        }

        void DeferredRenderer::renderInternal(const RenderData& renderData)
        {
            const ShaderProgram* shader;

            m_fbo->bind();
            GL_ASSERT(glViewport(0, 0, m_width, m_height));

            GL_ASSERT(glDepthMask(GL_TRUE));
            GL_ASSERT(glColorMask(1, 1, 1, 1));

            const float clearColor[] = { 0.1, 0.1, 0.1, 0.0 };
            const float clearDepth   = 1.0;

            GL_ASSERT(glDrawBuffers(5, buffers));

            GL_ASSERT(glClearBufferfv(GL_DEPTH, 0, &clearDepth));
            GL_ASSERT(glClearBufferfv(GL_COLOR, 4, clearColor));

            GL_ASSERT(glDrawBuffers(4, buffers));
            GL_ASSERT(glClearColor(0, 0, 0, 0));
            GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));

            GL_ASSERT(glEnable(GL_DEPTH_TEST));
            GL_ASSERT(glDepthFunc(GL_LESS));

            shader = m_shaderMgr->getShaderProgram("GeometryPass");
            shader->bind();

            for (const auto& ro : m_fancyRenderObjects)
            {
                if (ro->isVisible())
                {
                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 N = M.inverse().transpose();

                    shader->setUniform("transform.proj", renderData.projMatrix);
                    shader->setUniform("transform.view", renderData.viewMatrix);
                    shader->setUniform("transform.model", M);
                    shader->setUniform("transform.worldNormal", N);

                    ro->getRenderTechnique()->material->bind(shader);
                    ro->getMesh()->render();
                }
            }

            GL_ASSERT(glDepthMask(GL_FALSE));
            GL_ASSERT(glDisable(GL_DEPTH_TEST));
            GL_ASSERT(glDrawBuffers(1, buffers + 4));

            GL_ASSERT(glEnable(GL_BLEND));
            GL_ASSERT(glBlendFunc(GL_ONE, GL_ONE));

            shader = m_shaderMgr->getShaderProgram("LightPass");
            shader->bind();

            shader->setUniform("uPosition", m_textures[TEX_POSITION].get(), 0);
            shader->setUniform("uNormal", m_textures[TEX_NORMAL].get(), 1);
            shader->setUniform("uDiffuse", m_textures[TEX_ALBEDO].get(), 2);
            shader->setUniform("uSpecular", m_textures[TEX_SPECULAR].get(), 3);
            shader->setUniform("uView", renderData.viewMatrix);

            shader->setUniform("ambientLight", 1);
            m_quadMesh->render();
            shader->setUniform("ambientLight", 0);

            for (const auto& l : m_lights)
            {
                RenderParameters params;
                l->getRenderParameters(params);

                params.bind(shader);

                m_quadMesh->render();
            }

            GL_ASSERT(glDisable(GL_BLEND));
            GL_ASSERT(glDepthMask(GL_TRUE));

            m_fbo->unbind();
        }

        void DeferredRenderer::postProcessInternal(const RenderData& renderData)
        {
            CORE_UNUSED(renderData);

            m_postprocessFbo->bind();
            glViewport(0, 0, m_width, m_height);

            GL_ASSERT(glDepthMask(GL_FALSE));
            GL_ASSERT(glColorMask(1, 1, 1, 1));

            glDrawBuffers(1, buffers);
            auto shader = m_shaderMgr->getShaderProgram("DrawScreen");
            shader->bind();
            shader->setUniform("screenTexture", m_textures[TEX_LIT].get(), 0);
            m_quadMesh->render();
        }

        void DeferredRenderer::resizeInternal()
        {
            m_textures[TEX_DEPTH]->initGL(GL_DEPTH_COMPONENT24, m_width, m_height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
            m_textures[TEX_POSITION]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_NORMAL]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_ALBEDO]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_SPECULAR]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_LIT]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            m_fbo->bind();
            m_fbo->setSize(m_width, m_height);
            m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_textures[TEX_DEPTH].get());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[TEX_POSITION].get());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[TEX_NORMAL].get());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT2, m_textures[TEX_ALBEDO].get());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT3, m_textures[TEX_SPECULAR].get());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT4, m_textures[TEX_LIT].get());
            m_fbo->check();
            m_fbo->unbind();

            m_postprocessFbo->bind();
            m_postprocessFbo->setSize(m_width, m_height);
            m_postprocessFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_fancyTexture.get());
            m_postprocessFbo->check();
            m_postprocessFbo->unbind(true);

            GL_CHECK_ERROR;

            GL_ASSERT(glDrawBuffer(GL_BACK));
            GL_ASSERT(glReadBuffer(GL_BACK));
        }
    }
}
