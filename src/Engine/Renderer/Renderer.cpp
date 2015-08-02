#include <Engine/Renderer/Renderer.hpp>

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/Log/Log.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Shader/ShaderProgramManager.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

namespace Ra
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

Engine::Renderer::Renderer(uint width, uint height)
    : m_engine(nullptr)
    , m_width(width)
    , m_height(height)
    , m_shaderManager(nullptr)
    , m_displayedTexture(nullptr)
    , m_displayedIsDepth(false)
    , m_depthAmbientShader(nullptr)
    , m_postprocessShader(nullptr)
    , m_drawScreenShader(nullptr)
    , m_quadMesh(nullptr)
    , m_fbo(nullptr)
    , m_postprocessFbo(nullptr)
{
}

Engine::Renderer::~Renderer()
{
    ShaderProgramManager::destroyInstance();
}

void Engine::Renderer::initialize()
{
    std::string shaderPath("../Shaders");
    std::string defaultShader("Default");

    m_shaderManager = ShaderProgramManager::createInstance(shaderPath, defaultShader);
    m_textureManager = TextureManager::createInstance();

    initShaders();
    initBuffers();

    Core::TriangleMesh mesh;
    mesh.m_vertices.push_back({ Scalar(-1), Scalar(-1), Scalar(0) });
    mesh.m_vertices.push_back({ Scalar(-1), Scalar( 1), Scalar(0) });
    mesh.m_vertices.push_back({ Scalar( 1), Scalar( 1), Scalar(0) });
    mesh.m_vertices.push_back({ Scalar( 1), Scalar(-1), Scalar(0) });
    mesh.m_normals.push_back ({ Scalar( 0), Scalar( 0), Scalar(0) });
    mesh.m_normals.push_back ({ Scalar( 0), Scalar( 0), Scalar(0) });
    mesh.m_normals.push_back ({ Scalar( 0), Scalar( 0), Scalar(0) });
    mesh.m_normals.push_back ({ Scalar( 0), Scalar( 0), Scalar(0) });

    mesh.m_triangles.push_back({ Core::TriangleIdx(0), Core::TriangleIdx(2), Core::TriangleIdx(1) });
    mesh.m_triangles.push_back({ Core::TriangleIdx(0), Core::TriangleIdx(3), Core::TriangleIdx(2) });

    m_quadMesh.reset(new Mesh("quad"));
    m_quadMesh->loadGeometry(mesh);
    m_quadMesh->updateGL();
}

void Engine::Renderer::initShaders()
{
    m_depthAmbientShader = m_shaderManager->addShaderProgram("DepthAmbientPass");
    m_renderpassCompositingShader = m_shaderManager->addShaderProgram("RenderPassCompose");
    m_oiTransparencyShader = m_shaderManager->addShaderProgram("OITransparency");
    m_postprocessShader = m_shaderManager->addShaderProgram("PostProcess");
    m_drawScreenShader  = m_shaderManager->addShaderProgram("DrawScreen");
}

void Engine::Renderer::initBuffers()
{
    m_fbo.reset(new FBO(FBO::Components(FBO::COLOR | FBO::DEPTH), m_width, m_height));
    m_oitFbo.reset(new FBO(FBO::Components(FBO::COLOR | FBO::DEPTH), m_width, m_height));
    m_postprocessFbo.reset(new FBO(FBO::Components(FBO::COLOR), m_width, m_height));

    // Render pass
    m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]      .reset(new Texture("Depth", GL_TEXTURE_2D));
    m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]    .reset(new Texture("Ambient", GL_TEXTURE_2D));
    m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]   .reset(new Texture("Position", GL_TEXTURE_2D));
    m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]     .reset(new Texture("Normal", GL_TEXTURE_2D));
    m_renderpassTextures[RENDERPASS_TEXTURE_PICKING]    .reset(new Texture("Picking", GL_TEXTURE_2D));
    m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]    .reset(new Texture("Color", GL_TEXTURE_2D));
    m_renderpassTexture                                 .reset(new Texture("RenderPass", GL_TEXTURE_2D));

    // OIT pass
    m_oitTextures[OITPASS_TEXTURE_ACCUM]                .reset(new Texture("OITAccum", GL_TEXTURE_2D));
    m_oitTextures[OITPASS_TEXTURE_REVEALAGE]            .reset(new Texture("OITPRevealage", GL_TEXTURE_2D));

    // Post process pass
    m_finalTexture                                      .reset(new Texture("Final", GL_TEXTURE_2D));

    resize(m_width, m_height);

    m_displayedTexture = m_finalTexture.get();
}

