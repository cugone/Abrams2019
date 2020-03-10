#pragma once

#include <array>
#include <filesystem>
#include <istream>
#include <string>
#include <vector>

struct MidiNote {
    uint8_t key = 0;
    uint8_t velocity = 0;
    uint32_t startTime = 0;
    uint32_t duration = 0;
};

struct MidiEvent {
    enum class Type {
        NoteOff,
        NoteOn,
        Other
    };
    Type event;
    uint8_t key = 0;
    uint8_t velocity = 0;
    uint32_t wallTick = 0;
    uint32_t deltaTick = 0;
};

struct MidiTrack {
    std::string copyright;
    std::string name;
    std::string instrument;
    std::vector<MidiEvent> events;
    std::vector<MidiNote> notes;
    uint8_t maxNote = 64;
    uint8_t minNote = 64;
};

struct MidiChannel {
    std::vector<MidiTrack*> tracks;
};

class Midi {
public:
    std::vector<MidiTrack> tracks;
    std::array<MidiChannel, 16> channels;

    enum class EventName : uint8_t {
        VoiceNoteOff = 0x80,
        VoiceNoteOn = 0x90,
        VoiceAftertouch = 0xA0,
        VoiceControlChange = 0xB0,
        VoiceProgramChange = 0xC0,
        VoiceChannelPressure = 0xD0,
        VoicePitchBend = 0xE0,
        SystemExclusive = 0xF0,
    };

    enum class MetaEventName : uint8_t {
        Sequence = 0x00,
        Text = 0x01,
        Copyright = 0x02,
        TrackName = 0x03,
        InstrumentName = 0x04,
        Lyrics = 0x05,
        Marker = 0x06,
        CuePoint = 0x07,
        ChannelPrefix = 0x20,
        EndOfTrack = 0x2F,
        SetTempo = 0x51,
        SMPTEOffset = 0x54,
        TimeSignature = 0x58,
        KeySignature = 0x59,
        SequencerSpecific = 0x7F,
    };

    Midi() = default;
    explicit Midi(const std::filesystem::path& filepath);
    explicit Midi(std::istream& inputStream);
    bool Parse(const std::filesystem::path& filepath);
    bool Parse(std::istream& inputStream);

protected:
private:
};
