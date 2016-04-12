#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Renderers/DebugRender.hpp>


#define NO_TRANSPARENCY
namespace Ra
{
    namespace Engine
    {

        namespace
        {
            const GLenum buffers[] =
            {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3,
                GL_COLOR_ATTACHMENT4,
                GL_COLOR_ATTACHMENT5,
                GL_COLOR_ATTACHMENT6,
                GL_COLOR_ATTACHMENT7
            };
        }

        ForwardRenderer::ForwardRenderer( uint width, uint height )
            : Renderer( width, height )
            , m_fbo( nullptr )
            , m_postprocessFbo( nullptr )
            , m_dummy( "YoloPass", width, height, 1, 1 )
            , m_lumin( "LumPass",  width, height, 1, 1 )
            , m_highp( "HighPass", width, height, 2, 1 )
            , m_blurp( "BlurPass", width, height, 1, 1, 16 )
            , m_tonmp( "TonemapPass", width, height, 1, 1 )
            , m_compp( "CompositePass", width, height, 1, 1 )
        {
        }

        ForwardRenderer::~ForwardRenderer()
        {
            ShaderProgramManager::destroyInstance();
        }

        void ForwardRenderer::initializeInternal()
        {
            initShaders();
            initBuffers();

            DebugRender::createInstance();
            DebugRender::getInstance()->initialize();
        }

        void ForwardRenderer::initShaders()
        {
            m_shaderMgr->addShaderProgram("DepthAmbientPass", "../Shaders/DepthAmbientPass.vert.glsl", "../Shaders/DepthAmbientPass.frag.glsl");
            m_shaderMgr->addShaderProgram("FinalCompose", "../Shaders/Basic2D.vert.glsl", "../Shaders/FinalCompose.frag.glsl");
            m_shaderMgr->addShaderProgram("Tonemapping", "../Shaders/Basic2D.vert.glsl", "../Shaders/Tonemapping.frag.glsl");
            m_shaderMgr->addShaderProgram("Luminance", "../Shaders/Basic2D.vert.glsl", "../Shaders/Luminance.frag.glsl");
            m_shaderMgr->addShaderProgram("MinMax", "../Shaders/Basic2D.vert.glsl", "../Shaders/MinMax.frag.glsl");
            m_shaderMgr->addShaderProgram("Blur", "../Shaders/Basic2D.vert.glsl", "../Shaders/Blur.frag.glsl");
            m_shaderMgr->addShaderProgram("Dummy", "../Shaders/Basic2D.vert.glsl", "../Shaders/Dummy.frag.glsl");
            m_shaderMgr->addShaderProgram("Highpass", "../Shaders/Basic2D.vert.glsl", "../Shaders/Highpass.frag.glsl");
            m_shaderMgr->addShaderProgram("DummyGreen", "../Shaders/Basic2D.vert.glsl", "../Shaders/DummyGreen.frag.glsl");
            m_shaderMgr->addShaderProgram("HighpassTEST", "../Shaders/Basic2D.vert.glsl", "../Shaders/HighpassNoFetch.frag.glsl");
        }

