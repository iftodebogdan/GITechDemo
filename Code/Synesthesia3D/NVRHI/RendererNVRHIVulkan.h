/**
 * @file        RendererNVRHIVulkan.h
 *
 * @note        This file is part of the "Synesthesia3D" graphics engine
 *
 * @copyright   Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * @copyright
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * @copyright
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RENDERERNVRHIVULKAN_H
#define RENDERERNVRHIVULKAN_H

#include "ResourceData.h"

#if ENABLE_NVRHI_VULKAN

#include "RendererNVRHI.h"

namespace Synesthesia3D
{
    class RendererNVRHIVulkan : public RendererNVRHI
    {

    };
}

#else // ENABLE_NVRHI_VULKAN

#include "RendererNULL.h"

namespace Synesthesia3D
{
    typedef RendererNULL RendererNVRHIVulkan;
}

#endif // ENABLE_NVRHI_VULKAN

#endif // RENDERERNVRHIVULKAN_H
