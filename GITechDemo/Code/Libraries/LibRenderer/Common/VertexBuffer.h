//////////////////////////////////////////////////////////////////////////
// This file is part of the "LibRenderer" 3D graphics library           //
//                                                                      //
// Copyright (C) 2014 - Iftode Bogdan-Marius <iftode.bogdan@gmail.com>  //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program. If not, see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////
#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif // LIBRENDERER_DLL

#include "Buffer.h"
#include "VertexFormat.h"

namespace LibRendererDll
{
	class IndexBuffer;

	//This is a platform independent class which allows the creation and manipulation of a vertex buffer.
	class VertexBuffer : public Buffer
	{
	public:
		//Enable the vertex buffer. You can specify offset N to start from the (N+1)th vertex.
		//This also enables the proper vertex format and index buffer (if it exists) for the vertex buffer so you don't have to.
		virtual	LIBRENDERER_DLL		void		Enable(const unsigned int offset = 0) = 0;
		//This also disabled the vertex format and index buffer (if it exists) for the vertex buffer so you don't have to.
		virtual	LIBRENDERER_DLL		void		Disable() = 0;
		//Locking the buffer allows for modifications to be made to its' contents. The flow is Lock -> Update -> Unlock.
		virtual	LIBRENDERER_DLL		void		Lock(const BufferLocking lockMode) = 0;
		//Unlock our buffer
		virtual	LIBRENDERER_DLL		void		Unlock() = 0;
		//Sync our local modifications to the buffer's content.
		virtual	LIBRENDERER_DLL		void		Update() = 0;

		// Create a corresponding platform specific resource
		virtual	LIBRENDERER_DLL		void		Bind() = 0;
		// Destroy the platform specific resource
		virtual	LIBRENDERER_DLL		void		Unbind() = 0;

				LIBRENDERER_DLL VertexFormat*	GetVertexFormat() const { return m_pVertexFormat; }
				LIBRENDERER_DLL void			SetIndexBuffer(IndexBuffer* const indexBuffer) { m_pIndexBuffer = indexBuffer; }
				LIBRENDERER_DLL IndexBuffer*	GetIndexBuffer() const { return m_pIndexBuffer; }

		template <typename T>
		//Position accessor
		inline	LIBRENDERER_DLL T&				Position(const unsigned int vertexIdx) const;
		inline	LIBRENDERER_DLL const bool		HasPosition() const;

		template <typename T>
		//Normal accessor
		inline	LIBRENDERER_DLL T&				Normal(const unsigned int vertexIdx) const;
		inline	LIBRENDERER_DLL const bool		HasNormal() const;

		template <typename T>
		//Tangent accessor
		inline	LIBRENDERER_DLL T&				Tangent(const unsigned int vertexIdx) const;
		inline	LIBRENDERER_DLL const bool		HasTangent() const;

		template <typename T>
		//Binormal accessor
		inline	LIBRENDERER_DLL T&				Binormal(const unsigned int vertexIdx) const;
		inline	LIBRENDERER_DLL const bool		HasBinormal() const;

		template <typename T>
		//TexCoord accessor
		inline	LIBRENDERER_DLL T&				TexCoord(const unsigned int vertexIdx, const unsigned int usageIdx) const;
		inline	LIBRENDERER_DLL const bool		HasTexCoord(const unsigned int usageIdx) const;

		template <typename T>
		//Color accessor
		inline	LIBRENDERER_DLL T&				Color(const unsigned int vertexIdx, const unsigned int usageIdx) const;
		inline	LIBRENDERER_DLL const bool		HasColor(const unsigned int usageIdx) const;

		template <typename T>
		//BlendIndices accessor
		inline	LIBRENDERER_DLL T&				BlendIndices(const unsigned int vertexIdx) const;
		inline	LIBRENDERER_DLL const bool		HasBlendIndices() const;

		template <typename T>
		//BlendWeight accessor
		inline	LIBRENDERER_DLL T&				BlendWeight(const unsigned int vertexIdx) const;
		inline	LIBRENDERER_DLL const bool		HasBlendWeight() const;

	protected:
		VertexBuffer(
			VertexFormat* const vertexFormat, const unsigned int vertexCount,
			IndexBuffer* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC);
		virtual	~VertexBuffer();

		VertexFormat*	m_pVertexFormat;	// Holds a pointer to the associated vertex format
		IndexBuffer*	m_pIndexBuffer;		// Holds a pointer to the associated index buffer

		friend class ResourceManager;

		friend std::ostream& operator<<(std::ostream& output_out, VertexBuffer &vb_in);
		friend std::istream& operator>>(std::istream& s_in, VertexBuffer &vb_out);
	};

//Include our inline functions file
#include "VertexBuffer.inl"
}

#endif //VERTEXBUFFER_H
