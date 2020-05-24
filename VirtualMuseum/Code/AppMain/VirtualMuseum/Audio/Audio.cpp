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

#include <map>

#include <sndfile.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include "Framework.h"
using namespace AppFramework;

#include "Audio.h"
using namespace VirtualMuseumApp;

#include "AppResources.h"

// OpenAL configuration ----------------------------------------------------------------

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
#define PREFERRED_HRTF 1 // IRC_1049

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

static const unsigned int HRTFCount = HRTFNameCount * HRTFFreqCount;

// -------------------------------------------------------------------------------------

// OpenAL Soft extension functions
static LPALCGETSTRINGISOFT alcGetStringiSOFT = nullptr;
static LPALCRESETDEVICESOFT alcResetDeviceSOFT = nullptr;

// Wrapper for OpenAL - abstracts and hides the library implementation from the rest of the project code
class AudioImplement : public Audio
{
public:
    AudioImplement();
    ~AudioImplement();

    static AudioImplement* const GetInstance() { return (AudioImplement*)Audio::GetInstance(); }

    void SetListenerPosition(const Vec3f position);
    void SetListenerOrientation(const Vec3f lookAt, const Vec3f up);

    void BeginUpdate();
    void EndUpdate();

    class SoundSourceImplement : public Audio::SoundSource
    {
    public:
        void SetSoundFile(const char* const sndFileName);
        void SetPosition(const Vec3f position);

        void Play(const bool repeat = false);
        void Pause();
        void Stop();

        Status GetStatus();

    protected:
        SoundSourceImplement();
        ~SoundSourceImplement();

        void Update();

        ALuint m_alSource;
        bool m_bRepeat;

        friend class Audio;
        friend class AudioImplement;
    };

protected:
    void SetupALSoftLocalPath();
    void CreateDevice();
    void InitializeExtensions();
    void DestroyDevice();

    ALCdevice* const GetAudioDevice() const { return m_pAudioDevice; }
    ALCcontext* const GetAudioContext() const { return m_pAudioContext; }

    const bool HasHRTFExtension() const { return m_bHasHRTFExtension; }

    ALuint GetOrLoadSound(const char* const fileName);

    ALCdevice* m_pAudioDevice;
    ALCcontext* m_pAudioContext;

    bool m_bHasHRTFExtension;

    typedef map<string, ALuint> ALBufferMap;
    ALBufferMap m_alBuffers;
};


AudioImplement::AudioImplement()
    : m_pAudioDevice(nullptr)
    , m_pAudioContext(nullptr)
    , m_bHasHRTFExtension(false)
{
    SetupALSoftLocalPath();
    CreateDevice();
    InitializeExtensions();
}

AudioImplement::~AudioImplement()
{
    DestroyDevice();
}

