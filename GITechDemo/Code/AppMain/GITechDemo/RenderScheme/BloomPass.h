/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   BloomPass.h
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

#ifndef BLOOM_PASS_H_
#define BLOOM_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
    class BloomPass : public RenderPass
    {
        IMPLEMENT_RENDER_PASS(BloomPass)

    private:
        void BloomBrightnessFilter();
        void BloomBlur();
        void BloomApply();
    };
}

#endif //BLOOM_PASS_H_
