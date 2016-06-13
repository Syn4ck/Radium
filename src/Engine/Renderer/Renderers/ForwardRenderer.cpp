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
            // set useful callbacks
//            m_passgraph.m_val_access   = Pass::getVal;
            m_passgraph.m_connect      = Pass::connect;
            m_passgraph.m_slotname_in  = Pass::getParamNameIn;
            m_passgraph.m_slotname_out = Pass::getParamNameOut;

            // add nodes
            m_passgraph.addNode("LIT",    std::shared_ptr<Pass>(m_passes[0]), 0, 1);
            m_passgraph.addNode("VEC",    std::shared_ptr<Pass>(m_passes[1]), 0, 1);
            m_passgraph.addNode("DUMMY",  std::shared_ptr<Pass>(m_passes[2]), 2, 1);
            m_passgraph.addNode("BLUR",   std::shared_ptr<Pass>(m_passes[3]), 2, 1);
            m_passgraph.addNode("OFF",    std::shared_ptr<Pass>(m_passes[4]), 0, 1);
//            m_passgraph.addNode("LUM",    std::shared_ptr<Pass>(m_passes[1]), 1, 1);
//            m_passgraph.addNode("MMM",    std::shared_ptr<Pass>(m_passes[2]), 1, 1);
//            m_passgraph.addNode("TON",    std::shared_ptr<Pass>(m_passes[3]), 3, 1);
//            m_passgraph.addNode("PPS",    std::shared_ptr<Pass>(m_passes[4]), 0, 1);

            // connect them
            m_passgraph["BLUR" ]->setParent(0, m_passgraph["LIT" ], 0);
            m_passgraph["BLUR" ]->setParent(0, m_passgraph["OFF" ], 1);
            m_passgraph["DUMMY"]->setParent(0, m_passgraph["BLUR"], 0);
            m_passgraph["DUMMY"]->setParent(0, m_passgraph["VEC" ], 1);
//            m_passgraph["LUM"  ]->setParent(0, m_passgraph["DUMMY"],0);
//            m_passgraph["LUM"]->setParent(0, m_passgraph["LIT"],  0);
//            m_passgraph["MMM"]->setParent(0, m_passgraph["LUM"],  0);
//            m_passgraph["TON"]->setParent(0, m_passgraph["LIT"],  0);
//            m_passgraph["TON"]->setParent(0, m_passgraph["MMM"],  1);
//            m_passgraph["TON"]->setParent(0, m_passgraph["PPS"],  2);

            // levelize and sort on the same run
            m_passgraph.levelize(false);
