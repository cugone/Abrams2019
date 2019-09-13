#include "Engine/Core/Image.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/Math/MathUtils.hpp"

#include "ThirdParty/stb/stb_image.h"
#include "ThirdParty/stb/stb_image_write.h"

#include <sstream>
#include <vector>

Image::Image(std::filesystem::path filepath) noexcept
: m_filepath(filepath)
{

    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        std::ostringstream ss;
        ss << "Failed to load image. Could not find file: " << filepath << ".\n";
        ERROR_AND_DIE(ss.str().c_str());
    }
    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    std::vector<unsigned char> buf = {};
    if(FileUtils::ReadBufferFromFile(buf, filepath)) {
        m_isGif = (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F' && buf[3] == '8' && (buf[4] == '9' || buf[4] == '7') && buf[5] == 'a');
        if(!m_isGif) {
            int comp = 0;
            auto texel_bytes = stbi_load_from_memory(buf.data(), static_cast<int>(buf.size()), &m_dimensions.x, &m_dimensions.y, &comp, 4);
            m_bytesPerTexel = comp;
            m_texelBytes = std::vector<unsigned char>(texel_bytes, texel_bytes + (static_cast<std::size_t>(m_dimensions.x) * m_dimensions.y * m_bytesPerTexel));
            stbi_image_free(texel_bytes);
        } else {
            int depth = 0;
            int* delays = nullptr;
            int comp = 0;
            auto texel_bytes = stbi_load_gif_from_memory(buf.data(), static_cast<int>(buf.size()), &delays, &m_dimensions.x, &m_dimensions.y, &depth, &comp, 4);
            m_bytesPerTexel = comp;
            m_gifDelays.resize(depth);
            for(int i = 0; i < depth; ++i) {
                m_gifDelays[i] = delays[i];
            }
            m_dimensions.y *= depth;
            m_texelBytes = std::vector<unsigned char>(texel_bytes, texel_bytes + (static_cast<std::size_t>(m_dimensions.x) * m_dimensions.y * m_bytesPerTexel));
            stbi_image_free(texel_bytes);
        }
    } else {
        std::ostringstream ss;
        ss << "Failed to load image. " << filepath << " is not a supported image type.";
        DebuggerPrintf(ss.str().c_str());
        GUARANTEE_RECOVERABLE(!m_texelBytes.empty(), ss.str());
    }
}
Image::Image(unsigned int width, unsigned int height) noexcept
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_texelBytes(static_cast<std::size_t>(width) * height * m_bytesPerTexel, 0u)
{
    /* DO NOTHING */
}

Image::Image(unsigned char* data, unsigned int width, unsigned int height) noexcept
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_texelBytes(data, data + (static_cast<std::size_t>(width) * height * m_bytesPerTexel))
{
    /* DO NOTHING */
}

Image::Image(Rgba* data, unsigned int width, unsigned int height) noexcept
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_texelBytes(reinterpret_cast<unsigned char*>(data), reinterpret_cast<unsigned char*>(data) + static_cast<std::size_t>(width) * height * m_bytesPerTexel)
{
    /* DO NOTHING */
}

Image::Image(const std::vector<Rgba>& data, unsigned int width, unsigned int height) noexcept
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_texelBytes(reinterpret_cast<const unsigned char*>(data.data()), reinterpret_cast<const unsigned char*>(data.data()) + static_cast<std::size_t>(width) * height * m_bytesPerTexel)
{
    /* DO NOTHING */
}

Image::Image(const std::vector<unsigned char>& data, unsigned int width, unsigned int height) noexcept
    : m_dimensions(width, height)
    , m_bytesPerTexel(4)
    , m_texelBytes(data.data(), data.data() + static_cast<std::size_t>(width) * height * m_bytesPerTexel)
{
    /* DO NOTHING */
}

Image::Image(Image&& img) noexcept
    : m_dimensions(std::move(img.m_dimensions))
    , m_bytesPerTexel(std::move(img.m_bytesPerTexel))
    , m_gifDelays(std::move(img.m_gifDelays))
    , m_filepath(std::move(img.m_filepath))
    , m_isGif(std::move(m_isGif))
{
    std::scoped_lock<std::mutex, std::mutex>(_cs, img._cs);
    m_texelBytes = std::move(img.m_texelBytes);
}


