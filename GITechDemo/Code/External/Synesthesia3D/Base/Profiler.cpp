/**
 * @file        Profiler.cpp
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

#include "Profiler.h"
#include "Renderer.h"
using namespace Synesthesia3D;

#include "Utility/Mutex.h"

int Profiler::ms_nProfileMarkerCounter = 0;

// A mutex to guarantee thread-safety for profile marker operations
MUTEX gProfileMarkerMutex;

Profiler::Profiler()
{
    MUTEX_INIT(gProfileMarkerMutex);
}

Profiler::~Profiler()
{
    MUTEX_DESTROY(gProfileMarkerMutex);

#if ENABLE_PROFILE_MARKERS
    assert(ms_nProfileMarkerCounter == 0);
#endif
}

void Profiler::PushProfileMarker(const char* const label, const bool issueGPUQuery)
{
#if ENABLE_PROFILE_MARKERS
    MUTEX_LOCK(gProfileMarkerMutex);
    ms_nProfileMarkerCounter++;
    MUTEX_UNLOCK(gProfileMarkerMutex);
#endif
}

void Profiler::PopProfileMarker()
{
#if ENABLE_PROFILE_MARKERS
    MUTEX_LOCK(gProfileMarkerMutex);
    ms_nProfileMarkerCounter--;
    assert(ms_nProfileMarkerCounter >= 0);
    MUTEX_UNLOCK(gProfileMarkerMutex);
#endif
}

const float Profiler::RetrieveGPUProfileMarkerStart(const char* const label) const
{
    float time = -1.f;
#if ENABLE_PROFILE_MARKERS
    MUTEX_LOCK(gProfileMarkerMutex);
    for (unsigned int i = 0; i < m_arrGPUProfileMarkerResult.size(); i++)
    {
        GPUProfileMarkerResult* marker = m_arrGPUProfileMarkerResult[i];
        if (marker && marker->m_eStatus == GPUProfileMarkerResult::GPMRS_VALID && strcmp(label, marker->m_szLabel) == 0)
        {
            time = marker->m_fStart;
            break;
        }
    }
    MUTEX_UNLOCK(gProfileMarkerMutex);
#endif
    return time;
}

const float Profiler::RetrieveGPUProfileMarkerEnd(const char* const label) const
{
    float time = -1.f;
#if ENABLE_PROFILE_MARKERS
    MUTEX_LOCK(gProfileMarkerMutex);
    for (unsigned int i = 0; i < m_arrGPUProfileMarkerResult.size(); i++)
    {
        GPUProfileMarkerResult* marker = m_arrGPUProfileMarkerResult[i];
        if (marker && marker->m_eStatus == GPUProfileMarkerResult::GPMRS_VALID && strcmp(label, marker->m_szLabel) == 0)
        {
            time = marker->m_fEnd;
            break;
        }
    }
    MUTEX_UNLOCK(gProfileMarkerMutex);
#endif
    return time;
}

const float Profiler::RetrieveGPUProfileMarkerResult(const char* const label) const
{
    float time = -1.f;
#if ENABLE_PROFILE_MARKERS
    MUTEX_LOCK(gProfileMarkerMutex);
    for (unsigned int i = 0; i < m_arrGPUProfileMarkerResult.size(); i++)
    {
        GPUProfileMarkerResult* marker = m_arrGPUProfileMarkerResult[i];
        if (marker && marker->m_eStatus == GPUProfileMarkerResult::GPMRS_VALID && strcmp(label, marker->m_szLabel) == 0)
        {
            time = marker->m_fTime;
            break;
        }
    }
    MUTEX_UNLOCK(gProfileMarkerMutex);
#endif
    return time;
}

const GPUProfileMarkerResult* const Profiler::RetrieveGPUProfileMarker(const unsigned int handle) const
{
    assert(handle < m_arrGPUProfileMarkerResult.size() && handle >= 0);

    const GPUProfileMarkerResult* ret = nullptr;
#if ENABLE_PROFILE_MARKERS
    MUTEX_LOCK(gProfileMarkerMutex);
    if (handle < m_arrGPUProfileMarkerResult.size() && handle >= 0 && m_arrGPUProfileMarkerResult[handle]->m_eStatus == GPUProfileMarkerResult::GPMRS_VALID)
    {
        ret = m_arrGPUProfileMarkerResult[handle];
    }
    MUTEX_UNLOCK(gProfileMarkerMutex);
#endif
    return ret;
}

const GPUProfileMarkerResult* const Profiler::RetrieveGPUProfileMarker(const char* const label) const
{
    const GPUProfileMarkerResult* ret = nullptr;
#if ENABLE_PROFILE_MARKERS
    MUTEX_LOCK(gProfileMarkerMutex);
    for (unsigned int i = 0; i < m_arrGPUProfileMarkerResult.size(); i++)
    {
        const GPUProfileMarkerResult* const marker = m_arrGPUProfileMarkerResult[i];
        if (marker && marker->m_eStatus == GPUProfileMarkerResult::GPMRS_VALID && strcmp(label, marker->m_szLabel) == 0)
        {
            ret = marker;
            break;
        }
    }
    MUTEX_UNLOCK(gProfileMarkerMutex);
#endif
    return ret;
}

const unsigned int Profiler::GetGPUProfileMarkerCount() const
{
    return (unsigned int)m_arrGPUProfileMarkerResult.size();
}

void Profiler::ReleaseGPUProfileMarkerResults()
{
#if ENABLE_PROFILE_MARKERS
    for (unsigned int i = 0; i < (unsigned int)m_arrGPUProfileMarkerResult.size(); i++)
    {
        delete m_arrGPUProfileMarkerResult[i];
    }

    m_arrGPUProfileMarkerResult.clear();
#endif
}

void Profiler::UpdateGPUProfileMarkerResults()
{
#if ENABLE_PROFILE_MARKERS
    // Cleanup old timestamps
    for (int i = (int)m_arrGPUProfileMarkerResult.size() - 1; i >= 0; i--)
    {
        GPUProfileMarkerResult* result = m_arrGPUProfileMarkerResult[i];
        if (result && result->m_eStatus == GPUProfileMarkerResult::GPMRS_VALID)
        {
            for (int j = i - 1; j >= 0; j--)
            {
                GPUProfileMarkerResult* otherResult = m_arrGPUProfileMarkerResult[j];
                if (otherResult && strcmp(otherResult->m_szLabel, result->m_szLabel) == 0)
                {
                    delete otherResult;
                    m_arrGPUProfileMarkerResult.erase(m_arrGPUProfileMarkerResult.begin() + j);
                    i--;
                    continue;
                }
            }
        }
    }
#endif
}

//////////////////////////////////
// GPUProfileMarkerResult class //
//////////////////////////////////
GPUProfileMarkerResult::GPUProfileMarkerResult(const char* const label)
    : m_fTime(-1.f)
    , m_fStart(-1.f)
    , m_fEnd(-1.f)
    , m_eStatus(GPMRS_ISSUED)
    , m_szLabel(nullptr)
{
#if ENABLE_PROFILE_MARKERS
    const unsigned int size = (unsigned int)strlen(label) + 1;
    m_szLabel = new char[size];
    strcpy_s(m_szLabel, size, label);
#endif
}

GPUProfileMarkerResult::~GPUProfileMarkerResult()
{
#if ENABLE_PROFILE_MARKERS
    delete m_szLabel;
#endif
}

const char* const GPUProfileMarkerResult::GetLabel() const
{
    return m_szLabel;
}

const float GPUProfileMarkerResult::GetTiming() const
{
    return m_fTime;
}

const float GPUProfileMarkerResult::GetStart() const
{
    return m_fStart;
}

const float GPUProfileMarkerResult::GetEnd() const
{
    return m_fEnd;
}