void Engine::Renderer::render(const RenderData& data)
{
    std::lock_guard<std::mutex> renderLock(m_renderMutex);

    m_timerData.renderStart = Core::Timer::Clock::now();

    // 0. Gather render objects.
    std::vector<std::shared_ptr<RenderObject>> renderObjects;

    CORE_ASSERT(m_engine != nullptr, "no engine in renderer");
    renderObjects = m_engine->getRenderObjectManager()->getRenderObjects();
    saveExternalFBOInternal();
    updateRenderObjectsInternal(data, renderObjects);
    m_timerData.updateEnd = Core::Timer::Clock::now();

    // 1. Do the rendering.
    renderInternal(data, renderObjects);
    m_timerData.mainRenderEnd = Core::Timer::Clock::now();

    // 2. Post processing
    postProcessInternal(data, renderObjects);
    m_timerData.postProcessEnd = Core::Timer::Clock::now();

    //3. write image to framebuffer.
    drawScreenInternal();
    m_timerData.renderEnd = Core::Timer::Clock::now();
}

void Engine::Renderer::saveExternalFBOInternal()
{
    GL_ASSERT(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_qtPlz));
}

void Engine::Renderer::updateRenderObjectsInternal(const RenderData &renderData,
                                               const std::vector<std::shared_ptr<RenderObject> > &renderObjects)
{
    CORE_UNUSED(renderData);

    for (auto& ro : renderObjects)
    {
        ro->updateGL();
    }
}

