#include "Engine/Audio/Wav.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include <sstream>

namespace a2de {
    namespace FileUtils {

        namespace WavChunkID {
            constexpr const bool IsValid(const char* id) noexcept {
                return (StringUtils::FourCC(id) == WavChunkID::FMT
                    || StringUtils::FourCC(id) == WavChunkID::FACT
                    || StringUtils::FourCC(id) == WavChunkID::DATA);
            }
        } // namespace WavChunkID

        unsigned int Wav::Load(std::filesystem::path filepath) noexcept {
            Riff riff_data{};
            if(riff_data.Load(filepath) != Riff::RIFF_SUCCESS) {
                return WAV_ERROR_NOT_A_WAV;
            }

            if(const auto* next_chunk = riff_data.GetNextChunk()) {
                if(!next_chunk->data) {
                    return WAV_SUCCESS; //Successfully read an empty file!
                }
                if(const auto wavfcc = StringUtils::FourCC(next_chunk->data->fourcc); wavfcc != RiffChunkID::WAVE) {
                    return WAV_ERROR_NOT_A_WAV;
                }

                std::stringstream ss(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
                ss.write(reinterpret_cast<const char*>(next_chunk->data->subdata.get()), next_chunk->header.length - 4);
                ss.clear();
                ss.seekp(0);
                ss.seekg(0);
                WavHeader cur_header{};
                while(ss.read(reinterpret_cast<char*>(&cur_header), sizeof(cur_header))) {
                    switch(StringUtils::FourCC(cur_header.fourcc)) {
                    case WavChunkID::FMT: {
                        if(!ss.read(reinterpret_cast<char*>(&_fmt), cur_header.length)) {
                            return WAV_ERROR_BAD_FILE;
                        }
                        break;
                    }
                    case WavChunkID::DATA: {
                        _data.length = cur_header.length;
                        _data.data = std::move(std::make_unique<uint8_t[]>(_data.length));
                        if(!ss.read(reinterpret_cast<char*>(_data.data.get()), _data.length)) {
                            return WAV_ERROR_BAD_FILE;
                        }
                        break;
                    }
                    case WavChunkID::FACT: {
                        if(!ss.read(reinterpret_cast<char*>(&_fact), cur_header.length)) {
                            return WAV_ERROR_BAD_FILE;
                        }
                        break;
                    }
                    default: {
                        {
                            DebuggerPrintf("Unknown WAV Chunk ID: %c%c%c%c Length: %u\n", cur_header.fourcc[0], cur_header.fourcc[1], cur_header.fourcc[2], cur_header.fourcc[3], cur_header.length);
                        }
                        ss.seekp(cur_header.length, std::ios_base::cur);
                        ss.seekg(cur_header.length, std::ios_base::cur);
                    }
                    }
                }
            }
            return WAV_SUCCESS;
        }

        unsigned char* Wav::GetFormatAsBuffer() noexcept {
            return reinterpret_cast<unsigned char*>(&_fmt);
        }

        unsigned char* Wav::GetDataBuffer() const noexcept {
            return _data.data.get();
        }

        uint32_t Wav::GetDataBufferSize() const noexcept {
            return _data.length;
        }

        const Wav::WavFormatChunk& Wav::GetFormatChunk() const noexcept {
            return _fmt;
        }

        const Wav::WavDataChunk& Wav::GetDataChunk() const noexcept {
            return _data;
        }

    } // namespace FileUtils
} // namespace a2de
