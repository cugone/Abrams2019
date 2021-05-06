#pragma once

#include "Engine/Core/Riff.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Math/IntVector2.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace a2de {

    namespace FileUtils {

        namespace AviChunkID {
            constexpr const uint32_t HDRL = StringUtils::FourCC("hdrl");
            constexpr const uint32_t MOVI = StringUtils::FourCC("movi");
            constexpr const uint32_t AVIH = StringUtils::FourCC("avih");
            constexpr const uint32_t LIST = StringUtils::FourCC("LIST");
            constexpr const uint32_t INFO = StringUtils::FourCC("INFO");
            constexpr const uint32_t JUNK = StringUtils::FourCC("JUNK");
            [[nodiscard]] constexpr const bool IsValid(const char* id) noexcept;
        } // namespace AviChunkID

        class Avi {
        public:
            struct AviHeader {
                char fourcc[4]{};
                uint32_t length{};
            };

            struct AviHdrlChunk {
                uint32_t usPerFrame{};
                uint32_t maxBytesPerSecond{};
                uint32_t paddingGranularity{};
                uint32_t flags{};
                uint32_t totalFrames{};
                uint32_t initialFrames{};
                uint32_t streams{};
                uint32_t suggestedBufferSize{};
                uint32_t width{};
                uint32_t height{};
                uint32_t reserved[4]{};
            };

            struct AviMoviChunk {
                uint32_t length{};
                std::unique_ptr<uint8_t[]> data;
                AviMoviChunk() = default;
                ~AviMoviChunk() = default;
                AviMoviChunk(const AviMoviChunk& other) = delete;
                AviMoviChunk(AviMoviChunk&& other) = default;
                AviMoviChunk& operator=(const AviMoviChunk& other) = delete;
                AviMoviChunk& operator=(AviMoviChunk&& other) = default;
                AviMoviChunk(uint32_t length, std::unique_ptr<uint8_t[]> data)
                    : length(length)
                    , data(std::move(data)) {
                    /* DO NOTHING */
                }
            };
            struct AviSubChunk {
                char fourcc[4]{};
                std::unique_ptr<uint8_t[]> subdata{};
                uint32_t data_length{};
            };
            struct AviChunk {
                AviHeader header{};
                std::unique_ptr<AviSubChunk> data{};
            };

            static constexpr const unsigned int AVI_SUCCESS = 0;
            static constexpr const unsigned int AVI_ERROR_NOT_A_AVI = 1;
            static constexpr const unsigned int AVI_ERROR_BAD_FILE = 2;

            [[nodiscard]] unsigned int Load(std::filesystem::path filepath) noexcept;
            [[nodiscard]] const AviHdrlChunk& GetHdrlChunk() const noexcept;
            [[nodiscard]] const AviMoviChunk* GetFrame(std::size_t frame_idx) const noexcept;
            [[nodiscard]] const std::size_t GetFrameCount() const noexcept;
            [[nodiscard]] TimeUtils::FPSeconds GetLengthInSeconds() const noexcept;
            [[nodiscard]] TimeUtils::FPMicroseconds GetLengthInMicroSeconds() const noexcept;
            [[nodiscard]] IntVector2 GetFrameDimensions() const noexcept;

        protected:
        private:
            AviHdrlChunk _hdrl{};
            std::vector<AviMoviChunk> _frames{};
            decltype(_frames)::iterator _current_frame{};
        };

    } // namespace FileUtils
} // namespace a2de