void Engine::Renderer::renderInternal(const RenderData& renderData,
                                      const std::vector<std::shared_ptr<RenderObject>>& renderObjects)
{
    uint size = renderObjects.size();
    std::vector<uint> opaque; opaque.reserve(size);
    std::vector<uint> transparent; transparent.reserve(size);

    // Get translucent objects
    for (uint i = 0; i < size; ++i)
    {
        Material::MaterialType type = renderObjects[i]->getMaterial()->getMaterialType();
        if (type == Material::MAT_TRANSPARENT)
        {
            transparent.push_back(i);
        }
        else
        {
            opaque.push_back(i);
        }
    }
    uint numOpaque = opaque.size();
    uint numTransparent = transparent.size();

    m_fbo->useAsTarget();

    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glColorMask(1, 1, 1, 1));

    GL_ASSERT(glDrawBuffers(6, buffers));

    const Core::Color clearColor(0.1, 0.1, 0.1, 1.0);
    const Core::Color clearZeros(0.0, 0.0, 0.0, 0.0);
    const Core::Color clearOnes(1.0, 1.0, 1.0, 1.0);
    const Scalar clearDepth = 1.0;

    GL_ASSERT(glClearBufferfv(GL_COLOR, 0, clearColor.data())); // Clear ambient
    GL_ASSERT(glClearBufferfv(GL_COLOR, 1, clearZeros.data()));  // Clear position
    GL_ASSERT(glClearBufferfv(GL_COLOR, 2, clearZeros.data()));  // Clear normal
    GL_ASSERT(glClearBufferfv(GL_COLOR, 3, clearOnes.data()));  // Clear picking
    GL_ASSERT(glClearBufferfv(GL_COLOR, 4, clearZeros.data()));  // Clear color
    GL_ASSERT(glClearBufferfv(GL_COLOR, 5, clearZeros.data()));  // Clear renderpass
    GL_ASSERT(glClearBufferfv(GL_DEPTH, 0, &clearDepth));       // Clear depth

    Core::Matrix4 view = renderData.viewMatrix;
    Core::Matrix4 proj = renderData.projMatrix;

    // Z + Ambient Prepass
    GL_ASSERT(glEnable(GL_DEPTH_TEST));
    GL_ASSERT(glDepthFunc(GL_LESS));
    GL_ASSERT(glDepthMask(GL_TRUE));

    GL_ASSERT(glDisable(GL_BLEND));

    GL_ASSERT(glDrawBuffers(4, buffers)); // Draw ambient, position, normal, picking

    m_depthAmbientShader->bind();

    for (uint i = 0; i < numOpaque; ++i)
    {
        auto ro = renderObjects[opaque[i]];
        // Object ID
        int index = ro->idx.getValue();
        Scalar r = Scalar((index & 0x000000FF) >> 0) / 255.0;
        Scalar g = Scalar((index & 0x0000FF00) >> 8) / 255.0;
        Scalar b = Scalar((index & 0x00FF0000) >> 16) / 255.0;

        m_depthAmbientShader->setUniform("objectId", Core::Vector3(r, g, b));
        ro->draw(view, proj, m_depthAmbientShader);
    }

    // Light pass
    GL_ASSERT(glDepthFunc(GL_LEQUAL));
    GL_ASSERT(glDepthMask(GL_FALSE));

    GL_ASSERT(glEnable(GL_BLEND));
    GL_ASSERT(glBlendFunc(GL_ONE, GL_ONE));

    GL_ASSERT(glDrawBuffers(1, buffers + 4)); // Draw color texture

    if (m_lights.size() > 0)
    {
        for (const auto& l : m_lights)
        {
            for (uint i = 0; i < numOpaque; ++i)
            {
                renderObjects[opaque[i]]->draw(view, proj, l);
            }
        }
    }
    else
    {
        DirectionalLight l;
        l.setDirection(Core::Vector3(0.3f, 1, 0));

        for (uint i = 0; i < numOpaque; ++i)
        {
            renderObjects[opaque[i]]->draw(view, proj, &l);
        }
    }

    m_fbo->unbind();

    m_oitFbo->useAsTarget();

    GL_ASSERT(glDrawBuffers(2, buffers));

    // RT0 stores a sum, RT1 stores a product.
    GL_ASSERT(glClearBufferfv(GL_COLOR, 0, clearZeros.data())); // Clear
    GL_ASSERT(glClearBufferfv(GL_COLOR, 1, clearOnes.data()));  // Clear

    GL_ASSERT(glDepthFunc(GL_LESS))
    GL_ASSERT(glEnable(GL_BLEND));
    GL_ASSERT(glBlendEquation(GL_FUNC_ADD));
    GL_ASSERT(glBlendFunci(0, GL_ONE, GL_ONE));
    GL_ASSERT(glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA));

    m_oiTransparencyShader->bind();
    // FIXME(Charly): weight parameter tweakability ?
    m_oiTransparencyShader->setUniform("depthScale", Scalar(0.5));

    for (uint i = 0; i < numTransparent; ++i)
    {
        renderObjects[transparent[i]]->draw(view, proj, m_oiTransparencyShader);
    }

    GL_ASSERT(glDisable(GL_BLEND));

    m_oitFbo->unbind();

    // Draw renderpass texture
    m_fbo->bind();
    GL_ASSERT(glDrawBuffers(1, buffers + 5));
    GL_ASSERT(glDepthFunc(GL_ALWAYS));

    m_renderpassCompositingShader->bind();

    m_renderpassCompositingShader->setUniform("ambient", m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT].get(), 0);
    m_renderpassCompositingShader->setUniform("color", m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED].get(), 1);
    m_renderpassCompositingShader->setUniform("renderpass", 0);
    m_quadMesh->draw();

    GL_ASSERT(glEnable(GL_BLEND));
    GL_ASSERT(glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA));

    m_renderpassCompositingShader->setUniform("oitSumColor", m_oitTextures[OITPASS_TEXTURE_ACCUM].get(), 2);
    m_renderpassCompositingShader->setUniform("oitSumWeight", m_oitTextures[OITPASS_TEXTURE_REVEALAGE].get(), 3);
    m_renderpassCompositingShader->setUniform("renderpass", 1);
    m_quadMesh->draw();

    GL_ASSERT(glDepthFunc(GL_LESS));

    GL_ASSERT(glDisable(GL_BLEND));
    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glDepthFunc(GL_LESS));

    m_fbo->unbind();
}

