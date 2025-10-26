/**
 * @file        ProfilerNVRHI.cpp
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

#include "stdafx.h"

#include "ProfilerNVRHI.h"
using namespace Synesthesia3D;

#if ENABLE_NVRHI

ProfilerNVRHI::ProfilerNVRHI()
{

}

ProfilerNVRHI::~ProfilerNVRHI()
{

}

void ProfilerNVRHI::PushProfileMarker(const char* const label, const bool issueGPUQuery)
{
#if ENABLE_PROFILE_MARKERS

#endif
}

void ProfilerNVRHI::PopProfileMarker()
{
#if ENABLE_PROFILE_MARKERS

#endif
}

void ProfilerNVRHI::IssueDisjointQueryBegin()
{
#if ENABLE_PROFILE_MARKERS

#endif
}

void ProfilerNVRHI::IssueDisjointQueryEnd()
{
#if ENABLE_PROFILE_MARKERS
    UpdateGPUProfileMarkerResults();
#endif
}

void ProfilerNVRHI::ReleaseGPUProfileMarkerResults()
{
#if ENABLE_PROFILE_MARKERS
    Profiler::ReleaseGPUProfileMarkerResults();
#endif
}

void ProfilerNVRHI::UpdateGPUProfileMarkerResults()
{
#if ENABLE_PROFILE_MARKERS
    // Cleanup old timestamps
    Profiler::UpdateGPUProfileMarkerResults();
#endif
}

GPUProfileMarkerResultNVRHI::GPUProfileMarkerResultNVRHI(const char* const label, DisjointQuery disjointQuery)
    : GPUProfileMarkerResult(label)
{
#if ENABLE_PROFILE_MARKERS

#endif
}

GPUProfileMarkerResultNVRHI::~GPUProfileMarkerResultNVRHI()
{

}

void GPUProfileMarkerResultNVRHI::IssueEndQuery()
{
#if ENABLE_PROFILE_MARKERS

#endif
}

#endif // ENABLE_NVRHI
