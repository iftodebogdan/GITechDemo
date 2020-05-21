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
#include <AL/alext.h>

#include "Framework.h"
using namespace AppFramework;

#include "Audio.h"
using namespace VirtualMuseumApp;

#include "AppResources.h"

Audio* Audio::ms_pInstance = nullptr;

// The preferred audio device - OpenAL Soft supports binaural sound synthesis via HRTFs
#define PREFERRED_AUDIO_DEVICE "OpenAL Soft"

// Preferred OpanAL context attributes
#define PREFERRED_FREQUENCY         44100
#define PREFERRED_REFRESH           30
#define PREFERRED_SYNC              ALC_FALSE
#define PREFERRED_MONO_SOURCES      16
#define PREFERRED_STEREO_SOURCES    0

// Some strings for extensions and function names
#define ALC_HRTF_EXTENSION_NAME "ALC_SOFT_HRTF"
#define ALC_GET_STRINGI_SOFT_FUNC_NAME "alcGetStringiSOFT"
#define ALC_RESET_DEVICE_SOFT_FUNC_NAME "alcResetDeviceSOFT"
#define AL_EXT_STEREO_ANGLES_EXTENSION_NAME "AL_EXT_STEREO_ANGLES"

// HRTF config
#define HRTF_DATA_SET_RELATIVE_PATH "hrtfs"
static const unsigned int HRTFNameCount = 3;
static const char* const HRTFName[HRTFNameCount] =
{
    "IRC_1005",
    "IRC_1049",
    "IRC_1057"
};

static const unsigned int HRTFFreqCount = 2;
static const ALCint HRTFFreq[HRTFFreqCount] =
{
    44100,
    48000
};

#define HRTF_COUNT (HRTFNameCount * HRTFFreqCount)
#define PREFERRED_HRTF 1 // IRC_1049

// OpenAL Soft extension functions
static LPALCGETSTRINGISOFT alcGetStringiSOFT = nullptr;
static LPALCRESETDEVICESOFT alcResetDeviceSOFT = nullptr;

Audio::Audio()
    : m_pAudioDevice(nullptr)
    , m_pAudioContext(nullptr)
    , m_bHasHRTFExtension(false)
    , m_bHasStereoAnglesExtension(false)
{
    SetupALSoftLocalPath();
    CreateDevice();
    InitializeExtensions();
}

Audio::~Audio()
{
    DestroyDevice();
}

void Audio::SetupALSoftLocalPath()
{
    // This is the cleanest way I've found to set the path where OpenAL searches for HRTF data sets
    // Alternatives (that I'm aware of) are:
    //  - have the user set the path via alsoft-config.exe - cumbersome, nobody wants to deal with that
    //  - generate an alsoft.ini file in the exe's folder with the correct "hrtf-paths" property - kinda hacky
    //    (yes, generate, because relative paths did not seem to work so it has to be
    //     absolute and it can change between runs if the build folder is relocated)
    //  - put the HRTF data set files alongside the exe - I refuse
    //  - just use the built-in HRTF - 4Head

    char envVar[64];
    sprintf_s(envVar, "%s%s", "ALSOFT_LOCAL_PATH=", HRTF_DATA_SET_RELATIVE_PATH);
    _putenv(envVar);
}

