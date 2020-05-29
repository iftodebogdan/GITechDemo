/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   SceneData.h
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

#ifndef SCENE_DATA_H_
#define SCENE_DATA_H_

#include <string>
using namespace std;

namespace VirtualMuseumApp
{
    class Texture;

    struct SceneData
    {
        string IntroText;

        struct RoomData
        {
            string HallSnd;
            string RoomName;
            unsigned int ExhibitCount;
            Texture* Exhibits[8];
            string ExhibitDesc[2];
        };

        RoomData Room[5];

        static const SceneData& GetSceneData() { return m_sSceneData; }

    private:
        static SceneData& m_sSceneData;
    };
}

#endif // SCENE_DATA_H_