Image& Image::operator=(Image&& rhs) noexcept {
    std::scoped_lock<std::mutex, std::mutex> _lock(_cs, rhs._cs);
    m_bytesPerTexel = std::move(rhs.m_bytesPerTexel);
    m_dimensions = std::move(rhs.m_dimensions);
    m_filepath = std::move(rhs.m_filepath);
    m_gifDelays = std::move(rhs.m_gifDelays);
    m_isGif = std::move(rhs.m_isGif);
    m_texelBytes = std::move(rhs.m_texelBytes);

    rhs.m_bytesPerTexel = 0;
    rhs.m_dimensions = IntVector2::ZERO;
    rhs.m_filepath = std::string{};
    rhs.m_gifDelays.clear();
    rhs.m_gifDelays.shrink_to_fit();
    rhs.m_texelBytes.clear();
    rhs.m_texelBytes.shrink_to_fit();
    rhs.m_isGif = false;
    return *this;
}

Rgba Image::GetTexel(const IntVector2& texelPos) const noexcept {
    int index = texelPos.x + texelPos.y * m_dimensions.x;
    int byteOffset = index * m_bytesPerTexel;
    //HACK: If too slow, use following commented line instead.
    Rgba color;// = *(reinterpret_cast<Rgba>(&m_texelBytes[byteOffset]));
    color.r = m_texelBytes[byteOffset + 0];
    color.g = m_texelBytes[byteOffset + 1];
    color.b = m_texelBytes[byteOffset + 2];
    if(m_bytesPerTexel == 4) {
        color.a = m_texelBytes[byteOffset + 3];
    } else {
        color.a = 255;
    }
    return color;
}
void Image::SetTexel(const IntVector2& texelPos, const Rgba& color) noexcept {
    Rgba oldColor = GetTexel(texelPos);
    int index = texelPos.x + texelPos.y * m_dimensions.x;
    int byteOffset = index * m_bytesPerTexel;
    //HACK: If too slow, use following commented line instead.
    //m_texelBytes[byteOffset] = reinterpret_cast<unsigned char*>(&color);
    m_texelBytes[byteOffset + 0] = color.r;
    m_texelBytes[byteOffset + 1] = color.g;
    m_texelBytes[byteOffset + 2] = color.b;
    if(m_bytesPerTexel == 4) {
        m_texelBytes[byteOffset + 3] = color.a;
    }
}

const std::filesystem::path& Image::GetFilepath() const noexcept {
    return m_filepath;
}

const IntVector2& Image::GetDimensions() const noexcept {
    return m_dimensions;
}

const unsigned char* Image::GetData() const noexcept {
    return m_texelBytes.data();
}

unsigned char* Image::GetData() noexcept {
    return m_texelBytes.data();
}

std::size_t Image::GetDataLength() const noexcept {
    return static_cast<std::size_t>(m_dimensions.x) * m_dimensions.y * m_bytesPerTexel;
}

int Image::GetBytesPerTexel() const noexcept {
    return m_bytesPerTexel;
}

const std::vector<int>& Image::GetDelaysIfGif() const noexcept {
    return m_gifDelays;
}

