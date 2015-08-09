#include "stdafx.h"

#include "Renderer.h"
using namespace LibRendererDll;

#include "GITechDemo.h"

#include "RenderPass.h"
using namespace GITechDemoApp;

RenderPass::RenderPass(const char* const passName, RenderPass* const parentPass)
	: m_szPassName(passName)
{
	if(parentPass)
		parentPass->AddChildPass(this);
}

RenderPass::~RenderPass()
{}

void RenderPass::AddChildPass(RenderPass* const childPass)
{
	m_arrChildList.push_back(childPass);
}

void RenderPass::Draw()
{
	DrawChildren();
}

void RenderPass::DrawChildren()
{
	for (unsigned int child = 0; child < m_arrChildList.size(); child++)
	{
		if (m_arrChildList[child] != nullptr)
		{
			PUSH_PROFILE_MARKER(m_arrChildList[child]->GetPassName());
			m_arrChildList[child]->OnUpdate(((GITechDemo*)AppMain)->GetDeltaTime());
			m_arrChildList[child]->OnDraw();
			POP_PROFILE_MARKER();
		}
	}
}