void AudioImplement::SetListenerPosition(const Vec3f position)
{
    alListenerfv(AL_POSITION, position.getData());

    ALCenum error = alcGetError(GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

void AudioImplement::SetListenerOrientation(const Vec3f lookAt, const Vec3f up)
{
    ALfloat orientation[6];
    memcpy(orientation, lookAt.getData(), sizeof(ALfloat) * 3);
    memcpy(orientation + 3, up.getData(), sizeof(ALfloat) * 3);
    alListenerfv(AL_ORIENTATION, orientation);

    ALCenum error = alcGetError(GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

void AudioImplement::BeginUpdate()
{
    alcSuspendContext(GetAudioContext());

    ALCenum error = alcGetError(GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

void AudioImplement::EndUpdate()
{
    for (unsigned int i = 0; i < m_pSoundSource.size(); i++)
    {
        if (m_pSoundSource[i])
        {
            ((SoundSourceImplement*)m_pSoundSource[i])->Update();
        }
    }

    alcProcessContext(GetAudioContext());

    ALCenum error = alcGetError(GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

void AudioImplement::SetupALSoftLocalPath()
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

void AudioImplement::CreateDevice()
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

void AudioImplement::InitializeExtensions()
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
                // I ended up hacking my way to a solution by modifying the name of the DLL referenced in the x64 .lib file
                // that was provided with the OpenAL Soft SDK. Now it's looking for OpenAL32.dll on x86 and OpenAL64.dll on x64.
                // This allowed me to have both DLLs in the same folder with the build executables AND have a functioning
                // HRTF extension - i.e. alcResetDeviceSOFT() and alcGetStringiSOFT() seem to work correctly now.
                if (!hrtfNamesExist && hrtfCount == HRTFCount + 1) // +1 being the built-in HRTF... I hope...
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
                if ((hrtfNamesExist && hrtfNameFound) || (!hrtfNamesExist && hrtfCount == HRTFCount + 1))
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
}

void AudioImplement::DestroyDevice()
{
    while (m_alBuffers.size())
    {
        alDeleteBuffers(1, &m_alBuffers.begin()->second);
        m_alBuffers.erase(m_alBuffers.begin());
    }

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

ALuint AudioImplement::GetOrLoadSound(const char* const fileName)
{
    assert(GetAudioContext() && GetAudioDevice());
    if (!GetAudioContext() || !GetAudioDevice())
        return AL_INVALID;

    ALBufferMap::iterator it = m_alBuffers.find(string(fileName));
    if (it != m_alBuffers.end())
    {
        return it->second;
    }

    Framework* const pFW = Framework::GetInstance();
    assert(pFW);
    if (!pFW)
        return AL_INVALID;

    SF_INFO sfInfo;
    SNDFILE* sndFile = sf_open(fileName, SFM_READ, &sfInfo);

    // Open sound file
    if (!sndFile)
    {
        char errorMsg[256];
        sprintf_s(errorMsg, "Could not open audio file %s", fileName);
        pFW->CreateMessageBox("Error", errorMsg);

        return AL_INVALID;
    }

    // Check sound file integrity
    if (sfInfo.frames < 1 || sfInfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfInfo.channels)
    {
        char errorMsg[256];
        sprintf_s(errorMsg, "Bas sample count in %s: %llu", fileName, sfInfo.frames);
        pFW->CreateMessageBox("Error", errorMsg);

        sf_close(sndFile);
        sndFile = nullptr;

        return AL_INVALID;
    }

    // Determine sound format
    ALenum sndFormat = AL_INVALID;
    switch (sfInfo.channels)
    {
    case 1:
        if (sfInfo.format & SF_FORMAT_PCM_S8 || sfInfo.format & SF_FORMAT_PCM_U8)
        {
            sndFormat = AL_FORMAT_MONO8;
        }
        else if (sfInfo.format & SF_FORMAT_PCM_16)
        {
            sndFormat = AL_FORMAT_MONO16;
        }
        break;

    case 2:
        if (sfInfo.format & SF_FORMAT_PCM_S8 || sfInfo.format & SF_FORMAT_PCM_U8)
        {
            sndFormat = AL_FORMAT_STEREO8;
        }
        else if (sfInfo.format & SF_FORMAT_PCM_16)
        {
            sndFormat = AL_FORMAT_STEREO16;
        }
        break;
    }

    if (sndFormat == AL_INVALID)
    {
        char errorMsg[256];
        sprintf_s(errorMsg, "Unsupported sound format for %s", fileName);
        pFW->CreateMessageBox("Error", errorMsg);

        sf_close(sndFile);
        sndFile = nullptr;

        return AL_INVALID;
    }

    short* sndBuffer = (short* const)malloc((size_t)(sfInfo.frames * sfInfo.channels) * sizeof(short));

    // Decode audio file to buffer
    sf_count_t frameCount = sf_readf_short(sndFile, sndBuffer, sfInfo.frames);
    if (frameCount < 1)
    {
        char errorMsg[256];
        sprintf_s(errorMsg, "Failed to read %llu samples from %s", frameCount, fileName);
        pFW->CreateMessageBox("Error", errorMsg);

        free(sndBuffer);
        sndBuffer = nullptr;

        sf_close(sndFile);
        sndFile = nullptr;

        return AL_INVALID;
    }

    const ALsizei byteCount = (ALsizei)(frameCount * sfInfo.channels) * (ALsizei)sizeof(short);

    // Create the OpenAL buffer
    ALuint alBuffer = AL_INVALID;
    alGenBuffers(1, &alBuffer);
    alBufferData(alBuffer, sndFormat, sndBuffer, byteCount, sfInfo.samplerate);

    free(sndBuffer);
    sndBuffer = nullptr;

    sf_close(sndFile);
    sndFile = nullptr;

    // Check for errors
    ALCenum error = alcGetError(GetAudioDevice());
    assert(error == ALC_NO_ERROR);

    if (error != AL_NO_ERROR)
    {
        char errorMsg[256];
        sprintf_s(errorMsg, "Could not create OpenAL buffer for %s", fileName);
        pFW->CreateMessageBox("Error", errorMsg);

        if (alBuffer && alIsBuffer(alBuffer))
        {
            alDeleteBuffers(1, &alBuffer);
            alBuffer = AL_INVALID;
        }

        return AL_INVALID;
    }

    m_alBuffers.insert(make_pair(string(fileName), alBuffer));

    return alBuffer;
}

Audio::SoundSource* Audio::CreateSoundSource()
{
    SoundSource* source = new AudioImplement::SoundSourceImplement();
    m_pSoundSource.push_back(source);
    return source;
}

void Audio::RemoveSoundSource(SoundSource*& soundSource)
{
    m_pSoundSource.erase(std::remove(m_pSoundSource.begin(), m_pSoundSource.end(), soundSource), m_pSoundSource.end());
    delete soundSource;
    soundSource = nullptr;
}

AudioImplement::SoundSourceImplement::SoundSourceImplement()
    : m_alSource(AL_INVALID)
    , m_bRepeat(false)
{
    assert(AudioImplement::GetInstance());
    if (!AudioImplement::GetInstance())
        return;

    alGenSources(1, &m_alSource);

    ALCenum error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
    assert(error == ALC_NO_ERROR);

    if (m_alSource == AL_INVALID)
        return;

    alSourcei(m_alSource, AL_SOURCE_RELATIVE, AL_FALSE);

    error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

AudioImplement::SoundSourceImplement::~SoundSourceImplement()
{
    assert(AudioImplement::GetInstance() && m_alSource != AL_INVALID);
    if (!AudioImplement::GetInstance() || m_alSource == AL_INVALID)
        return;

    alDeleteSources(1, &m_alSource);
    m_alSource = AL_INVALID;

    ALCenum error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

void AudioImplement::SoundSourceImplement::SetSoundFile(const char* const sndFileName)
{
    assert(AudioImplement::GetInstance() && m_alSource != AL_INVALID);
    if (!AudioImplement::GetInstance() || m_alSource == AL_INVALID)
        return;

    alSourcei(m_alSource, AL_BUFFER, AudioImplement::GetInstance()->GetOrLoadSound(sndFileName));

    ALCenum error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

void AudioImplement::SoundSourceImplement::SetPosition(const Vec3f position)
{
    assert(AudioImplement::GetInstance() && m_alSource != AL_INVALID);
    if (!AudioImplement::GetInstance() || m_alSource == AL_INVALID)
        return;

    alSource3f(m_alSource, AL_POSITION, position[0], position[1], position[2]);

    ALCenum error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

void AudioImplement::SoundSourceImplement::Play(const bool repeat /* = false */)
{
    assert(AudioImplement::GetInstance() && m_alSource != AL_INVALID);
    if (!AudioImplement::GetInstance() || m_alSource == AL_INVALID)
        return;

    alSourcePlay(m_alSource);
    m_bRepeat = repeat;

    ALCenum error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

void AudioImplement::SoundSourceImplement::Pause()
{
    assert(AudioImplement::GetInstance() && m_alSource != AL_INVALID);
    if (!AudioImplement::GetInstance() || m_alSource == AL_INVALID)
        return;

    alSourcePause(m_alSource);

    ALCenum error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

void AudioImplement::SoundSourceImplement::Stop()
{
    assert(AudioImplement::GetInstance() && m_alSource != AL_INVALID);
    if (!AudioImplement::GetInstance() || m_alSource == AL_INVALID)
        return;

    alSourceStop(m_alSource);
    m_bRepeat = false;

    ALCenum error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
    assert(error == ALC_NO_ERROR);
}

Audio::SoundSource::Status AudioImplement::SoundSourceImplement::GetStatus()
{
    ALenum sourceState;
    alGetSourcei(m_alSource, AL_SOURCE_STATE, &sourceState);

    ALCenum error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
    assert(error == ALC_NO_ERROR);

    if (alGetError() == AL_NO_ERROR)
    {
        switch (sourceState)
        {
        case AL_PLAYING:
            return PLAYING;
        case AL_PAUSED:
            return PAUSED;
        case AL_STOPPED:
        case AL_INITIAL:
            return STOPPED;
        default:
            assert(0);
        }
    }

    return STOPPED;
}

void AudioImplement::SoundSourceImplement::Update()
{
    assert(AudioImplement::GetInstance() && m_alSource != AL_INVALID);
    if (!AudioImplement::GetInstance() || m_alSource == AL_INVALID)
        return;

    if (m_bRepeat)
    {
        ALenum sourceState;
        alGetSourcei(m_alSource, AL_SOURCE_STATE, &sourceState);

        ALCenum error = alcGetError(AudioImplement::GetInstance()->GetAudioDevice());
        assert(error == ALC_NO_ERROR);

        if (alGetError() == AL_NO_ERROR && sourceState == AL_STOPPED)
        {
            Play(m_bRepeat);
        }
    }
}

Audio* Audio::ms_pInstance = nullptr;

void Audio::CreateInstance()
{
    assert(ms_pInstance == nullptr);

    if (ms_pInstance != nullptr)
        return;

    ms_pInstance = new AudioImplement();
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

Audio::~Audio()
{
    while (m_pSoundSource.size())
    {
        delete m_pSoundSource.back();
        m_pSoundSource.pop_back();
    }
}