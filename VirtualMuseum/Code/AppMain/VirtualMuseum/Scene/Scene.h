/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Scene.h
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

#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
using namespace std;

namespace VirtualMuseumApp
{
    class Scene
    {
    public:
        virtual ~Scene();

        class Actor
        {
        public:
            Actor();
            virtual ~Actor() {}

            enum DrawMode
            {
                DEPTH_ONLY,
                DEPTH_ONLY_ALPHA_TEST,
                SHADOW,
                COLOR
            };

            virtual void Update(const float deltaTime) = 0;
            virtual void Draw(DrawMode drawMode, const unsigned int cascade = ~0u) = 0;

            void SetPosition(Vec3f pos) { m_vPosition = pos; }
            void SetOrientation(float angle) { m_fOrientation = angle; }

        protected:
            Vec3f m_vPosition;
            float m_fOrientation;
        };

        void SetupScene();
        void Update(const float deltaTime);
        void Draw(Actor::DrawMode drawMode, unsigned int cascade = ~0u);

    protected:
        vector<Actor*> m_pActors;
    };
}

#endif // SCENE_H_