void Audio::CreateDevice()
{
    Framework* const pFW = Framework::GetInstance();
    assert(pFW);
    if (!pFW)
        return;

    // Attempt to initialize preferred audio device, if available
    m_pAudioDevice = alcOpenDevice(PREFERRED_AUDIO_DEVICE);
    if (!m_pAudioDevice)
    {
        pFW->CreateMessageBox("Error", "Could not initialize preferred OpenAL device. Attempting fallback.");

        // Try to initialize the default device if:
        //   - enumeration extension is not available
        //   - preferred device is not found
        //   - preferred device could not be initialized
        m_pAudioDevice = alcOpenDevice(nullptr);
        if (!m_pAudioDevice)
        {
            pFW->CreateMessageBox("Error", "Could not initialize fallback OpenAL device. Sound will not function.");
            return;
        }
    }

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
        ALC_HRTF_SOFT, ALC_TRUE,    // ALC_HRTF_SOFT shouldn't be here normally, but OpenAL Soft bugs out in certain situations
        0                           // and alcResetDeviceSOFT() doesn't do anything - this means I can't enable it later.
    };                              // It doesn't seem to cause any issues even when using the default device, so I'll leave it
                                    // here as a last resort to _maybe_ initialize the built-in HRTF at least.

    // Create context
    m_pAudioContext = alcCreateContext(m_pAudioDevice, nullptr);// attrList);

    error = alcGetError(m_pAudioDevice);
    assert(m_pAudioContext && error == ALC_NO_ERROR);

    // If the context couldn't be created with the requested attributes
    // try to at least create one with the preferred output frequency
    if (!m_pAudioContext)
    {
        pFW->CreateMessageBox("Error", "Could not initialize OpenAL context with preferred attributes. Attempting fallback.");

        const ALCint attrList[] =
        {
            ALC_FREQUENCY, PREFERRED_FREQUENCY,
            0
        };

        m_pAudioContext = alcCreateContext(m_pAudioDevice, attrList);

        error = alcGetError(m_pAudioDevice);
        assert(m_pAudioContext && error == ALC_NO_ERROR);

        // If the context couldn't be created with the requested attributes
        // try a last resort attempt with the default attributes
        if (!m_pAudioContext)
        {
            pFW->CreateMessageBox("Error", "Could not initialize OpenAL context with fallback attributes. Attempting last resort.");

            m_pAudioContext = alcCreateContext(m_pAudioDevice, nullptr);

            error = alcGetError(m_pAudioDevice);
            assert(m_pAudioContext && error == ALC_NO_ERROR);

            if (!m_pAudioContext)
            {
                pFW->CreateMessageBox("Error", "Could not initialize OpenAL context with default attributes. Sound will not function.");
                DestroyDevice();
                return;
            }
        }
    }

    // If we successfully created a context, set it
    const ALCboolean makeCtxCurr = alcMakeContextCurrent(m_pAudioContext);

    error = alcGetError(m_pAudioDevice);
    assert(makeCtxCurr && error == ALC_NO_ERROR);

    if (!makeCtxCurr || error != ALC_NO_ERROR)
    {
        pFW->CreateMessageBox("Error", "Could not set OpenAL audio context. Sound will not function.");
        DestroyDevice();
        return;
    }
}

