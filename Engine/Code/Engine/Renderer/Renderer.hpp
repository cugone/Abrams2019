#pragma once

#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/Vertex3D.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/RHI/RHI.hpp"
#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/RenderTargetStack.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

class AABB2;
class BlendState;
class Config;
class ConstantBuffer;
class DepthStencilState;
struct DepthStencilDesc;
class Disc2;
class Frustum;
class FileLogger;
class IndexBuffer;
class IntVector3;
class KerningFont;
class Material;
class OBB2;
class Polygon2;
class RasterState;
struct RasterDesc;
class Renderer;
class Rgba;
class Sampler;
struct SamplerDesc;
class Shader;
class ShaderProgram;
struct ShaderProgramDesc;
class SpriteSheet;
class StructuredBuffer;
class Texture;
class Texture1D;
class Texture2D;
class Texture3D;
class VertexBuffer;

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
    light_t lights[max_light_count] = {light_t{}};
    Vector4 ambient = Vector4::ZERO;
    Vector4 specular_glossy_emissive_factors = Vector4(1.0f, 8.0f, 0.0f, 1.0f);
    Vector4 eye_position = Vector4::ZERO;
    int useVertexNormals = 0;
    float padding[3] = {0.0f, 0.0f, 0.0f};
};

struct ComputeJob {
    Renderer& renderer;
    std::size_t uavCount = 0;
    std::vector<Texture*> uavTextures{};
    Shader* computeShader = nullptr;
    unsigned int threadGroupCountX = 1;
    unsigned int threadGroupCountY = 1;
    unsigned int threadGroupCountZ = 1;
    ComputeJob() = default;
    ComputeJob(Renderer& renderer,
               std::size_t uavCount,
               const std::vector<Texture*>& uavTextures,
               Shader* computeShader,
               unsigned int threadGroupCountX,
               unsigned int threadGroupCountY,
               unsigned int threadGroupCountZ) noexcept;
    ~ComputeJob() noexcept;
};

class Renderer : public EngineSubsystem {
public:
    struct DrawInstruction {
        PrimitiveType type;
        std::size_t vertexStart;
        std::size_t indexStart;
        std::size_t count;
        std::size_t baseVertexLocation;
        Material* material;
        [[nodiscard]] bool operator==(const DrawInstruction& rhs) {
            return material == rhs.material && type == rhs.type;
        }
        [[nodiscard]] bool operator!=(const DrawInstruction& rhs) {
            return !(*this == rhs);
        }
    };

    Renderer(FileLogger& fileLogger, Config& theConfig) noexcept;
    ~Renderer() noexcept;

    [[nodiscard]] FileLogger& GetFileLogger() noexcept;

    [[nodiscard]] bool ProcessSystemMessage(const EngineMessage& msg) noexcept override;
    void Initialize() override;
    void BeginFrame() override;
    void Update(TimeUtils::FPSeconds deltaSeconds) override;
    void Render() const override;
    void EndFrame() override;

    [[nodiscard]] TimeUtils::FPSeconds GetGameFrameTime() const noexcept;
    [[nodiscard]] TimeUtils::FPSeconds GetSystemFrameTime() const noexcept;
    [[nodiscard]] TimeUtils::FPSeconds GetGameTime() const noexcept;
    [[nodiscard]] TimeUtils::FPSeconds GetSystemTime() const noexcept;

    void SetFullscreen(bool isFullscreen) noexcept;
    void SetFullscreenMode() noexcept;
    void SetWindowedMode() noexcept;
    void SetWindowTitle(const std::string& newTitle) noexcept;
    [[nodiscard]] std::string GetWindowTitle() const noexcept;

    [[nodiscard]] std::unique_ptr<VertexBuffer> CreateVertexBuffer(const VertexBuffer::buffer_t& vbo) const noexcept;
    [[nodiscard]] std::unique_ptr<IndexBuffer> CreateIndexBuffer(const IndexBuffer::buffer_t& ibo) const noexcept;
    [[nodiscard]] std::unique_ptr<ConstantBuffer> CreateConstantBuffer(void* const& buffer, const std::size_t& buffer_size) const noexcept;
    [[nodiscard]] std::unique_ptr<StructuredBuffer> CreateStructuredBuffer(const StructuredBuffer::buffer_t& sbo, std::size_t element_size, std::size_t element_count) const noexcept;

