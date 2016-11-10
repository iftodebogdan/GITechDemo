/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RenderPass.h
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#ifndef RENDER_PASS_H_
#define RENDER_PASS_H_

#include <vector>

#define IMPLEMENT_RENDER_PASS(Class) \
	public: \
	Class (const char* const passName, RenderPass* const parentPass); \
	~ Class (); \
	protected: \
	void Update(const float fDeltaTime); \
	void Draw();

namespace GITechDemoApp
{
	class RenderPass
	{
	public:
		RenderPass(const char* const passName, RenderPass* const parentPass);
		~RenderPass();

		void AddChildPass(RenderPass* const childPass);
		const char* GetPassName() const { return m_szPassName.c_str(); }

		const std::vector<RenderPass*>&		GetChildren() const { return m_arrChildList; }

	protected:
		virtual void Update(const float fDeltaTime) {}
		virtual void Draw();

		void DrawChildren();

	private:
		// Disallow some member functions
		RenderPass();
		RenderPass(const RenderPass&);
		void operator=(const RenderPass&) {}

		std::string					m_szPassName;
		std::vector<RenderPass*>	m_arrChildList;

		friend class RenderScheme;
	};
}

#endif //RENDER_PASS_H_