void Engine::Renderer::postProcessInternal(const RenderData &renderData,
                                           const std::vector<std::shared_ptr<RenderObject>>& renderObjects)
{
    CORE_UNUSED(renderData);
    CORE_UNUSED(renderObjects);

    // This pass does nothing by default

    m_postprocessFbo->useAsTarget(m_width, m_height);

    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glColorMask(1, 1, 1, 1));

    GL_ASSERT(glClearColor(1.0, 0.0, 0.0, 0.0));
    GL_ASSERT(glClearDepth(0.0));
    m_postprocessFbo->clear(FBO::Components(FBO::COLOR | FBO::DEPTH));

    // FIXME(Charly): Do we really need to clear the depth buffer ?
    GL_ASSERT(glDrawBuffers(1, buffers));

    GL_ASSERT(glDepthFunc(GL_ALWAYS));
	
    m_postprocessShader->bind();
    m_postprocessShader->setUniform("renderpassColor", m_renderpassTexture.get(), 0);

    m_quadMesh->draw();

    GL_ASSERT(glDepthFunc(GL_LESS));

    m_postprocessFbo->unbind();
}

void Engine::Renderer::drawScreenInternal()
{
    if (m_qtPlz == 0)
    {
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        glDrawBuffer(GL_BACK);
    }
    else
    {
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, m_qtPlz));
        GL_ASSERT(glDrawBuffers(1, buffers));
    }

    GL_ASSERT(glClearColor(0.0, 0.0, 0.0, 0.0));
    // FIXME(Charly): Do we really need to clear the depth buffer ?
    GL_ASSERT(glClearDepth(1.0));
    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    GL_ASSERT(glDepthFunc(GL_ALWAYS));

    GL_ASSERT(glViewport(0, 0, m_width, m_height));

    m_drawScreenShader->bind();
    m_drawScreenShader->setUniform("screenTexture", m_finalTexture.get(), 0);

    m_drawScreenShader->setUniform("depth", m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH].get(), 1);
    m_drawScreenShader->setUniform("ambient", m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT].get(), 2);
    m_drawScreenShader->setUniform("position", m_renderpassTextures[RENDERPASS_TEXTURE_POSITION].get(), 3);
    m_drawScreenShader->setUniform("normal", m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL].get(), 4);
    m_drawScreenShader->setUniform("color", m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED].get(), 5);
    m_drawScreenShader->setUniform("picking", m_renderpassTextures[RENDERPASS_TEXTURE_PICKING].get(), 6);
    m_drawScreenShader->setUniform("renderpass", m_renderpassTexture.get(), 7);

    m_drawScreenShader->setUniform("oitAccum", m_oitTextures[OITPASS_TEXTURE_ACCUM].get(), 8);
    m_drawScreenShader->setUniform("oitRevealage", m_oitTextures[OITPASS_TEXTURE_REVEALAGE].get(), 9);

    m_quadMesh->draw();

	GL_ASSERT(glDepthFunc(GL_LESS));
}

