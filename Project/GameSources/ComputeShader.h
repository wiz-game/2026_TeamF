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
    struct BufferContext;
    class ComputeShaderUtil {
    public:
        static bool CreateElementBuffer(ID3D11Device* device, D3D11_BUFFER_DESC* desc, BufferContext& context);
        static bool CreateSRV(ID3D11Device* device, BufferContext& context);
        static bool CreateUAV(ID3D11Device* device, BufferContext& context);
        static bool CreateReadBackBuffer(ID3D11Device* device, BufferContext& context);
    };

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
    struct BufferContext {
        ComPtr<ID3D11Buffer> m_Buffer;
        ComPtr<ID3D11ShaderResourceView> m_SRV;
        ComPtr<ID3D11UnorderedAccessView> m_UAV;
        ComPtr<ID3D11Buffer> m_ReadBackBuffer;
        D3D11_BUFFER_DESC m_InputDesc;
        UINT m_BufferSize;
        UINT m_ArraySize;

        BufferContext(UINT bufferSize, UINT arraySize);

        bool CreateSRV() {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto device = devResource->GetD3DDevice();

            if (ComputeShaderUtil::CreateSRV(device, *this)) {
                return false;
            }

            return true;
        }
        bool CreateUAV() {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto device = devResource->GetD3DDevice();

            if (!ComputeShaderUtil::CreateUAV(device, *this)) {
                return false;
            }
            if (!ComputeShaderUtil::CreateReadBackBuffer(device, *this)) {
                return false;
            }

            return true;
        }

        void ResetUAV() {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();
            UINT clear[4] = { 0,0,0,0 };
            devContext->ClearUnorderedAccessViewUint(m_UAV.Get(), clear);
        }

        UINT GetFullDataSize() const{
            return m_BufferSize * m_ArraySize;
        }
    };

    template<typename InputType>
    class DX11ComputeShader {
        ComPtr<ID3D11Buffer> m_ConstantBuffer;
        ComPtr<ID3D11ComputeShader> m_Shader;

        vector<ComPtr<ID3D11ShaderResourceView>> m_SRVs;
        vector<ComPtr<ID3D11UnorderedAccessView>> m_UAVs;

        ThreadGroupContext m_ThreadGroupContext;
        UINT m_ResultSize;

        bool m_UseTexture;

        void Bind(ID3D11DeviceContext2* context) {
            vector<ID3D11ShaderResourceView*> shaderViews;
            for (auto& srv : m_SRVs) {
                shaderViews.push_back(srv.Get());
            }
            context->CSSetShaderResources(0, shaderViews.size(), shaderViews.data());

            vector<ID3D11UnorderedAccessView*> accessViews;
            for (auto& uav : m_UAVs) {
                accessViews.push_back(uav.Get());
            }
            context->CSSetUnorderedAccessViews(0, accessViews.size(), accessViews.data(), nullptr);
        }
        void UnBind(ID3D11DeviceContext2* context) {
            vector<ID3D11UnorderedAccessView*> accessViewNULL(m_UAVs.size(), nullptr);
            context->CSSetUnorderedAccessViews(0, accessViewNULL.size(), accessViewNULL.data(), nullptr);
            vector < ID3D11ShaderResourceView*> shaderViewNULL(m_SRVs.size(), nullptr);
            context->CSSetShaderResources(0, shaderViewNULL.size(), shaderViewNULL.data());
        }
    public:
        DX11ComputeShader() : m_UseTexture(false) {}

        bool Initialize(const ThreadGroupContext& threadGroupCount) {
            if (threadGroupCount.IsZero()) {
                return false;
            }
            m_ThreadGroupContext = threadGroupCount;
        }

        void Execute(const vector<InputType>& inputData) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();

            //シェーダーのバインドを解除(一応)
            UnBind(devContext);
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
        void GetResult(vector<Type>& result, BufferContext& context) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();

            result.resize(context.m_ArraySize);
            D3D11_MAPPED_SUBRESOURCE mappedResource = { 0 };
            devContext->CopyResource(context.m_ReadBackBuffer.Get(), context.m_Buffer.Get());
            if (SUCCEEDED(devContext->Map(context.m_ReadBackBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource))) {
                memcpy(&result[0], mappedResource.pData, context.GetFullDataSize());
                devContext->Unmap(context.m_ReadBackBuffer.Get(), 0);
            }
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

        void UploadBufferData(ID3D11Buffer* buffer,void* data) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();

            devContext->UpdateSubresource(buffer, 0, nullptr, data, 0, 0);
        }
        
        void AddSRV(ID3D11ShaderResourceView* srv) {
            m_SRVs.push_back(srv);
        }
        void AddUAV(ID3D11UnorderedAccessView* uav) {
            m_UAVs.push_back(uav);
        }

        void SetSRV(int index, ID3D11ShaderResourceView* srv) {
            if (index < 0 || index > m_SRVs.size()) return;

            m_SRVs[index] = srv;
        }
        void SetUAV(int index, ID3D11UnorderedAccessView* uav) {
            if (index < 0 || index > m_UAVs.size()) return;

            m_UAVs[index] = uav;
        }
    };
}

//end basecross
