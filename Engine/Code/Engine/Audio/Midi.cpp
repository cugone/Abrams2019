#include "Engine/Audio/Midi.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include <fstream>
#include <sstream>
#include <string>

Midi::Midi(const std::filesystem::path& filepath) {
    if(!Parse(filepath)) {
        ERROR_AND_DIE("Unable to parse MIDI file.");
    }
}

Midi::Midi(std::istream& inputStream) {
    if(!Parse(inputStream)) {
        ERROR_AND_DIE("Unable to parse MIDI file.");
    }
}

bool Midi::Parse(const std::filesystem::path& filepath) {
    std::ifstream ifs;
    ifs.open(filepath, std::fstream::in | std::ios::binary);
    if(!ifs.is_open()) {
        ERROR_RECOVERABLE("Unable to open MIDI file.");
        return false;
    }

    return Parse(ifs);
}

bool Midi::Parse(std::istream& inputStream) {
    auto old_fmt = inputStream.flags();
    inputStream.flags(inputStream.flags() | std::ios::binary);

    auto ReadString = [&inputStream](uint32_t length) {
        std::string s;
        for(uint32_t i = 0; i < length; ++i) {
            s += static_cast<uint8_t>(inputStream.get());
        }
        return s;
    };

    auto ReadByte = [&inputStream]() {
        uint8_t byte = 0;
        inputStream.read(reinterpret_cast<char*>(&byte), sizeof(byte));
        return byte;
    };

    auto ReadWord = [&inputStream]() {
        uint16_t word = 0;
        uint8_t byte1 = 0;
        uint8_t byte2 = 0;
        inputStream.read(reinterpret_cast<char*>(&byte1), sizeof(byte1));
        inputStream.read(reinterpret_cast<char*>(&byte2), sizeof(byte2));
        word = (byte1 << 8) | byte2;
        return word;
    };

    auto Read3Byte = [&inputStream]() {
        uint32_t word = 0;
        uint8_t byte1 = 0;
        uint8_t byte2 = 0;
        uint8_t byte3 = 0;
        inputStream.read(reinterpret_cast<char*>(&byte1), sizeof(byte1));
        inputStream.read(reinterpret_cast<char*>(&byte2), sizeof(byte2));
        inputStream.read(reinterpret_cast<char*>(&byte3), sizeof(byte3));
        word = (byte1 << 16) | (byte2 << 8) | byte3;
        return word;
    };

    auto ReadValue = [&inputStream]() {
        uint32_t value = 0;
        uint8_t byte = 0;

        inputStream.read(reinterpret_cast<char*>(&byte), sizeof(byte));

        value = byte;
        if(value & 0x80) {
            value &= 0x7F;

            do {
                inputStream.read(reinterpret_cast<char*>(&byte), sizeof(byte));
                value = (value << 7) | (byte & 0x7F);
            } while(byte & 0x80);
        }
        return value;
    };

    uint32_t n32 = 0;
    uint16_t n16 = 0;

    struct MidiHeader {
        uint32_t fileId = 0;
        uint32_t length = 0;
        uint16_t format = 0;
        uint16_t chunkCount = 0;
        uint16_t division = 0;
    };
    inputStream.read(reinterpret_cast<char*>(&n32), sizeof(n32));
    uint32_t fileId = FileUtils::EndianSwap(n32);

    if(fileId != StringUtils::FourCC("MThd")) {
        DebuggerPrintf("Not a MIDI file.");
        return false;
    }

    inputStream.read(reinterpret_cast<char*>(&n32), sizeof(n32));
    uint32_t headerLength = FileUtils::EndianSwap(n32);
    inputStream.read(reinterpret_cast<char*>(&n16), sizeof(n16));
    uint16_t format = FileUtils::EndianSwap(n16);
    inputStream.read(reinterpret_cast<char*>(&n16), sizeof(n16));
    uint16_t trackChunkCount = FileUtils::EndianSwap(n16);
    inputStream.read(reinterpret_cast<char*>(&n16), sizeof(n16));
    uint16_t division = FileUtils::EndianSwap(n16);

    MidiHeader header;
    header.fileId = fileId;
    header.length = headerLength;
    header.format = format;
    header.chunkCount = trackChunkCount;
    header.division = division;

    for(uint16_t chunk = 0; chunk < trackChunkCount; ++chunk) {
        DebuggerPrintf("===== TRACK %d\n", chunk);
        
        inputStream.read(reinterpret_cast<char*>(&n32), sizeof(n32));
        uint32_t trackId = FileUtils::EndianSwap(n32);
        if(trackId != StringUtils::FourCC("MTrk")) {
            DebuggerPrintf("Not a MIDI track.");
            return false;
        }
        inputStream.read(reinterpret_cast<char*>(&n32), sizeof(n32));
        uint32_t trackLength = FileUtils::EndianSwap(n32);

        DebuggerPrintf("Length: %d\n", trackLength);

        bool endOfTrack = false;
        uint8_t prevStatus = 0;

        tracks.push_back(MidiTrack());

        while(inputStream && !endOfTrack) {
            uint32_t statusTimeDelta = 0;
            uint8_t status = 0;

            statusTimeDelta = ReadValue();
            status = ReadByte();
            if(status < 0x80) {
                status = prevStatus;
                inputStream.seekg(-1, std::ios_base::cur);
            }

            const auto eventType = static_cast<Midi::EventName>(status & 0xF0);
            switch(eventType) {
            case Midi::EventName::VoiceNoteOff: {
                prevStatus = status;
                uint8_t channel = status & 0x0F;
                uint8_t noteId = ReadByte();
                uint8_t noteVelocity = ReadByte();

                channels[channel].tracks.push_back(&tracks[chunk]);
                tracks[chunk].events.push_back({MidiEvent::Type::NoteOff, noteId, noteVelocity, statusTimeDelta});
                break;
            }
            case Midi::EventName::VoiceNoteOn: {
                prevStatus = status;
                uint8_t channel = status & 0x0F;
                uint8_t noteId = ReadByte();
                uint8_t noteVelocity = ReadByte();
                channels[channel].tracks.push_back(&tracks[chunk]);
                if(noteVelocity == 0) {
                    tracks[chunk].events.push_back({MidiEvent::Type::NoteOff, noteId, noteVelocity, statusTimeDelta});
                } else {
                    tracks[chunk].events.push_back({MidiEvent::Type::NoteOn, noteId, noteVelocity, statusTimeDelta});
                }
                break;
            }
            case Midi::EventName::VoiceAftertouch: {
                prevStatus = status;
                uint8_t channel = status & 0x0F;
                uint8_t noteId = ReadByte();
                (void)noteId;
                uint8_t noteVelocity = ReadByte();
                (void)noteVelocity;
                tracks[chunk].events.push_back({MidiEvent::Type::Other});
                channels[channel].tracks.push_back(&tracks[chunk]);
                break;
            }
            case Midi::EventName::VoiceControlChange: {
                prevStatus = status;
                uint8_t channel = status & 0x0F;
                uint8_t noteId = ReadByte();
                (void)noteId;
                uint8_t noteVelocity = ReadByte();
                (void)noteVelocity;
                tracks[chunk].events.push_back({MidiEvent::Type::Other});
                channels[channel].tracks.push_back(&tracks[chunk]);
                break;
            }
            case Midi::EventName::VoiceProgramChange: {
                prevStatus = status;
                uint8_t channel = status & 0x0F;
                uint8_t programId = ReadByte();
                (void)programId;
                tracks[chunk].events.push_back({MidiEvent::Type::Other});
                channels[channel].tracks.push_back(&tracks[chunk]);
                break;
            }
            case Midi::EventName::VoiceChannelPressure: {
                prevStatus = status;
                uint8_t channel = status & 0x0F;
                uint8_t channelPressure = ReadByte();
                (void)channelPressure;
                tracks[chunk].events.push_back({MidiEvent::Type::Other});
                channels[channel].tracks.push_back(&tracks[chunk]);
                break;
            }
            case Midi::EventName::VoicePitchBend: {
                prevStatus = status;
                uint8_t channel = status & 0x0F;
                uint8_t ls7bits = ReadByte();
                (void)ls7bits;
                uint8_t ms7bits = ReadByte();
                (void)ms7bits;
                tracks[chunk].events.push_back({MidiEvent::Type::Other});
                channels[channel].tracks.push_back(&tracks[chunk]);
                break;
            }
            case Midi::EventName::SystemExclusive: {
                if(status == 0xF0) {
                    const auto s = ReadString(ReadValue());
                    DebuggerPrintf("System Exclusive Begin: %s\n", s.c_str());
                }
                if(status == 0xF7) {
                    const auto s = ReadString(ReadValue());
                    DebuggerPrintf("System Exclusive End: %s\n", s.c_str());
                }
                if(status == 0xFF) {
                    Midi::MetaEventName type = static_cast<Midi::MetaEventName>(ReadByte());
                    switch(type) {
                    case Midi::MetaEventName::Sequence: {
                        uint16_t sequence = ReadWord();
                        DebuggerPrintf("Sequence Number: %d\n", sequence);
                        break;
                    }
                    case Midi::MetaEventName::Text: {
                        const auto length = ReadByte();
                        const auto text = ReadString(length);
                        DebuggerPrintf("Text: %s\n", text.c_str());
                        break;
                    }
                    case Midi::MetaEventName::Copyright: {
                        const auto length = ReadByte();
                        const auto copyright = ReadString(length);
                        tracks[chunk].copyright = copyright;
                        DebuggerPrintf("Copyright: %s\n", copyright.c_str());
                        break;
                    }
                    case Midi::MetaEventName::TrackName: {
                        const auto length = ReadByte();
                        const auto name = ReadString(length);
                        tracks[chunk].name = name;
                        DebuggerPrintf("Track Name: %s\n", name.c_str());
                        break;
                    }
                    case Midi::MetaEventName::InstrumentName: {
                        const auto length = ReadByte();
                        const auto name = ReadString(length);
                        tracks[chunk].instrument = name;
                        DebuggerPrintf("Instrument Name: %s\n", name.c_str());
                        break;
                    }
                    case Midi::MetaEventName::Lyrics: {
                        const auto length = ReadByte();
                        const auto lyrics = ReadString(length);
                        DebuggerPrintf("Lyrics: %s\n", lyrics.c_str());
                        break;
                    }
                    case Midi::MetaEventName::Marker: {
                        const auto length = ReadByte();
                        const auto marker = ReadString(length);
                        DebuggerPrintf("Marker: %s\n", marker.c_str());
                        break;
                    }
                    case Midi::MetaEventName::CuePoint: {
                        const auto length = ReadByte();
                        const auto cuePoint = ReadString(length);
                        DebuggerPrintf("Cue: %s\n", cuePoint.c_str());
                        break;
                    }
                    case Midi::MetaEventName::ChannelPrefix: {
                        const auto length = ReadByte();
                        uint8_t channelId = 0;
                        inputStream.read(reinterpret_cast<char*>(&channelId), sizeof(channelId));
                        break;
                    }
                    case Midi::MetaEventName::EndOfTrack: {
                        const auto length = ReadByte();
                        endOfTrack = true;
                        break;
                    }
                    case Midi::MetaEventName::SetTempo: {
                        const auto length = ReadByte();
                        const auto tempo = std::chrono::microseconds{Read3Byte()};
                        const auto bpm = std::chrono::microseconds{std::chrono::minutes{1}} / tempo;
                        DebuggerPrintf("Tempo: %d, BPM: %d\n", tempo.count(), bpm);
                        break;
                    }
                    case Midi::MetaEventName::SMPTEOffset: {
                        const auto length = ReadByte();
                        uint8_t hour = ReadByte();
                        uint8_t fps_bits = (hour & 0x60) >> 5;
                        hour = hour & 0x1F;
                        float fps = fps_bits == 0b00 ? 24.0f : fps_bits == 0b01 ? 25.0f : fps_bits == 0b10 ? 29.97f : fps_bits == 0b11 ? 30.0f : 0.0f;
                        const auto minute = ReadByte();
                        const auto second = ReadByte();
                        const auto frame = ReadByte();
                        const auto subframe = ReadByte();
                        DebuggerPrintf("Offset: FR: %.3f H: %d M: %d S: %d F: %d SubF: %d\n", fps, hour, minute, second, frame, subframe);
                        break;
                    }
                    case Midi::MetaEventName::TimeSignature: {
                        const auto length = ReadValue();
                        const auto numerator = ReadByte();
                        const auto denominator = 2 << ReadByte();
                        const auto clocksPerClick = ReadByte();
                        const auto count32perBeat = ReadByte();
                        DebuggerPrintf("Time: %d/%d\n", numerator, denominator);
                        DebuggerPrintf("Clocks: %d\n", clocksPerClick);
                        DebuggerPrintf("32nds per Beat: %d\n", count32perBeat);
                        break;
                    }
                    case Midi::MetaEventName::KeySignature: {
                        const auto length = ReadByte();
                        const auto key_sig = ReadByte();
                        const auto scale = ReadByte();
                        std::ostringstream ss;
                        const auto sig = [key_sig]()->std::string {
                            switch(key_sig) {
                            case -7: return "Cb";
                            case -6: return "Gb";
                            case -5: return "Db";
                            case -4: return "Ab";
                            case -3: return "Eb";
                            case -2: return "Bb";
                            case -1: return "F";
                            case 0: return "C";
                            case 1: return "G";
                            case 2: return "D";
                            case 3: return "A";
                            case 4: return "E";
                            case 5: return "B";
                            case 6: return "F#";
                            case 7: return "C#";
                            default: return "C";
                            }
                        }();
                        ss << "Key: " << sig << (scale ? " Minor" : " Major") << '\n';
                        DebuggerPrintf(ss.str().c_str());
                        break;
                    }
                    case Midi::MetaEventName::SequencerSpecific: {
                        const auto length = ReadByte();
                        const auto msg = ReadString(length);
                        DebuggerPrintf("Sequencer Specific: %s\n", msg.c_str());
                        break;
                    }
                    default: {
                        const auto length = ReadByte();
                        inputStream.seekg(length, std::ios_base::cur);
                        DebuggerPrintf("Unknown System Exclusive Message\nLength: %d\n", length);
                        break;
                    }
                    }
                }
                break;
            }
            default: {
                DebuggerPrintf("Unrecognized Midi Event.\n");
            }
            }
        }
    }

    inputStream.flags(old_fmt);
    return true;
}
