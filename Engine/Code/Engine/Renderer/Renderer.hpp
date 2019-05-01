#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/Vertex3D.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Matrix4.hpp"

#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/RenderTargetStack.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include "Engine/RHI/RHI.hpp"

#include <filesystem>
#include <map>
#include <string>
#include <vector>

class AABB2;
class AnimatedSprite;
class BlendState;
class ConstantBuffer;
class DepthStencilState;
struct DepthStencilDesc;
class IndexBuffer;
class IntVector3;
class KerningFont;
class Material;
class OBB2;
class RasterState;
struct RasterDesc;
class Renderer;
class Rgba;
class Sampler;
struct SamplerDesc;
class Shader;
class ShaderProgram;
class SpriteSheet;
class StructuredBuffer;
class Texture;
class Texture1D;
class Texture2D;
class Texture3D;
class VertexBuffer;
class Frustum;

struct matrix_buffer_t {
    Matrix4 model{};
    Matrix4 view{};
    Matrix4 projection{};
};

struct time_buffer_t {
    float game_time = 0.0f;
    float system_time = 0.0f;
    float game_frame_time = 0.0f;
    float system_frame_time = 0.0f;
};

struct PointLightDesc {
    Vector3 position = Vector3::ZERO;
    Vector3 attenuation = Vector3::Z_AXIS;
    float intensity = 1.0f;
    Rgba color = Rgba::White;
};

struct DirectionalLightDesc {
    Vector3 direction = Vector3::X_AXIS;
    Vector3 attenuation = Vector3::X_AXIS;
    float intensity = 1.0f;
    Rgba color = Rgba::White;
};

struct SpotLightDesc {
    Vector3 position = Vector3::ZERO;
    Vector3 direction = Vector3::X_AXIS;
    Vector3 attenuation = Vector3::Z_AXIS;
    Vector2 inner_outer_anglesDegrees = Vector2{30.0f, 60.0f};
    float intensity = 1.0f;
    Rgba color = Rgba::White;
};

struct light_t {
    Vector4 position = Vector4::ZERO;
    Vector4 color = Vector4::ONE_XYZ_ZERO_W;
    Vector4 attenuation = Vector4::Z_AXIS;
    Vector4 specAttenuation = Vector4::X_AXIS;
    Vector4 innerOuterDotThresholds = Vector4(-2.0f, -3.0f, 0.0f, 0.0f);
    Vector4 direction = -Vector4::Z_AXIS;
};

constexpr const unsigned int max_light_count = 16;

struct lighting_buffer_t {
    light_t lights[max_light_count] = { light_t{} };
    Vector4 ambient = Vector4::ZERO;
    Vector4 specular_glossy_emissive_factors = Vector4(1.0f, 8.0f, 0.0f, 1.0f);
    Vector4 eye_position = Vector4::ZERO;
    int useVertexNormals = 0;
    float padding[3] = {0.0f, 0.0f, 0.0f};
};

struct ComputeJob {
    Renderer* renderer = nullptr;
    std::size_t uavCount = 0;
    std::vector<Texture*> uavTextures{};
    Shader* computeShader = nullptr;
    unsigned int threadGroupCountX = 1;
    unsigned int threadGroupCountY = 1;
    unsigned int threadGroupCountZ = 1;
    ComputeJob() = default;
    ComputeJob(Renderer* renderer,
               std::size_t uavCount,
               const std::vector<Texture*>& uavTextures,
               Shader* computeShader,
               unsigned int threadGroupCountX,
               unsigned int threadGroupCountY,
               unsigned int threadGroupCountZ);
    ~ComputeJob();
};

class Renderer {
public:
    Renderer() = default;
    Renderer(unsigned int width, unsigned int height);
    ~Renderer();

    void Initialize(bool headless = false);
    void BeginFrame();
    void Update(TimeUtils::FPSeconds deltaSeconds);
    void Render() const;
    void EndFrame();

    TimeUtils::FPSeconds GetGameFrameTime() const;
    TimeUtils::FPSeconds GetSystemFrameTime() const;
    TimeUtils::FPSeconds GetGameTime() const;
    TimeUtils::FPSeconds GetSystemTime() const;

