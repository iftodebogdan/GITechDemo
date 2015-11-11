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

#include "Buffer.h"

namespace LibRendererDll
{
	class IndexBuffer;
	class VertexFormat;

	//This is a platform independent class which allows the creation and manipulation of a vertex buffer.
	class VertexBuffer : public Buffer
	{
	public:
		//Enable the vertex buffer. You can specify offset N to start from the (N+1)th vertex.
		//This also enables the proper vertex format and index buffer (if it exists) for the vertex buffer so you don't have to.
		virtual	LIBRENDERER_DLL		void		Enable(const unsigned int offset = 0) PURE_VIRTUAL
		//This also disabled the vertex format and index buffer (if it exists) for the vertex buffer so you don't have to.
		virtual	LIBRENDERER_DLL		void		Disable() PURE_VIRTUAL
		//Locking the buffer allows for modifications to be made to its' contents. The flow is Lock -> Update -> Unlock.
		virtual	LIBRENDERER_DLL		void		Lock(const BufferLocking lockMode) PURE_VIRTUAL
		//Unlock our buffer
		virtual	LIBRENDERER_DLL		void		Unlock() PURE_VIRTUAL
		//Sync our local modifications to the buffer's content.
		virtual	LIBRENDERER_DLL		void		Update() PURE_VIRTUAL

		// Create a corresponding platform specific resource
		virtual	LIBRENDERER_DLL		void		Bind() PURE_VIRTUAL
		// Destroy the platform specific resource
		virtual	LIBRENDERER_DLL		void		Unbind() PURE_VIRTUAL

		LIBRENDERER_DLL VertexFormat*	GetVertexFormat() const;
		LIBRENDERER_DLL void			SetIndexBuffer(IndexBuffer* const indexBuffer);
		LIBRENDERER_DLL IndexBuffer*	GetIndexBuffer() const;

		template <typename T>
		//Position accessor
			inline			T&			Position(const unsigned int vertexIdx) const;
		LIBRENDERER_DLL const bool		HasPosition() const;

		template <typename T>
		//Normal accessor
			inline			T&			Normal(const unsigned int vertexIdx) const;
		LIBRENDERER_DLL const bool		HasNormal() const;

		template <typename T>
		//Tangent accessor
			inline			T&			Tangent(const unsigned int vertexIdx) const;
		LIBRENDERER_DLL const bool		HasTangent() const;

		template <typename T>
		//Binormal accessor
			inline			T&			Binormal(const unsigned int vertexIdx) const;
		LIBRENDERER_DLL const bool		HasBinormal() const;

		template <typename T>
		//TexCoord accessor
			inline			T&			TexCoord(const unsigned int vertexIdx, const unsigned int usageIdx) const;
		LIBRENDERER_DLL const bool		HasTexCoord(const unsigned int usageIdx) const;

		template <typename T>
		//Color accessor
			inline			T&			Color(const unsigned int vertexIdx, const unsigned int usageIdx) const;
		LIBRENDERER_DLL const bool		HasColor(const unsigned int usageIdx) const;

		template <typename T>
		//BlendIndices accessor
			inline			T&			BlendIndices(const unsigned int vertexIdx) const;
		LIBRENDERER_DLL const bool		HasBlendIndices() const;

		template <typename T>
		//BlendWeight accessor
			inline			T&			BlendWeight(const unsigned int vertexIdx) const;
		LIBRENDERER_DLL const bool		HasBlendWeight() const;

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
