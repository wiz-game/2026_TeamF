#include "stdafx.h"
#include "DXModelResource.h"
#include <windows.h>
#include <string>
#include <memory>

// ファイル全体の説明:
// このファイルは、DXModel（アプリ独自のメッシュ/マテリアル/ボーン情報を持つモデル表現）を
// アプリのリソースシステム（MeshResource や TextureResource）に登録するためのユーティリティを提供します。
// ローカルユーティリティ関数（パス操作や UTF-8 → UTF-16 変換など）と、
// モデルから MeshResource を生成して App に登録する処理を含みます。

using namespace dxm;

namespace
{
	// UTF-8 → std::wstring（UTF-16）変換
	// - MultiByteToWideChar を使って必要な長さを取得し、バッファへ変換を行う。
	// - 無効な入力や変換失敗時は空の文字列を返す。
	std::wstring Utf8ToWide(const std::string& utf8)
	{
		if (utf8.empty()) return {};

		const int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), (int)utf8.size(), nullptr, 0);
		if (len <= 0) return {};

		std::wstring w(len, L'\0');
		MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), (int)utf8.size(), &w[0], len);
		return w;
	}

	// パス結合ユーティリティ（簡易）
	// - baseDir が末尾にパス区切りを持っているかを確認して適切に結合する。
	// - どちらかが空ならもう一方を返す。
	std::wstring CombinePath(const std::wstring& baseDir, const std::wstring& filename)
	{
		if (baseDir.empty()) return filename;
		if (filename.empty()) return baseDir;
		const wchar_t last = baseDir.back();
		if (last == L'\\' || last == L'/') return baseDir + filename;
		return baseDir + L'\\' + filename;
	}

	// 親ディレクトリを取得する
	// - 最後のパス区切り以前の部分を返す。ルート（"C:\\" 等）は特別扱いして "C:\\" 相当を返す。
	std::wstring GetParentPath(const std::wstring& path)
	{
		if (path.empty()) return {};
		auto pos1 = path.find_last_of(L"\\/");
		if (pos1 == std::wstring::npos) return {};
		// ドライブルート (例: "C:\") の場合は長さ 3 を返す
		if (pos1 == 2 && path.size() > 2 && path[1] == L':') return path.substr(0, 3);
		return path.substr(0, pos1);
	}

	// パスからファイル名部分を取得する（最後の区切り以降）
	std::wstring GetFileNameFromPath(const std::wstring& path)
	{
		if (path.empty()) return {};
		auto pos = path.find_last_of(L"\\/");
		if (pos == std::wstring::npos) return path;
		return path.substr(pos + 1);
	}

	// ファイル/ディレクトリの存在確認
	// - GetFileAttributesW が INVALID_FILE_ATTRIBUTES を返すかで判定する。
	bool PathExists(const std::wstring& path)
	{
		if (path.empty()) return false;
		DWORD attrs = GetFileAttributesW(path.c_str());
		return (attrs != INVALID_FILE_ATTRIBUTES);
	}

	// テクスチャファイルの単純なフォールバック探索
	// - まず baseDir と filename を直接結合して存在確認。
	// - 見つからなければ baseDir 配下のファイル一覧を走査して同名ファイルを探す。
	// - 見つかった場合はフルパスを返す。見つからなければ空文字列。
	std::wstring FindTexturePathFallback(const std::wstring& baseDir, const std::wstring& filename)
	{
		if (filename.empty()) return {};

		// まず直接結合して存在するか確認
		{
			const std::wstring direct = CombinePath(baseDir, filename);
			if (PathExists(direct)) return direct;
		}

		// ワイルドカード付きでディレクトリを列挙する
		std::wstring search = baseDir;
		if (search.empty()) search = L".";
		if (search.back() != L'\\' && search.back() != L'/') search.push_back(L'\\');
		search.push_back(L'*');

		WIN32_FIND_DATAW fd;
		HANDLE hFind = FindFirstFileW(search.c_str(), &fd);
		if (hFind == INVALID_HANDLE_VALUE) return {};

		std::wstring result;
		do
		{
			// ディレクトリは無視
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
			std::wstring cand = fd.cFileName;
			// 同名ファイルがあればそのパスを組み立てて返す
			if (cand == filename)
			{
				// search は "...\\*" の形式なので GetParentPath(search) で親ディレクトリが得られる
				result = CombinePath(GetParentPath(search), cand);
				break;
			}
		} while (FindNextFileW(hFind, &fd) != 0);

		FindClose(hFind);
		return result;
	}
}