    void SetFullscreen(bool isFullscreen);
    void SetBorderless(bool isBorderless);
    void SetFullscreenMode();
    void SetWindowedMode();
    void SetBorderlessWindowedMode();
    void SetWindowTitle(const std::string& newTitle);

    VertexBuffer* CreateVertexBuffer(const VertexBuffer::buffer_t& vbo) const;
    IndexBuffer* CreateIndexBuffer(const IndexBuffer::buffer_t& ibo) const;
    ConstantBuffer* CreateConstantBuffer(void* const& buffer, const std::size_t& buffer_size) const;
    StructuredBuffer* CreateStructuredBuffer(const StructuredBuffer::buffer_t& sbo, std::size_t element_size, std::size_t element_count) const;

    Texture* CreateOrGetTexture(const std::string& filepath, const IntVector3& dimensions);
    void RegisterTexturesFromFolder(const std::string& folderpath, bool recursive = false);
    bool RegisterTexture(const std::string& name, Texture* texture);
    void SetTexture(Texture* texture, unsigned int registerIndex = 0);

    Texture* GetTexture(const std::string& nameOrFile);

    Texture* CreateDepthStencil(const RHIDevice* owner, const IntVector2& dimensions);
    Texture* CreateRenderableDepthStencil(const RHIDevice* owner, const IntVector2& dimensions);

    Texture* GetDefaultDepthStencil() const;
    void SetDepthStencilState(DepthStencilState* depthstencil);
    DepthStencilState* GetDepthStencilState(const std::string& name);
    void CreateAndRegisterDepthStencilStateFromDepthStencilDescription(const std::string& name, const DepthStencilDesc& desc);
    void EnableDepth();
    void DisableDepth();

    Texture* Create1DTexture(const std::string& filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat);
    Texture* Create1DTextureFromMemory(const unsigned char* data, unsigned int width = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create1DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTexture(const std::string& filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat);
    Texture* Create2DTextureFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTextureArrayFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTextureFromGifBuffer(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create2DTextureArrayFromGifBuffer(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create3DTexture(const std::string& filepath, const IntVector3& dimensions, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat);
    Texture* Create3DTextureFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* Create3DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);
    Texture* CreateTexture(const std::string& filepath
        , const IntVector3& dimensions
        , const BufferUsage& bufferUsage = BufferUsage::Static
        , const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource
        , const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm);

    SpriteSheet* CreateSpriteSheet(const std::string& filepath, unsigned int width = 1, unsigned int height = 1);
    SpriteSheet* CreateSpriteSheet(const XMLElement& elem);
    AnimatedSprite* CreateAnimatedSprite(const std::string& filepath);
    AnimatedSprite* CreateAnimatedSprite(SpriteSheet* sheet);
    AnimatedSprite* CreateAnimatedSprite(const XMLElement& elem);

    const RenderTargetStack& GetRenderTargetStack() const;
    void PushRenderTarget(const RenderTargetStack::Node& newRenderTarget = RenderTargetStack::Node{});
    void PopRenderTarget();
    void ClearRenderTargets(const RenderTargetType& rtt);
    void SetRenderTarget(Texture* color_target = nullptr, Texture* depthstencil_target = nullptr);
    void SetRenderTargetsToBackBuffer();
    void SetViewport(const ViewportDesc& desc);
    void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void SetViewportAndScissor(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void SetViewports(const std::vector<AABB3>& viewports);
    void SetViewportAsPercent(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f);

    void SetScissor(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void SetScissorAndViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void SetScissors(const std::vector<AABB2>& scissors);

    void ClearColor(const Rgba& color);
    void ClearTargetColor(Texture* target, const Rgba& color);
    void ClearTargetDepthStencilBuffer(Texture* target, bool depth = true, bool stencil = true, float depthValue = 1.0f, unsigned char stencilValue = 0);
    void ClearDepthStencilBuffer();
    void Present();

    void DrawPoint(const Vertex3D& point);
    void DrawPoint(const Vector3& point, const Rgba& color = Rgba::White, const Vector2& tex_coords = Vector2::ZERO);
    void DrawFrustum(const Frustum& frustum, const Rgba& color = Rgba::Yellow, const Vector2& tex_coords = Vector2::ZERO);
    void DrawWorldGridXZ(float radius = 500.0f, float major_gridsize = 20.0f, float minor_gridsize = 5.0f, const Rgba& major_color = Rgba::White, const Rgba& minor_color = Rgba::DarkGray);
    void DrawWorldGridXY(float radius = 500.0f, float major_gridsize = 20.0f, float minor_gridsize = 5.0f, const Rgba& major_color = Rgba::White, const Rgba& minor_color = Rgba::DarkGray);
    void DrawWorldGrid2D(const IntVector2& dimensions, const Rgba& color = Rgba::White);
    void DrawWorldGrid2D(int width, int height, const Rgba& color = Rgba::White);

    void DrawAxes(float maxlength = 1000.0f, bool disable_unit_depth = true);
    void DrawDebugSphere(const Rgba& color);

    void Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo);
    void Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, std::size_t vertex_count);
    void DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo);
    void DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo, std::size_t vertex_count, std::size_t startVertex = 0, std::size_t baseVertexLocation = 0);

