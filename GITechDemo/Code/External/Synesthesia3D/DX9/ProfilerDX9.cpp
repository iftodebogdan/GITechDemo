/**
 *	@file		ProfilingDX9.cpp
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2016 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *	@copyright
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	@copyright
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	@copyright
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

#include "ProfilerDX9.h"
#include "RendererDX9.h"
using namespace Synesthesia3D;

#include "Utility/Mutex.h"

 // From Profiling.cpp
extern MUTEX gProfileMarkerMutex;

ProfilerDX9::ProfilerDX9()
{

}

ProfilerDX9::~ProfilerDX9()
{

}

void ProfilerDX9::PushProfileMarker(const char* const label, const bool issueGPUQuery)
{
#if ENABLE_PROFILE_MARKERS
	MUTEX_LOCK(gProfileMarkerMutex);

	Profiler::PushProfileMarker(label);
	unsigned int len = (unsigned int)strlen(label) + 1;
	wchar_t* labelWide = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, label, -1, labelWide, len);
	D3DPERF_BeginEvent((D3DCOLOR)0xffffffff, labelWide);
	delete[] labelWide;

	if (m_arrD3DDisjointQuery.size() > 0)
	{
		// Issue GPU begin query for current label
		GPUProfileMarkerResultDX9* marker = nullptr;
		if (issueGPUQuery && !RendererDX9::GetInstance()->IsDeviceLost())
		{
			marker = new GPUProfileMarkerResultDX9(label, m_arrD3DDisjointQuery.back());
			m_arrGPUProfileMarkerResult.push_back(marker);
		}
		m_arrGPUProfileMarkerDX9Stack.push(marker);
	}

	MUTEX_UNLOCK(gProfileMarkerMutex);
#endif
}

void ProfilerDX9::PopProfileMarker()
{
#if ENABLE_PROFILE_MARKERS
	MUTEX_LOCK(gProfileMarkerMutex);

	if (m_arrGPUProfileMarkerDX9Stack.size() > 0)
	{
		// Issue GPU end query for current label
		GPUProfileMarkerResultDX9* marker = m_arrGPUProfileMarkerDX9Stack.top();
		m_arrGPUProfileMarkerDX9Stack.pop();
		if (marker)
			marker->IssueEndQuery();
	}

	D3DPERF_EndEvent();
	Profiler::PopProfileMarker();
	MUTEX_UNLOCK(gProfileMarkerMutex);
#endif
}

void ProfilerDX9::IssueDisjointQueryBegin()
{
#if ENABLE_PROFILE_MARKERS
	m_arrD3DDisjointQuery.push_back(DisjointQuery());

	HRESULT hr = RendererDX9::GetInstance()->GetDevice()->CreateQuery(D3DQUERYTYPE_TIMESTAMPDISJOINT, &m_arrD3DDisjointQuery.back().disjointQuery);
	S3D_VALIDATE_HRESULT(hr);

	hr = RendererDX9::GetInstance()->GetDevice()->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ, &m_arrD3DDisjointQuery.back().freqQuery);
	S3D_VALIDATE_HRESULT(hr);

	if (SUCCEEDED(hr))
		hr = m_arrD3DDisjointQuery.back().disjointQuery->Issue(D3DISSUE_BEGIN);
	S3D_VALIDATE_HRESULT(hr);
#endif
}

void ProfilerDX9::IssueDisjointQueryEnd()
{
#if ENABLE_PROFILE_MARKERS
	HRESULT hr = m_arrD3DDisjointQuery.back().disjointQuery->Issue(D3DISSUE_END);
	S3D_VALIDATE_HRESULT(hr);

	hr = m_arrD3DDisjointQuery.back().freqQuery->Issue(D3DISSUE_END);
	S3D_VALIDATE_HRESULT(hr);

	UpdateGPUProfileMarkerResults();
#endif
}

void ProfilerDX9::ReleaseGPUProfileMarkerResults()
{
#if ENABLE_PROFILE_MARKERS
	Profiler::ReleaseGPUProfileMarkerResults();

	for (int i = 0; i < m_arrD3DDisjointQuery.size(); i++)
	{
		m_arrD3DDisjointQuery[i].disjointQuery->Release();
		m_arrD3DDisjointQuery[i].freqQuery->Release();
	}

	m_arrD3DDisjointQuery.clear();
#endif
}

void ProfilerDX9::UpdateGPUProfileMarkerResults()
{
#if ENABLE_PROFILE_MARKERS
	// Update disjoint queries and their respective timestamp queries
	for (int i = 0; i < m_arrD3DDisjointQuery.size(); i++)
	{
		LPDIRECT3DQUERY9 djQuery = m_arrD3DDisjointQuery[i].disjointQuery;
		LPDIRECT3DQUERY9 freqQuery = m_arrD3DDisjointQuery[i].freqQuery;
		if (djQuery->GetData(NULL, 0, 0) != S_FALSE)
		{
			BOOL djData;
			djQuery->GetData(&djData, sizeof(djData), 0);

			if (djData)
			{
				// Disjoint timestamps: remove them
				for (unsigned int j = 0; j < m_arrGPUProfileMarkerResult.size(); j++)
				{
					GPUProfileMarkerResultDX9* result = (GPUProfileMarkerResultDX9*)m_arrGPUProfileMarkerResult[j];
					if (result && result->m_tD3DDisjointQuery.disjointQuery == djQuery)
					{
						delete result;
						m_arrGPUProfileMarkerResult.erase(m_arrGPUProfileMarkerResult.begin() + j);
						j--;
						continue;
					}
				}

				// Remove the disjoint query
				djQuery->Release();
				freqQuery->Release();
				m_arrD3DDisjointQuery.erase(m_arrD3DDisjointQuery.begin() + i);
				i--;
			}
			else
			{
				bool isOldDisjointQuery = true;

				UINT64 freq;
				freqQuery->GetData(&freq, sizeof(freq), 0);

				// Valid timestamps: update them
				for (unsigned int j = 0; j < m_arrGPUProfileMarkerResult.size(); j++)
				{
					GPUProfileMarkerResultDX9* result = (GPUProfileMarkerResultDX9*)m_arrGPUProfileMarkerResult[j];
					if (result && result->m_eStatus == GPUProfileMarkerResult::GPMRS_ISSUED && result->m_tD3DDisjointQuery.disjointQuery == djQuery)
					{
						isOldDisjointQuery = false;

						if (result->m_pD3DBeginQuery->GetData(NULL, 0, 0) != S_FALSE && result->m_pD3DEndQuery->GetData(NULL, 0, 0) != S_FALSE)
						{
							UINT64 begin, end;
							result->m_pD3DBeginQuery->GetData(&begin, sizeof(begin), 0);
							result->m_pD3DEndQuery->GetData(&end, sizeof(end), 0);
							result->m_fTime = float(end - begin) * 1000.0f / float(freq);
							if (result->m_fTime < 0.0001f)
								result->m_fTime = 0.f;
							result->m_eStatus = GPUProfileMarkerResult::GPMRS_VALID;
						}
					}
				}

				// Remove older disjoint queries and their corresponding timestamp queries
				if (isOldDisjointQuery)
				{
					for (int j = 0; j < i; j++)
					{
						LPDIRECT3DQUERY9 oldDjQuery = m_arrD3DDisjointQuery[j].disjointQuery;
						LPDIRECT3DQUERY9 oldFreqQuery = m_arrD3DDisjointQuery[j].freqQuery;
						for (int k = 0; k < m_arrGPUProfileMarkerResult.size(); k++)
						{
							GPUProfileMarkerResultDX9* result = (GPUProfileMarkerResultDX9*)m_arrGPUProfileMarkerResult[k];
							if (result && result->m_tD3DDisjointQuery.disjointQuery == oldDjQuery)
							{
								delete result;
								m_arrGPUProfileMarkerResult.erase(m_arrGPUProfileMarkerResult.begin() + k);
								k--;
							}
						}
						oldDjQuery->Release();
						oldFreqQuery->Release();
						m_arrD3DDisjointQuery.erase(m_arrD3DDisjointQuery.begin() + j);
						j--;
						i--;
					}
				}
			}
		}
	}

	// Cleanup old timestamps
	Profiler::UpdateGPUProfileMarkerResults();
#endif
}

GPUProfileMarkerResultDX9::GPUProfileMarkerResultDX9(const char* const label, DisjointQuery disjointQuery)
	: GPUProfileMarkerResult(label)
	, m_pD3DBeginQuery(nullptr)
	, m_pD3DEndQuery(nullptr)
	, m_tD3DDisjointQuery(disjointQuery)
{
#if ENABLE_PROFILE_MARKERS
	HRESULT hr = E_FAIL;
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

	if (device)
	{
		hr = device->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &m_pD3DBeginQuery);
		S3D_VALIDATE_HRESULT(hr);

		if (SUCCEEDED(hr))
			hr = m_pD3DBeginQuery->Issue(D3DISSUE_END);
		S3D_VALIDATE_HRESULT(hr);
	}
#endif
}

GPUProfileMarkerResultDX9::~GPUProfileMarkerResultDX9()
{
	if (m_pD3DBeginQuery)
		m_pD3DBeginQuery->Release();
	if (m_pD3DEndQuery)
		m_pD3DEndQuery->Release();
}

void GPUProfileMarkerResultDX9::IssueEndQuery()
{
#if ENABLE_PROFILE_MARKERS
	HRESULT hr = E_FAIL;
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

	if (device)
	{
		hr = device->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &m_pD3DEndQuery);
		S3D_VALIDATE_HRESULT(hr);

		if (SUCCEEDED(hr))
			hr = m_pD3DEndQuery->Issue(D3DISSUE_END);
		S3D_VALIDATE_HRESULT(hr);
	}
#endif
}