//            m_passgraph.print();
        }

        void ForwardRenderer::initPasses()
        {
            // create passes
            m_passes.push_back( std::unique_ptr<Pass>( new PassRegular         ("source",    m_width, m_height, 1, 1,   "DrawScreen")) );
//            m_passes.push_back( std::unique_ptr<Pass>( new PassT<Core::Vector3>("vec3single",                      1,    Core::Vector3(0.6f,0.6f,0.f), PARAM_VEC3)) );
            m_passes.push_back( std::unique_ptr<Pass>( new PassT<Core::Vector3>("vec3single",                         Core::Vector3(0.f,0.4f,0.3f), PARAM_VEC3)) );
            m_passes.push_back( std::unique_ptr<Pass>( new PassRegular         ("dummy",     m_width, m_height, 2, 1,   "Dummy")) );
            m_passes.push_back( std::unique_ptr<Pass>( new PassPingPong        ("blur",      m_width, m_height, 2, 1, 8,"Blur" )) );
            m_passes.push_back( std::unique_ptr<Pass>( new PassT<Core::Vector2>("offset",                             Core::Vector2(0.0, 8.0 / m_height), PARAM_VEC2)) );
//            m_passes.push_back( std::unique_ptr<Pass>( new PassRegular         ("lumina",    m_width, m_height, 1, 1,   "Luminance")) );
//            m_passes.push_back( std::unique_ptr<Pass>( new PassRedux           ("minmax",    m_width, m_height, 1, 1,   "MinMax")) );
//            m_passes.push_back( std::unique_ptr<Pass>( new PassRegular         ("tonmap",    m_width, m_height, 3, 1,   "Tonemapping")) );
//            m_passes.push_back( std::unique_ptr<Pass>( new PassT<uint>         ("ppsize",                          1, m_pingPongSize, PARAM_UINT)) );

            // init every passes
            for (auto const& pass: m_passes)
            {
                m_passmap[pass->getName()] = pass.get();

                // init
                pass->setCanvas(m_quadMesh.get());
                pass->init();
            }

            // initiate the HDR source FIXME(hugo) find a better way without duplication
            m_passmap["source"]->setIn("screenTexture", m_textures[TEX_LIT].get());
        }

        void ForwardRenderer::initShaders()
        {
            m_shaderMgr->addShaderProgram("DepthAmbientPass", "../Shaders/DepthAmbientPass.vert.glsl", "../Shaders/DepthAmbientPass.frag.glsl");
            m_shaderMgr->addShaderProgram("Luminance", "../Shaders/Basic2D.vert.glsl", "../Shaders/Luminance.frag.glsl");
            m_shaderMgr->addShaderProgram("Tonemapping", "../Shaders/Basic2D.vert.glsl", "../Shaders/Tonemapping.frag.glsl");
            m_shaderMgr->addShaderProgram("MinMax", "../Shaders/Basic2D.vert.glsl", "../Shaders/MinMax.frag.glsl");
            m_shaderMgr->addShaderProgram("Highpass", "../Shaders/Basic2D.vert.glsl", "../Shaders/Highpass.frag.glsl");
            m_shaderMgr->addShaderProgram("Blur", "../Shaders/Basic2D.vert.glsl", "../Shaders/Blur.frag.glsl");
            m_shaderMgr->addShaderProgram("FinalCompose", "../Shaders/Basic2D.vert.glsl", "../Shaders/FinalCompose.frag.glsl");
        }

        void ForwardRenderer::initBuffers()
        {
            m_fbo.reset( new FBO( FBO::Components( FBO::COLOR | FBO::DEPTH ), m_width, m_height ) );

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
                RenderParameters params;
                ro->render(params, renderData.viewMatrix, renderData.projMatrix, shader);
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
                        ro->render(params, renderData.viewMatrix, renderData.projMatrix);
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
                    ro->render(params, renderData.viewMatrix, renderData.projMatrix);
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
            RenderParameters params;

            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );

            m_fbo->useAsTarget( m_width, m_height );
            glDrawBuffers(1, buffers + 2);

            glViewport(0, 0, m_width, m_height);

            if ( m_drawDebug )
            {
                for ( const auto& ro : m_debugRenderObjects )
                {
                    ro->render(params, renderData.viewMatrix, renderData.projMatrix);
                }

                DebugRender::getInstance()->render(renderData.viewMatrix, renderData.projMatrix);

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
                        ro->render(params, renderData.viewMatrix, renderData.projMatrix);
                    }
                }
            }

             m_fbo->unbind();
        }

        // Draw UI stuff, always drawn on top of everything else + clear ZMask
        void ForwardRenderer::uiInternal( const RenderData& renderData )
        {
            const float clearDepth( 1.0 );

            m_fbo->useAsTarget( m_width, m_height );

            glViewport(0, 0, m_width, m_height);
            glDrawBuffers(1, buffers + 2);

            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT(glDisable(GL_DEPTH_TEST));
            // FIXME(charly): Clear depth ?
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );

            for ( const auto& ro : m_uiRenderObjects )
            {
                if ( ro->isVisible() )
                {
                    // scaling
                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 MV = renderData.viewMatrix * M;
                    Core::Vector3 V = MV.block<3, 1>( 0, 3 );
                    Scalar d = V.norm();

                    Core::Matrix4 S = Core::Matrix4::Identity();
                    S( 0, 0 ) = S( 1, 1 ) = S( 2, 2 ) = d;

                    M = M * S;

                    RenderParameters params;
                    params.addParameter("transform.model", M);

                    // render
                    ro->render(params, renderData.viewMatrix, renderData.projMatrix);
                }
            }

            m_fbo->unbind();
        }

        void ForwardRenderer::postProcessInternal( const RenderData& renderData )
        {
            // a first check is to be performed to know wether or not the graph
            // has changed and thus is still valid, and abort on error
            if (m_passgraph.m_status == Core::GRAPH_UPDATE)
            {
                // and check correctness
                if (checkPassGraph())
                {
                    m_passgraph.levelize( true );
                    m_postProcessEnabled = true;
                }
                else
                {
                    m_postProcessEnabled = false;
                }
            }

            Texture* last = m_textures[TEX_LIT].get();
            CORE_UNUSED( renderData );

            GL_ASSERT( glDisable(GL_DEPTH_TEST) );

            m_fbo->useAsTarget( m_width, m_height );

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT( glDepthFunc( GL_ALWAYS ) );

            const ShaderProgram* shader = nullptr;

            if (m_postProcessEnabled)
            {
                // update graph parameters
                m_passgraph.updateNodes();

                // render everything from the graph
                for (auto const& nodePass: m_passgraph)
                {
                    nodePass->m_data->renderPass();
                    last = nodePass->m_data->getTex("");
                }
            }

            m_fbo->useAsTarget( m_width, m_height );
            GL_ASSERT( glDrawBuffers(1, buffers + 2) );
            GL_ASSERT( glViewport(0, 0, m_width, m_height) );
            shader = m_shaderMgr->getShaderProgram("DrawScreen");
            shader->bind();
            shader->setUniform("screenTexture", last);
            m_quadMesh->render();

            GL_ASSERT( glDepthFunc( GL_LESS ) );
            m_fbo->unbind();
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
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT2, m_fancyTexture.get());
            m_fbo->check();
            m_fbo->unbind( true );

            for (auto const& pass: m_passes)
            {
                pass->resizePass(m_width, m_height);
            }

            // Reset framebuffer state
            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        Core::MultiGraph<Pass>* ForwardRenderer::getPassGraphRw()
        {
            return &m_passgraph;
        }

        bool ForwardRenderer::checkPassGraph()
        {
            bool valid = true;

            // 1. check every input of every node
            for (auto const& node: m_passgraph)
            {
                valid = valid && (node->m_parents.size() == node->m_nbIn);
            }

            // 2. check it is not a forest TODO(hugo)

            // 3. set graph status accordingly
            m_passgraph.m_status = (valid) ? Core::GRAPH_VALID : Core::GRAPH_ERROR;

            return valid;
        }

    }
} // namespace Ra