void Audio::InitializeExtensions()
{
    if (!m_pAudioDevice)
        return;

    Framework* const pFW = Framework::GetInstance();
    assert(pFW);
    if (!pFW)
        return;

    // Reset error state
    ALCenum error = alcGetError(m_pAudioDevice);
    assert(error == ALC_NO_ERROR);

    // Check for HRTF extension
    if (alcIsExtensionPresent(m_pAudioDevice, ALC_HRTF_EXTENSION_NAME))
    {
        alcGetStringiSOFT = (LPALCGETSTRINGISOFT)alcGetProcAddress((m_pAudioDevice), ALC_GET_STRINGI_SOFT_FUNC_NAME);
        error = alcGetError(m_pAudioDevice);
        assert(alcGetStringiSOFT && error == ALC_NO_ERROR);

        alcResetDeviceSOFT = (LPALCRESETDEVICESOFT)alcGetProcAddress((m_pAudioDevice), ALC_RESET_DEVICE_SOFT_FUNC_NAME);
        error = alcGetError(m_pAudioDevice);
        assert(alcResetDeviceSOFT && error == ALC_NO_ERROR);

        if (alcGetStringiSOFT && alcResetDeviceSOFT)
        {
            m_bHasHRTFExtension = true;

            ALCint hrtfCount = 0;
            alcGetIntegerv(m_pAudioDevice, ALC_NUM_HRTF_SPECIFIERS_SOFT, 1, &hrtfCount);

            error = alcGetError(m_pAudioDevice);
            assert(error == ALC_NO_ERROR);

            if (hrtfCount > 0 && error == ALC_NO_ERROR)
            {
                ALCint freq = 0;
                alcGetIntegerv(m_pAudioDevice, ALC_FREQUENCY, 1, &freq);

                error = alcGetError(m_pAudioDevice);
                assert(error == ALC_NO_ERROR);

                if (error != ALC_NO_ERROR || freq == 0)
                {
                    freq = PREFERRED_FREQUENCY;
                }

                char preferredHRTF[32];
                sprintf_s(preferredHRTF, "%s_%d", HRTFName[PREFERRED_HRTF], freq);

                ALCint selectedHRTF = 0;

                bool hrtfNamesExist = false;
                bool hrtfNameFound = false;
                for (ALCint idx = 0; idx < hrtfCount; idx++)
                {
                    const ALCchar* hrtfName = alcGetStringiSOFT(m_pAudioDevice, ALC_HRTF_SPECIFIER_SOFT, idx);

                    error = alcGetError(m_pAudioDevice);
                    assert(error == ALC_NO_ERROR);

                    if (hrtfName && error == ALC_NO_ERROR)
                    {
                        hrtfNamesExist = true;
                        if (strcmp(hrtfName, preferredHRTF) == 0)
                        {
                            hrtfNameFound = true;
                            selectedHRTF = idx;
                            break;
                        }
                    }
                }

                // This is a fallback in case alcGetStringiSOFT() returns null strings... again...
                // The two imported functions - alcGetStringiSOFT() and alcResetDeviceSOFT() - seem to not do anything (they
                // return 0 regardless of parameters and don't set any errors). Renaming the OpenAL Soft DLL to OpenAL32.dll
                // "fixes" this but at the cost of not being able to have both x86 and x64 DLLs in the same folder.
                // It possibly has something to do with how the "official" OpenAL32.dll from Creative Labs does the "routing"
                // to the OpenAL Soft DLL, but I can't keep looking into this considering I sort of have a deadline...
                if (!hrtfNamesExist && hrtfCount == HRTF_COUNT + 1) // +1 being the built-in HRTF... I hope...
                {
                    // I'm going to assume they are in the same order as they appear in alsoft-config.exe
                    selectedHRTF = PREFERRED_HRTF * HRTFFreqCount;

                    ALCint freq = 0;
                    alcGetIntegerv(m_pAudioDevice, ALC_FREQUENCY, 1, &freq);

                    error = alcGetError(m_pAudioDevice);
                    assert(error == ALC_NO_ERROR);

                    if (error != ALC_NO_ERROR || freq == 0)
                    {
                        freq = PREFERRED_FREQUENCY;
                    }

                    for (unsigned int i = 0; i < HRTFFreqCount; i++)
                    {
                        if (HRTFFreq[i] == freq)
                        {
                            selectedHRTF += (ALCint)i;
                            break;
                        }
                    }
                }

                ALCboolean reset = ALC_FALSE;
                if ((hrtfNamesExist && hrtfNameFound) || (!hrtfNamesExist && hrtfCount == HRTF_COUNT + 1))
                {
                    const ALCint attrList[] =
                    {
                        ALC_HRTF_SOFT, ALC_TRUE,
                        ALC_HRTF_ID_SOFT, selectedHRTF,
                        0
                    };

                    reset = alcResetDeviceSOFT(m_pAudioDevice, attrList);

                    error = alcGetError(m_pAudioDevice);
                    assert(error == ALC_NO_ERROR);

                    if (!reset || error != ALC_NO_ERROR)
                    {
                        pFW->CreateMessageBox("Error", "Could not set preferred HRTF. Attempting fallback to built-in HRTF.");
                    }
                }
                else
                {
                    pFW->CreateMessageBox("Error", "Could not find preferred HRTF data set. Attempting to fallback to built-in HRTF.");
                }

                if (!reset || error != ALC_NO_ERROR)
                {
                    const ALCint attrList[] =
                    {
                        ALC_HRTF_SOFT, ALC_TRUE,
                        0
                    };

                    reset = alcResetDeviceSOFT(m_pAudioDevice, attrList);

                    error = alcGetError(m_pAudioDevice);
                    assert(error == ALC_NO_ERROR);

                    if (!reset || error != ALC_NO_ERROR)
                    {
                        pFW->CreateMessageBox("Error", "Could not set built-in HRTF. Binaural sound synthesis will be disabled.");
                    }
                }
            }
            else
            {
                pFW->CreateMessageBox("Error", "No HRTFs were found. Binaural sound synthesis will be disabled.");
            }
        }
        else
        {
            char errorMsg[256];
            sprintf_s(errorMsg,
                "Could not retrieve the function pointers for %s and/or %s. HRTF extension will not be initialized and binaural sound synthesis will be disabled.",
                ALC_GET_STRINGI_SOFT_FUNC_NAME, ALC_RESET_DEVICE_SOFT_FUNC_NAME);

            pFW->CreateMessageBox("Error", errorMsg);
        }
    }
    else
    {
        pFW->CreateMessageBox("Error", "HRTF extension was not found. Binaural sound synthesis will be disabled.");
    }

    // Check for stereo source rotation extension
    if (alIsExtensionPresent(AL_EXT_STEREO_ANGLES_EXTENSION_NAME))
    {
        m_bHasStereoAnglesExtension = true;
    }
    else
    {
        pFW->CreateMessageBox("Error", "Stereo source rotation extension not found.");
    }
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
