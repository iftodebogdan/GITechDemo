/**
 *	@file		ProfilerDX9.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef PROFILINGDX9_H
#define PROFILINGDX9_H

#include <vector>
#include <stack>

#include <d3d9.h>

#include "Profiler.h"

namespace Synesthesia3D
{
	struct DisjointQuery
	{
		LPDIRECT3DQUERY9 disjointQuery;
		LPDIRECT3DQUERY9 freqQuery;
	};

	class GPUProfileMarkerResultDX9 : public GPUProfileMarkerResult
	{
	private:
		GPUProfileMarkerResultDX9(const char* const label, DisjointQuery disjointQuery);
		~GPUProfileMarkerResultDX9();

		void IssueEndQuery();

		LPDIRECT3DQUERY9	m_pD3DBeginQuery;
		LPDIRECT3DQUERY9	m_pD3DEndQuery;
		DisjointQuery		m_tD3DDisjointQuery;

		friend class ProfilerDX9;
	};

	class ProfilerDX9 : public Profiler
	{
	public:
		void PushProfileMarker(const char* const label, const bool issueGPUQuery = false);
		void PopProfileMarker();

	private:
		ProfilerDX9();
		~ProfilerDX9();

		void IssueDisjointQueryBegin();
		void IssueDisjointQueryEnd();

		void ReleaseGPUProfileMarkerResults();
		void UpdateGPUProfileMarkerResults();

		std::vector<DisjointQuery>				m_arrD3DDisjointQuery;
		std::stack<GPUProfileMarkerResultDX9*>	m_arrGPUProfileMarkerDX9Stack;

		friend class RendererDX9;
	};
}

#endif // PROFILINGDX9_H
