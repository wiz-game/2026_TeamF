/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{

	enum class JsonType {
		Number, String, Bool, Object, Array, Null, Unknown
	};

	class JsonValue {
	protected:
		/// <summary>
		/// 出力時に指定した数のインデントを追加
		/// </summary>
		/// <param name="indent">インデント深度</param>
		/// <param name="str">文字列(参照)</param>
		void Indent(int indent,wstring& str) {
			for (int i = 0; i < indent; i++) {
				str += L"\t";
			}
		}
	public:
		virtual ~JsonValue() {}
		virtual JsonType GetType()const { return JsonType::Unknown; }
		virtual wstring ToString(int indent = 0) { return L""; }

		bool CompareType(JsonType type) const {
			return GetType() == type;
		}

		/// <summary>
		/// shared_ptrに変換。スライス対策
		/// </summary>
		/// <returns>shared_ptr</returns>
		virtual shared_ptr<JsonValue> Clone()const {
			return make_shared<JsonValue>(*this);
		}
	};

	class JsonNumber : public JsonValue {
		float m_Value;
	public:
		JsonNumber(float value) :m_Value(value) {}
		virtual JsonType GetType() const override { return JsonType::Number; }
		virtual shared_ptr<JsonValue> Clone()const override {
			return make_shared<JsonNumber>(*this);
		}
		virtual wstring ToString(int indent = 0)override {
			return to_wstring(m_Value);
		}

		/// <summary>
		/// 値をfloat型で取得
		/// </summary>
		/// <returns>float</returns>
		float GetFloatValue() const { return m_Value; }

		/// <summary>
		/// 値をint型で取得
		/// </summary>
		/// <returns>int</returns>
		int GetIntValue() const { return static_cast<int>(m_Value); }
	};

	class JsonString : public JsonValue {
		wstring m_Value;
	public:
		JsonString(const wstring& value) :m_Value(value) {}
		virtual JsonType GetType() const override { return JsonType::String; }
		virtual shared_ptr<JsonValue> Clone()const override {
			return make_shared<JsonString>(*this);
		}
		virtual wstring ToString(int indent = 0) override {
			return L"\"" + m_Value + L"\"";
		}

		const wstring& GetValue() const { return m_Value; }
	};

	class JsonBool : public JsonValue {
		bool m_Value;
	public:
		JsonBool(bool value) :m_Value(value) {}
		virtual JsonType GetType() const override { return JsonType::Bool; }
		virtual shared_ptr<JsonValue> Clone()const override {
			return make_shared<JsonBool>(*this);
		}
		virtual wstring ToString(int indent = 0) override {
			return m_Value ? L"true" : L"false";
		}

		bool GetValue() const { return m_Value; }
	};
	class JsonNull : public JsonValue {
		public:
		JsonNull() {}
		virtual JsonType GetType() const override { return JsonType::Null; }
	};
	
	class JsonObject;
	class JsonArray : public JsonValue {
		JsonType m_ArrayType;

	public:
		vector<shared_ptr<JsonValue>> m_Values;

		JsonArray() : m_ArrayType(JsonType::Unknown) {}
    //---------配列の初期化----------
	//    std::variant使いたい

		JsonArray(const vector<int>& array);
		JsonArray(const vector<float>& array);
		JsonArray(const vector<bool>& array);
		JsonArray(const vector<wstring>& array);
		JsonArray(const vector<JsonObject>& array);

		virtual JsonType GetType() const override { return JsonType::Array; }
		virtual shared_ptr<JsonValue> Clone()const override {
			return make_shared<JsonArray>(*this);
		}
		virtual wstring ToString(int indent = 0)override {
			wstring str = L"[";
			for (auto it = m_Values.begin(); it != m_Values.end(); it++) {
				auto value = *it;
				str += value->ToString(indent);

				if (next(it) != m_Values.end()) {
					str += L",";
				}
			}
			str += L"]";

			return str;
		}

		vector<int> GetIntArray();
		vector<float> GetFloatArray();
		vector<bool> GetBoolArray();
		vector<wstring> GetStringArray();
		vector<shared_ptr<JsonObject>> GetObjectArray();

		/// <summary>
		/// 配列に値を追加
		/// 異なる型を追加しようとした場合はエラー
		/// </summary>
		/// <param name="value">値</param>
		void Add(const JsonValue& value) {
			Add(value.Clone());
		}

		/// <summary>
		/// 配列に値を追加
		/// 異なる型を追加しようとした場合はエラー
		/// </summary>
		/// <param name="value">値のポインター</param>
		void Add(const shared_ptr<JsonValue>& value) {
			if (!m_Values.empty()) {
				if (!value->CompareType(m_Values[0]->GetType())) {
					throw BaseException(L"異なる型を配列に含めることはできません",L"", L"JsonArray::Add(const shared_ptr<JsonValue>&)");
				}
			}
			else {
				m_ArrayType = value->GetType();
			}
			m_Values.push_back(value);
		}
	};
	struct ObjectData;

	class JsonObject : public JsonValue {
		map<wstring, shared_ptr<JsonValue>> m_Values;

	public:
		JsonObject() {}
		JsonObject(initializer_list<ObjectData> list);
		virtual JsonType GetType() const override { return JsonType::Object; }
		virtual shared_ptr<JsonValue> Clone()const override {
			return make_shared<JsonObject>(*this);
		}
		virtual wstring ToString(int indent = 0)override {
			wstring str = L"{\n";

			for (auto it = m_Values.begin(); it != m_Values.end(); it++) {
				Indent(indent, str);
				auto value = *it;
				str += L"\"" + value.first + L"\" : " + value.second->ToString(indent + 1);
				if (next(it) != m_Values.end()) {
					str += L",\n";
				}
			}
			str += L"\n";
			Indent(indent - 1, str);
			str += L"}";

			return str;
		}

		/// <summary>
		/// 指定されたキーが存在するか判定
		/// </summary>
		/// <param name="key">キー</param>
		/// <returns>判定結果</returns>
		bool HasKey(const wstring& key) const {
			return m_Values.find(key) != m_Values.end();
		}

		/// <summary>
		/// 指定されたキーの値をJsonValue型で取得(存在しない場合はエラー)
		/// </summary>
		/// <param name="key"></param>
		/// <returns>値(JsonValue型)</returns>
		inline shared_ptr<JsonValue> At(const wstring& key) {
			auto it = m_Values.find(key);
			if (it != m_Values.end()) {
				return it->second;
			}
			else {
				return nullptr;
			}
		}

		/// <summary>
		/// 指定されたキーの値を指定された型で取得(存在しない場合はエラー)
		/// </summary>
		/// <typeparam name="T">型</typeparam>
		/// <param name="key">キー</param>
		/// <returns>値(T型)</returns>
		template<class T>
		inline shared_ptr<T> At(const wstring& key) {
			auto value = At(key);
			if (!value) return nullptr;
			auto casted = dynamic_pointer_cast<T>(value);
			if(!casted) throw BaseException(L"型が異なります", key + L"/" + Util::GetWSTypeName<T>(),L"JsonObject::At<T>(const wstring&)");
			return casted;
		}

		/// <summary>
		/// 指定したキーの値を設定(存在しない場合は追加)
		/// 既に存在する場合は上書きされる
		/// </summary>
		/// <param name="key">キー</param>
		/// <param name="value">値</param>
		inline void Set(const wstring& key, const shared_ptr<JsonValue>& value) {
			m_Values[key] = value;
		}

		/// <summary>
		/// 指定したキーと値を追加
		/// 上書きはできない(エラー)
		/// </summary>
		/// <param name="key">キー</param>
		/// <param name="value">値</param>
		/// <returns>追加した値</returns>
		inline shared_ptr<JsonValue> Add(const wstring& key, const JsonValue& value) {
			return Add(key, value.Clone());
		}
		/// <summary>
		/// 指定したキーと値を追加
		/// 上書きはできない(エラー)
		/// </summary>
		/// <param name="key">キー</param>
		/// <param name="value">値のポインター</param>
		/// <returns>追加した値</returns>
		inline shared_ptr<JsonValue> Add(const wstring& key, const shared_ptr<JsonValue>& value) {
			if (HasKey(key)) {
				throw BaseException(L"キーがすでに存在します", key, L"JsonObject::Add(const wstring&, const shared_ptr<JsonValue>&)");
			}
			m_Values[key] = value;

			return value;
		}

		/// <summary>
		/// 登録されているキーを取得
		/// </summary>
		/// <returns>キー配列</returns>
		inline vector<wstring> GetKeys() const {
			vector<wstring> keys;
			for (auto& pair : m_Values) {
				keys.push_back(pair.first);
			}
			return keys;
		}
	};
//---------データとファイルデータの変換をするクラス----------
//       Jsonクラス内で使用される
	class JsonHelper {
	//----------数値変換----------
		
		/// <summary>
		/// 文字列から値のデータに変換
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="pos">開始位置</param>
		/// <returns>値</returns>
		shared_ptr<JsonValue> ParseValue(const wstring& str, size_t& pos);

		/// <summary>
		/// 文字列から値のデータ(int、float)に変換
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="pos">開始位置</param>
		/// <returns>値</returns>
		shared_ptr<JsonNumber> ParseNumber(const wstring& str, size_t& pos);

		/// <summary>
		/// 文字列から値のデータ(bool)に変換
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="pos">開始位置</param>
		/// <returns>値</returns>
		shared_ptr<JsonBool> ParseBool(const wstring& str, size_t& pos);

		/// <summary>
		/// 文字列から値のデータ(文字列)に変換
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="pos">開始位置</param>
		/// <returns>値</returns>
		shared_ptr<JsonString> ParseString(const wstring& str, size_t& pos);

		/// <summary>
		/// 文字列から値のデータ(配列)に変換
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="pos">開始位置</param>
		/// <returns>値</returns>
		shared_ptr<JsonArray> ParseArray(const wstring& str, size_t& pos);

		/// <summary>
		/// 文字列から値のデータ(オブジェクト)に変換
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="pos">開始位置</param>
		/// <returns>値</returns>
		shared_ptr<JsonObject> ParseObject(const wstring& str, size_t& pos);

	//----------文字列操作----------

		/// <summary>
		/// 開始地点から次の文字までの空白をスキップ
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="pos">開始位置</param>
		void SkipSpace(const wstring& str, size_t& pos) {
			while (isspace(Peek(str, pos))) Get(str, pos);
		}

		/// <summary>
		/// 指定された位置の文字を取得
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="pos">位置</param>
		/// <returns>文字</returns>
		wchar_t Peek(const wstring& str, size_t& pos) {
			if (pos >= str.size()) return L'\0';
			return str[pos];
		}

		/// <summary>
		/// 指定された位置の文字を取得し、位置を1つ進める
		/// </summary>
		/// <param name="str">文字列</param>
		/// <param name="pos">位置</param>
		/// <returns>文字</returns>
		wchar_t Get(const wstring& str, size_t& pos) {
			if (pos >= str.size()) return L'\0';
			return str[pos++];
		}
	public:
		/// <summary>
		/// JsonObject型のデータをファイルに保存する
		/// </summary>
		/// <param name="filename">保存先</param>
		/// <param name="data">データ</param>
		void ParseFile(const wstring& filename,const shared_ptr<JsonObject>& data);

		/// <summary>
		/// ファイルを読み込んでJsonObject型のデータに変換する
		/// </summary>
		/// <param name="filename">参照先</param>
		/// <returns>データ</returns>
		shared_ptr<JsonObject> ParseValue(const wstring& filename);

		/// <summary>
		/// パスをキーに変換する
		/// </summary>
		/// <param name="root">ルートパス</param>
		/// <returns>キー</returns>
		vector<wstring> SplitKey(const wstring& root);

		/// <summary>
		/// パスをたどる。ない場合は新しく作成される
		/// </summary>
		/// <param name="keys">キーの配列</param>
		/// <returns>たどった結果</returns>
		shared_ptr<JsonObject> EnsurePath(const vector<wstring>& keys,shared_ptr<JsonObject>& root);
	};

//---------Jsonのデータ管理を行うクラス----------
//       実際に使う際はこのクラスを宣言する
	class Json {
		shared_ptr<JsonObject> m_Root;	//データ本体
		JsonHelper m_Helper;	//変換補助

		static wstring s_DefaultFilePath; //デフォルトのファイル名
	public:
		static void SetDefaultFilePath(const wstring& path) { s_DefaultFilePath = path; }

		Json() :m_Root(make_shared<JsonObject>()) {
			m_Helper = JsonHelper();
		}
		/// <summary>
		/// 作成とともにファイルを読み込むコンストラクタ
		/// </summary>
		/// <param name="filename">ファイル名</param>
		Json(const wstring& filename) {
			m_Helper = JsonHelper();
			Load(filename);
		}

		shared_ptr<JsonObject> GetRoot() const { return m_Root; }

		/// <summary>
		/// 指定されたファイルからデータを読み込む
		/// </summary>
		/// <param name="filename">ファイル名</param>
		void Load(const wstring& filename) {
			m_Root = m_Helper.ParseValue(s_DefaultFilePath + filename);
		}

		/// <summary>
		/// 指定されたファイル名でデータを保存します。
		/// </summary>
		/// <param name="filename">保存先のファイル名</param>
		void Save(const wstring& filename) {
			m_Helper.ParseFile(s_DefaultFilePath + filename, m_Root);
		}

		/// <summary>
		/// 指定されたパス・キーに値を設定
		/// </summary>
		/// <param name="root">パス</param>
		/// <param name="value">値のポインター</param>
		void Set(const wstring& root, const shared_ptr<JsonValue>& value);

		/// <summary>
		/// 指定されたパス・キーに値を設定
		/// </summary>
		/// <param name="root">パス</param>
		/// <param name="value">値</param>
		void Set(const wstring& root, const JsonValue& value) {
			Set(root, value.Clone());
		}

		/// <summary>
		/// 指定されたパスに設定された値をJsonValue型で取得
		/// </summary>
		/// <param name="root">パス</param>
		/// <returns>値(JsonValue型)</returns>
		shared_ptr<JsonValue> At(const wstring& root);

		/// <summary>
		/// 指定されたパスに設定された値を指定された型で取得
		/// </summary>
		/// <typeparam name="T">取得型</typeparam>
		/// <param name="root">パス</param>
		/// <returns>値(T型)</returns>
		template <class T>
		shared_ptr<T> At(const wstring& root) {
			return dynamic_pointer_cast<T>(At(root));
		}

		/// <summary>
		/// 指定したキーに対応する値を取得。
		/// 現状使いづらい。At<T>を使うべき。
		/// virantを使わせろ
		/// </summary>
		/// <param name="key">キー</param>
		/// <returns>値</returns>
		inline shared_ptr<JsonValue> operator[](const wstring& key) {
			if (!m_Root->HasKey(key)) {
				throw BaseException(L"指定したキーが存在しません",key, L"Json::operator[](const wstring&)");
			}
			return m_Root->At(key);
		}
	};
}
//end basecross
