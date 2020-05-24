/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Audio.h
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

#ifndef AUDIO_H_
#define AUDIO_H_

#include <vector>
using namespace std;

#include <gmtl/gmtl.h>
using namespace gmtl;

namespace VirtualMuseumApp
{
    class Audio
    {
    public:
        static void CreateInstance();
        static void DestoryInstance();
        static Audio* const GetInstance();

        class SoundSource
        {
        public:
            virtual void SetSoundFile(const char* const sndFileName) = 0;
            virtual void SetPosition(const Vec3f position) = 0;

            virtual void Play(const bool repeat = false) = 0;
            virtual void Pause() = 0;
            virtual void Stop() = 0;

            enum Status
            {
                PLAYING,
                PAUSED,
                STOPPED
            };

            virtual Status GetStatus() = 0;

        protected:
            SoundSource() {}
            virtual ~SoundSource() {}

            virtual void Update() = 0;

            friend class Audio;
        };

        SoundSource* CreateSoundSource();
        void RemoveSoundSource(SoundSource*& soundSource);

        virtual void SetListenerPosition(const Vec3f position) = 0;
        virtual void SetListenerOrientation(const Vec3f lookAt, const Vec3f up) = 0;

        virtual void BeginUpdate() = 0;
        virtual void EndUpdate() = 0;

    protected:
        Audio() {}
        virtual ~Audio();

        static Audio* ms_pInstance;

        vector<SoundSource*> m_pSoundSource;
    };
}

#endif // AUDIO_H_
