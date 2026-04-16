/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {
    /*
    * コンピュートシェーダーの使い方
    * 1.インスタンス作成
    * 2.初期化
    * 3.シェーダーと定数バッファのセット
    * 4.実行
    */
    struct ThreadGroupContext {
        UINT m_ThreadGroupCountX = 1;
        UINT m_ThreadGroupCountY = 1;
        UINT m_ThreadGroupCountZ = 1;

        UINT m_DataSizeX = 1;
        UINT m_DataSizeY = 1;
        UINT m_DataSizeZ = 1;

        UINT GetFullDataSize() const {
            return m_DataSizeX * m_DataSizeY * m_DataSizeZ;
        }
        bool IsZero() const {
            return m_ThreadGroupCountX == 0 || m_ThreadGroupCountY == 0 || m_ThreadGroupCountZ == 0 ||
                m_DataSizeX == 0 || m_DataSizeY == 0 || m_DataSizeZ == 0;
        }
        UINT GetDispatchCountX() const {
            return (m_DataSizeX + m_ThreadGroupCountX - 1) / m_ThreadGroupCountX;
        }
        UINT GetDispatchCountY() const {
            return (m_DataSizeY + m_ThreadGroupCountY - 1) / m_ThreadGroupCountY;
        }
        UINT GetDispatchCountZ() const {
            return (m_DataSizeZ + m_ThreadGroupCountZ - 1) / m_ThreadGroupCountZ;
        }
    };

    enum BufferType {
        Default,PingPong
    };
    struct ResultBufferContext {
        ComPtr<ID3D11Buffer> m_OutputBuffer;
        ComPtr<ID3D11UnorderedAccessView> m_AccessView;
        ComPtr<ID3D11Buffer> m_ReadBackBuffer;

        UINT m_ResultBufferSize = 1;
        void* m_ResultBuffer = nullptr;
        UINT m_ResultArraySize = 1;

        ResultBufferContext(void* data, UINT bufferSize, UINT arraySize) :
            m_ResultBuffer(data), m_ResultBufferSize(bufferSize), m_ResultArraySize(arraySize) {
        }

        UINT GetByteWidth() const {
            return m_ResultArraySize * m_ResultBufferSize;
        }

    };

    template<typename InputType>
    class DX11ComputeShader {
        ComPtr<ID3D11Buffer> m_InputBuffer;
        ComPtr<ID3D11Buffer> m_ConstantBuffer;
        ComPtr<ID3D11ComputeShader> m_Shader;
        ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;

        vector<ResultBufferContext> m_ResultBufferContexts;
        ThreadGroupContext m_ThreadGroupContext;
        UINT m_ResultSize;

        bool m_UseTexture;

        void Bind(ID3D11DeviceContext2* context) {
            context->CSSetShaderResources(0, 1, m_ShaderResourceView.GetAddressOf());

            vector<ID3D11UnorderedAccessView*> accessViews;
            for (auto& context : m_ResultBufferContexts) {
                accessViews.push_back(context.m_AccessView.Get());
            }
            context->CSSetUnorderedAccessViews(0, accessViews.size(), accessViews.data(), nullptr);
        }
        void UnBind(ID3D11DeviceContext2* context) {
            vector<ID3D11UnorderedAccessView*> accessViewNULL(m_ResultBufferContexts.size(), nullptr);
            context->CSSetUnorderedAccessViews(0, accessViewNULL.size(), accessViewNULL.data(), nullptr);
            ID3D11ShaderResourceView* shaderViewNULL[1] = { nullptr };
            context->CSSetShaderResources(0, 1, shaderViewNULL);
        }

        void CreateResultBuffers(ResultBufferContext& context) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto device = devResource->GetD3DDevice();
            auto devContext = devResource->GetD3DDeviceContext();

            D3D11_BUFFER_DESC outputBufferDesc = {};
            CreateElementBuffer(device, &outputBufferDesc, context.m_OutputBuffer.GetAddressOf(), context.m_ResultBufferSize, context.m_ResultArraySize);
            CreateAccessView(device, context);
            CreateReadBackBuffer(device, context);
        }
        bool CreateElementBuffer(ID3D11Device* device, D3D11_BUFFER_DESC* desc, ID3D11Buffer** buffer, UINT bufferSize, UINT arraySize) {
            desc->ByteWidth = bufferSize * arraySize;
            desc->Usage = D3D11_USAGE_DEFAULT;
            desc->BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
            desc->MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            desc->StructureByteStride = bufferSize;

            auto result = device->CreateBuffer(desc, nullptr, buffer);

            return SUCCEEDED(result);
        }
        template<typename BufferType>
        bool CreateElementBuffer(ID3D11Device* device, D3D11_BUFFER_DESC* desc, ID3D11Buffer** buffer, UINT fullDataSize) {
            desc->ByteWidth = sizeof(BufferType) * fullDataSize;
            desc->Usage = D3D11_USAGE_DEFAULT;
            desc->BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
            desc->MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            desc->StructureByteStride = sizeof(BufferType);

            auto result = device->CreateBuffer(desc, nullptr, buffer);

            return SUCCEEDED(result);
        }
        bool CreateShaderResourceView(ID3D11Device* device, const D3D11_BUFFER_DESC* inputBufferDesc) {
            D3D11_SHADER_RESOURCE_VIEW_DESC shaderBuffurDesc = {};
            shaderBuffurDesc.Format = DXGI_FORMAT_UNKNOWN;
            shaderBuffurDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            shaderBuffurDesc.Buffer.FirstElement = 0;
            shaderBuffurDesc.Buffer.NumElements = inputBufferDesc->ByteWidth / inputBufferDesc->StructureByteStride;
            shaderBuffurDesc.Buffer.ElementWidth = m_ThreadGroupContext.GetFullDataSize();

            auto result = device->CreateShaderResourceView(m_InputBuffer.Get(), &shaderBuffurDesc, &m_ShaderResourceView);

            return SUCCEEDED(result);
        }
        bool CreateAccessView(ID3D11Device* device, ResultBufferContext& context) {
            D3D11_UNORDERED_ACCESS_VIEW_DESC accessBufferDesc = {};
            accessBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
            accessBufferDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            accessBufferDesc.Buffer.NumElements = context.m_ResultArraySize;

            auto result = device->CreateUnorderedAccessView(context.m_OutputBuffer.Get(), &accessBufferDesc, &context.m_AccessView);

            return SUCCEEDED(result);
        }
        bool CreateReadBackBuffer(ID3D11Device* device, ResultBufferContext& context) {
            D3D11_BUFFER_DESC resultBufferDesc = {};
            resultBufferDesc.ByteWidth = context.GetByteWidth();
            resultBufferDesc.Usage = D3D11_USAGE_STAGING;
            resultBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            resultBufferDesc.StructureByteStride = context.m_ResultBufferSize;

            auto result = device->CreateBuffer(&resultBufferDesc, nullptr, &context.m_ReadBackBuffer);

            return SUCCEEDED(result);
        }
    public:
        DX11ComputeShader() : m_UseTexture(false) {}

        bool Initialize(const ThreadGroupContext& threadGroupCount, UINT resultBufferSize) {
            if (resultBufferSize == 0 || threadGroupCount.IsZero()) {
                return false;
            }
            m_ThreadGroupContext = threadGroupCount;
            m_ResultSize = resultBufferSize;

            auto devResource = App::GetApp()->GetDeviceResources();
            auto device = devResource->GetD3DDevice();
            auto devContext = devResource->GetD3DDeviceContext();

            D3D11_BUFFER_DESC inputBufferDesc = {};
            D3D11_BUFFER_DESC outputBufferDesc = {};
            if (!CreateElementBuffer<InputType>(device, &inputBufferDesc, m_InputBuffer.GetAddressOf(), m_ThreadGroupContext.GetFullDataSize())) {
                return false;
            }
            if (!m_UseTexture) {
                if (!CreateShaderResourceView(device, &inputBufferDesc)) {
                    return false;
                }
            }
        }

        void Execute(const vector<InputType>& inputData) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();

            //シェーダーのバインドを解除(一応)
            UnBind(devContext);
            if (!m_UseTexture) {
                //シェーダーに情報を送る
                devContext->UpdateSubresource(m_InputBuffer.Get(), 0, nullptr, inputData.data(), 0, 0);
            }

            //シェーダーのバインド
            Bind(devContext);

            //実行
            devContext->Dispatch(
                m_ThreadGroupContext.GetDispatchCountX(),
                m_ThreadGroupContext.GetDispatchCountY(),
                m_ThreadGroupContext.GetDispatchCountZ());

            //シェーダーのバインドを解除
            UnBind(devContext);
        }

        template<typename Type>
        void GetResult(vector<Type>& result, int index) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();
            if (index < 0 || index >= m_ResultBufferContexts.size()) return;
            ResultBufferContext context = m_ResultBufferContexts[index];
            if (context.m_ResultBufferSize != sizeof(Type)) return;

            result.resize(context.m_ResultArraySize);
            D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
            devContext->CopyResource(context.m_ReadBackBuffer.Get(), context.m_OutputBuffer.Get());
            if (SUCCEEDED(devContext->Map(context.m_ReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource))) {
                memcpy(&result[0], mappedResource.pData, context.GetByteWidth());
                devContext->Unmap(context.m_ReadBackBuffer.Get(), 0);
            }
        }

        void SwapUavToSrv() {
            
        }

        template<typename ConstantType>
        void SetConstantBuffer(ConstantType& cb, ID3D11Buffer* buffur) {
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
            devContext->CSSetShader(m_Shader.Get(), nullptr, 0);
        }

        void UseTexture(ID3D11ShaderResourceView* srv) {
            m_UseTexture = true;
            m_ShaderResourceView = srv;
        }

        void RegisterResult(const ResultBufferContext& context) {
            ResultBufferContext bufferContext = context;
            CreateResultBuffers(bufferContext);
            m_ResultBufferContexts.push_back(bufferContext);
        }
        void RegisterResult(void* data, UINT dataSize, UINT arraySize) {
            RegisterResult(ResultBufferContext(data, dataSize, arraySize));
        }
        
        void ResetUAV(int index) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();

            if (index < 0 || index >= m_ResultBufferContexts.size()) return;

            ResultBufferContext context = m_ResultBufferContexts[index];
            UINT clear[4] = { 0,0,0,0 };
            devContext->ClearUnorderedAccessViewUint(context.m_AccessView.Get(), clear);
        }
        
    };
}

//end basecross
