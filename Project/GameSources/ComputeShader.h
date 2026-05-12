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

    struct BufferContext {
        ComPtr<ID3D11Buffer> m_Buffer;
        ComPtr<ID3D11ShaderResourceView> m_SRV;
        ComPtr<ID3D11UnorderedAccessView> m_UAV;
        ComPtr<ID3D11Buffer> m_ReadBackBuffer;
        D3D11_BUFFER_DESC m_InputDesc;
        size_t m_BufferSize;
        size_t m_ArraySize;
        BufferContext();
        BufferContext(size_t bufferSize, size_t arraySize);

        bool CreateSRV();
        bool CreateUAV();

        void ResetUAV();

        void Upload(void* data);
        void ReadBuffer(void* data);

        size_t GetFullDataSize() const;
    };

    class DX11ComputeShader {
        ComPtr<ID3D11Buffer> m_ConstantBuffer;
        ComPtr<ID3D11ComputeShader> m_Shader;

        vector<ComPtr<ID3D11ShaderResourceView>> m_SRVs;
        vector<ComPtr<ID3D11UnorderedAccessView>> m_UAVs;

        ThreadGroupContext m_ThreadGroupContext;

        void Bind(ID3D11DeviceContext2* context) {
            context->CSSetShader(m_Shader.Get(), nullptr, 0);

            vector<ID3D11ShaderResourceView*> shaderViews;
            for (auto& srv : m_SRVs) {
                shaderViews.push_back(srv.Get());
            }
            context->CSSetShaderResources(0, (UINT)shaderViews.size(), shaderViews.data());

            vector<ID3D11UnorderedAccessView*> accessViews;
            for (auto& uav : m_UAVs) {
                accessViews.push_back(uav.Get());
            }
            context->CSSetUnorderedAccessViews(0, (UINT)accessViews.size(),accessViews.data(), nullptr);

            if (m_ConstantBuffer) {
                context->CSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());
            }
        }
        void UnBind(ID3D11DeviceContext2* context) {
            ID3D11UnorderedAccessView* accessViewNulls[8]{};
            ID3D11ShaderResourceView* shaderViewNulls[8]{};
            context->CSSetUnorderedAccessViews(0, 8, accessViewNulls, nullptr);
            context->CSSetShaderResources(0, 8, shaderViewNulls);
        }
    public:
        DX11ComputeShader(){}

        bool Initialize(const ThreadGroupContext& threadGroupCount) {
            if (threadGroupCount.IsZero()) {
                return false;
            }
            m_ThreadGroupContext = threadGroupCount;
            return true;
        }

        void Execute() {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();

            //シェーダーのバインド
            Bind(devContext);

            //実行
            devContext->Dispatch(
                m_ThreadGroupContext.GetDispatchCountX(),
                m_ThreadGroupContext.GetDispatchCountY(),
                m_ThreadGroupContext.GetDispatchCountZ());
            //待ち
            devContext->Flush();

            //シェーダーのバインドを解除
            UnBind(devContext);
        }
       
        template<typename ConstantType>
        void SetConstantBuffer(ConstantType& cb, ID3D11Buffer* buffur) {
            auto devResource = App::GetApp()->GetDeviceResources();
            auto devContext = devResource->GetD3DDeviceContext();

            m_ConstantBuffer = buffur;
            devContext->UpdateSubresource(m_ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
        }
        void SetShader(ComPtr<ID3D11ComputeShader> shader) {
            m_Shader = shader;
        }
        
        void AddSRV(ID3D11ShaderResourceView* srv) {
            m_SRVs.push_back(srv);
        }
        void AddUAV(ID3D11UnorderedAccessView* uav) {
            m_UAVs.push_back(uav);
        }

        void SetSRV(int index, ID3D11ShaderResourceView* srv) {
            if (index < 0 || index >= m_SRVs.size()) return;

            m_SRVs[index] = srv;
        }
        void SetUAV(int index, ID3D11UnorderedAccessView* uav) {
            if (index < 0 || index >= m_UAVs.size()) return;

            m_UAVs[index] = uav;
        }
    };
}

//end basecross
