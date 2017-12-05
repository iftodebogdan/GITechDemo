/**
 * @file        Profiler.h
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

#ifndef PROFILING_H
#define PROFILING_H

#ifndef SYNESTHESIA3D_DLL
#ifdef SYNESTHESIA3D_EXPORTS
#define SYNESTHESIA3D_DLL __declspec(dllexport) /**< @brief Export/import directive keyword. */
#else
#define SYNESTHESIA3D_DLL __declspec(dllimport) /**< @brief Export/import directive keyword. */
#endif
#endif // SYNESTHESIA3D_DLL

#include <vector>

#ifndef ENABLE_PROFILE_MARKERS
    #if defined(_DEBUG) || defined(_PROFILE)
        #define ENABLE_PROFILE_MARKERS (1)  /**< @brief Enable/disable profile markers. */
    #else
        #define ENABLE_PROFILE_MARKERS (0)  /**< @brief Enable/disable profile markers. */
    #endif
#endif

#if ENABLE_PROFILE_MARKERS
    extern int g_nProfileMarkerCounter; /**< Counter for keeping track of profile markers pushes/pops. */
    #ifndef PUSH_PROFILE_MARKER
        /**
         * @brief   Macro for simple introduction of a profile marker.
         */
        #define PUSH_PROFILE_MARKER(label) \
            if(Synesthesia3D::Renderer::GetInstance() && Synesthesia3D::Renderer::GetInstance()->GetProfiler()) \
                Synesthesia3D::Renderer::GetInstance()->GetProfiler()->PushProfileMarker(label)
    #endif
    #ifndef PUSH_PROFILE_MARKER_WITH_GPU_QUERY
        /**
         * @brief   Macro for simple introduction of a profile marker with GPU timing (use RetrieveGPUProfileMarkerResult() to retrieve it).
         */
        #define PUSH_PROFILE_MARKER_WITH_GPU_QUERY(label) \
            if(Synesthesia3D::Renderer::GetInstance() && Synesthesia3D::Renderer::GetInstance()->GetProfiler()) \
                Synesthesia3D::Renderer::GetInstance()->GetProfiler()->PushProfileMarker(label, true)
    #endif
    #ifndef POP_PROFILE_MARKER
        /**
         * @brief   Macro for simple removing of a profile marker.
         */
        #define POP_PROFILE_MARKER() \
            if(Synesthesia3D::Renderer::GetInstance() && Synesthesia3D::Renderer::GetInstance()->GetProfiler()) \
                Synesthesia3D::Renderer::GetInstance()->GetProfiler()->PopProfileMarker()
    #endif
#else
    #ifndef PUSH_PROFILE_MARKER
        #define PUSH_PROFILE_MARKER(label) ((void)0)
    #endif
    #ifndef PUSH_PROFILE_MARKER_WITH_GPU_QUERY
        #define PUSH_PROFILE_MARKER_WITH_GPU_QUERY(label) ((void)0)
    #endif
    #ifndef POP_PROFILE_MARKER
        #define POP_PROFILE_MARKER() ((void)0)
    #endif
#endif

namespace Synesthesia3D
{
    class GPUProfileMarkerResult
    {
    public:
        enum GPUProfileMarkerResultStatus
        {
            GPMRS_ISSUED,
            GPMRS_VALID
        };

        SYNESTHESIA3D_DLL   const char* const   GetLabel() const;
        SYNESTHESIA3D_DLL   const float         GetTiming() const;
        SYNESTHESIA3D_DLL   const float         GetStart() const;
        SYNESTHESIA3D_DLL   const float         GetEnd() const;

    protected:
        GPUProfileMarkerResult(const char* const label);
        virtual ~GPUProfileMarkerResult();

        char* m_szLabel;
        float m_fTime;
        float m_fStart;
        float m_fEnd;
        GPUProfileMarkerResultStatus m_eStatus;

        friend class Profiler;
    };

    class Profiler
    {
    public:
        /**
        * @brief    Marks the beginning of a user-defined event, viewable in graphical analysis tools.
        *
        * @note Must be paried with a corresponding @ref PopProfileMarker().
        *
        * @param[in]    label   A name for the event.
        */
        virtual SYNESTHESIA3D_DLL                   void                PushProfileMarker(const char* const label, const bool issueGPUQuery = false);

        /**
        * @brief    Marks the end of a user-defined event.
        *
        * @note Must be paried with a corresponding @ref PushProfileMarker().
        */
        virtual SYNESTHESIA3D_DLL                   void                PopProfileMarker();
        
        /**
        * @brief    Retrieves the GPU absolute start time, in seconds, for the specified profile marker label.
        *
        * @note Returns <0 if marker is still in flight, non-existant or otherwise invalid.
        */
                SYNESTHESIA3D_DLL               const float             RetrieveGPUProfileMarkerStart(const char* const label) const;

        /**
        * @brief    Retrieves the GPU absolute end time, in seconds, for the specified profile marker label.
        *
        * @note Returns <0 if marker is still in flight, non-existant or otherwise invalid.
        */
                SYNESTHESIA3D_DLL               const float             RetrieveGPUProfileMarkerEnd(const char* const label) const;

        /**
        * @brief    Retrieves the GPU timing, in miliseconds, for the specified profile marker label.
        *
        * @note Returns <0 if marker is still in flight, non-existant or otherwise invalid.
        */
                SYNESTHESIA3D_DLL               const float             RetrieveGPUProfileMarkerResult(const char* const label) const;

        /**
        * @brief    Retrieves the GPU profile marker result for the specified profile marker handle.
        *
        * @note Returns nullptr if marker is still in flight, non-existant or handle is invalid.
        */
                SYNESTHESIA3D_DLL   const GPUProfileMarkerResult* const RetrieveGPUProfileMarker(const unsigned int handle) const;
                
        /**
        * @brief    Retrieves the GPU profile marker result for the specified profile marker label.
        *
        * @note Returns nullptr if marker is still in flight, non-existant or handle is invalid.
        */
                SYNESTHESIA3D_DLL   const GPUProfileMarkerResult* const RetrieveGPUProfileMarker(const char* const label) const;

        /**
        * @brief    Retrieves the number of GPU profile markers.
        *
        * @note Returns <0 if marker is still in flight, non-existant or otherwise invalid.
        */
                SYNESTHESIA3D_DLL           const unsigned int          GetGPUProfileMarkerCount() const;

    protected:
        Profiler();
        virtual ~Profiler();

        virtual void ReleaseGPUProfileMarkerResults();
        virtual void UpdateGPUProfileMarkerResults();

        std::vector<GPUProfileMarkerResult*>    m_arrGPUProfileMarkerResult;    /**< @brief A list of issued GPU profile markers. */
        static  int             ms_nProfileMarkerCounter;   /**< @brief Keeps track of profiler marker start/end pairs. */

        friend class Renderer;
        friend class ResourceManager;
    };
}

#endif // PROFILING_H
