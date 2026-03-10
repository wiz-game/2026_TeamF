#include "DXModel.h"
#include <string>
#include <fstream>
#include <limits>
#include <windows.h>
#include <DirectXMath.h>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#include <unordered_set>
#include <algorithm>

namespace dxm
{
	using namespace DirectX;

	// -------------------------------------------------------------------------
	// Wide <-> UTF-8 ヘルパー（内部で使う）
	// -------------------------------------------------------------------------
	namespace
	{
		inline std::string WideToUtf8(const std::wstring& w)
		{
			if (w.empty())
			{
				return {};
			}
			const int len = WideCharToMultiByte(CP_UTF8, 0, w.data(), static_cast<int>(w.size()), nullptr, 0, nullptr, nullptr);
			if (len <= 0)
			{
				return {};
			}
			std::string s(static_cast<size_t>(len), '\0');
			WideCharToMultiByte(CP_UTF8, 0, w.data(), static_cast<int>(w.size()), &s[0], len, nullptr, nullptr);
			return s;
		}

		inline std::wstring Utf8ToWide(const std::string& utf8)
		{
			if (utf8.empty())
			{
				return {};
			}
			const int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
			if (len <= 0)
			{
				return {};
			}
			std::wstring w(static_cast<size_t>(len), L'\0');
			MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), static_cast<int>(utf8.size()), &w[0], len);
			return w;
		}

		// ローカル表示ユーティリティ（DXModel 単体完結）
		inline void ShowErrorDialog(HWND owner, const std::wstring& m1, const std::wstring& m2 = L"", const std::wstring& m3 = L"")
		{
			std::wstring msg = m1;
			if (!m2.empty())
			{
				msg += L"\r\n";
				msg += m2;
			}
			if (!m3.empty())
			{
				msg += L"\r\n";
				msg += m3;
			}
			MessageBoxW(owner, msg.c_str(), L"DXModel エラー", MB_OK | MB_ICONERROR);
		}

		// 指定ディレクトリパスの各階層を作成する（成功=true）
		inline bool EnsureDirectoryExists(const std::wstring& dir)
		{
			if (dir.empty())
			{
				return true;
			}

			// 正規化：末尾の区切りを除去
			std::wstring path = dir;
			while (!path.empty() && (path.back() == L'\\' || path.back() == L'/'))
			{
				path.pop_back();
			}
			if (path.empty())
			{
				return true;
			}

			std::wstring cur;
			size_t pos = 0;

			// ドライブ指定 (例: "C:\") がある場合は先に追加
			if (path.size() >= 2 && path[1] == L':')
			{
				// "C:" の後ろに '\' を付けて開始点とする
				if (path.size() >= 3 && (path[2] == L'\\' || path[2] == L'/'))
				{
					cur = path.substr(0, 3);
					pos = 3;
				}
				else
				{
					cur = path.substr(0, 2);
					pos = 2;
				}
			}
			else if (path.size() >= 1 && (path[0] == L'\\' || path[0] == L'/'))
			{
				// UNC 等の先頭 '\' を残して開始
				cur = L"\\";
				pos = 1;
			}

			while (pos <= path.size())
			{
				size_t next = path.find_first_of(L"\\/", pos);
				std::wstring part;
				if (next == std::wstring::npos)
				{
					part = path.substr(pos);
					pos = path.size() + 1;
				}
				else
				{
					part = path.substr(pos, next - pos);
					pos = next + 1;
				}

				if (!part.empty())
				{
					if (!cur.empty() && cur.back() != L'\\')
					{
						cur.push_back(L'\\');
					}
					cur += part;
				}

				// CreateDirectoryW は既に存在する場合 ERROR_ALREADY_EXISTS を返す
				if (!cur.empty())
				{
					if (!CreateDirectoryW(cur.c_str(), NULL))
					{
						DWORD err = GetLastError();
						if (err != ERROR_ALREADY_EXISTS)
						{
							return false;
						}
					}
				}
			}

			return true;
		}
	} // anonymous namespace

	// -------------------------------------------------------------------------
	// Exception 実装
	// -------------------------------------------------------------------------
	Exception::Exception(const std::wstring& m1, const std::wstring& m2, const std::wstring& m3) noexcept
	{
		m_msgW = m1;
		if (!m2.empty())
		{
			m_msgW += L"\r\n";
			m_msgW += m2;
		}
		if (!m3.empty())
		{
			m_msgW += L"\r\n";
			m_msgW += m3;
		}
		m_msgA = WideToUtf8(m_msgW);
		if (m_msgA.empty())
		{
			m_msgA = "DXModel exception";
		}
	}

	// -------------------------------------------------------------------------
	// 例外表示ユーティリティ（ヘッダで宣言済み）
	// -------------------------------------------------------------------------
	void ShowExceptionMessageBox(HWND owner, const std::exception& e, const std::wstring& title)
	{
		if (const Exception* de = dynamic_cast<const Exception*>(&e))
		{
			const std::wstring& msgw = de->what_w();
			MessageBoxW(owner, msgw.empty() ? L"(メッセージなし)" : msgw.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
			return;
		}

		std::string a = e.what();
		std::wstring w = Utf8ToWide(a);
		if (w.empty())
		{
			w = L"(エラーメッセージがありません)";
		}
		MessageBoxW(owner, w.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
	}

	void ShowUnknownExceptionMessageBox(HWND owner, const std::wstring& title)
	{
		MessageBoxW(owner, L"不明な例外が発生しました。詳細はログを確認してください。", title.c_str(), MB_OK | MB_ICONERROR);
	}

	// -------------------------------------------------------------------------
	// dxm シリアライズ実装
	// -------------------------------------------------------------------------
	namespace
	{
		const char DXM_MAGIC[4] = { 'D', 'X', 'M', '\0' };
		const std::uint32_t DXM_VERSION = 1;

		template<typename T>
		void WritePod(std::ofstream& os, const T& v)
		{
			os.write(reinterpret_cast<const char*>(&v), sizeof(T));
		}

		template<typename T>
		void ReadPod(std::ifstream& is, T& v)
		{
			is.read(reinterpret_cast<char*>(&v), sizeof(T));
		}

		void WriteWString(std::ofstream& os, const std::wstring& s)
		{
			const std::uint64_t len = static_cast<std::uint64_t>(s.size());
			WritePod(os, len);
			if (len > 0)
			{
				os.write(reinterpret_cast<const char*>(s.data()), sizeof(wchar_t) * static_cast<std::streamsize>(len));
			}
		}

		void ReadWString(std::ifstream& is, std::wstring& s)
		{
			std::uint64_t len = 0;
			ReadPod(is, len);
			if (len > 0 && len <= (std::numeric_limits<size_t>::max)())
			{
				s.resize(static_cast<size_t>(len));
				is.read(reinterpret_cast<char*>(&s[0]), sizeof(wchar_t) * static_cast<std::streamsize>(len));
			}
			else
			{
				s.clear();
			}
		}

		// 指定パスが存在し、ディレクトリであるか
		inline bool DirectoryExists(const std::wstring& dir)
		{
			if (dir.empty()) return false;
			DWORD attrs = GetFileAttributesW(dir.c_str());
			if (attrs == INVALID_FILE_ATTRIBUTES) return false;
			return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
		}

		// 親ディレクトリを返す。ドライブルートは "C:\" として返す。見つからない場合は空文字列。
		inline std::wstring GetParentPath(const std::wstring& path)
		{
			if (path.empty()) return {};
			auto pos = path.find_last_of(L"\\/");
			if (pos == std::wstring::npos) return {};
			if (pos == 2 && path.size() > 2 && path[1] == L':') return path.substr(0, 3);
			return path.substr(0, pos);
		}
	} // anonymous namespace

	// 新しい保存API（DXM 名称）
	bool DXModel::SaveToDxm(const std::wstring& path) const
	{
		// 保存先ディレクトリが存在しなければ作成する
		size_t pos = path.find_last_of(L"\\/");
		if (pos != std::wstring::npos)
		{
			std::wstring dir = path.substr(0, pos);
			if (!EnsureDirectoryExists(dir))
			{
				ShowErrorDialog(nullptr, L"保存先ディレクトリを作成できません", dir, L"DXModel::SaveToDxm()");
				return false;
			}
		}

		// path std::ofstream に渡す（CreateFileW を使うほうが良いかも）
		std::ofstream os(path, std::ios::binary);
		if (!os)
		{
			ShowErrorDialog(nullptr, L"ファイルを開けません", path, L"DXModel::SaveToDxm()");
			return false;
		}

		// header
		os.write(DXM_MAGIC, sizeof(DXM_MAGIC));
		WritePod(os, DXM_VERSION);

		// filePath
		WriteWString(os, m_filePath);

		// materials
		const std::uint32_t matCount = static_cast<std::uint32_t>(materials.size());
		WritePod(os, matCount);
		for (const auto& m : materials)
		{
			WriteWString(os, m.name);
			WritePod(os, m.diffuseColor.x); WritePod(os, m.diffuseColor.y); WritePod(os, m.diffuseColor.z); WritePod(os, m.diffuseColor.w);
			WritePod(os, m.ambientColor.x); WritePod(os, m.ambientColor.y); WritePod(os, m.ambientColor.z); WritePod(os, m.ambientColor.w);
			WritePod(os, m.specularColor.x); WritePod(os, m.specularColor.y); WritePod(os, m.specularColor.z); WritePod(os, m.specularColor.w);
			WritePod(os, m.emissiveColor.x); WritePod(os, m.emissiveColor.y); WritePod(os, m.emissiveColor.z); WritePod(os, m.emissiveColor.w);
			WritePod(os, m.opacity);
			WritePod(os, m.shininess);
			WriteWString(os, m.textureFilename);
		}

		// meshes
		const std::uint32_t meshCount = static_cast<std::uint32_t>(meshes.size());
		WritePod(os, meshCount);
		for (const auto& mesh : meshes)
		{
			WriteWString(os, mesh.name);
			WritePod(os, mesh.materialIndex);

			const std::uint32_t vCount = static_cast<std::uint32_t>(mesh.vertices.size());
			WritePod(os, vCount);
			for (const auto& v : mesh.vertices)
			{
				WritePod(os, v.pos.x); WritePod(os, v.pos.y); WritePod(os, v.pos.z);
				WritePod(os, v.normal.x); WritePod(os, v.normal.y); WritePod(os, v.normal.z);
				WritePod(os, v.uv.x); WritePod(os, v.uv.y);
				WritePod(os, v.boneIndices.x); WritePod(os, v.boneIndices.y); WritePod(os, v.boneIndices.z); WritePod(os, v.boneIndices.w);
				WritePod(os, v.weights.x); WritePod(os, v.weights.y); WritePod(os, v.weights.z); WritePod(os, v.weights.w);
			}

			const std::uint32_t idxCount = static_cast<std::uint32_t>(mesh.indices.size());
			WritePod(os, idxCount);
			for (const auto i : mesh.indices)
			{
				WritePod(os, i);
			}
		}

		// bones
		const std::uint32_t boneCount = static_cast<std::uint32_t>(bones.size());
		WritePod(os, boneCount);
		for (const auto& b : bones)
		{
			WriteWString(os, b.name);
			WritePod(os, static_cast<int32_t>(b.parentIndex));

			// store matrices as 4x4 floats (row-major)
			XMFLOAT4X4 lf;
			XMStoreFloat4x4(&lf, b.localTransform);
			for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) WritePod(os, lf.m[r][c]);

			XMFLOAT4X4 of;
			XMStoreFloat4x4(&of, b.offsetMatrix);
			for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) WritePod(os, of.m[r][c]);
		}

		// animations
		const std::uint32_t animCount = static_cast<std::uint32_t>(animations.size());
		WritePod(os, animCount);
		for (const auto& a : animations)
		{
			WriteWString(os, a.name);
			WritePod(os, a.m_duration);
			WritePod(os, a.ticksPerSecond);

			const std::uint32_t chCount = static_cast<std::uint32_t>(a.channels.size());
			WritePod(os, chCount);
			for (const auto& ch : a.channels)
			{
				WriteWString(os, ch.nodeName);

				const std::uint32_t pk = static_cast<std::uint32_t>(ch.positionKeys.size());
				WritePod(os, pk);
				for (const auto& k : ch.positionKeys)
				{
					WritePod(os, k.time);
					WritePod(os, k.value.x); WritePod(os, k.value.y); WritePod(os, k.value.z);
				}

				const std::uint32_t rk = static_cast<std::uint32_t>(ch.rotationKeys.size());
				WritePod(os, rk);
				for (const auto& k : ch.rotationKeys)
				{
					WritePod(os, k.time);
					WritePod(os, k.value.x); WritePod(os, k.value.y); WritePod(os, k.value.z); WritePod(os, k.value.w);
				}

				const std::uint32_t sk = static_cast<std::uint32_t>(ch.scalingKeys.size());
				WritePod(os, sk);
				for (const auto& k : ch.scalingKeys)
				{
					WritePod(os, k.time);
					WritePod(os, k.value.x); WritePod(os, k.value.y); WritePod(os, k.value.z);
				}
			}
		}

		os.flush();
		if (!os)
		{
			ShowErrorDialog(nullptr, L"ファイル書き込みエラー", path, L"DXModel::SaveToDxm()");
			return false;
		}
		return true;
	}

	std::shared_ptr<DXModel> DXModel::CreateFromFile(const std::wstring& path, const std::wstring& textureDir)
	{
		// path を UTF-8 に変換して std::ifstream に渡す
		std::ifstream is(WideToUtf8(path), std::ios::binary);
		if (!is)
		{
			ShowErrorDialog(nullptr, L"ファイルを開けません", path, L"DXModel::CreateFromFile()");
			return nullptr;
		}

		// header
		char magic[4];
		is.read(magic, sizeof(magic));
		if (is.gcount() != sizeof(magic) || std::memcmp(magic, DXM_MAGIC, sizeof(magic)) != 0)
		{
			ShowErrorDialog(nullptr, L"不正な DXM ヘッダです", path, L"DXModel::CreateFromFile()");
			return nullptr;
		}
		std::uint32_t ver = 0;
		ReadPod(is, ver);
		if (ver != DXM_VERSION)
		{
			ShowErrorDialog(nullptr, L"DXM バージョンが異なります", path, L"DXModel::CreateFromFile()");
			return nullptr;
		}

		auto model = std::make_shared<DXModel>();

		// 保存されたモデル内の filePath を読み込む（従来の用途）
		ReadWString(is, model->m_filePath);

		// source file を呼び出し時のパスで保存（フルパス）
		model->m_sourceFileName = path;
		std::wstring finalTexDir;
		if (!textureDir.empty() && DirectoryExists(textureDir))
		{
			finalTexDir = textureDir;
		}
		else
		{
			std::wstring parent = GetParentPath(path);
			if (!parent.empty() && DirectoryExists(parent))
			{
				finalTexDir = parent;
			}
			else
			{
				// どちらも見つからなければ、ファイル内に保存されていた m_filePath を優先（既存互換）
				finalTexDir = model->m_filePath;
			}
		}

		// 正規化：末尾の区切りを除去（既存コードに合わせる）
		while (!finalTexDir.empty() && (finalTexDir.back() == L'\\' || finalTexDir.back() == L'/'))
		{
			finalTexDir.pop_back();
		}
		model->m_filePath = finalTexDir;

		// materials
		std::uint32_t matCount = 0;
		ReadPod(is, matCount);
		model->materials.resize(matCount);
		for (std::uint32_t i = 0; i < matCount; ++i)
		{
			auto& m = model->materials[i];
			ReadWString(is, m.name);
			ReadPod(is, m.diffuseColor.x); ReadPod(is, m.diffuseColor.y); ReadPod(is, m.diffuseColor.z); ReadPod(is, m.diffuseColor.w);
			ReadPod(is, m.ambientColor.x); ReadPod(is, m.ambientColor.y); ReadPod(is, m.ambientColor.z); ReadPod(is, m.ambientColor.w);
			ReadPod(is, m.specularColor.x); ReadPod(is, m.specularColor.y); ReadPod(is, m.specularColor.z); ReadPod(is, m.specularColor.w);
			ReadPod(is, m.emissiveColor.x); ReadPod(is, m.emissiveColor.y); ReadPod(is, m.emissiveColor.z); ReadPod(is, m.emissiveColor.w);
			ReadPod(is, m.opacity);
			ReadPod(is, m.shininess);
			ReadWString(is, m.textureFilename);
		}

		// meshes
		std::uint32_t meshCount = 0;
		ReadPod(is, meshCount);
		model->meshes.resize(meshCount);
		for (std::uint32_t mi = 0; mi < meshCount; ++mi)
		{
			auto& mesh = model->meshes[mi];
			ReadWString(is, mesh.name);
			ReadPod(is, mesh.materialIndex);

			std::uint32_t vCount = 0;
			ReadPod(is, vCount);
			mesh.vertices.resize(vCount);
			for (std::uint32_t vi = 0; vi < vCount; ++vi)
			{
				auto& v = mesh.vertices[vi];
				ReadPod(is, v.pos.x); ReadPod(is, v.pos.y); ReadPod(is, v.pos.z);
				ReadPod(is, v.normal.x); ReadPod(is, v.normal.y); ReadPod(is, v.normal.z);
				ReadPod(is, v.uv.x); ReadPod(is, v.uv.y);
				ReadPod(is, v.boneIndices.x); ReadPod(is, v.boneIndices.y); ReadPod(is, v.boneIndices.z); ReadPod(is, v.boneIndices.w);
				ReadPod(is, v.weights.x); ReadPod(is, v.weights.y); ReadPod(is, v.weights.z); ReadPod(is, v.weights.w);
			}

			std::uint32_t idxCount = 0;
			ReadPod(is, idxCount);
			mesh.indices.resize(idxCount);
			for (std::uint32_t ii = 0; ii < idxCount; ++ii)
			{
				ReadPod(is, mesh.indices[ii]);
			}
		}

		// bones
		std::uint32_t boneCount = 0;
		ReadPod(is, boneCount);
		model->bones.resize(boneCount);
		for (std::uint32_t bi = 0; bi < boneCount; ++bi)
		{
			auto& b = model->bones[bi];
			ReadWString(is, b.name);
			int32_t p = -1;
			ReadPod(is, p);
			b.parentIndex = static_cast<int>(p);

			XMFLOAT4X4 lf{};
			for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) ReadPod(is, lf.m[r][c]);
			b.localTransform = XMLoadFloat4x4(&lf);

			XMFLOAT4X4 of{};
			for (int r = 0; r < 4; ++r) for (int c = 0; c < 4; ++c) ReadPod(is, of.m[r][c]);
			b.offsetMatrix = XMLoadFloat4x4(&of);
		}

		// animations
		std::uint32_t animCount = 0;
		ReadPod(is, animCount);
		model->animations.resize(animCount);
		for (std::uint32_t ai = 0; ai < animCount; ++ai)
		{
			auto& a = model->animations[ai];
			ReadWString(is, a.name);
			ReadPod(is, a.m_duration);
			ReadPod(is, a.ticksPerSecond);

			std::uint32_t chCount = 0;
			ReadPod(is, chCount);
			a.channels.resize(chCount);
			for (std::uint32_t ci = 0; ci < chCount; ++ci)
			{
				auto& ch = a.channels[ci];
				ReadWString(is, ch.nodeName);

				std::uint32_t pk = 0;
				ReadPod(is, pk);
				ch.positionKeys.resize(pk);
				for (std::uint32_t k = 0; k < pk; ++k)
				{
					ReadPod(is, ch.positionKeys[k].time);
					ReadPod(is, ch.positionKeys[k].value.x); ReadPod(is, ch.positionKeys[k].value.y); ReadPod(is, ch.positionKeys[k].value.z);
				}

				std::uint32_t rk = 0;
				ReadPod(is, rk);
				ch.rotationKeys.resize(rk);
				for (std::uint32_t k = 0; k < rk; ++k)
				{
					ReadPod(is, ch.rotationKeys[k].time);
					ReadPod(is, ch.rotationKeys[k].value.x); ReadPod(is, ch.rotationKeys[k].value.y); ReadPod(is, ch.rotationKeys[k].value.z); ReadPod(is, ch.rotationKeys[k].value.w);
				}

				std::uint32_t sk = 0;
				ReadPod(is, sk);
				ch.scalingKeys.resize(sk);
				for (std::uint32_t k = 0; k < sk; ++k)
				{
					ReadPod(is, ch.scalingKeys[k].time);
					ReadPod(is, ch.scalingKeys[k].value.x); ReadPod(is, ch.scalingKeys[k].value.y); ReadPod(is, ch.scalingKeys[k].value.z);
				}
			}
		}

		if (!is)
		{
			ShowErrorDialog(nullptr, L"ファイル読み込みエラー", path, L"DXModel::CreateFromFile()");
			return nullptr;
		}

		return model;
	}

	// モデル名ダンプ（インスタンスメンバ版）
	void DXModel::DumpModelNames(const std::wstring& outDir) const
	{
		try
		{
			// outDir を作成する（存在しないパスの各階層を順に作る）
			auto EnsureDirectoryExists = [](const std::wstring& dir) -> bool
			{
				if (dir.empty())
				{
					return true;
				}

				// 正規化：末尾の区切りを除去
				std::wstring path = dir;
				while (!path.empty() && (path.back() == L'\\' || path.back() == L'/'))
				{
					path.pop_back();
				}
				if (path.empty())
				{
					return true;
				}

				std::wstring cur;
				size_t pos = 0;

				// ドライブ指定 (例: "C:\") がある場合は先に追加
				if (path.size() >= 2 && path[1] == L':')
				{
					// "C:" の後ろに '\' を付けて開始点とする
					if (path.size() >= 3 && (path[2] == L'\\' || path[2] == L'/'))
					{
						cur = path.substr(0, 3);
						pos = 3;
					}
					else
					{
						cur = path.substr(0, 2);
						pos = 2;
					}
				}
				else if (path.size() >= 1 && (path[0] == L'\\' || path[0] == L'/'))
				{
					// UNC 等の先頭 '\' を残して開始
					cur = L"\\";
					pos = 1;
				}

				while (pos <= path.size())
				{
					size_t next = path.find_first_of(L"\\/", pos);
					std::wstring part;
					if (next == std::wstring::npos)
					{
						part = path.substr(pos);
						pos = path.size() + 1;
					}
					else
					{
						part = path.substr(pos, next - pos);
						pos = next + 1;
					}

					if (!part.empty())
					{
						if (!cur.empty() && cur.back() != L'\\')
						{
							cur.push_back(L'\\');
						}
						cur += part;
					}

					// CreateDirectoryW は既に存在する場合 ERROR_ALREADY_EXISTS を返す
					if (!cur.empty())
					{
						if (!CreateDirectoryW(cur.c_str(), NULL))
						{
							DWORD err = GetLastError();
							if (err != ERROR_ALREADY_EXISTS)
							{
								return false;
							}
						}
					}
				}

				return true;
			};

			if (!EnsureDirectoryExists(outDir))
			{
				ShowErrorDialog(nullptr, L"ダンプ先ディレクトリを作成できません", outDir, L"DXModel::DumpModelNames()");
				return;
			}

			// ソースファイル名（拡張子付き）を取得（優先順：m_sourceFileName -> m_filePath）
			std::wstring sourceForName;
			if (!m_sourceFileName.empty())
			{
				sourceForName = m_sourceFileName;
			}
			else if (!m_filePath.empty())
			{
				sourceForName = m_filePath;
			}

			// ファイル名（拡張子付き）とベース名（拡張子なし）を取得
			std::wstring filenameWithExt = L"DXModel";
			std::wstring baseName = L"DXModel";
			if (!sourceForName.empty())
			{
				size_t pos = sourceForName.find_last_of(L"\\/");
				std::wstring filename = (pos == std::wstring::npos) ? sourceForName : sourceForName.substr(pos + 1);
				if (!filename.empty())
				{
					filenameWithExt = filename;
					size_t dot = filename.find_last_of(L'.');
					if (dot != std::wstring::npos && dot > 0)
					{
						baseName = filename.substr(0, dot); // 拡張子を除く
					}
					else
					{
						baseName = filename;
					}
				}
			}

			// 出力ファイル名は "sourceFileName（拡張子なし）.txt"
			std::wstring outPath = outDir;
			if (!outPath.empty() && outPath.back() != L'\\' && outPath.back() != L'/')
			{
				outPath.push_back(L'\\');
			}
			outPath += baseName;
			outPath += L".txt";

			// ofstream は narrow path を取るため WideToUtf8 を使う
			std::ofstream os(WideToUtf8(outPath), std::ios::binary);
			if (!os)
			{
				ShowErrorDialog(nullptr, L"ダンプファイルを開けません", outPath, L"DXModel::DumpModelNames()");
				return;
			}

			auto writeLine = [&](const std::wstring& w)
				{
					const std::string a = WideToUtf8(w);
					os << a << '\n';
				};

			// 出力：フルパス＋ファイル名（拡張子なし）を明示
			writeLine(L"Source File : ");
			writeLine(L"  " + m_sourceFileName);
			writeLine(L"");

			// Materials
			writeLine(L"Materials :");
			for (size_t i = 0; i < materials.size(); ++i)
			{
				writeLine(std::wstring(L"  [") + std::to_wstring(i) + L"] " + materials[i].name);
			}
			writeLine(L"");

			// Textures
			writeLine(L"Textures :");
			std::unordered_set<std::wstring> uniqueTextures;
			uniqueTextures.reserve(materials.size());

			int texIdx = 0;
			for (const auto& material : materials)
			{
				if (material.textureFilename.empty())
				{
					continue;
				}

				const auto inserted = uniqueTextures.insert(material.textureFilename);
				if (!inserted.second)
				{
					continue;
				}

				writeLine(std::wstring(L"  [") + std::to_wstring(texIdx++) + L"] " + material.textureFilename);
			}
			writeLine(L"");

			// Meshes
			writeLine(L"Meshes :");
			for (size_t i = 0; i < meshes.size(); ++i)
			{
				writeLine(std::wstring(L"  [") + std::to_wstring(i) + L"] " + meshes[i].name);
			}
			writeLine(L"");

			// Bones
			writeLine(L"Bones :");
			for (size_t i = 0; i < bones.size(); ++i)
			{
				writeLine(std::wstring(L"  [") + std::to_wstring(i) + L"] " + bones[i].name + L" (parent=" + std::to_wstring(bones[i].parentIndex) + L")");
			}
			writeLine(L"");

			// Animations and channels
			writeLine(L"Animations :");
			for (size_t ai = 0; ai < animations.size(); ++ai)
			{
				const auto& a = animations[ai];
				writeLine(std::wstring(L"  [") + std::to_wstring(ai) + L"] " + a.name + L" (m_duration=" + std::to_wstring(a.m_duration) + L", ticksPerSecond=" + std::to_wstring(a.ticksPerSecond) + L")");
				for (size_t ci = 0; ci < a.channels.size(); ++ci)
				{
					const auto& ch = a.channels[ci];
					writeLine(std::wstring(L"    [") + std::to_wstring(ci) + L"] node=" + ch.nodeName);
				}
			}

			os.flush();
			if (!os)
			{
				ShowErrorDialog(nullptr, L"ダンプファイル書き込みエラー", outPath, L"DXModel::DumpModelNames()");
				return;
			}
		}
		catch (const std::exception& e)
		{
			ShowExceptionMessageBox(nullptr, e, L"DXModel::DumpModelNames()");
		}
		catch (...)
		{
			ShowUnknownExceptionMessageBox(nullptr, L"DXModel::DumpModelNames()");
		}
	}
} // namespace dxm