    [[nodiscard]] Texture* CreateOrGetTexture(const std::filesystem::path& filepath, const IntVector3& dimensions) noexcept;
    void RegisterTexturesFromFolder(std::filesystem::path folderpath, bool recursive = false) noexcept;
    [[nodiscard]] bool RegisterTexture(const std::string& name, std::unique_ptr<Texture> texture) noexcept;
    void SetTexture(Texture* texture, unsigned int registerIndex = 0) noexcept;

    [[nodiscard]] Texture* GetFullscreenTexture() const noexcept;
    [[nodiscard]] Texture* GetTexture(const std::string& nameOrFile) noexcept;

    [[nodiscard]] std::unique_ptr<Texture> CreateDepthStencil(const RHIDevice& owner, const IntVector2& dimensions) noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateRenderableDepthStencil(const RHIDevice& owner, const IntVector2& dimensions) noexcept;

    [[nodiscard]] Texture* GetDefaultDepthStencil() const noexcept;
    void SetDepthStencilState(DepthStencilState* depthstencil) noexcept;
    [[nodiscard]] DepthStencilState* GetDepthStencilState(const std::string& name) noexcept;
    void CreateAndRegisterDepthStencilStateFromDepthStencilDescription(const std::string& name, const DepthStencilDesc& desc) noexcept;
    void EnableDepth(bool isDepthEnabled) noexcept;
    void EnableDepth() noexcept;
    void DisableDepth() noexcept;
    void EnableDepthWrite(bool isDepthWriteEnabled) noexcept;
    void EnableDepthWrite() noexcept;
    void DisableDepthWrite() noexcept;

    void SetDepthComparison(ComparisonFunction cf) noexcept;
    [[nodiscard]] ComparisonFunction GetDepthComparison() const noexcept;
    void SetStencilFrontComparison(ComparisonFunction cf) noexcept;
    void SetStencilBackComparison(ComparisonFunction cf) noexcept;
    void EnableStencilWrite() noexcept;
    void DisableStencilWrite() noexcept;

