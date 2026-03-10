
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <windows.h>
#include <DirectXMath.h>

namespace dxm
{
	// 独自例外: std::exception と互換かつワイド文字列メッセージを保持
	class Exception : public std::exception
	{
		std::wstring m_msgW;
		std::string m_msgA;
	public:
		Exception(const std::wstring& m1, const std::wstring& m2 = L"", const std::wstring& m3 = L"") noexcept;
		const char* what() const noexcept override { return m_msgA.c_str(); }
		const std::wstring& what_w() const noexcept { return m_msgW; }
	};

	// 例外メッセージ表示ユーティリティ（DXModel 単体で使用可能）
	// owner は表示先ウィンドウハンドル、nullptr 可
	void ShowExceptionMessageBox(HWND owner, const std::exception& e, const std::wstring& title = L"エラー");
	void ShowUnknownExceptionMessageBox(HWND owner, const std::wstring& title = L"エラー");

	struct RuntimeMaterial
	{
		std::wstring name;

		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 specularColor;
		DirectX::XMFLOAT4 emissiveColor;

		float opacity;
		float shininess;

		std::wstring textureFilename;
	};

	struct RuntimeVertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
		DirectX::XMINT4 boneIndices;
		DirectX::XMFLOAT4 weights;
	};

	struct RuntimeMesh
	{
		std::wstring name;
		std::uint32_t materialIndex;
		std::vector<RuntimeVertex> vertices;
		std::vector<std::uint32_t> indices;
	};

	struct RuntimeBone
	{
		std::wstring name;
		int parentIndex;
		DirectX::XMMATRIX localTransform;
		DirectX::XMMATRIX offsetMatrix;
	};

	struct RuntimeKeyVector
	{
		float time;
		DirectX::XMFLOAT3 value;
	};

	struct RuntimeKeyQuaternion
	{
		float time;
		DirectX::XMFLOAT4 value;
	};

	struct RuntimeAnimationChannel
	{
		std::wstring nodeName;
		std::vector<RuntimeKeyVector> positionKeys;
		std::vector<RuntimeKeyQuaternion> rotationKeys;
		std::vector<RuntimeKeyVector> scalingKeys;
	};

	struct RuntimeAnimation
	{
		std::wstring name;
		float m_duration;
		float ticksPerSecond;
		std::vector<RuntimeAnimationChannel> channels;
	};

	struct DXModel
	{
		// 実装側（DXModel.cpp）と一致する名前に修正しました
		std::wstring m_filePath;           // モデル内に保存されているパス情報（従来の用途）
		std::wstring m_sourceFileName;     // 実際に作成／読み込みに使ったファイルのフルパスまたは元名

		std::vector<RuntimeMaterial> materials;
		std::vector<RuntimeMesh> meshes;

		std::vector<RuntimeBone> bones;
		std::vector<RuntimeAnimation> animations;

		// DXM シリアライズ: 保存 / 読み込み
		bool SaveToDxm(const std::wstring& path) const;

		// ファイル読み込み（DXM フォーマットを読み込む）
		// textureDir: 有効なディレクトリパスが渡された場合、そちらからテクスチャを探索する（既存呼び出し互換のためデフォルト引数あり）
		static std::shared_ptr<DXModel> CreateFromFile(const std::wstring& path, const std::wstring& textureDir = L"");

		// モデルの構成要素名をダンプしてテキストファイルとして出力するユーティリティ
		// outDir は出力先ディレクトリ（末尾の区切りは不要）
		void DumpModelNames(const std::wstring& outDir) const;
	};
}