/**
 * @file        ProfilerNVRHI.h
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

#ifndef PROFILERNVRHI_H
#define PROFILERNVRHI_H

#include "ResourceData.h"

#if ENABLE_NVRHI

#include "Profiler.h"

namespace Synesthesia3D
{
    struct DisjointQuery
    {

    };

    class GPUProfileMarkerResultNVRHI : public GPUProfileMarkerResult
    {
    private:
        GPUProfileMarkerResultNVRHI(const char* const label, DisjointQuery disjointQuery);
        ~GPUProfileMarkerResultNVRHI();

        void IssueEndQuery();

        friend class ProfilerNVRHI;
    };

    class ProfilerNVRHI : public Profiler
    {
    public:
        void PushProfileMarker(const char* const label, const bool issueGPUQuery = false);
        void PopProfileMarker();

    private:
        ProfilerNVRHI();
        ~ProfilerNVRHI();

        void IssueDisjointQueryBegin();
        void IssueDisjointQueryEnd();

        void ReleaseGPUProfileMarkerResults();
        void UpdateGPUProfileMarkerResults();

        friend class RendererNVRHI;
    };
}

#endif // ENABLE_NVRHI

#endif // PROFILERNVRHI_H
