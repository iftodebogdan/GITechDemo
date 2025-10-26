/**
 * @file        ShaderProgramNVRHI.h
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

#ifndef SHADERPROGRAMNVRHI_H
#define SHADERPROGRAMNVRHI_H

#include "ResourceData.h"

#if ENABLE_NVRHI

#include "ShaderProgram.h"

namespace Synesthesia3D
{
    class ShaderProgramNVRHI : public ShaderProgram
    {
    private:
        ShaderProgramNVRHI(const ShaderProgramType programType, const char* srcData = "", const char* entryPoint = "", const char* profile = "");
        ~ShaderProgramNVRHI();

        void Enable(ShaderInput* const shaderInput) override;
        void Disable() override;
        const bool Compile(const char* filePath, const char* entryPoint = "") override;

        void Bind() override;
        void Unbind() override;

        const unsigned int GetConstantCount() const override;
        const char* GetConstantName(const unsigned int handle) const override;
        const InputType GetConstantType(const unsigned int handle) const override;
        const RegisterType GetConstantRegisterType(const unsigned int handle) const override;
        const unsigned int GetConstantRegisterIndex(const unsigned int handle) const override;
        const unsigned int GetConstantRegisterCount(const unsigned int handle) const override;
        const unsigned int GetConstantRowCount(const unsigned int handle) const override;
        const unsigned int GetConstantColumnCount(const unsigned int handle) const override;
        const unsigned int GetConstantArrayElementCount(const unsigned int handle) const override;
        const unsigned int GetConstantStructMemberCount(const unsigned int handle) const override;
        const unsigned int GetConstantSizeBytes(const unsigned int handle) const override;

        void SetFloat(const unsigned int registerIndex, const float* const data, const unsigned int registerCount) override;
        void SetInt(const unsigned int registerIndex, const int* const data, const unsigned int registerCount) override;
        void SetBool(const unsigned int registerIndex, const bool* const data, const unsigned int registerCount) override;
        void SetTexture(const unsigned int registerIndex, const Texture* const tex) override;

        friend class ResourceManagerNVRHI;
    };
}

#endif // ENABLE_NVRHI

#endif // SHADERPROGRAMNVRHI_H
