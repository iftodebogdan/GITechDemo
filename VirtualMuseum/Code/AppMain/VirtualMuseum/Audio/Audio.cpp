/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Audio.cpp
 *      Author: Bogdan Iftode
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include <AL/alc.h>

#include "Audio.h"
using namespace VirtualMuseumApp;

#include "AppResources.h"

Audio* Audio::ms_pInstance = nullptr;

// The preferred audio device - OpenAL Soft supports binaural sound synthesis via HRTFs
#define PREFERRED_AUDIO_DEVICE "OpenAL Soft"

#define PREFERRED_FREQUENCY         44100
#define PREFERRED_REFRESH           30
#define PREFERRED_SYNC              ALC_FALSE
#define PREFERRED_MONO_SOURCES      16
#define PREFERRED_STEREO_SOURCES    0

Audio::Audio()
    : m_pAudioDevice(nullptr)
    , m_pAudioContext(nullptr)
{
    // Attempt to initialize preferred audio device, if available
    if (!CreateDevice(PREFERRED_AUDIO_DEVICE))
    {
        // TODO: create message box for error - route through Framework class
        // MessageBox(NULL, "body", "title", MB_OK);

        // Try to initialize the default device if:
        //   - enumeration extension is not available
        //   - preferred device is not found
        //   - preferred device could not be initialized
        if (!CreateDevice())
        {
            // If not even the default device could be initialized, then clean up and drop audio support entirely
            DestroyDevice();
            return;
        }
    }

    // TODO: init HRTF
}

Audio::~Audio()
{
    DestroyDevice();
}

const bool Audio::CreateDevice(const char* const deviceName)
{
    m_pAudioDevice = alcOpenDevice(deviceName);
    assert(m_pAudioDevice);

    if (m_pAudioDevice)
    {
        // Reset error state
        ALCenum error = alcGetError(m_pAudioDevice);
        assert(error == ALC_NO_ERROR);

        // Set up preferred context attributes
        const ALCint attrList[] =
        {
            ALC_FREQUENCY, PREFERRED_FREQUENCY,
            ALC_REFRESH, PREFERRED_REFRESH,
            ALC_SYNC, PREFERRED_SYNC,
            ALC_MONO_SOURCES, PREFERRED_MONO_SOURCES,
            ALC_STEREO_SOURCES, PREFERRED_STEREO_SOURCES,
            0
        };

        // Create context
        m_pAudioContext = alcCreateContext(m_pAudioDevice, attrList);
        error = alcGetError(m_pAudioDevice);
        assert(m_pAudioContext && error == ALC_NO_ERROR);

        // If the context couldn't be created with the requested attributes
        // try to at least create one with the preferred output frequency
        if (!m_pAudioContext && error == ALC_INVALID_VALUE)
        {
            const ALCint attrList[] =
            {
                ALC_FREQUENCY, PREFERRED_FREQUENCY,
                0
            };

            m_pAudioContext = alcCreateContext(m_pAudioDevice, attrList);
            error = alcGetError(m_pAudioDevice);
            assert(m_pAudioContext && error == ALC_NO_ERROR);
        }

        // If the context couldn't be created with the requested attributes
        // try a last resort attempt with the default attributes
        if (!m_pAudioContext && error == ALC_INVALID_VALUE)
        {
            m_pAudioContext = alcCreateContext(m_pAudioDevice, nullptr);
            error = alcGetError(m_pAudioDevice);
            assert(m_pAudioContext && error == ALC_NO_ERROR);
        }

        // If we successfully created a context, set it
        if (m_pAudioContext && error == ALC_NO_ERROR)
        {
            const ALCboolean makeCtxCurr = alcMakeContextCurrent(m_pAudioContext);
            error = alcGetError(m_pAudioDevice);
            assert(makeCtxCurr && error == ALC_NO_ERROR);
            return makeCtxCurr;
        }
    }

    return (m_pAudioDevice && m_pAudioContext);
}

void Audio::DestroyDevice()
{
    alcMakeContextCurrent(nullptr);

    if (m_pAudioContext)
    {
        alcDestroyContext(m_pAudioContext);
        m_pAudioContext = nullptr;
    }

    if (m_pAudioDevice)
    {
        alcCloseDevice(m_pAudioDevice);
        m_pAudioDevice = nullptr;
    }
}

void Audio::CreateInstance()
{
    assert(ms_pInstance == nullptr);

    if (ms_pInstance != nullptr)
        return;

    ms_pInstance = new Audio();
}

void Audio::DestoryInstance()
{
    assert(ms_pInstance != nullptr);

    if (ms_pInstance)
    {
        delete ms_pInstance;
        ms_pInstance = nullptr;
    }
}

Audio* const Audio::GetInstance()
{
    return ms_pInstance;
}