    void SetLightingEyePosition(const Vector3& position);
    void SetAmbientLight(const Rgba& ambient);
    void SetAmbientLight(const Rgba& color, float intensity);
    void SetSpecGlossEmitFactors(Material* mat);
    void SetUseVertexNormalsForLighting(bool value);

    const light_t& GetLight(unsigned int index) const;
    void SetPointLight(unsigned int index, const PointLightDesc& desc);
    void SetDirectionalLight(unsigned int index, const DirectionalLightDesc& desc);
    void SetSpotlight(unsigned int index, const SpotLightDesc& desc);

    RHIDeviceContext* GetDeviceContext() const;
    const RHIDevice* GetDevice() const;
    RHIOutput* GetOutput() const;

    ShaderProgram* GetShaderProgram(const std::string& nameOrFile);
    ShaderProgram* CreateShaderProgramFromHlslFile(const std::string& filepath, const std::string& entryPointList, const PipelineStage& target) const;
    void CreateAndRegisterShaderProgramFromHlslFile(const std::string& filepath, const std::string& entryPointList, const PipelineStage& target);
    void RegisterShaderProgramsFromFolder(const std::string& folderpath, const std::string& entrypoint, const PipelineStage& target, bool recursive = false);
    void CreateAndRegisterRasterStateFromRasterDescription(const std::string& name, const RasterDesc& desc);
    void SetRasterState(RasterState* raster);
    RasterState* GetRasterState(const std::string& name);

    void CreateAndRegisterSamplerFromSamplerDescription(const std::string& name, const SamplerDesc& desc);
    Sampler* GetSampler(const std::string& name);
    void SetSampler(Sampler* sampler);

    void SetVSync(bool value);

    Material* CreateMaterialFromFont(KerningFont* font);
    bool RegisterMaterial(const std::string& filepath);
    void RegisterMaterial(Material* mat);
    void RegisterMaterialsFromFolder(const std::string& folderpath, bool recursive = false);
    std::size_t GetMaterialCount();
    Material* GetMaterial(const std::string& nameOrFile);
    void SetMaterial(Material* material);
    const std::map<std::string, Texture*>& GetLoadedTextures() const;

    bool RegisterShader(const std::string& filepath);
    void RegisterShader(Shader* shader);
    void RegisterShadersFromFolder(const std::string& filepath, bool recursive = false);
    std::size_t GetShaderCount() const;
    Shader* GetShader(const std::string& nameOrFile);
    void SetComputeShader(Shader* shader);
    void DispatchComputeJob(const ComputeJob& job);

    std::size_t GetFontCount() const;
    KerningFont* GetFont(const std::string& nameOrFile);

    bool RegisterFont(const std::string& filepath);
    void RegisterFont(KerningFont* font);
    void RegisterFontsFromFolder(const std::string& folderpath, bool recursive = false);

    void UpdateGameTime(TimeUtils::FPSeconds deltaSeconds);