    [[nodiscard]] Texture* Create1DTexture(std::filesystem::path filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create1DTextureFromMemory(const unsigned char* data, unsigned int width = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create1DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept;
    [[nodiscard]] Texture* Create2DTexture(std::filesystem::path filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureFromMemory(const void* data, std::size_t elementSize, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureArrayFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureFromGifBuffer(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureArrayFromGifBuffer(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept;
    [[nodiscard]] Texture* Create3DTexture(std::filesystem::path filepath, const IntVector3& dimensions, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create3DTextureFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept;
    [[nodiscard]] std::unique_ptr<Texture> Create3DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept;
    [[nodiscard]] Texture* CreateTexture(std::filesystem::path filepath, const IntVector3& dimensions, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept;

    [[nodiscard]] std::shared_ptr<SpriteSheet> CreateSpriteSheet(const std::filesystem::path& filepath, unsigned int width = 1, unsigned int height = 1) noexcept;
    [[nodiscard]] std::shared_ptr<SpriteSheet> CreateSpriteSheet(const XMLElement& elem) noexcept;
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(std::filesystem::path filepath) noexcept;
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(const AnimatedSpriteDesc& desc) noexcept;
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(std::weak_ptr<SpriteSheet> sheet, const IntVector2& startSpriteCoords = IntVector2::ZERO) noexcept;
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(std::weak_ptr<SpriteSheet> sheet, const XMLElement& elem) noexcept;
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(const XMLElement& elem) noexcept;

    [[nodiscard]] const RenderTargetStack& GetRenderTargetStack() const noexcept;
    void PushRenderTarget(const RenderTargetStack::Node& newRenderTarget = RenderTargetStack::Node{}) noexcept;
    void PopRenderTarget() noexcept;
    void ClearRenderTargets(const RenderTargetType& rtt) noexcept;
    void SetRenderTarget(Texture* color_target = nullptr, Texture* depthstencil_target = nullptr) noexcept;
    void SetRenderTargetsToBackBuffer() noexcept;
    [[nodiscard]] ViewportDesc GetCurrentViewport() const;
    [[nodiscard]] float GetCurrentViewportAspectRatio() const;
    [[nodiscard]] std::vector<ViewportDesc> GetAllViewports() const;
    void SetViewport(const ViewportDesc& desc) noexcept;
    void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) noexcept;
    void SetViewport(const AABB2& viewport) noexcept;
    void SetViewportAndScissor(unsigned int x, unsigned int y, unsigned int width, unsigned int height) noexcept;
    void SetViewportAndScissor(const AABB2& viewport_and_scissor) noexcept;
    void SetViewports(const std::vector<AABB3>& viewports) noexcept;
    void SetViewportAsPercent(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f) noexcept;
    void SetViewportAndScissorAsPercent(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f) noexcept;

    void EnableScissorTest();
    void DisableScissorTest();

    void SetScissor(unsigned int x, unsigned int y, unsigned int width, unsigned int height) noexcept;
    void SetScissor(const AABB2& scissor) noexcept;
    void SetScissorAsPercent(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f) noexcept;
    void SetScissorAndViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) noexcept;
    void SetScissorAndViewport(const AABB2& scissor_and_viewport) noexcept;
    void SetScissorAndViewportAsPercent(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f) noexcept;
    void SetScissors(const std::vector<AABB2>& scissors) noexcept;

    void ClearColor(const Rgba& color) noexcept;
    void ClearTargetColor(Texture* target, const Rgba& color) noexcept;
    void ClearTargetDepthStencilBuffer(Texture* target, bool depth = true, bool stencil = true, float depthValue = 1.0f, unsigned char stencilValue = 0) noexcept;
    void ClearDepthStencilBuffer() noexcept;
    void Present() noexcept;

    void DrawPoint(const Vertex3D& point) noexcept;
    void DrawPoint(const Vector3& point, const Rgba& color = Rgba::White, const Vector2& tex_coords = Vector2::ZERO) noexcept;
    void DrawFrustum(const Frustum& frustum, const Rgba& color = Rgba::Yellow, const Vector2& tex_coords = Vector2::ZERO) noexcept;
    void DrawWorldGridXZ(float radius = 500.0f, float major_gridsize = 20.0f, float minor_gridsize = 5.0f, const Rgba& major_color = Rgba::White, const Rgba& minor_color = Rgba::DarkGray) noexcept;
    void DrawWorldGridXY(float radius = 500.0f, float major_gridsize = 20.0f, float minor_gridsize = 5.0f, const Rgba& major_color = Rgba::White, const Rgba& minor_color = Rgba::DarkGray) noexcept;
    void DrawWorldGrid2D(const IntVector2& dimensions, const Rgba& color = Rgba::White) noexcept;
    void DrawWorldGrid2D(int width, int height, const Rgba& color = Rgba::White) noexcept;

    void DrawAxes(float maxlength = 1000.0f, bool disable_unit_depth = true) noexcept;
    void DrawDebugSphere(const Rgba& color) noexcept;

    void Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo) noexcept;
    void Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, std::size_t vertex_count) noexcept;
    void DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo) noexcept;
    void DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo, std::size_t index_count, std::size_t startVertex = 0, std::size_t baseVertexLocation = 0) noexcept;

    void SetLightingEyePosition(const Vector3& position) noexcept;
    void SetAmbientLight(const Rgba& ambient) noexcept;
    void SetAmbientLight(const Rgba& color, float intensity) noexcept;
    void SetSpecGlossEmitFactors(Material* mat) noexcept;
    void SetUseVertexNormalsForLighting(bool value) noexcept;

    [[nodiscard]] const light_t& GetLight(unsigned int index) const noexcept;
    void SetPointLight(unsigned int index, const PointLightDesc& desc) noexcept;
    void SetDirectionalLight(unsigned int index, const DirectionalLightDesc& desc) noexcept;
    void SetSpotlight(unsigned int index, const SpotLightDesc& desc) noexcept;

    [[nodiscard]] RHIDeviceContext* GetDeviceContext() const noexcept;
    [[nodiscard]] const RHIDevice* GetDevice() const noexcept;
    [[nodiscard]] RHIOutput* GetOutput() const noexcept;
    [[nodiscard]] RHIInstance* GetInstance() const noexcept;

    [[nodiscard]] ShaderProgram* GetShaderProgram(const std::string& nameOrFile) noexcept;
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateShaderProgramFromHlslFile(std::filesystem::path filepath, const std::string& entryPointList, const PipelineStage& target) const noexcept;
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateShaderProgramFromCsoFile(std::filesystem::path filepath, const PipelineStage& target) const noexcept;
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateShaderProgramFromDesc(ShaderProgramDesc&& desc) const noexcept;
    void CreateAndRegisterShaderProgramFromHlslFile(std::filesystem::path filepath, const std::string& entryPointList, const PipelineStage& target) noexcept;
    void CreateAndRegisterShaderProgramFromCsoFile(std::filesystem::path filepath, const PipelineStage& target) noexcept;
    void CreateAndRegisterRasterStateFromRasterDescription(const std::string& name, const RasterDesc& desc) noexcept;
    void SetRasterState(RasterState* raster) noexcept;
    void SetRasterState(FillMode fillmode, CullMode cullmode) noexcept;
    [[nodiscard]] RasterState* GetRasterState(const std::string& name) noexcept;

    void SetWireframeRaster(CullMode cullmode = CullMode::Back) noexcept;
    void SetSolidRaster(CullMode cullmode = CullMode::Back) noexcept;

    void CreateAndRegisterSamplerFromSamplerDescription(const std::string& name, const SamplerDesc& desc) noexcept;
    [[nodiscard]] Sampler* GetSampler(const std::string& name) noexcept;
    void SetSampler(Sampler* sampler) noexcept;

    void SetVSync(bool value) noexcept;

    [[nodiscard]] std::unique_ptr<Material> CreateMaterialFromFont(KerningFont* font) noexcept;
    [[nodiscard]] bool RegisterMaterial(std::filesystem::path filepath) noexcept;
    void RegisterMaterial(std::unique_ptr<Material> mat) noexcept;
    void RegisterMaterialsFromFolder(std::filesystem::path folderpath, bool recursive = false) noexcept;
    void ReloadMaterials() noexcept;

    [[nodiscard]] std::size_t GetMaterialCount() noexcept;
    [[nodiscard]] Material* GetMaterial(const std::string& nameOrFile) noexcept;
    void SetMaterial(Material* material) noexcept;
    void SetMaterial(const std::string& nameOrFile) noexcept;

    [[nodiscard]] bool IsTextureLoaded(const std::string& nameOrFile) const noexcept;
    [[nodiscard]] bool IsTextureNotLoaded(const std::string& nameOrFile) const noexcept;

    [[nodiscard]] bool RegisterShader(std::filesystem::path filepath) noexcept;
    void RegisterShader(std::unique_ptr<Shader> shader) noexcept;

    [[nodiscard]] std::size_t GetShaderCount() const noexcept;
    [[nodiscard]] Shader* GetShader(const std::string& nameOrFile) noexcept;
    [[nodiscard]] std::string GetShaderName(const std::filesystem::path filepath) noexcept;

    void SetComputeShader(Shader* shader) noexcept;
    void DispatchComputeJob(const ComputeJob& job) noexcept;

    [[nodiscard]] std::size_t GetFontCount() const noexcept;
    [[nodiscard]] KerningFont* GetFont(const std::string& nameOrFile) noexcept;

    void RegisterFont(std::unique_ptr<KerningFont> font) noexcept;
    [[nodiscard]] bool RegisterFont(std::filesystem::path filepath) noexcept;
    void RegisterFontsFromFolder(std::filesystem::path folderpath, bool recursive = false) noexcept;

    void UpdateGameTime(TimeUtils::FPSeconds deltaSeconds) noexcept;

    void ResetModelViewProjection() noexcept;
    void AppendModelMatrix(const Matrix4& modelMatrix) noexcept;
    void SetModelMatrix(const Matrix4& mat = Matrix4::I) noexcept;
    void SetViewMatrix(const Matrix4& mat = Matrix4::I) noexcept;
    void SetProjectionMatrix(const Matrix4& mat = Matrix4::I) noexcept;
    void SetOrthoProjection(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& near_far) noexcept;
    void SetOrthoProjection(const Vector2& dimensions, const Vector2& origin, float nearz, float farz) noexcept;
    void SetOrthoProjectionFromViewHeight(float viewHeight, float aspectRatio, float nearz, float farz) noexcept;
    void SetOrthoProjectionFromViewWidth(float viewWidth, float aspectRatio, float nearz, float farz) noexcept;
    void SetOrthoProjectionFromCamera(const Camera3D& camera) noexcept;
    void SetPerspectiveProjection(const Vector2& vfovDegrees_aspect, const Vector2& nz_fz) noexcept;
    void SetPerspectiveProjectionFromCamera(const Camera3D& camera) noexcept;
    void SetCamera(const Camera3D& camera) noexcept;
    void SetCamera(const Camera2D& camera) noexcept;
    [[nodiscard]] Camera3D GetCamera() const noexcept;

    [[nodiscard]] Vector2 ConvertWorldToScreenCoords(const Vector3& worldCoords) const noexcept;
    [[nodiscard]] Vector2 ConvertWorldToScreenCoords(const Vector2& worldCoords) const noexcept;
    [[nodiscard]] Vector2 ConvertWorldToScreenCoords(const Camera3D& camera, const Vector3& worldCoords) const noexcept;
    [[nodiscard]] Vector2 ConvertWorldToScreenCoords(const Camera2D& camera, const Vector2& worldCoords) const noexcept;
    [[nodiscard]] Vector3 ConvertScreenToWorldCoords(const Vector2& mouseCoords) const noexcept;
    [[nodiscard]] Vector3 ConvertScreenToWorldCoords(const Camera3D& camera, const Vector2& mouseCoords) const noexcept;
    [[nodiscard]] Vector2 ConvertScreenToWorldCoords(const Camera2D& camera, const Vector2& mouseCoords) const noexcept;

    [[nodiscard]] Vector3 ConvertScreenToNdcCoords(const Camera3D& camera, const Vector2& mouseCoords) const noexcept;
    [[nodiscard]] Vector2 ConvertScreenToNdcCoords(const Camera2D& camera, const Vector2& mouseCoords) const noexcept;
    [[nodiscard]] Vector3 ConvertScreenToNdcCoords(const Vector2& mouseCoords) const noexcept;


    void SetConstantBuffer(unsigned int index, ConstantBuffer* buffer) noexcept;
    void SetStructuredBuffer(unsigned int index, StructuredBuffer* buffer) noexcept;
    void SetComputeConstantBuffer(unsigned int index, ConstantBuffer* buffer) noexcept;
    void SetComputeStructuredBuffer(unsigned int index, StructuredBuffer* buffer) noexcept;

    void DrawCube(const Vector3& position = Vector3::ZERO, const Vector3& halfExtents = Vector3::ONE * 0.5f, const Rgba& color = Rgba::White);
    void DrawQuad(const Vector3& position = Vector3::ZERO, const Vector3& halfExtents = Vector3::XY_AXIS * 0.5f, const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_AXIS, const Vector3& normalFront = Vector3::Z_AXIS, const Vector3& worldUp = Vector3::Y_AXIS) noexcept;
    void DrawQuad(const Rgba& frontColor, const Rgba& backColor, const Vector3& position = Vector3::ZERO, const Vector3& halfExtents = Vector3::XY_AXIS * 0.5f, const Vector4& texCoords = Vector4::ZW_AXIS, const Vector3& normalFront = Vector3::Z_AXIS, const Vector3& worldUp = Vector3::Y_AXIS) noexcept;
    void DrawPoint2D(float pointX, float pointY, const Rgba& color = Rgba::White) noexcept;
    void DrawPoint2D(const Vector2& point, const Rgba& color = Rgba::White) noexcept;
    void DrawLine2D(float startX, float startY, float endX, float endY, const Rgba& color = Rgba::White, float thickness = 0.0f) noexcept;
    void DrawLine2D(const Vector2& start, const Vector2& end, const Rgba& color = Rgba::White, float thickness = 0.0f) noexcept;
    void DrawQuad2D(float left, float bottom, float right, float top, const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_AXIS) noexcept;
    void DrawQuad2D(const Vector2& position = Vector2::ZERO, const Vector2& halfExtents = Vector2(0.5f, 0.5f), const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_AXIS) noexcept;
    void DrawQuad2D(const Matrix4& transform, const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_AXIS) noexcept;
    void DrawQuad2D(const Rgba& color) noexcept;
    void DrawQuad2D(const Vector4& texCoords) noexcept;
    void DrawQuad2D(const Rgba& color, const Vector4& texCoords) noexcept;
    void DrawCircle2D(float centerX, float centerY, float radius, const Rgba& color = Rgba::White) noexcept;
    void DrawCircle2D(const Vector2& center, float radius, const Rgba& color = Rgba::White) noexcept;
    void DrawCircle2D(const Disc2& circle, const Rgba& color = Rgba::White) noexcept;
    void DrawFilledCircle2D(const Disc2& circle, const Rgba& color = Rgba::White) noexcept;
    void DrawFilledCircle2D(const Vector2& center, float radius, const Rgba& color = Rgba::White) noexcept;
    void DrawAABB2(const AABB2& bounds, const Rgba& edgeColor, const Rgba& fillColor, const Vector2& edgeHalfExtents = Vector2::ZERO) noexcept;
    void DrawAABB2(const Rgba& edgeColor, const Rgba& fillColor) noexcept;
    void DrawOBB2(float orientationDegrees, const Rgba& edgeColor, const Rgba& fillColor = Rgba::NoAlpha) noexcept;
    void DrawOBB2(const OBB2& obb, const Rgba& edgeColor, const Rgba& fillColor = Rgba::NoAlpha, const Vector2& edgeHalfExtents = Vector2::ZERO) noexcept;
    void DrawPolygon2D(float centerX, float centerY, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::White) noexcept;
    void DrawPolygon2D(const Vector2& center, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::White) noexcept;
    void DrawPolygon2D(const Polygon2& polygon, const Rgba& color = Rgba::White);
    void DrawX2D(const Vector2& position = Vector2::ZERO, const Vector2& half_extents = Vector2(0.5f, 0.5f), const Rgba& color = Rgba::White) noexcept;
    void DrawX2D(const Rgba& color) noexcept;
    void DrawTextLine(const KerningFont* font, const std::string& text, const Rgba& color = Rgba::White) noexcept;
    void DrawMultilineText(KerningFont* font, const std::string& text, const Rgba& color = Rgba::White) noexcept;
    void AppendMultiLineTextBuffer(KerningFont* font, const std::string& text, const Vector2& start_position, const Rgba& color, std::vector<Vertex3D>& vbo, std::vector<unsigned int>& ibo) noexcept;

    constexpr static unsigned int MATRIX_BUFFER_INDEX = 0;
    constexpr static unsigned int TIME_BUFFER_INDEX = 1;
    constexpr static unsigned int LIGHTING_BUFFER_INDEX = 2;
    constexpr static unsigned int CONSTANT_BUFFER_START_INDEX = 3;
    constexpr static unsigned int STRUCTURED_BUFFER_START_INDEX = 64;
    constexpr static unsigned int MAX_LIGHT_COUNT = max_light_count;

    [[nodiscard]] std::vector<std::unique_ptr<ConstantBuffer>> CreateConstantBuffersFromShaderProgram(const ShaderProgram* _shader_program) const noexcept;

    void SetWinProc(const std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)>& windowProcedure) noexcept;

    void CopyTexture(const Texture* src, Texture* dst) const noexcept;
    void ResizeBuffers() noexcept;
    void ClearState() noexcept;

protected:
private:
    void UpdateSystemTime(TimeUtils::FPSeconds deltaSeconds) noexcept;
    [[nodiscard]] bool RegisterTexture(const std::filesystem::path& filepath) noexcept;
    void RegisterShaderProgram(const std::string& name, std::unique_ptr<ShaderProgram> sp) noexcept;
    void RegisterShaderProgramsFromFolder(std::filesystem::path folderpath, const std::string& entrypoint, const PipelineStage& target, bool recursive = false) noexcept;
    void RegisterShader(const std::string& name, std::unique_ptr<Shader> shader) noexcept;
    void RegisterShadersFromFolder(std::filesystem::path folderpath, bool recursive = false) noexcept;
    void RegisterMaterial(const std::string& name, std::unique_ptr<Material> mat) noexcept;
    void RegisterRasterState(const std::string& name, std::unique_ptr<RasterState> raster) noexcept;
    void RegisterDepthStencilState(const std::string& name, std::unique_ptr<DepthStencilState> depthstencil) noexcept;
    void RegisterSampler(const std::string& name, std::unique_ptr<Sampler> sampler) noexcept;
    void RegisterFont(const std::string& name, std::unique_ptr<KerningFont> font) noexcept;

    void CreateDefaultConstantBuffers() noexcept;
    void CreateWorkingVboAndIbo() noexcept;
    void UpdateVbo(const VertexBuffer::buffer_t& vbo) noexcept;
    void UpdateIbo(const IndexBuffer::buffer_t& ibo) noexcept;

    void Draw(const PrimitiveType& topology, VertexBuffer* vbo, std::size_t vertex_count) noexcept;
    void DrawIndexed(const PrimitiveType& topology, VertexBuffer* vbo, IndexBuffer* ibo, std::size_t index_count, std::size_t startVertex = 0, std::size_t baseVertexLocation = 0) noexcept;

    [[nodiscard]] std::shared_ptr<SpriteSheet> CreateSpriteSheetFromGif(std::filesystem::path filepath) noexcept;
    [[nodiscard]] std::shared_ptr<SpriteSheet> CreateSpriteSheet(Texture* texture, int tilesWide, int tilesHigh) noexcept;
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSpriteFromGif(std::filesystem::path filepath) noexcept;

    void SetLightAtIndex(unsigned int index, const light_t& light) noexcept;
    void SetPointLight(unsigned int index, const light_t& light) noexcept;
    void SetDirectionalLight(unsigned int index, const light_t& light) noexcept;
    void SetSpotlight(unsigned int index, const light_t& light) noexcept;

    void CreateAndRegisterDefaultTextures() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateDefaultTexture() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateInvalidTexture() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateDefaultDiffuseTexture() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateDefaultNormalTexture() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateDefaultDisplacementTexture() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateDefaultSpecularTexture() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateDefaultOcclusionTexture() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateDefaultEmissiveTexture() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateDefaultFullscreenTexture() noexcept;

    void CreateDefaultColorTextures() noexcept;
    [[nodiscard]] std::unique_ptr<Texture> CreateDefaultColorTexture(const Rgba& color) noexcept;

    void CreateAndRegisterDefaultShaderPrograms() noexcept;
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateDefaultShaderProgram() noexcept;
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateDefaultUnlitShaderProgram() noexcept;
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateDefaultNormalShaderProgram() noexcept;
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateDefaultNormalMapShaderProgram() noexcept;
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateDefaultFontShaderProgram() noexcept;

    [[nodiscard]] void CreateAndRegisterDefaultShaders() noexcept;
    [[nodiscard]] std::unique_ptr<Shader> CreateDefaultShader() noexcept;
    [[nodiscard]] std::unique_ptr<Shader> CreateDefaultUnlitShader() noexcept;
    [[nodiscard]] std::unique_ptr<Shader> CreateDefault2DShader() noexcept;
    [[nodiscard]] std::unique_ptr<Shader> CreateDefaultNormalShader() noexcept;
    [[nodiscard]] std::unique_ptr<Shader> CreateDefaultNormalMapShader() noexcept;
    [[nodiscard]] std::unique_ptr<Shader> CreateDefaultInvalidShader() noexcept;
    [[nodiscard]] std::unique_ptr<Shader> CreateDefaultFontShader() noexcept;
    [[nodiscard]] std::unique_ptr<Shader> CreateShaderFromFile(std::filesystem::path filepath) noexcept;

    void CreateAndRegisterDefaultMaterials() noexcept;
    [[nodiscard]] std::unique_ptr<Material> CreateDefaultMaterial() noexcept;
    [[nodiscard]] std::unique_ptr<Material> CreateDefaultUnlitMaterial() noexcept;
    [[nodiscard]] std::unique_ptr<Material> CreateDefault2DMaterial() noexcept;
    [[nodiscard]] std::unique_ptr<Material> CreateDefaultNormalMaterial() noexcept;
    [[nodiscard]] std::unique_ptr<Material> CreateDefaultNormalMapMaterial() noexcept;
    [[nodiscard]] std::unique_ptr<Material> CreateDefaultInvalidMaterial() noexcept;

    void CreateAndRegisterDefaultEngineFonts() noexcept;

    void CreateAndRegisterDefaultSamplers() noexcept;
    [[nodiscard]] std::unique_ptr<Sampler> CreateDefaultSampler() noexcept;
    [[nodiscard]] std::unique_ptr<Sampler> CreateLinearSampler() noexcept;
    [[nodiscard]] std::unique_ptr<Sampler> CreatePointSampler() noexcept;
    [[nodiscard]] std::unique_ptr<Sampler> CreateInvalidSampler() noexcept;

    void CreateAndRegisterDefaultRasterStates() noexcept;
    [[nodiscard]] std::unique_ptr<RasterState> CreateDefaultRaster() noexcept;
    [[nodiscard]] std::unique_ptr<RasterState> CreateScissorEnableRaster() noexcept;
    [[nodiscard]] std::unique_ptr<RasterState> CreateScissorDisableRaster() noexcept;
    [[nodiscard]] std::unique_ptr<RasterState> CreateWireframeRaster() noexcept;
    [[nodiscard]] std::unique_ptr<RasterState> CreateSolidRaster() noexcept;
    [[nodiscard]] std::unique_ptr<RasterState> CreateWireframeNoCullingRaster() noexcept;
    [[nodiscard]] std::unique_ptr<RasterState> CreateSolidNoCullingRaster() noexcept;
    [[nodiscard]] std::unique_ptr<RasterState> CreateWireframeFrontCullingRaster() noexcept;
    [[nodiscard]] std::unique_ptr<RasterState> CreateSolidFrontCullingRaster() noexcept;

    void CreateAndRegisterDefaultDepthStencilStates() noexcept;
    [[nodiscard]] std::unique_ptr<DepthStencilState> CreateDefaultDepthStencilState() noexcept;
    [[nodiscard]] std::unique_ptr<DepthStencilState> CreateDisabledDepth() noexcept;
    [[nodiscard]] std::unique_ptr<DepthStencilState> CreateEnabledDepth() noexcept;
    [[nodiscard]] std::unique_ptr<DepthStencilState> CreateDisabledStencil() noexcept;
    [[nodiscard]] std::unique_ptr<DepthStencilState> CreateEnabledStencil() noexcept;

    void CreateAndRegisterDefaultFonts() noexcept;
    [[nodiscard]] std::unique_ptr<KerningFont> CreateDefaultSystem32Font() noexcept;

    void UnbindAllResourcesAndBuffers() noexcept;
    void UnbindAllResources() noexcept;
    void UnbindAllBuffers() noexcept;
    void UnbindAllShaderResources() noexcept;
    void UnbindAllConstantBuffers() noexcept;
    void UnbindWorkingVboAndIbo() noexcept;
    void UnbindComputeShaderResources() noexcept;
    void UnbindComputeConstantBuffers() noexcept;

    void LogAvailableDisplays() noexcept;

    [[nodiscard]] Vector2 GetScreenCenter() const noexcept;
    [[nodiscard]] Vector2 GetWindowCenter() const noexcept;
    [[nodiscard]] Vector2 GetWindowCenter(const Window& window) const noexcept;

    Camera3D _camera{};
    matrix_buffer_t _matrix_data{};
    time_buffer_t _time_data{};
    lighting_buffer_t _lighting_data{};
    std::size_t _current_vbo_size = 0;
    std::size_t _current_ibo_size = 0;
    std::unique_ptr<RenderTargetStack> _target_stack = nullptr;
    RHIInstance* _rhi_instance = nullptr;
    std::unique_ptr<RHIDevice> _rhi_device = nullptr;
    std::unique_ptr<RHIDeviceContext> _rhi_context = nullptr;
    std::unique_ptr<RHIOutput> _rhi_output = nullptr;
    Texture* _current_target = nullptr;
    Texture* _current_depthstencil = nullptr;
    Texture* _default_depthstencil = nullptr;
    DepthStencilState* _current_depthstencil_state = nullptr;
    RasterState* _current_raster_state = nullptr;
    Sampler* _current_sampler = nullptr;
    Material* _current_material = nullptr;
    FileLogger& _fileLogger;
    Config& _theConfig;
    IntVector2 _window_dimensions = IntVector2::ZERO;
    RHIOutputMode _current_outputMode = RHIOutputMode::Windowed;
    std::unique_ptr<VertexBuffer> _temp_vbo = nullptr;
    std::unique_ptr<IndexBuffer> _temp_ibo = nullptr;
    std::unique_ptr<ConstantBuffer> _matrix_cb = nullptr;
    std::unique_ptr<ConstantBuffer> _time_cb = nullptr;
    std::unique_ptr<ConstantBuffer> _lighting_cb = nullptr;
    std::map<std::string, std::unique_ptr<Texture>> _textures{};
    std::map<std::string, std::unique_ptr<ShaderProgram>> _shader_programs;
    std::map<std::string, std::unique_ptr<Shader>> _shaders;
    std::map<std::string, std::unique_ptr<Material>> _materials;
    std::map<std::string, std::unique_ptr<Sampler>> _samplers;
    std::map<std::string, std::unique_ptr<RasterState>> _rasters;
    std::map<std::string, std::unique_ptr<DepthStencilState>> _depthstencils;
    std::map<std::string, std::unique_ptr<KerningFont>> _fonts;
    bool _vsync = false;
    bool _materials_need_updating = true;
    bool _sizemove_in_progress = false;
    bool _is_minimized = false;
    friend class Shader;
};
