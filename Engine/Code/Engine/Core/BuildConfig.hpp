#pragma once
#ifndef ABRAMS2019_BUILDCONFIG_HPP
    #define ABRAMS2019_BUILDCONFIG_HPP

    #define TRACK_MEMORY_BASIC (0)
    #define TRACK_MEMORY_VERBOSE (1)

    #define UNUSED(x) (void)(x)

    #ifdef _DEBUG
        #define TRACK_MEMORY TRACK_MEMORY_VERBOSE
        #define AUDIO_DEBUG
        #define RENDER_DEBUG
        #define PROFILE_BUILD
        #define UI_DEBUG
    #elif defined(FINAL_BUILD)
        #undef UI_DEBUG
        #undef AUDIO_DEBUG
        #undef RENDER_DEBUG
        #undef TRACK_MEMORY
        #undef PROFILE_BUILD
    #else
        #define TRACK_MEMORY TRACK_MEMORY_BASIC
        #define AUDIO_DEBUG
        #define RENDER_DEBUG
        #define PROFILE_BUILD
        #define UI_DEBUG
    #endif

    #define MAX_LOGS 3u

    #define TOKEN_PASTE_SIMPLE(x, y) x##y
    #define TOKEN_PASTE(x, y) TOKEN_PASTE_SIMPLE(x, y)
    #define TOKEN_STRINGIZE_SIMPLE(x) #x
    #define TOKEN_STRINGIZE(x) TOKEN_STRINGIZE_SIMPLE(x)

    #if !defined(__cplusplus)
        #error "This program only compiles in C++"
    #endif

    #if defined(__APPLE__) || defined(__MACH__)
        #if defined(__clang__)
            #define PLATFORM_CLANG
            #define PLATFORM_APPLE
        #elif defined(__GNUC__) || defined(__GNUG__)
            #define PLATFORM_GNUC
            #define PLATFORM_APPLE
        #endif
    #endif
    #if defined(__clang__)
        #define PLATFORM_CLANG
        #define PLATFORM_LINUX
    #elif defined(__GNUC__) || defined(__GNUG__)
        #define PLATFORM_GNUC
        #define PLATFORM_LINUX
    #endif

    #if defined(_WIN64) || defined(_WIN32)
        #ifndef PLATFORM_WINDOWS
            #define PLATFORM_WINDOWS
        #endif
    #endif

    #if defined(_WIN64)
        #ifdef PROFILE_BUILD
            #define MAX_PROFILE_HISTORY 0xFFull
            #define MAX_PROFILE_TREES 50ull
        #endif
    #elif defined(_WIN32)
        #ifdef PROFILE_BUILD
            #define MAX_PROFILE_HISTORY 0xFFu
            #define MAX_PROFILE_TREES 50u
        #endif
    #endif

#endif