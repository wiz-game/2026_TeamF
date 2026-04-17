/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "ComputeShader.h"

namespace basecross {
    bool ComputeShaderUtil::CreateElementBuffer(ID3D11Device* device, D3D11_BUFFER_DESC* desc, BufferContext& context) {
        desc->ByteWidth = context.GetFullDataSize();
        desc->Usage = D3D11_USAGE_DEFAULT;
        desc->BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        desc->MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc->StructureByteStride = context.m_BufferSize;

        auto result = device->CreateBuffer(desc, nullptr, &context.m_Buffer);

        return SUCCEEDED(result);
    }
    bool ComputeShaderUtil::CreateSRV(ID3D11Device* device, BufferContext& context) {
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderBufferDesc = {};
        shaderBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        shaderBufferDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        shaderBufferDesc.Buffer.FirstElement = 0;
        shaderBufferDesc.Buffer.NumElements = context.m_InputDesc.ByteWidth / context.m_InputDesc.StructureByteStride;
        shaderBufferDesc.Buffer.ElementWidth = context.GetFullDataSize();

        auto result = device->CreateShaderResourceView(context.m_Buffer.Get(), &shaderBufferDesc, &context.m_SRV);

        return SUCCEEDED(result);
    }
    bool ComputeShaderUtil::CreateUAV(ID3D11Device* device, BufferContext& context) {
        D3D11_UNORDERED_ACCESS_VIEW_DESC accessBufferDesc = {};
        accessBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        accessBufferDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        accessBufferDesc.Buffer.NumElements = context.m_ArraySize;

        auto result = device->CreateUnorderedAccessView(context.m_Buffer.Get(), &accessBufferDesc, &context.m_UAV);

        return SUCCEEDED(result);
    }
    bool ComputeShaderUtil::CreateReadBackBuffer(ID3D11Device* device, BufferContext& context) {
        D3D11_BUFFER_DESC resultBufferDesc = {};
        resultBufferDesc.ByteWidth = context.GetFullDataSize();
        resultBufferDesc.Usage = D3D11_USAGE_STAGING;
        resultBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        resultBufferDesc.StructureByteStride = context.m_BufferSize;

        auto result = device->CreateBuffer(&resultBufferDesc, nullptr, &context.m_ReadBackBuffer);

        return SUCCEEDED(result);
    }

    BufferContext::BufferContext(UINT bufferSize, UINT arraySize) : m_BufferSize(bufferSize),m_ArraySize(arraySize) {
        auto devResource = App::GetApp()->GetDeviceResources();
        auto device = devResource->GetD3DDevice();

        m_InputDesc = {};
        ComputeShaderUtil::CreateElementBuffer(device, &m_InputDesc, *this);
    }
}
//end basecross
