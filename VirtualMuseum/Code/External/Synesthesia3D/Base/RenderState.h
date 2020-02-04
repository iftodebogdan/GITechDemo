/**
 * @file        RenderState.h
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

#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include "ResourceData.h"

namespace Synesthesia3D
{
    class Renderer;

    /**
     * @brief   Render state manager class.
     */
    class RenderState
    {

    public:

        /**
         * @brief   Enables alpha testing.
         *
         * @param[in]   enabled     Enable alpha testing.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetAlphaTestEnabled(const bool enabled);

        /**
         * @brief   Sets alpha test function.
         *
         * @param[in]   alphaFunc   The comparison method.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetAlphaTestFunc(const Cmp alphaFunc);

        /**
         * @brief   Sets the reference value for alpha testing.
         *
         * @param[in]   alphaRef    The reference value for alpha testing (range is 0.0 - 1.0).
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetAlphaTestRef(const float alphaRef);



        /**
         * @brief   Enables color blending.
         *
         * @param[in]   enabled     Enable color blending.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetColorBlendEnabled(const bool enabled);

        /**
         * @brief   Sets source color blending operation.
         *
         * @param[in]   colorSrc    Source color blending operation.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetColorSrcBlend(const Blend colorSrc);

        /**
         * @brief   Sets destination color blending operation.
         *
         * @param[in]   colorDst    Destination color blending operation.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetColorDstBlend(const Blend colorDst);

        /**
         * @brief   Sets color blending factor.
         *
         * @note    Has effect only if BLEND_BLENDFACTOR or BLEND_INVBLENDFACTOR
         *          is set as source or destination color blending operation.
         *
         * @param[in]   rgba    Color blending factor (range 0.0 - 1.0 per color channel).
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetColorBlendFactor(const Vec4f rgba);



        /**
         * @brief   Sets the culling mode.
         *
         * @param[in]   cullMode    Culling mode.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetCullMode(const Cull cullMode);



        /**
         * @brief   Enables Z buffering.
         *
         * @param[in]   enabled     Z buffering mode.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetZEnabled(const ZBuffer enabled);

        /**
         * @brief   Sets depth comparison function.
         *
         * @param[in]   zFunc   Depth comparison function.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetZFunc(const Cmp zFunc);

        /**
         * @brief   Enables depth writing.
         *
         * @param[in]   enabled     Enable depth writing.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetZWriteEnabled(const bool enabled);



        /**
         * @brief   Enables color writing.
         *
         * @param[in]   red     Enable color writing on red channel.
         * @param[in]   green   Enable color writing on green channel.
         * @param[in]   blue    Enable color writing on blue channel.
         * @param[in]   alpha   Enable color writing on alpha channel.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetColorWriteEnabled(const bool red, const bool green, const bool blue, const bool alpha);



        /**
         * @brief   Sets slope scaled depth bias.
         *
         * @param[in]   scale   Slope scaled depth bias value.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetSlopeScaledDepthBias(const float scale);

        /**
         * @brief   Sets depth bias.
         *
         * @param[in]   bias    Depth bias value.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetDepthBias(const float bias);



        /**
         * @brief   Enables stencil bufer.
         *
         * @param[in]   enabled     Enable stencil buffer.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetStencilEnabled(const bool enabled);

        /**
         * @brief   Sets stencil comparison function.
         *
         * @param[in]   stencilFunc     Stencil comparison function.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetStencilFunc(const Cmp stencilFunc);

        /**
         * @brief   Sets stencil reference value.
         *
         * @param[in]   stencilRef      Stencil reference value.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetStencilRef(const unsigned long stencilRef);

        /**
         * @brief   Sets stencil mask.
         *
         * @param[in]   stencilMask     Stencil mask value.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetStencilMask(const unsigned long stencilMask);

        /**
         * @brief   Sets stencil write mask.
         *
         * @param[in]   stencilWriteMask    Stencil write mask value.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetStencilWriteMask(const unsigned long stencilWriteMask);

        /**
         * @brief   Sets stencil fail operation.
         *
         * @param[in]   stencilFail     Stencil fail operation.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetStencilFail(const StencilOp stencilFail);

        /**
         * @brief   Sets stencil pass / depth fail operation.
         *
         * @param[in]   stencilZFail    Stencil pass / depth fail operation.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetStencilZFail(const StencilOp stencilZFail);

        /**
         * @brief   Sets stencil pass operation.
         *
         * @param[in]   stencilPass     Stencil pass operation.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetStencilPass(const StencilOp stencilPass);



        /**
         * @brief   Sets triangle rasterization fill mode.
         *
         * @param[in]   fillMode    Fill mode.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetFillMode(const Fill fillMode);



        /**
         * @brief   Enables scissor testing.
         *
         * @param[in]   enabled     Enable scissor testing.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetScissorEnabled(const bool enabled);

        /**
         * @brief   Sets scissor.
         *
         * @param   size    The size of the scissor rectangle.
         * @param   offset  The offset of the scissor rectangle with respect to the upper left corner.
         *
         * @return  Success of operation.
         */
        virtual SYNESTHESIA3D_DLL       const bool      SetScissor(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) PURE_VIRTUAL;



        /**
         * @brief   Enables sRGB writing.
         *
         * @param[in]   enabled     Enable sRGB writing.
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL       const bool      SetSRGBWriteEnabled(const bool enabled);



        /**
         * @brief   Retrieves alpha testing state.
         */
                SYNESTHESIA3D_DLL       const bool      GetAlphaTestEnabled() const;
        
        /**
         * @brief   Retrieves alpha testing reference value.
         */
                SYNESTHESIA3D_DLL       const float     GetAlphaTestRef() const;

        /**
         * @brief   Retrieves alpha testing comparison function.
         */
                SYNESTHESIA3D_DLL       const Cmp       GetAlphaTestFunc() const;



        /**
         * @brief   Retrieves color blending state.
         */
                SYNESTHESIA3D_DLL       const bool      GetColorBlendEnabled() const;
        
        /**
         * @brief   Retrieves source color blending operation.
         */
                SYNESTHESIA3D_DLL       const Blend     GetColorSrcBlend() const;
        
        /**
         * @brief   Retrieves destination color blending operation.
         */
                SYNESTHESIA3D_DLL       const Blend     GetColorDstBlend() const;
        
        /**
         * @brief   Retrieves color blending factor.
         */
                SYNESTHESIA3D_DLL       const Vec4f     GetColorBlendFactor() const;



        /**
         * @brief   Retrieves culling mode.
         */
                SYNESTHESIA3D_DLL       const Cull      GetCullMode() const;



        /**
         * @brief   Retrieves Z buffering mode.
         */
                SYNESTHESIA3D_DLL       const ZBuffer   GetZEnabled() const;
        
        /**
         * @brief   Retrieves depth comparison function.
         */
                SYNESTHESIA3D_DLL       const Cmp       GetZFunc() const;
        
        /**
         * @brief   Retrieves Z buffering mode.
         */
                SYNESTHESIA3D_DLL       const bool      GetZWriteEnabled() const;


        
        /**
         * @brief   Retrieves color writing states for each channel.
         *
         * @param[out]  red     Color writing state for the red channel.
         * @param[out]  green   Color writing state for the green channel.
         * @param[out]  blue    Color writing state for the blue channel.
         * @param[out]  alpha   Color writing state for the alpha channel.
         */
                SYNESTHESIA3D_DLL           void        GetColorWriteEnabled(bool& red, bool& green, bool& blue, bool& alpha) const;
        
        /**
         * @brief   Retrieves color writing states for the red channel.
         */
                SYNESTHESIA3D_DLL       const bool      GetColorWriteRedEnabled() const;
        
        /**
         * @brief   Retrieves color writing states for the green channel.
         */
                SYNESTHESIA3D_DLL       const bool      GetColorWriteGreenEnabled() const;
        
        /**
         * @brief   Retrieves color writing states for the blue channel.
         */
                SYNESTHESIA3D_DLL       const bool      GetColorWriteBlueEnabled() const;
        
        /**
         * @brief   Retrieves color writing states for the alpha channel.
         */
                SYNESTHESIA3D_DLL       const bool      GetColorWriteAlphaEnabled() const;



        /**
         * @brief   Retrieves slope scaled depth bias value.
         */
                SYNESTHESIA3D_DLL       const float     GetSlopeScaledDepthBias() const;
        
        /**
         * @brief   Retrieves depth bias value.
         */
                SYNESTHESIA3D_DLL       const float     GetDepthBias() const;



        /**
         * @brief   Retrieves stencil buffering state.
         */
                SYNESTHESIA3D_DLL       const bool      GetStencilEnabled() const;
        
        /**
         * @brief   Retrieves stencil comparison function.
         */
                SYNESTHESIA3D_DLL       const Cmp       GetStencilFunc() const;
        
        /**
         * @brief   Retrieves stencil reference value.
         */
                SYNESTHESIA3D_DLL   const unsigned long GetStencilRef() const;
        
        /**
         * @brief   Retrieves stencil mask.
         */
                SYNESTHESIA3D_DLL   const unsigned long GetStencilMask() const;
        
        /**
         * @brief   Retrieves stencil write mask.
         */
                SYNESTHESIA3D_DLL   const unsigned long GetStencilWriteMask() const;
        
        /**
         * @brief   Retrieves stencil fail operation.
         */
                SYNESTHESIA3D_DLL   const StencilOp     GetStencilFail() const;
        
        /**
         * @brief   Retrieves stencil pass / depth fail operation.
         */
                SYNESTHESIA3D_DLL   const StencilOp     GetStencilZFail() const;
        
        /**
         * @brief   Retrieves stencil pass operation.
         */
                SYNESTHESIA3D_DLL   const StencilOp     GetStencilPass() const;



        /**
         * @brief   Retrieves triangle rasterization fill mode.
         */
                SYNESTHESIA3D_DLL       const Fill      GetFillMode() const;



        /**
         * @brief   Retrieves scissor state.
         */
                SYNESTHESIA3D_DLL       const bool      GetScissorEnabled() const;



        /**
         * @brief   Retrieves sRGB writing state.
         */
                SYNESTHESIA3D_DLL       const bool      GetSRGBWriteEnabled() const;



        /**
         * @brief   Resets render states to their default values.
         */
        virtual SYNESTHESIA3D_DLL           void        Reset();

    protected:

        /**
         * @brief Constructor.
         *
         * @details Meant to be used only by @ref Renderer::Initialize().
         */
        RenderState();
        
        /**
         * @brief Destructor.
         *
         * @details Meant to be used only by @ref Renderer::DestroyInstance().
         */
        virtual ~RenderState();



        /**
         * @brief   Pushes the current render states to the underlying API
         */
        virtual const bool  Flush() PURE_VIRTUAL;



        bool            m_bColorBlendEnabled;       /**< @brief Color blending state. */
        Blend           m_eColorSrcBlend;           /**< @brief Source color blending operation. */
        Blend           m_eColorDstBlend;           /**< @brief Destination color blending operation. */
        Vec4f           m_vColorBlendFactor;        /**< @brief Color blending factor. */

        bool            m_bAlphaTestEnabled;        /**< @brief Alpha testing state. */
        Cmp             m_eAlphaFunc;               /**< @brief Alpha testing comparison function. */
        float           m_fAlphaRef;                /**< @brief Alpha testing reference value. */

        Cull            m_eCullMode;                /**< @brief Culling mode. */

        ZBuffer         m_eZEnabled;                /**< @brief Z buffering state. */
        Cmp             m_eZFunc;                   /**< @brief Depth comparison function. */
        bool            m_bZWriteEnabled;           /**< @brief Depth writing state. */
        
        bool            m_bColorWriteRed;           /**< @brief Color writing on red channel. */
        bool            m_bColorWriteGreen;         /**< @brief Color writing on green channel. */
        bool            m_bColorWriteBlue;          /**< @brief Color writing on blur channel.*/
        bool            m_bColorWriteAlpha;         /**< @brief Color writing on alpha channel. */
        
        float           m_fSlopeScaledDepthBias;    /**< @brief Slope scaled depth bias value. */
        float           m_fDepthBias;               /**< @brief Depth bias value. */

        bool            m_bStencilEnabled;          /**< @brief Stencil buffering state. */
        Cmp             m_eStencilFunc;             /**< @brief Stencil comparison function. */
        unsigned long   m_lStencilRef;              /**< @brief Stencil reference value. */
        unsigned long   m_lStencilMask;             /**< @brief Stencil mask. */
        unsigned long   m_lStencilWriteMask;        /**< @brief Stencil write mask. */
        StencilOp       m_eStencilFail;             /**< @brief Stencil fail operation. */
        StencilOp       m_eStencilZFail;            /**< @brief Stencil pass / depth fail operation. */
        StencilOp       m_eStencilPass;             /**< @brief Stencil pass operation. */

        Fill            m_eFillMode;                /**< @brief Triangle rasterization fill mode. */

        bool            m_bScissorEnabled;          /**< @brief Scissor state. */

        bool            m_bSRGBEnabled;             /**< @brief sRGB writing state. */



        friend class Renderer;
    };
}

#endif //RENDERSTATE_H