        void ForwardRenderer::initBuffers()
        {
            m_fbo.reset( new FBO( FBO::Components( FBO::COLOR | FBO::DEPTH ), m_width, m_height ) );
            m_postprocessFbo.reset( new FBO( FBO::Components( FBO::COLOR ), m_width, m_height ) );

            // Render pass
            m_textures[TEX_DEPTH].reset( new Texture( "Depth", GL_TEXTURE_2D ) );
            m_textures[TEX_NORMAL].reset( new Texture( "Normal", GL_TEXTURE_2D ) );
            m_textures[TEX_LIT].reset( new Texture( "HDR", GL_TEXTURE_2D ) );

            // this is where the branching occurs, maybe it should be done later on
            // but if we want to initiate passes' FBOs we need to branch them before
            m_dummy.setIn(0, m_textures[TEX_LIT].get());
            m_dummy.initFbos();

            m_lumin.setIn(0, m_dummy.getOut(0));
            m_lumin.initFbos();

            m_highp.setIn(0, m_dummy.getOut(0));
            m_highp.setIn(1, m_lumin.getOut(0));
            m_highp.initFbos();

            m_blurp.setIn(0, m_highp.getOut(0));
            m_blurp.initFbos();

            //m_tonmp.setIn(0, m_textures[TEX_LIT].get());
            m_tonmp.setIn(0, m_dummy.getOut(0));
            m_tonmp.setIn(1, m_lumin.getOut(0));
            m_tonmp.initFbos();

            m_compp.setIn(0, m_tonmp.getOut(0));
            m_compp.setIn(1, m_blurp.getOut(0));
            m_compp.initFbos();

            m_secondaryTextures["Depth Texture"]        = m_textures[TEX_DEPTH].get();
            m_secondaryTextures["Normal Texture"]       = m_textures[TEX_NORMAL].get();
            m_secondaryTextures["HDR Texture"]          = m_textures[TEX_LIT].get();

            m_secondaryTextures["sec.1 Dummy pass"]     = m_dummy.getOut(0);
            m_secondaryTextures["sec.2 Luminance Pass"] = m_lumin.getOut(0);
            m_secondaryTextures["sec.3 Highpass Pass"]  = m_highp.getOut(0);
            m_secondaryTextures["sec.4 Blur Pass"]      = m_blurp.getOut(0);
            m_secondaryTextures["sec.5 Tonemap Pass"]   = m_tonmp.getOut(0);
            m_secondaryTextures["sec.6 Composite Pass"] = m_compp.getOut(0);
        }

        void ForwardRenderer::updateStepInternal( const RenderData& renderData )
        {
            // Do nothing right now
        }

        void ForwardRenderer::renderInternal( const RenderData& renderData )
        {
            // FIXME(Charly): Do a bit of cleanup in the forward renderer
            // (e.g. Remove the "depth ambient pass")
            const ShaderProgram* shader;

            m_fbo->useAsTarget();

            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );

            GL_ASSERT( glDrawBuffers( 2, buffers ) );