bool Image::Export(std::filesystem::path filepath, int bytes_per_pixel /*= 4*/, int jpg_quality /*= 100*/) noexcept {
    if(m_texelBytes.empty()) {
        std::ostringstream ss;
        ss << "Attempting to write empty Image: " << filepath;
        DebuggerPrintf(ss.str().c_str());
        return false;
    }
    namespace FS = std::filesystem;
    filepath = FS::absolute(filepath);
    filepath.make_preferred();
    std::string extension = StringUtils::ToLowerCase(filepath.extension().string());
    std::string p_str = filepath.string();
    const auto& dims = GetDimensions();
    int w = dims.x;
    int h = dims.y;
    int bbp = bytes_per_pixel;
    int stride = bbp * w;
    int quality = std::clamp(jpg_quality, 0, 100);
    int result = 0;
    if(extension == ".png") {
        std::scoped_lock<std::mutex> lock(_cs);
        result = stbi_write_png(p_str.c_str(), w, h, bbp, m_texelBytes.data(), stride);
    } else if(extension == ".bmp") {
        std::scoped_lock<std::mutex> lock(_cs);
        result = stbi_write_bmp(p_str.c_str(), w, h, bbp, m_texelBytes.data());
    } else if(extension == ".tga") {
        std::scoped_lock<std::mutex> lock(_cs);
        result = stbi_write_tga(p_str.c_str(), w, h, bbp, m_texelBytes.data());
    } else if(extension == ".jpg") {
        std::scoped_lock<std::mutex> lock(_cs);
        result = stbi_write_jpg(p_str.c_str(), w, h, bbp, m_texelBytes.data(), quality);
    } else if(extension == ".hdr") {
        std::ostringstream ss;
        ss << "Attempting to export " << filepath << " to an unsupported type: " << extension;
        ss << "\nHigh Dynamic Range output is not supported.";
        ERROR_RECOVERABLE(ss.str().c_str());
    }
    return 0 != result;
}

Image Image::CreateImageFromFileBuffer(const std::vector<unsigned char>& data) noexcept {
    if(data.empty()) {
        std::ostringstream ss;
        ss << "Attempting to create image from empty data buffer.\n";
        DebuggerPrintf(ss.str().c_str());
        return{};
    }
    int dim_x = 0;
    int dim_y = 0;
    int comp = 0;
    std::vector<unsigned char> texel_bytes{};
    {
        int req_comp = 4;
        auto* bytes = stbi_load_from_memory(data.data(), static_cast<int>(data.size()), &dim_x, &dim_y, &comp, req_comp);
        if(!bytes) {
            std::ostringstream ss;
            ss << "Data does not represent an image.\n";
            DebuggerPrintf(ss.str().c_str());
            return{};
        }
        std::size_t size = dim_x * dim_y * comp;
        texel_bytes.assign(bytes, bytes + size);
        stbi_image_free(bytes);
        bytes = nullptr;
    }
    Image result{};
    result.m_dimensions = IntVector2(dim_x, dim_y);
    result.m_bytesPerTexel = comp;
    result.m_isGif = data.size() > 6 ? (data[0] == 'G' && data[1] == 'I' && data[2] == 'F' && data[3] == '8' && (data[4] == '9' || data[4] == '7') && data[5] == 'a') : false;
    if(!result.m_isGif) {
        result.m_texelBytes = texel_bytes;
    } else {
        int depth = 0;
        int* delays = nullptr;
        int req_comp = 4;
        comp = 0;
        auto* bytes = stbi_load_gif_from_memory(data.data(), static_cast<int>(data.size()), &delays, &dim_x, &dim_y, &depth, &comp, req_comp);
        if(!bytes) {
            return{};
        }
        auto size = std::size_t(dim_x) * dim_y * comp;
        result.m_gifDelays.assign(delays, delays + depth);
        texel_bytes.assign(bytes, bytes + size);
        stbi_image_free(delays);
        delays = nullptr;
        stbi_image_free(bytes);
        bytes = nullptr;
        result.m_dimensions.x = dim_x;
        result.m_dimensions.y = dim_y;
        result.m_dimensions.y *= depth;
        result.m_texelBytes = texel_bytes;
    }
    return std::move(result);
}

std::string Image::GetSupportedExtensionsList() noexcept {
    return std::string(".png,.bmp,.tga,.jpg");

}

void swap(Image& a, Image& b) noexcept {
    std::scoped_lock<std::mutex, std::mutex> _lock(a._cs, b._cs);
    std::swap(a.m_bytesPerTexel, b.m_bytesPerTexel);
    std::swap(a.m_dimensions, b.m_dimensions);
    std::swap(a.m_filepath, b.m_filepath);
    std::swap(a.m_gifDelays, b.m_gifDelays);
    std::swap(a.m_isGif, b.m_isGif);
    std::swap(a.m_texelBytes, b.m_texelBytes);
}
