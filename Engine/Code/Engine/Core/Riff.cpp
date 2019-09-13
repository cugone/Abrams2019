#include "Engine/Core/Riff.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include <iostream>
#include <sstream>

namespace FileUtils {

namespace RiffChunkID {
constexpr const bool IsValid(const char* id) noexcept {
    return (StringUtils::FourCC(id) == RiffChunkID::RIFF
            || StringUtils::FourCC(id) == RiffChunkID::LIST
            || StringUtils::FourCC(id) == RiffChunkID::INFO
            || StringUtils::FourCC(id) == RiffChunkID::WAVE
            || StringUtils::FourCC(id) == RiffChunkID::AVI);
}
} //End RiffChunkID

bool Riff::ParseDataIntoChunks(std::vector<unsigned char>& buffer) noexcept {
    std::stringstream stream(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    buffer.clear();
    buffer.shrink_to_fit();
    stream.clear();
    stream.seekp(0);
    stream.seekg(0);
    RiffHeader cur_header{};
    while(stream.read(reinterpret_cast<char*>(&cur_header), sizeof(cur_header))) {
        auto cur_chunk = std::make_unique<RiffChunk>();
        cur_chunk->header = cur_header;
        switch(StringUtils::FourCC(cur_header.fourcc)) {
            case RiffChunkID::RIFF:
            {
                auto subdata = std::make_unique<RiffSubChunk>();
                if(!stream.read(reinterpret_cast<char*>(&subdata->fourcc), 4)) {
                    return false;
                }
                subdata->subdata_length = cur_header.length - 4;
                subdata->subdata = std::move(std::make_unique<uint8_t[]>(subdata->subdata_length));
                if(!stream.read(reinterpret_cast<char*>(subdata->subdata.get()), subdata->subdata_length)) {
                    return false;
                }
                cur_chunk->data = std::move(subdata);
                break;
            }
            case RiffChunkID::INFO:
            {
                auto subdata = std::make_unique<RiffSubChunk>();
                if(!stream.read(reinterpret_cast<char*>(&subdata->fourcc), 4)) {
                    return false;
                }
                subdata->subdata_length = cur_header.length - 4;
                subdata->subdata = std::move(std::make_unique<uint8_t[]>(subdata->subdata_length));
                if(!stream.read(reinterpret_cast<char*>(subdata->subdata.get()), subdata->subdata_length)) {
                    return false;
                }
                {
                    std::ostringstream err_ss{};
                    std::string hdr{"RIFF INFO Chunk."};
                    err_ss.write(hdr.c_str(), hdr.size());
                    std::string len{"Length: "};
                    len += std::to_string(cur_header.length);
                    err_ss.write(len.c_str(), len.size());
                    err_ss.write(reinterpret_cast<char*>(subdata->subdata.get()), subdata->subdata_length);
                    DebuggerPrintf(err_ss.str().c_str());
                }
                cur_chunk->data = std::move(subdata);
                break;
            }
            case RiffChunkID::LIST:
            {
                auto subdata = std::make_unique<RiffSubChunk>();
                if(!stream.read(reinterpret_cast<char*>(&subdata->fourcc), 4)) {
                    return false;
                }
                subdata->subdata_length = cur_header.length - 4;
                subdata->subdata = std::move(std::make_unique<uint8_t[]>(subdata->subdata_length));
                auto subdata_head = subdata->subdata.get();
                if(!stream.read(reinterpret_cast<char*>(subdata_head), subdata->subdata_length)) {
                    return false;
                }
                cur_chunk = std::move(ReadListChunk(stream));
                break;
            }
            default:
            {
                {
                    std::ostringstream err_ss{};
                    std::string hdr{ "Unknown RIFF Chunk ID: " };
                    hdr += cur_header.fourcc[0];
                    hdr += cur_header.fourcc[1];
                    hdr += cur_header.fourcc[2];
                    hdr += cur_header.fourcc[3];
                    err_ss.write(hdr.c_str(), hdr.size());
                    std::string len{ " Length: " };
                    len += std::to_string(cur_header.length);
                    err_ss.write(len.c_str(), len.size());
                    DebuggerPrintf(err_ss.str().c_str());
                }
                stream.seekp(cur_header.length, std::ios_base::cur);
                stream.seekg(cur_header.length, std::ios_base::cur);
                break;
            }
        }
        _chunks.push_back(std::move(cur_chunk));
    }
    _current_chunk = _chunks.begin();
    return true;
}

void Riff::ShowRiffChunkHeaders() noexcept {
#ifdef AUDIO_DEBUG
    std::ostringstream ss;
    ss << "Chunks:\n";
    for(auto& chunk : _chunks) {
        ss << "Chunk ID: ";
        ss << chunk->header.fourcc[0]
           << chunk->header.fourcc[1]
           << chunk->header.fourcc[2]
           << chunk->header.fourcc[3]
           << '\n';
        ss << "Length: " << chunk->header.length << '\n';
        if(chunk->data) {
            ss << "------------\n";
            ss << "SubChunk ID: ";
            ss << chunk->data->fourcc[0]
               << chunk->data->fourcc[1]
               << chunk->data->fourcc[2]
               << chunk->data->fourcc[3];
            ss << '\n';
            ss << "Length: " << chunk->data->subdata_length << '\n';
        }
        ss << "------------\n";
    }
    ss.flush();
    DebuggerPrintf(ss.str().c_str());
#endif
}

Riff::RiffChunk* Riff::GetNextChunk() noexcept {
    if(_current_chunk == _chunks.end()) {
        return nullptr;
    }
    auto chunk = (*_current_chunk).get();
    _current_chunk++;
    return chunk;
}

unsigned int Riff::Load(std::filesystem::path filename) noexcept {
    std::vector<unsigned char> buffer{};
    if(!FileUtils::ReadBufferFromFile(buffer, filename)) {
        return RIFF_ERROR_INVALID_ARGUMENT;
    }
    return Load(buffer);
}

unsigned int Riff::Load(const std::vector<unsigned char>& data) noexcept {
    std::vector<unsigned char> buffer = data;
    if(!ParseDataIntoChunks(buffer)) {
        return RIFF_ERROR_NOT_A_RIFF;
    }
    ShowRiffChunkHeaders();
    return RIFF_SUCCESS;
}

std::unique_ptr<Riff::RiffChunk> Riff::ReadListChunk(std::stringstream& stream) noexcept {
    if(!stream) {
        return false;
    }
    RiffHeader cur_header{};
    if(stream.read(reinterpret_cast<char*>(&cur_header), sizeof(cur_header))) {
        auto cur_chunk = std::make_unique<RiffChunk>();
        cur_chunk->header = cur_header;
        {
            auto subdata = std::make_unique<RiffSubChunk>();
            StringUtils::CopyFourCC(subdata->fourcc, cur_header.fourcc);
            uint32_t subdata_length = cur_header.length - 4;
            subdata->subdata = std::move(std::make_unique<uint8_t[]>(subdata_length));
            if(!stream.read(reinterpret_cast<char*>(subdata->subdata.get()), subdata_length)) {
                return false;
            }
            cur_chunk->data = std::move(subdata);
            return cur_chunk;
        }
    }
    return false;
}

} //End FileUtils