            const Core::Colorf clearColor = Core::Colors::FromChars<Core::Colorf>(42, 42, 42, 0);
            const Core::Colorf clearZeros = Core::Colors::Black<Core::Colorf>();
            const float clearDepth( 1.0 );

            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearZeros.data() ) );   // Clear normals
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );   // Clear depth

            // Z prepass
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );

            GL_ASSERT( glDisable( GL_BLEND ) );

            GL_ASSERT( glDrawBuffers( 1, buffers ) );

            if (m_wireframe)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glEnable(GL_LINE_SMOOTH);
                glLineWidth(1.f);
                glEnable(GL_POLYGON_OFFSET_LINE);
                glPolygonOffset(-1.0f, -1.1f);
            }

            shader = m_shaderMgr->getShaderProgram("DepthAmbientPass");
            shader->bind();
            for ( const auto& ro : m_fancyRenderObjects )
            {
                if ( ro->isVisible() )
                {
                    // bind data
                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 N = M.inverse().transpose();

                    shader->setUniform( "transform.proj", renderData.projMatrix );
                    shader->setUniform( "transform.view", renderData.viewMatrix );
                    shader->setUniform( "transform.model", M );
                    shader->setUniform( "transform.worldNormal", N );

                    ro->getRenderTechnique()->material->bind( shader );

                    // render
                    ro->getMesh()->render();
                }
            }

            // Light pass
            GL_ASSERT( glDepthFunc( GL_LEQUAL ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );

            GL_ASSERT( glEnable( GL_BLEND ) );
            GL_ASSERT( glBlendFunc( GL_ONE, GL_ONE ) );

            GL_ASSERT( glDrawBuffers( 1, buffers + 1 ) );   // Draw color texture

            if ( m_lights.size() > 0 )
            {
                for ( const auto& l : m_lights )
                {
                    // TODO(Charly): Light render params
                    RenderParameters params;
                    l->getRenderParameters( params );

                    for ( const auto& ro : m_fancyRenderObjects )
                    {
                        if ( ro->isVisible() )
                        {
                            shader = ro->getRenderTechnique()->shader;

                            // bind data
                            shader->bind();

                            Core::Matrix4 M = ro->getTransformAsMatrix();
                            Core::Matrix4 N = M.inverse().transpose();

                            shader->setUniform( "transform.proj", renderData.projMatrix );
                            shader->setUniform( "transform.view", renderData.viewMatrix );
                            shader->setUniform( "transform.model", M );
                            shader->setUniform( "transform.worldNormal", N );
                            params.bind( shader );

                            ro->getRenderTechnique()->material->bind( shader );

                            // render
                            ro->getMesh()->render();
                        }
                    }
                }
            }
            else
            {
                DirectionalLight l;
                l.setDirection( Core::Vector3( 0.3, -1.0, 0.0 ) );

                RenderParameters params;
                l.getRenderParameters( params );

                for ( const auto& ro : m_fancyRenderObjects )
                {
                    shader = ro->getRenderTechnique()->shader;

                    // bind data
                    shader->bind();
                    shader->setUniform( "proj", renderData.projMatrix );
                    shader->setUniform( "view", renderData.viewMatrix );
                    shader->setUniform( "model", ro->getLocalTransformAsMatrix() );
                    params.bind( shader );

                    ro->getRenderTechnique()->material->bind( shader );

                    // render
                    ro->getMesh()->render();
                }
            }

            if (m_wireframe)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDisable(GL_POLYGON_OFFSET_LINE);
            }

            // Draw debug stuff, do not overwrite depth map but do depth testing
            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            if ( m_drawDebug )
            {
                for ( const auto& ro : m_debugRenderObjects )
                {
                    if ( ro->isVisible() )
                    {
                        shader = ro->getRenderTechnique()->shader;

                        // bind data
                        shader->bind();

                        Core::Matrix4 M = ro->getTransformAsMatrix();
                        shader->setUniform( "transform.proj", renderData.projMatrix );
                        shader->setUniform( "transform.view", renderData.viewMatrix );
                        shader->setUniform( "transform.model", M );

                        ro->getRenderTechnique()->material->bind( shader );

                        // render
                        ro->getMesh()->render();
                    }
                }

                DebugRender::getInstance()->render(renderData.viewMatrix,
                                                   renderData.projMatrix);

            }

            // Draw X rayed objects always on top of normal objects
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );
            if ( m_drawDebug )
            {
                for ( const auto& ro : m_xrayRenderObjects )
                {
                    if ( ro->isVisible() )
                    {
                        shader = ro->getRenderTechnique()->shader;

                        // bind data
                        shader->bind();

                        Core::Matrix4 M = ro->getTransformAsMatrix();
                        shader->setUniform( "transform.proj", renderData.projMatrix );
                        shader->setUniform( "transform.view", renderData.viewMatrix );
                        shader->setUniform( "transform.model", M );

                        ro->getRenderTechnique()->material->bind( shader );

                        // render
                        ro->getMesh()->render();
                    }
                }
            }

            // Draw UI stuff, always drawn on top of everything else
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );
            for ( const auto& ro : m_uiRenderObjects )
            {
                if ( ro->isVisible() )
                {
                    shader = ro->getRenderTechnique()->shader;

                    // bind data
                    shader->bind();

                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 MV = renderData.viewMatrix * M;
                    Core::Vector3 V = MV.block<3, 1>( 0, 3 );
                    Scalar d = V.norm();

                    Core::Matrix4 S = Core::Matrix4::Identity();
                    S( 0, 0 ) = S( 1, 1 ) = S( 2, 2 ) = d;

                    M = M * S;

                    shader->setUniform( "transform.proj", renderData.projMatrix );
                    shader->setUniform( "transform.view", renderData.viewMatrix );
                    shader->setUniform( "transform.model", M );

                    ro->getRenderTechnique()->material->bind( shader );

                    // render
                    ro->getMesh()->render();
                }
            }

            // Restore state
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );

            m_fbo->unbind();
        }

        void ForwardRenderer::postProcessInternal( const RenderData& renderData )
        {
            CORE_UNUSED( renderData );

            m_postprocessFbo->useAsTarget( m_width, m_height );

            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );

            GL_ASSERT( glDepthFunc( GL_ALWAYS ) );

            const ShaderProgram* shader = nullptr;

            if (m_postProcessEnabled)
            {
                glDrawBuffers(1, buffers);
                glViewport(0, 0, m_width, m_height);
                shader = m_shaderMgr->getShaderProgram("DrawScreen");
                shader->bind();
                shader->setUniform("screenTexture", m_textures[TEX_LIT].get(), 0);
                m_quadMesh->render();

                // dummy pass
                m_dummy.renderPass(m_shaderMgr, m_quadMesh.get());

                // luminance pass : TODO(Hugo) remove fetch
                m_lumin.renderPass(m_shaderMgr, m_quadMesh.get());
                Core::Color lum = m_lumin.getOut(0)->getTexel(0, 0);
                Scalar lumMin  = lum.x();
                Scalar lumMax  = lum.y();
                Scalar lumMean = std::exp(lum.z() / (m_pingPongSize * m_pingPongSize));

                // tonemapping pass
                //m_tonmp.renderPass(m_shaderMgr, m_quadMesh.get(), m_pingPongSize);
                m_tonmp.renderPass(m_shaderMgr, m_quadMesh.get(), lumMin, lumMax, lumMean);

                // bloom pass : TODO(Hugo) do a bloom pass to group highpass and blur
                //m_highp.renderPass(m_shaderMgr, m_quadMesh.get(), m_pingPongSize);
                m_highp.renderPass(m_shaderMgr, m_quadMesh.get(), lumMin, lumMax, lumMean);
                m_blurp.renderPass(m_shaderMgr, m_quadMesh.get());

                // do final composition
                m_compp.renderPass(m_shaderMgr, m_quadMesh.get());

                GL_ASSERT( glDepthFunc( GL_LESS ) );
                m_postprocessFbo->unbind();
            }
            else
            {
                glDrawBuffers(1, buffers);
                glViewport(0, 0, m_width, m_height);
                shader = m_shaderMgr->getShaderProgram("DrawScreen");
                shader->bind();
                shader->setUniform("screenTexture", m_textures[TEX_LIT].get(), 0);
                m_quadMesh->render();
            }
        }

        void ForwardRenderer::resizeInternal()
        {
            m_pingPongSize = std::pow(2.0, Scalar(uint(std::log2(std::min(m_width, m_height)))));

            m_textures[TEX_DEPTH]->initGL(GL_DEPTH_COMPONENT24, m_width, m_height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
            m_textures[TEX_NORMAL]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_LIT]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);

            m_fbo->bind();
            m_fbo->setSize( m_width, m_height );
            m_fbo->attachTexture( GL_DEPTH_ATTACHMENT , m_textures[TEX_DEPTH].get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_textures[TEX_NORMAL].get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT1, m_textures[TEX_LIT].get() );
            m_fbo->check();
            m_fbo->unbind( true );

            m_postprocessFbo->bind();
            m_postprocessFbo->setSize( m_width, m_height );
            m_postprocessFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_fancyTexture.get());
            m_postprocessFbo->check();
            m_postprocessFbo->unbind( true );

            m_dummy.resizePass(m_width, m_height);
            m_lumin.resizePass(m_width, m_height);
            m_highp.resizePass(m_width, m_height);
            m_blurp.resizePass(m_width, m_height);
            m_tonmp.resizePass(m_width, m_height);
            m_compp.resizePass(m_width, m_height);


            // Reset framebuffer state
            GL_CHECK_ERROR;
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }
    }
} // namespace Ra
