/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	VertexFormat.h
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

#ifndef VERTEXFORMAT_H
#define VERTEXFORMAT_H

#include "ResourceData.h"

namespace Synesthesia3D
{
	//This is a platform independent class with which you can define a vertex format for your vertex buffer.
	class VertexFormat
	{
	public:
				//Initialize() takes n triples of the form (VertexAttributeUsage, VertexAttributeType, unsigned int),
				//where n is the number of attributes of the vertex format.
				SYNESTHESIA3D_DLL void				Initialize(const VertexAttributeUsage usage, const VertexAttributeType type, const unsigned int usageIdx, ...);

				//Set individual attributes. Don't forget to call SetStride(CalculateStride()) when you're done.
				SYNESTHESIA3D_DLL void				SetAttribute(const unsigned int attrIdx, const unsigned int offset, const VertexAttributeUsage usage,  const VertexAttributeType type, const unsigned int usageIdx);
				//This can be used in conjunction with CalculateStride()
				SYNESTHESIA3D_DLL void				SetStride(const unsigned int stride);
				//Calculates the vertex format stride based on the attributes' sizes.
				SYNESTHESIA3D_DLL const unsigned int	CalculateStride() const;

				/* Get the number of attributes */
				SYNESTHESIA3D_DLL const unsigned int			GetAttributeCount() const;
				/* Get the offset in bytes of an attribute */
				SYNESTHESIA3D_DLL const unsigned int			GetOffset(const unsigned int attrIdx) const;
				/* Get the attribute's data type */
				SYNESTHESIA3D_DLL const VertexAttributeType	GetAttributeType(const unsigned int attrIdx) const;
				/* Get the usage/semantic of the attribute */
				SYNESTHESIA3D_DLL const VertexAttributeUsage	GetAttributeUsage(const unsigned int attrIdx) const;
				/* Get the usage/semantic index of the attribute */
				SYNESTHESIA3D_DLL const unsigned int			GetUsageIndex(const unsigned int attrIdx) const;
				/* Get the vertex format's stride */
				SYNESTHESIA3D_DLL const unsigned int			GetStride() const;

				static	SYNESTHESIA3D_DLL const unsigned int	GetAttributeTypeSize(const VertexAttributeType type);

		// Enable the vertex format (also called by the associated vertex buffer)
		virtual	SYNESTHESIA3D_DLL void	Enable() PURE_VIRTUAL
		// Disable the vertex format (also called by the associated vertex buffer)
		virtual	SYNESTHESIA3D_DLL void	Disable() PURE_VIRTUAL
		// Update the vertex format with the changes made
		virtual	SYNESTHESIA3D_DLL void	Update() PURE_VIRTUAL

		// Create a corresponding platform specific resource
		virtual	SYNESTHESIA3D_DLL void	Bind() PURE_VIRTUAL
		// Destroy the platform specific resource
		virtual	SYNESTHESIA3D_DLL void	Unbind() PURE_VIRTUAL

	protected:
		//When creating a vertex format, you must first set the number of attributes (this can't be changed later).
		//Each attribute can then be initialized separately using SetAttribute(), or all at once with Initialize().
		VertexFormat(const unsigned int attributeCount);
		virtual		~VertexFormat();

		unsigned int	m_nAttributeCount;	// The total number of attributes
		VertexElement*	m_pElements;		// A pointer to the array of elements
		unsigned int	m_nStride;			// The stride of the vertex format

		static const unsigned int VertexAttributeTypeSize[VAT_MAX];

		friend class ResourceManager;

		friend std::ostream& operator<<(std::ostream& output_out, VertexFormat &vf_in);
		friend std::istream& operator>>(std::istream& s_in, VertexFormat &vf_out);
	};
}

#endif //VERTEXFORMAT_H