    void ResetModelViewProjection();
    void AppendModelMatrix(const Matrix4& modelMatrix);
    void SetModelMatrix(const Matrix4& mat = Matrix4::I);
    void SetViewMatrix(const Matrix4& mat = Matrix4::I);
    void SetProjectionMatrix(const Matrix4& mat = Matrix4::I);
    void SetOrthoProjection(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& near_far);
    void SetOrthoProjection(const Vector2& dimensions, const Vector2& origin, float nearz, float farz);
    void SetOrthoProjectionFromViewHeight(float viewHeight, float aspectRatio, float nearz, float farz);
    void SetOrthoProjectionFromViewWidth(float viewWidth, float aspectRatio, float nearz, float farz);
    void SetOrthoProjectionFromCamera(const Camera3D& camera);
    void SetPerspectiveProjection(const Vector2& vfovDegrees_aspect, const Vector2& nz_fz);
    void SetPerspectiveProjectionFromCamera(const Camera3D& camera);
    void SetCamera(const Camera3D& camera);
    void SetCamera(const Camera2D& camera);
    Camera3D GetCamera() const;

    void SetConstantBuffer(unsigned int index, ConstantBuffer* buffer);
    void SetStructuredBuffer(unsigned int index, StructuredBuffer* buffer);
    void SetComputeConstantBuffer(unsigned int index, ConstantBuffer* buffer);
    void SetComputeStructuredBuffer(unsigned int index, StructuredBuffer* buffer);

    void DrawQuad(const Vector3& position = Vector3::ZERO, const Vector3& halfExtents = Vector3::XY_AXIS * 0.5f, const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_AXIS, const Vector3& normalFront = Vector3::Z_AXIS, const Vector3& worldUp = Vector3::Y_AXIS);
    void DrawQuad(const Rgba& frontColor, const Rgba& backColor, const Vector3& position = Vector3::ZERO, const Vector3& halfExtents = Vector3::XY_AXIS * 0.5f, const Vector4& texCoords = Vector4::ZW_AXIS, const Vector3& normalFront = Vector3::Z_AXIS, const Vector3& worldUp = Vector3::Y_AXIS);
    void DrawPoint2D(float pointX, float pointY, const Rgba& color = Rgba::White);
    void DrawPoint2D(const Vector2& point, const Rgba& color = Rgba::White);
    void DrawLine2D(float startX, float startY, float endX, float endY, const Rgba& color = Rgba::White, float thickness = 0.0f);
    void DrawLine2D(const Vector2& start, const Vector2& end, const Rgba& color = Rgba::White, float thickness = 0.0f);
    void DrawQuad2D(float left, float bottom, float right, float top, const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_AXIS);
    void DrawQuad2D(const Vector2& position = Vector2::ZERO, const Vector2& halfExtents = Vector2(0.5f, 0.5f), const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_AXIS);
    void DrawQuad2D(const Rgba& color);
    void DrawQuad2D(const Vector4& texCoords);
    void DrawQuad2D(const Rgba& color, const Vector4& texCoords);
    void DrawCircle2D(float centerX, float centerY, float radius, const Rgba& color = Rgba::White);
    void DrawCircle2D(const Vector2& center, float radius, const Rgba& color = Rgba::White);
    void DrawFilledCircle2D(const Vector2& center, float radius, const Rgba& color = Rgba::White);
    void DrawAABB2(const AABB2& bounds, const Rgba& edgeColor, const Rgba& fillColor, const Vector2& edgeHalfExtents = Vector2::ZERO);
    void DrawAABB2(const Rgba& edgeColor, const Rgba& fillColor);
    void DrawOBB2(const OBB2& obb, const Rgba& edgeColor, const Rgba& fillColor, const Vector2& edgeHalfExtents = Vector2::ZERO);
    void DrawOBB2(float orientationDegrees, const Rgba& edgeColor, const Rgba& fillColor);
    void DrawPolygon2D(float centerX, float centerY, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::White);
    void DrawPolygon2D(const Vector2& center, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::White);
    void DrawX2D(const Vector2& position = Vector2::ZERO, const Vector2& half_extents = Vector2(0.5f, 0.5f), const Rgba& color = Rgba::White);
    void DrawX2D(const Rgba& color);
    void DrawTextLine(const KerningFont* font, const std::string& text, const Rgba& color = Rgba::White);
    void DrawMultilineText(KerningFont* font, const std::string& text, const Rgba& color = Rgba::White);
    void AppendMultiLineTextBuffer(KerningFont* font, const std::string& text, const Vector2& start_position, const Rgba& color, std::vector<Vertex3D>& vbo, std::vector<unsigned int>& ibo);