namespace basecross
{
	// コンストラクタ / デストラクタ（特に追加の初期化はなし）
	DXModelResource::DXModelResource()
		: MultiMeshResource()
		, m_dxModel()
	{
	}

	DXModelResource::~DXModelResource()
	{
	}

	// 内部に保持する DXModel を設定/取得するアクセサ
	void DXModelResource::SetDXModel(const shared_ptr<DXModel>& model)
	{
		m_dxModel = model;
	}

	shared_ptr<DXModel> DXModelResource::GetDXModel() const
	{
		return m_dxModel;
	}

	// 既に生成済みの DXModel オブジェクトをリソースとして登録する
	std::shared_ptr<DXModelResource> DXModelResource::RegisterDXModel(
		const wstring& key,
		const shared_ptr<DXModel>& model,
		bool accessWrite
	)
	{
		// 引数チェック: model が null なら例外
		if (!model)
		{
			throw BaseException(
				L"DXModel が null です",
				L"if (!model)",
				L"DXModelResource::RegisterDXModel()"
			);
		}

		// 新しい DXModelResource を生成して DXModel をセット
		auto res = ObjectFactory::Create<DXModelResource>();
		res->SetDXModel(model);

		// モデルのベースディレクトリ/パス（テクスチャ探索等に使用）
		const std::wstring baseDir = model->m_filePath;

		// モデルに含まれるメッシュを一つずつ MeshResource に変換して登録する
		for (size_t mi = 0; mi < model->meshes.size(); ++mi)
		{
			const auto& mesh = model->meshes[mi];

			// 頂点配列の構築（頂点属性の変換）
			vector<VertexPositionNormalTextureSkinning> vertices;
			vertices.resize(mesh.vertices.size());
			for (size_t vi = 0; vi < mesh.vertices.size(); ++vi)
			{
				const auto& v = mesh.vertices[vi];

				VertexPositionNormalTextureSkinning out{};
				// 位置・法線・UV を内部表現にコピー
				out.position = bsm::Vec3(v.pos.x, v.pos.y, v.pos.z);
				out.normal = bsm::Vec3(v.normal.x, v.normal.y, v.normal.z);
				out.textureCoordinate = bsm::Vec2(v.uv.x, v.uv.y);

				// ボーンインデックスは uint32_t に格納（頂点シェイプの形式に合わせる）
				out.indices[0] = static_cast<uint32_t>(v.boneIndices.x);
				out.indices[1] = static_cast<uint32_t>(v.boneIndices.y);
				out.indices[2] = static_cast<uint32_t>(v.boneIndices.z);
				out.indices[3] = static_cast<uint32_t>(v.boneIndices.w);

				// ボーンウェイトをセット
				out.weights[0] = v.weights.x;
				out.weights[1] = v.weights.y;
				out.weights[2] = v.weights.z;
				out.weights[3] = v.weights.w;

				vertices[vi] = out;
			}

			// インデックス配列の構築（uint16_t に変換）
			// - 元のインデックスが 0xFFFF を超える場合は 0 に丸める（オーバーフロー回避）
			vector<uint16_t> indices;
			indices.resize(mesh.indices.size());
			for (size_t ii = 0; ii < mesh.indices.size(); ++ii)
			{
				if (mesh.indices[ii] > 0xFFFFu)
				{
					// 注意: 本来は 32bit インデックスをサポートするべきだが、
					// 現在の MeshResource が 16bit 前提ならここで安全化しておく。
					indices[ii] = 0;
					continue;
				}
				indices[ii] = static_cast<uint16_t>(mesh.indices[ii]);
			}

			// MeshResource を生成（テンプレートで頂点レイアウトを指定）
			auto meshRes = MeshResource::CreateMeshResource<VertexPositionNormalTextureSkinning>(vertices, indices, accessWrite);

			// 生成した MeshResource に対してメッシュ固有の追加情報を設定
			auto data = meshRes->GetMashData();
			data.m_IsSkining = true; // スキニング（ボーン）を使用
			data.m_BoneCount = static_cast<UINT>(model->bones.size());
			data.m_SampleCount = 1; // サンプル数（アニメーション補間等に使用）
			data.m_MultiMeshIndex = static_cast<UINT>(mi); // 元モデル内のメッシュインデックス

			// サンプル行列ベクタをボーン数分確保して初期化（単位行列）
			data.m_SampleMatrixVec.clear();
			data.m_SampleMatrixVec.resize(static_cast<size_t>(data.m_BoneCount));
			for (auto& m : data.m_SampleMatrixVec)
			{
				m.identity();
			}

			// マテリアル情報をクリアして構築
			data.m_MaterialExVec.clear();

			MaterialEx mat{};
			// デフォルト値を設定
			mat.m_StartIndex = 0;
			mat.m_IndexCount = static_cast<UINT>(indices.size());
			mat.m_Diffuse = bsm::Col4(1.0f, 1.0f, 1.0f, 1.0f);
			mat.m_Specular = bsm::Col4(0.0f, 0.0f, 0.0f, 1.0f);
			mat.m_Ambient = bsm::Col4(0.0f, 0.0f, 0.0f, 1.0f);
			mat.m_Emissive = bsm::Col4(0.0f, 0.0f, 0.0f, 1.0f);

			// モデルのマテリアル配列に該当インデックスがあればそこから色やテクスチャ情報を読み取る
			if (mesh.materialIndex < model->materials.size())
			{
				const auto& srcMat = model->materials[mesh.materialIndex];

				// カラー情報をコピー
				mat.m_Diffuse = bsm::Col4(srcMat.diffuseColor.x, srcMat.diffuseColor.y, srcMat.diffuseColor.z, srcMat.diffuseColor.w);
				mat.m_Specular = bsm::Col4(srcMat.specularColor.x, srcMat.specularColor.y, srcMat.specularColor.z, srcMat.specularColor.w);
				mat.m_Ambient = bsm::Col4(srcMat.ambientColor.x, srcMat.ambientColor.y, srcMat.ambientColor.z, srcMat.ambientColor.w);
				mat.m_Emissive = bsm::Col4(srcMat.emissiveColor.x, srcMat.emissiveColor.y, srcMat.emissiveColor.z, srcMat.emissiveColor.w);

				// テクスチャファイル名が指定されている場合、ファイルを見つけてテクスチャリソースを登録する
				if (!srcMat.textureFilename.empty())
				{
					// まずモデルのベースディレクトリと結合してフルパスを作成
					std::wstring texFullPath = CombinePath(baseDir, srcMat.textureFilename);

					// 直接存在しなければ親ディレクトリを使ってフォールバック検索を行う
					if (!PathExists(texFullPath))
					{
						const std::wstring parent = GetParentPath(baseDir);
						if (!parent.empty())
						{
							std::wstring fallback = FindTexturePathFallback(parent, srcMat.textureFilename);
							if (!fallback.empty())
							{
								texFullPath = fallback;
							}
						}
					}

					// 最終的にファイルが見つかればアプリのテクスチャリソースとして登録する
					if (PathExists(texFullPath))
					{
						// テクスチャのリソースキーは元キーに ":tex:" とファイル名を付加して一意化
						const wstring texKey = key + L":tex:" + GetFileNameFromPath(texFullPath);
						auto texRes = App::GetApp()->RegisterTexture(texKey, texFullPath, L"WIC");
						mat.m_TextureResource = texRes;
					}
				}
			}

			// マテリアルをメッシュデータに追加
			data.m_MaterialExVec.push_back(std::move(mat));

			// MeshResource にメッシュデータを設定してリソースに追加
			meshRes->SetMashData(data);
			res->AddMesh(meshRes);
		}

		// 最後に App にこの DXModelResource を登録して返す
		App::GetApp()->RegisterResource(key, res);
		return res;
	}
}