void Engine::Renderer::resize(uint w, uint h)
{
    m_width = w;
    m_height = h;
    glViewport(0, 0, m_width, m_height);

    if (m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->getId() != 0)
    {
        m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->deleteGL();
    }
    if (m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->getId() != 0)
    {
        m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->deleteGL();
    }
    if (m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->getId() != 0)
    {
        m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->deleteGL();
    }
    if (m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->getId() != 0)
    {
        m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->deleteGL();
    }
    if (m_renderpassTextures[RENDERPASS_TEXTURE_PICKING]->getId() != 0)
    {
        m_renderpassTextures[RENDERPASS_TEXTURE_PICKING]->deleteGL();
    }
    if (m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->getId() != 0)
    {
        m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->deleteGL();
    }
    if (m_renderpassTexture->getId() != 0)
    {
        m_renderpassTexture->deleteGL();
    }

    if (m_oitTextures[OITPASS_TEXTURE_ACCUM]->getId() != 0)
    {
        m_oitTextures[OITPASS_TEXTURE_ACCUM]->deleteGL();
    }
    if (m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->getId() != 0)
    {
        m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->deleteGL();
    }

    if (m_finalTexture->getId() != 0)
    {
        m_finalTexture->deleteGL();
    }

    m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->initGL(GL_DEPTH_COMPONENT24, w, h, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->setFilter(GL_LINEAR, GL_LINEAR);
    m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->setFilter(GL_LINEAR, GL_LINEAR);
    m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->setFilter(GL_LINEAR, GL_LINEAR);
    m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->setFilter(GL_LINEAR, GL_LINEAR);
    m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_renderpassTextures[RENDERPASS_TEXTURE_PICKING]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_renderpassTextures[RENDERPASS_TEXTURE_PICKING]->setFilter(GL_LINEAR, GL_LINEAR);
    m_renderpassTextures[RENDERPASS_TEXTURE_PICKING]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->setFilter(GL_LINEAR, GL_LINEAR);
    m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_renderpassTexture->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_renderpassTexture->setFilter(GL_LINEAR, GL_LINEAR);
    m_renderpassTexture->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_oitTextures[OITPASS_TEXTURE_ACCUM]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_oitTextures[OITPASS_TEXTURE_ACCUM]->setFilter(GL_LINEAR, GL_LINEAR);
    m_oitTextures[OITPASS_TEXTURE_ACCUM]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->initGL(GL_R32F, w, h, GL_RED, GL_FLOAT, nullptr);
    m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->setFilter(GL_LINEAR, GL_LINEAR);
    m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_finalTexture->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_finalTexture->setFilter(GL_LINEAR, GL_LINEAR);
    m_finalTexture->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_fbo->bind();
    m_fbo->setSize(w, h);
    m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH].get());
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT].get());
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT1, m_renderpassTextures[RENDERPASS_TEXTURE_POSITION].get());
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT2, m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL].get());
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT3, m_renderpassTextures[RENDERPASS_TEXTURE_PICKING].get());
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT4, m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED].get());
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT5, m_renderpassTexture.get());
    m_fbo->check();
    m_fbo->unbind(true);

    m_oitFbo->bind();
    m_oitFbo->setSize(w, h);
    m_oitFbo->attachTexture(GL_DEPTH_ATTACHMENT , m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH].get());
    m_oitFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_oitTextures[OITPASS_TEXTURE_ACCUM].get());
    m_oitFbo->attachTexture(GL_COLOR_ATTACHMENT1, m_oitTextures[OITPASS_TEXTURE_REVEALAGE].get());
    m_fbo->check();
    m_fbo->unbind(true);

    m_postprocessFbo->bind();
    m_postprocessFbo->setSize(w, h);
    m_postprocessFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_finalTexture.get());
    m_postprocessFbo->check();
    m_postprocessFbo->unbind(true);

    GL_CHECK_ERROR;

    // Reset framebuffer state
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    GL_ASSERT(glDrawBuffer(GL_BACK));
    GL_ASSERT(glReadBuffer(GL_BACK));
}

void Engine::Renderer::debugTexture(uint texIdx)
{
    if (texIdx > RENDERPASS_TEXTURE_COUNT)
    {
        m_displayedTexture = m_finalTexture.get();
        m_displayedIsDepth = false;
    }
    else
    {
        m_displayedTexture = m_renderpassTextures[texIdx].get();
        m_displayedIsDepth = (texIdx == 0);
    }
}

void Engine::Renderer::reloadShaders()
{
    ShaderProgramManager::getInstancePtr()->reloadAllShaderPrograms();
}

