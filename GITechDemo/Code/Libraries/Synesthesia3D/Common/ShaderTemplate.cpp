/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	ShaderTemplate.cpp
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

#include "stdafx.h"

#include "Renderer.h"
#include "ShaderTemplate.h"
#include "SamplerState.h"
#include "ShaderProgram.h"
#include "ShaderInput.h"
#include "Texture.h"
#include "ResourceManager.h"
using namespace Synesthesia3D;

ShaderTemplate::ShaderTemplate(ShaderProgram* const shaderProgram)
	: m_pProgram(shaderProgram)
	, m_pShaderInput(nullptr)
{
	assert(shaderProgram);
	DescribeShaderInputs();
}

ShaderTemplate::~ShaderTemplate()
{}

void ShaderTemplate::DescribeShaderInputs()
{
	unsigned int paramCount = m_pProgram->GetConstantCount();	
	unsigned int offset = 0;
	for (unsigned int i = 0; i < paramCount; i++)
	{
		ShaderInputDesc inputDesc;
		inputDesc.szName = m_pProgram->GetConstantName(i);
		inputDesc.eInputType = m_pProgram->GetConstantType(i);
		inputDesc.eRegisterType = m_pProgram->GetConstantRegisterType(i);
		inputDesc.nRegisterIndex = m_pProgram->GetConstantRegisterIndex(i);
		inputDesc.nRegisterCount = m_pProgram->GetConstantRegisterCount(i);
		inputDesc.nRows = m_pProgram->GetConstantRowCount(i);
		inputDesc.nColumns = m_pProgram->GetConstantColumnCount(i);
		inputDesc.nArrayElements = m_pProgram->GetConstantArrayElementCount(i);
		inputDesc.nBytes = m_pProgram->GetConstantSizeBytes(i);
		inputDesc.nOffsetInBytes = offset;
		offset += inputDesc.nBytes; //inputDesc.nRegisterCount * sizeof(float) * 4u;

		m_arrInputDesc.push_back(inputDesc);
	}
}

const unsigned int ShaderTemplate::GetTotalNumberOfUsedRegisters() const
{
	unsigned int ret = 0;
	for (unsigned int i = 0, n = (unsigned int)m_arrInputDesc.size(); i < n; i++)
	{
		ret += m_arrInputDesc[i].nRegisterCount;
	}

	return ret;
}

const unsigned int ShaderTemplate::GetTotalSizeOfInputConstants() const
{
	unsigned int ret = 0;
	for (unsigned int i = 0, n = (unsigned int)m_arrInputDesc.size(); i < n; i++)
	{
		ret += m_arrInputDesc[i].nBytes;
	}

	return ret;
}

void ShaderTemplate::Enable(ShaderInput* const shaderInput)
{
	PUSH_PROFILE_MARKER((m_pProgram->m_szSrcFile + " : " + m_pProgram->m_szEntryPoint + "() - " + m_pProgram->m_szProfile).c_str());

	assert(m_pShaderInput == nullptr);
	//assert((shaderInput && m_arrInputDesc.size()) || (!shaderInput && !m_arrInputDesc.size()) || (Renderer::GetAPI() == API_NULL));

	m_pShaderInput = shaderInput;

	for (unsigned int i = 0, n = (unsigned int)m_arrInputDesc.size(); i < n; i++)
	{
		PUSH_PROFILE_MARKER(m_arrInputDesc[i].szName.c_str());

		if (m_arrInputDesc[i].eInputType >= IT_BOOL && m_arrInputDesc[i].eInputType <= IT_FLOAT)
		{
			m_pProgram->SetValue(
				m_arrInputDesc[i].eRegisterType,
				m_arrInputDesc[i].nRegisterIndex,
				shaderInput->GetData() + m_arrInputDesc[i].nOffsetInBytes,
				m_arrInputDesc[i].nRegisterCount
				);
		}
		else
		{
			if (m_arrInputDesc[i].eInputType >= IT_SAMPLER && m_arrInputDesc[i].eInputType <= IT_SAMPLERCUBE)
			{
				const unsigned int texIdx = *(unsigned int*)(shaderInput->GetData() + m_arrInputDesc[i].nOffsetInBytes);
				const Texture* const tex = texIdx != -1 ? Renderer::GetInstance()->GetResourceManager()->GetTexture(texIdx) : nullptr;

				if (tex)
				{
					if(strlen(tex->GetSourceFileName()))
						PUSH_PROFILE_MARKER(tex->GetSourceFileName());

					tex->Enable(m_arrInputDesc[i].nRegisterIndex);
					SamplerState* ssm = Renderer::GetInstance()->GetSamplerStateManager();
					ssm->SetAnisotropy(m_arrInputDesc[i].nRegisterIndex, tex->GetAnisotropy());
					ssm->SetMipLodBias(m_arrInputDesc[i].nRegisterIndex, tex->GetMipLodBias());
					ssm->SetFilter(m_arrInputDesc[i].nRegisterIndex, tex->GetFilter());
					ssm->SetBorderColor(m_arrInputDesc[i].nRegisterIndex, tex->GetBorderColor());
					ssm->SetAddressingModeU(m_arrInputDesc[i].nRegisterIndex, tex->GetAddressingModeU());
					ssm->SetAddressingModeV(m_arrInputDesc[i].nRegisterIndex, tex->GetAddressingModeV());
					ssm->SetAddressingModeW(m_arrInputDesc[i].nRegisterIndex, tex->GetAddressingModeW());
					ssm->SetSRGBEnabled(m_arrInputDesc[i].nRegisterIndex, tex->GetSRGBEnabled());

					if (strlen(tex->GetSourceFileName()))
						POP_PROFILE_MARKER();
				}
			}
			else
				assert(false); // shouldn't happen
		}

		POP_PROFILE_MARKER();
	}

	m_pProgram->Enable();

	POP_PROFILE_MARKER();
}

void ShaderTemplate::Disable()
{
	//assert((m_pShaderInput && m_arrInputDesc.size()) || (!m_pShaderInput && !m_arrInputDesc.size()) || (Renderer::GetAPI() == API_NULL));

	m_pProgram->Disable();

	for (unsigned int i = 0, n = (unsigned int)m_arrInputDesc.size(); i < n; i++)
	{
		if (m_arrInputDesc[i].eInputType >= IT_SAMPLER && m_arrInputDesc[i].eInputType <= IT_SAMPLERCUBE)
		{
			const unsigned int texIdx = *(unsigned int*)(m_pShaderInput->GetData() + m_arrInputDesc[i].nOffsetInBytes);
			const Texture* const tex = texIdx != -1 ? Renderer::GetInstance()->GetResourceManager()->GetTexture(texIdx) : nullptr;
			if (tex)
				tex->Disable(m_arrInputDesc[i].nRegisterIndex);
		}
	}

	m_pShaderInput = nullptr;
}

void ShaderTemplate::Enable(ShaderInput& shaderInput)
{
	Enable(&shaderInput);
}

const std::vector<ShaderInputDesc> ShaderTemplate::GetConstantTable()
{
	return m_arrInputDesc;
}
