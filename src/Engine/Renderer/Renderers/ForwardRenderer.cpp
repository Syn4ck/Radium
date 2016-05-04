#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>

#include <iostream>

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
            initPasses();
            initGraph();

            DebugRender::createInstance();
            DebugRender::getInstance()->initialize();
        }

        void ForwardRenderer::initGraph()
        {
            // add nodes - randomly inserted to test levels efficiency
            m_passgraph.addNode("MINMAX", std::shared_ptr<Pass>(m_passes[2]), 1, 1);
            m_passgraph.addNode("BLUR",   std::shared_ptr<Pass>(m_passes[4]), 1, 1);
            m_passgraph.addNode("HGHPSS", std::shared_ptr<Pass>(m_passes[3]), 2, 1);
            m_passgraph.addNode("TONMAP", std::shared_ptr<Pass>(m_passes[5]), 2, 1);
            m_passgraph.addNode("LIT",    std::shared_ptr<Pass>(m_passes[0]), 0, 1);
            m_passgraph.addNode("ADD",    std::shared_ptr<Pass>(m_passes[6]), 2, 1);
            m_passgraph.addNode("LUM",    std::shared_ptr<Pass>(m_passes[1]), 1, 1);

            // connect them
            m_passgraph[   "LUM"]->setParent(0,m_passgraph["LIT"   ],0);
            m_passgraph["TONMAP"]->setParent(0,m_passgraph["LIT"   ],0);
            m_passgraph["TONMAP"]->setParent(0,m_passgraph["MINMAX"],1);
            m_passgraph["HGHPSS"]->setParent(0,m_passgraph["LIT"   ],0);
            m_passgraph["HGHPSS"]->setParent(0,m_passgraph["MINMAX"],1);
            m_passgraph["MINMAX"]->setParent(0,m_passgraph["LUM"   ],0);
            m_passgraph[  "BLUR"]->setParent(0,m_passgraph["HGHPSS"],0);
            m_passgraph[   "ADD"]->setParent(0,m_passgraph["BLUR"  ],0);
            m_passgraph[   "ADD"]->setParent(0,m_passgraph["TONMAP"],1);

            // levelize and sort on the same run
            m_passgraph.levelize(true);
            //m_passgraph.print();
        }

        void ForwardRenderer::initPasses()
        {
            // create passes
            m_passes.push_back(std::unique_ptr<Pass>(new PassRegular ("source",    m_width, m_height, 1, 1,    "DrawScreen")));
            m_passes.push_back(std::unique_ptr<Pass>(new PassRegular ("luminance", m_width, m_height, 1, 1,    "Luminance")));
            m_passes.push_back(std::unique_ptr<Pass>(new PassRedux   ("minmax",    m_width, m_height, 1, 1, 2, "MinMax")));
            m_passes.push_back(std::unique_ptr<Pass>(new PassRegular ("highpass",  m_width, m_height, 2, 1,    "Highpass")));
            m_passes.push_back(std::unique_ptr<Pass>(new PassPingPong("blur",      m_width, m_height, 1, 1, 2, "Blur")));
            m_passes.push_back(std::unique_ptr<Pass>(new PassRegular ("tonemap",   m_width, m_height, 2, 1,    "Tonemapping")));
            m_passes.push_back(std::unique_ptr<Pass>(new PassRegular ("composite", m_width, m_height, 2, 1,    "FinalCompose")));

            // set hashmap
            for (auto const& pass: m_passes)
            {
                m_passmap[pass->getName()] = pass.get();

                // init
                pass->setCanvas(m_quadMesh.get());
                pass->init();
            }

            // initiate the HDR source FIXME(hugo) find a better way without duplication
            m_passmap["source"]->setIn(m_textures[TEX_LIT].get());

            // set texture names
            m_passmap["source"   ]->m_texNames[0] = "screenTexture";
            m_passmap["luminance"]->m_texNames[0] = "hdr";
            m_passmap["minmax"   ]->m_texNames[0] = "color";
            m_passmap["highpass" ]->m_texNames[0] = "hdr";
            m_passmap["highpass" ]->m_texNames[1] = "lum";
            m_passmap["tonemap"  ]->m_texNames[0] = "hdr";
            m_passmap["tonemap"  ]->m_texNames[1] = "lum";
            m_passmap["blur"     ]->m_texNames[0] = "hdr";
            m_passmap["composite"]->m_texNames[0] = "texA";
            m_passmap["composite"]->m_texNames[1] = "texB";

            // reduce the size of highpass in order to obtain a box blur effect
            m_passmap["highpass"]->setSizeModifier(0.125, 0.125);

            // and show them into Qt
            for (auto const it_pass: m_passmap)
            {
                m_secondaryTextures["[post-fx] " + it_pass.first] = it_pass.second->getOut(0);
            }
        }

        void ForwardRenderer::initShaders()
        {
            m_shaderMgr->addShaderProgram("DepthAmbientPass", "../Shaders/DepthAmbientPass.vert.glsl", "../Shaders/DepthAmbientPass.frag.glsl");
            m_shaderMgr->addShaderProgram("FinalCompose", "../Shaders/Basic2D.vert.glsl", "../Shaders/FinalCompose.frag.glsl");
        }

        void ForwardRenderer::initBuffers()
        {
            m_fbo.reset( new FBO( FBO::Components( FBO::COLOR | FBO::DEPTH ), m_width, m_height ) );
            m_postprocessFbo.reset( new FBO( FBO::Components( FBO::COLOR | FBO::DEPTH), m_width, m_height ) );

            // Render pass
            m_textures[TEX_DEPTH].reset( new Texture( "Depth", GL_TEXTURE_2D ) );
            m_textures[TEX_NORMAL].reset( new Texture( "Normal", GL_TEXTURE_2D ) );
            m_textures[TEX_LIT].reset( new Texture( "HDR", GL_TEXTURE_2D ) );

            m_secondaryTextures["Depth Texture"]  = m_textures[TEX_DEPTH].get();
            m_secondaryTextures["Normal Texture"] = m_textures[TEX_NORMAL].get();
            m_secondaryTextures["HDR Texture"]    = m_textures[TEX_LIT].get();
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

            const Core::Colorf clearColor = Core::Colors::FromChars<Core::Colorf>(48, 48, 48, 0);
            const Core::Colorf clearZeros = Core::Colors::Black<Core::Colorf>();
            const float clearDepth( 1.0 );

            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearZeros.data() ) );   // Clear normals
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );         // Clear depth

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
                    RenderParameters params;
                    l->getRenderParameters( params );

                    for ( const auto& ro : m_fancyRenderObjects )
                    {
                        ro->render(params, renderData.viewMatrix, renderData.projMatrix, true);
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
                    ro->render(params, renderData.viewMatrix, renderData.projMatrix, false);
                }
            }

            if (m_wireframe)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDisable(GL_POLYGON_OFFSET_LINE);
            }

            // Restore state
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );

            m_fbo->unbind();
        }

        // Draw debug stuff, do not overwrite depth map but do depth testing
        void ForwardRenderer::debugInternal( const RenderData& renderData )
        {
            const ShaderProgram* shader;

            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );

            m_postprocessFbo->useAsTarget( m_width, m_height );
            glDrawBuffers(1, buffers);

            glViewport(0, 0, m_width, m_height);

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

             m_postprocessFbo->unbind();
        }

        // Draw UI stuff, always drawn on top of everything else + clear ZMask
        void ForwardRenderer::uiInternal( const RenderData& renderData )
        {
            const ShaderProgram* shader;
            const float          clearDepth( 1.0 );

            m_postprocessFbo->useAsTarget( m_width, m_height );

            glViewport(0, 0, m_width, m_height);
            glDrawBuffers(1, buffers);

            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT(glDisable(GL_DEPTH_TEST));
            // FIXME(charly): Clear depth ?
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

            m_postprocessFbo->unbind();
        }

        void ForwardRenderer::postProcessInternal( const RenderData& renderData )
        {
            Texture* last = m_textures[TEX_LIT].get();
            CORE_UNUSED( renderData );

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );

            m_postprocessFbo->useAsTarget( m_width, m_height );

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT( glDepthFunc( GL_ALWAYS ) );

            const ShaderProgram* shader = nullptr;

            if (m_postProcessEnabled)
            {
                // special settings for shaders that do not depend on the graph
                m_passmap["blur"]->setIn("offset", Core::Vector2(0.0, 8.0 / m_height), 0, 1);
                m_passmap["blur"]->setIn("offset", Core::Vector2(8.0 / m_width,  0.0), 0, 0);
                m_passmap["highpass"]->setIn("pingpongsz", m_pingPongSize);
                m_passmap["tonemap"]->setIn("pingpongsz", m_pingPongSize);

                // render everything from the graph
                for (auto const& nodePass: m_passgraph)
                {
                    nodePass->m_data->renderPass();
                }

                last = m_passmap["composite"]->getOut(0);
            }

            m_postprocessFbo->useAsTarget( m_width, m_height );
            GL_ASSERT( glDrawBuffers(1, buffers) );
            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            shader = m_shaderMgr->getShaderProgram("DrawScreen");
            shader->bind();
            shader->setUniform("screenTexture", last);
            m_quadMesh->render();

            GL_ASSERT( glDepthFunc( GL_LESS ) );
            m_postprocessFbo->unbind();
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
            m_postprocessFbo->attachTexture(GL_DEPTH_ATTACHMENT , m_textures[TEX_DEPTH].get());
            m_postprocessFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_fancyTexture.get());
            m_postprocessFbo->check();
            m_postprocessFbo->unbind( true );

            for (auto const& pass: m_passes)
            {
                pass->resizePass(m_width, m_height);
            }

            // Reset framebuffer state
            GL_CHECK_ERROR;
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }
    }
} // namespace Ra
