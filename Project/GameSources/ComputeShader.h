/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {


    template<typename ResultType>
    class DX11ComputeShader {
        ComPtr<ID3D11Buffer> m_InputBuffer;
        ComPtr<ID3D11Buffer> m_OutputBuffer;
        ComPtr<ID3D11Buffer> m_ReadBackBuffer;
        ComPtr<ID3D11Buffer> m_ConstantBuffer;
		ComPtr<ID3D11ComputeShader> m_Shader;
        ComPtr<ID3D11UnorderedAccessView> m_AccessView;
        ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;

        size_t m_InitializeSize;
		size_t m_ResultSize;

        void Bind(ID3D11DeviceContext2* context) {
            context->CSSetShaderResources(0, 1, m_ShaderResourceView.GetAddressOf());
            context->CSSetUnorderedAccessViews(0, 1, m_AccessView.GetAddressOf(), nullptr);
        }
        void UnBind(ID3D11DeviceContext2* context) {
            D3D11UnorderedAccessView* acceseViewNULL[1] = { nullptr };
            context->CSSetUnorderedAccessViews(0, 1, acceseViewNULL, nullptr);
            D3D11ShaderResourceView* shaderViewNULL[1] = { nullptr };
            context->CSSetShaderResources(0, 1, shaderViewNULL);
		}

        template<typename ResultType>
        void GetResult(vector<ResultType>& result, ID3D11DeviceContext2* context) {
            result.resize(m_ResultCount);
            D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
            context->CopyResource(m_ReadBackBuffer.Get(), m_OutputBuffer.Get());
            if (SUCCEEDED(context->Map(m_ReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource))) {
                memcpy(&result[0], mappedResource.pData, sizeof(T) * m_ResultCount);
                context->Unmap(m_ReadBackBuffer.Get(), 0);
            }
        }

        bool CreateElementBuffer(ID3D11Device* device,D3D11_BUFFER_DESC* desc, ID3D11Buffer* buffer) {
            desc.ByteWidth = sizeof(ResultType) * m_InitializeSize;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
            desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            desc.StructureByteStride = sizeof(ResultType);

            auto result = device->CreateBuffer(&bufferDesc, nullptr, &buffer);

            return SUCCEEDED(result);
        }
        bool CreateShaderResourceView(ID3D11Device* device, const D3D11_BUFFER_DESC* inputBufferDesc) {
            D3D11_SHADER_RESOURCE_VIEW_DESC shaderBuffurDesc = {};
            shaderBuffurDesc.Format = DXGI_FORMAT_UNKNOWN;
            shaderBuffurDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            shaderBuffurDesc.Buffer.FirstElement = 0;
            shaderBuffurDesc.Buffer.NumElements = inputBufferDesc.ByteWidth / inputBufferDesc.StructureByteStride;
            shaderBuffurDesc.Buffer.ElementWidth = m_InitializeSize;

            auto result = device->CreateShaderResourceView(m_InputBuffer.Get(), &shaderBuffurDesc, &m_ShaderResourceView);

            return SUCCEEDED(result);
        }
        bool CreateAccessView(ID3D11Device* device) {
            D3D11_UNORDERED_ACCESS_VIEW_DESC accessBufferDesc = {};
            accessBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
            accessBufferDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            accessBufferDesc.Buffer.NumElements = m_ResultCount;

            auto result = device->CreateUnorderedAccessView(m_OutputBuffer.Get(), &accessBufferDesc, &m_AccessView);

			return SUCCEEDED(result);
		}
        bool CreateReadBackBuffer(ID3D11Device* device) {
            D3D11_BUFFER_DESC resultBufferDesc = {};
            resultBufferDesc.ByteWidth = sizeof(ResultType) * m_ResultCount;
            resultBufferDesc.Usage = D3D11_USAGE_STAGING;
            resultBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            resultBufferDesc.StructureByteStride = sizeof(ResultType);

            auto result device->CreateBuffer(&resultBufferDesc, nullptr, &m_ResultBuffer);

			return SUCCEEDED(result);
        }
    public:
        bool Initialize(size_t outputBufferSize, size_t resultBufferSize) {
            if (outputBufferSize == 0 || resultBufferSize == 0) {
                return false;
            }
            m_InitializeSize = outputBufferSize;
            m_ResultSize = resultBufferSize;

            auto devResource = App::GetApp()->GetDeviceResources();
            auto device = devResource->GetD3DDevice();
            auto devContext = devResource->GetD3DDeviceContext();

			D3D11_BUFFER_DESC inputBufferDesc = {};
			D3D11_BUFFER_DESC outputBufferDesc = {};
            if (CreateElementBuffer(inputBufferDesc,m_InputBuffer.Get())) {
				return false;
            }
			if (CreateElementBuffer(outputBufferDesc,m_OutputBuffer.Get())) {
                return false;
            }
			if (CreateShaderResourceView(&inputBufferDesc)) {
                return false;
            }
			if (CreateAccessView()) {
                return false;
            }
			if (CreateReadBackBuffer()) {
                return false;
            }
        }

        template<typename InputType>
        vector<ResultType> Execute(vector<InputType>& inputData) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();

            //シェーダーのバインドを解除(一応)
            UnBind(devContext);

            //シェーダーに情報を送る
            devContext->UpdateSubresource(m_InputBuffer.Get(), 0, nullptr, inputData.data(), 0, 0);

            //シェーダーのバインド
			Bind(devContext);

            //実行
            UINT dispachCount = (UINT)m_InitializeSize + 63;//スレッドが足りないとき用
            devContext->Dispatch(dispachCount / 64, 1, 1);
            //結果の取得
            vector<ResultType> result = {};
			GetResult(result);

			//シェーダーのバインドを解除
            UnBind(devContext);

            return result;
        }

        template<typename ConstantType>
        void SetConstantBuffer(ConstantType cb, ID3D11Buffer* buffur) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();

            m_ConstantBuffer = buffur;
            devContext->UpdateSubresource(m_ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
            devContext->CSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());
        }
        void SetShader(ComPtr<ID3D11ComputeShader> shader) {
            m_Shader = shader;

            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();
            //コンピュータシェーダーの設定
            devContext->CSSetShader(m_Shader, nullptr, 0);
        }
    };
}

//end basecross
