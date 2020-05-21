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

struct ALCdevice;
struct ALCcontext;

namespace VirtualMuseumApp
{
    class Audio
    {
    public:
        static void CreateInstance();
        static void DestoryInstance();
        static Audio* const GetInstance();

    protected:
        Audio();
        ~Audio();

        void SetupALSoftLocalPath();
        void CreateDevice();
        void InitializeExtensions();
        void DestroyDevice();

        ALCdevice* m_pAudioDevice;
        ALCcontext* m_pAudioContext;

        bool m_bHasHRTFExtension;
        bool m_bHasStereoAnglesExtension;

        static Audio* ms_pInstance;
    };
}

#endif // AUDIO_H_