int Engine::Renderer::checkPicking(Scalar x, Scalar y) const
{
    Core::Color color = m_renderpassTextures[RENDERPASS_TEXTURE_PICKING]->getTexel(x, y);

    if (color == Core::Color(1.0, 1.0, 1.0, 1.0))
    {
        return -1;
    }
    color = color * 255;

    uint id = color.x() + color.y() * 256 + color.z() * 256 * 256;
    return id;

//    int index = d->idx.getValue();
}

void Engine::Renderer::handleFileLoading(const std::string &filename)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename,
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_GenSmoothNormals |
                                             aiProcess_SortByPType |
                                             aiProcess_FixInfacingNormals |
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_GenUVCoords);

    if (!scene)
    {
        return;
    }

    if (!scene->HasLights())
    {
        return;
    }

    // Load lights
    for (uint lightId = 0; lightId < scene->mNumLights; ++lightId)
    {
        aiLight* ailight = scene->mLights[lightId];

        aiString name = ailight->mName;
        aiNode* node = scene->mRootNode->FindNode(name);

        Core::Matrix4 transform(Core::Matrix4::Identity());

        if (node != nullptr)
        {
            Core::Matrix4 t0;
            Core::Matrix4 t1;

            for (uint i = 0; i < 4; ++i)
            {
                for (uint j = 0; j < 4; ++j)
                {
                    t0(i, j) = scene->mRootNode->mTransformation[i][j];
                    t1(i, j) = node->mTransformation[i][j];
                }
            }
            transform = t0 * t1;
        }

        Core::Color color(ailight->mColorDiffuse.r,
                          ailight->mColorDiffuse.g,
                          ailight->mColorDiffuse.b, 1.0);

        switch (ailight->mType)
        {
            case aiLightSource_DIRECTIONAL:
            {
                Core::Vector4 dir(ailight->mDirection[0],
                        ailight->mDirection[1],
                        ailight->mDirection[2], 0.0);
                dir = transform.transpose().inverse() * dir;

                Core::Vector3 finalDir(dir.x(), dir.y(), dir.z());
                finalDir = -finalDir;

                DirectionalLight* light = new DirectionalLight;
                light->setColor(color);
                light->setDirection(finalDir);

                addLight(light);

            } break;

            case aiLightSource_POINT:
            {
                Core::Vector4 pos(ailight->mPosition[0],
                        ailight->mPosition[1],
                        ailight->mPosition[2], 1.0);
                pos = transform * pos;
                pos /= pos.w();

                PointLight* light = new PointLight;
                light->setColor(color);
                light->setPosition(Core::Vector3(pos.x(), pos.y(), pos.z()));
                light->setAttenuation(ailight->mAttenuationConstant,
                                      ailight->mAttenuationLinear,
                                      ailight->mAttenuationQuadratic);

                addLight(light);

            } break;

            case aiLightSource_SPOT:
            {
                Core::Vector4 pos(ailight->mPosition[0],
                        ailight->mPosition[1],
                        ailight->mPosition[2], 1.0);
                pos = transform * pos;
                pos /= pos.w();

                Core::Vector4 dir(ailight->mDirection[0],
                        ailight->mDirection[1],
                        ailight->mDirection[2], 0.0);
                dir = transform.transpose().inverse() * dir;

                Core::Vector3 finalDir(dir.x(), dir.y(), dir.z());
                finalDir = -finalDir;

                SpotLight* light = new SpotLight;
                light->setColor(color);
                light->setPosition(Core::Vector3(pos.x(), pos.y(), pos.z()));
                light->setDirection(finalDir);

                light->setAttenuation(ailight->mAttenuationConstant,
                                      ailight->mAttenuationLinear,
                                      ailight->mAttenuationQuadratic);

                light->setInnerAngleInRadians(ailight->mAngleInnerCone);
                light->setOuterAngleInRadians(ailight->mAngleOuterCone);

                addLight(light);

            } break;

            case aiLightSource_UNDEFINED:
            default:
            {
                LOG(ERROR) << "Light " << name.C_Str() << " has undefined type.";
            } break;
        }
    }
}

} // namespace Ra