    constexpr static unsigned int MATRIX_BUFFER_INDEX = 0;
    constexpr static unsigned int TIME_BUFFER_INDEX = 1;
    constexpr static unsigned int LIGHTING_BUFFER_INDEX = 2;
    constexpr static unsigned int CONSTANT_BUFFER_START_INDEX = 3;
    constexpr static unsigned int STRUCTURED_BUFFER_START_INDEX = 64;
    constexpr static unsigned int MAX_LIGHT_COUNT = max_light_count;

    std::vector<ConstantBuffer*> CreateConstantBuffersFromShaderProgram(const ShaderProgram* _shader_program) const;

    void SetWinProc(const std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) >& windowProcedure);

    void CopyTexture(Texture* src, Texture* dst);
protected:
private:
    void UpdateSystemTime(TimeUtils::FPSeconds deltaSeconds);
    void RegisterTexturesFromFolder(const std::filesystem::path& folderpath, bool recursive = false);
    bool RegisterTexture(const std::filesystem::path& filepath);
    void RegisterShaderProgram(const std::string& name, ShaderProgram * sp);
    void RegisterShaderProgramsFromFolder(const std::filesystem::path& folderpath, const std::string& entrypoint, const PipelineStage& target, bool recursive = false);
    void RegisterShader(const std::string& name, Shader* shader);
    bool RegisterShader(std::filesystem::path filepath);
    void RegisterShadersFromFolder(const std::filesystem::path& folderpath, bool recursive = false);
    void RegisterMaterial(const std::string& name, Material* mat);
    bool RegisterMaterial(const std::filesystem::path& filepath);
    void RegisterMaterialsFromFolder(const std::filesystem::path& folderpath, bool recursive = false);
    void RegisterRasterState(const std::string& name, RasterState* raster);
    void RegisterDepthStencilState(const std::string& name, DepthStencilState* depthstencil);
    void RegisterSampler(const std::string& name, Sampler* sampler);
    void RegisterFont(const std::string& name, KerningFont* font);
    bool RegisterFont(const std::filesystem::path& filepath);
    void RegisterFontsFromFolder(const std::filesystem::path& folderpath, bool recursive = false);


    void CreateDefaultConstantBuffers();
    void CreateWorkingVboAndIbo();
    void UpdateVbo(const VertexBuffer::buffer_t& vbo);
    void UpdateIbo(const IndexBuffer::buffer_t& ibo);

    void Draw(const PrimitiveType& topology, VertexBuffer* vbo, std::size_t vertex_count);
    void DrawIndexed(const PrimitiveType& topology, VertexBuffer* vbo, IndexBuffer* ibo, std::size_t index_count, std::size_t startVertex = 0, std::size_t baseVertexLocation = 0);

    SpriteSheet* CreateSpriteSheetFromGif(const std::string& filepath);
    AnimatedSprite* CreateAnimatedSpriteFromGif(const std::string& filepath);

    void SetLightAtIndex(unsigned int index, const light_t& light);
    void SetPointLight(unsigned int index, const light_t& light);
    void SetDirectionalLight(unsigned int index, const light_t& light);
    void SetSpotlight(unsigned int index, const light_t& light);
    
    void CreateAndRegisterDefaultTextures();
    Texture* CreateDefaultTexture();
    Texture* CreateInvalidTexture();
    Texture* CreateDefaultDiffuseTexture();
    Texture* CreateDefaultNormalTexture();
    Texture* CreateDefaultDisplacementTexture();
    Texture* CreateDefaultSpecularTexture();
    Texture* CreateDefaultOcclusionTexture();
    Texture* CreateDefaultEmissiveTexture();
    void CreateAndRegisterDefaultDepthStencil();

    void CreateAndRegisterDefaultShaderPrograms();
    ShaderProgram* CreateDefaultShaderProgram();
    ShaderProgram* CreateDefaultUnlitShaderProgram();
    ShaderProgram* CreateDefaultNormalShaderProgram();
    ShaderProgram* CreateDefaultNormalMapShaderProgram();
    ShaderProgram* CreateDefaultFontShaderProgram();

    void CreateAndRegisterDefaultShaders();
    Shader* CreateDefaultShader();
    Shader* CreateDefaultUnlitShader();
    Shader* CreateDefault2DShader();
    Shader* CreateDefaultNormalShader();
    Shader* CreateDefaultNormalMapShader();
    Shader* CreateDefaultFontShader();
    Shader* CreateShaderFromFile(const std::string& filePath);

    void CreateAndRegisterDefaultMaterials();
    Material* CreateDefaultMaterial();
    Material* CreateDefaultUnlitMaterial();
    Material* CreateDefault2DMaterial();
    Material* CreateDefaultNormalMaterial();
    Material* CreateDefaultNormalMapMaterial();

    void CreateAndRegisterDefaultFonts();

    void CreateAndRegisterDefaultSamplers();
    Sampler* CreateDefaultSampler();
    Sampler* CreateLinearSampler();
    Sampler* CreatePointSampler();

    void CreateAndRegisterDefaultRasterStates();
    RasterState* CreateDefaultRaster();
    RasterState* CreateWireframeRaster();
    RasterState* CreateSolidRaster();
    RasterState* CreateWireframeNoCullingRaster();
    RasterState* CreateSolidNoCullingRaster();
    RasterState* CreateWireframeFrontCullingRaster();
    RasterState* CreateSolidFrontCullingRaster();

    void CreateAndRegisterDefaultDepthStencilStates();
    DepthStencilState* CreateDefaultDepthStencilState();
    DepthStencilState* CreateDisabledDepth();
    DepthStencilState* CreateEnabledDepth();
    DepthStencilState* CreateDisabledStencil();
    DepthStencilState* CreateEnabledStencil();

    void UnbindAllShaderResources();
    void UnbindAllConstantBuffers();
    void UnbindComputeShaderResources();
    void UnbindComputeConstantBuffers();

    void LogAvailableDisplays();

    Camera3D _camera{};
    matrix_buffer_t _matrix_data{};
    time_buffer_t _time_data{};
    lighting_buffer_t _lighting_data{};
    std::size_t _current_vbo_size = 0;
    std::size_t _current_ibo_size = 0;
    RenderTargetStack* _target_stack = nullptr;
    std::unique_ptr<RHIDeviceContext> _rhi_context = nullptr;
    std::unique_ptr<RHIDevice> _rhi_device = nullptr;
    std::unique_ptr<RHIOutput> _rhi_output = nullptr;
    RHIInstance* _rhi_instance = nullptr;
    Texture* _current_target = nullptr;
    Texture* _current_depthstencil = nullptr;
    Texture* _default_depthstencil = nullptr;
    DepthStencilState* _current_depthstencil_state = nullptr;
    RasterState* _current_raster_state = nullptr;
    Sampler* _current_sampler = nullptr;
    Material* _current_material = nullptr;
    IntVector2 _window_dimensions = IntVector2::ZERO;
    RHIOutputMode _current_outputMode = RHIOutputMode::Windowed;
    VertexBuffer* _temp_vbo = nullptr;
    IndexBuffer* _temp_ibo = nullptr;
    ConstantBuffer* _matrix_cb = nullptr;
    ConstantBuffer* _time_cb = nullptr;
    ConstantBuffer* _lighting_cb = nullptr;
    std::map<std::string, Texture*> _textures{};
    std::map<std::string, ShaderProgram*> _shader_programs{};
    std::map<std::string, Material*> _materials{};
    std::map<std::string, Shader*> _shaders{};
    std::map<std::string, Sampler*> _samplers{};
    std::map<std::string, RasterState*> _rasters{};
    std::map<std::string, DepthStencilState*> _depthstencils{};
    std::map<std::string, KerningFont*> _fonts{};
    bool _vsync = false;
    friend class Shader;
};
