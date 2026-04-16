/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{
	struct ObjectData {
		wstring m_Key;
		shared_ptr<JsonValue> m_Value;
		ObjectData(const wstring& key, const JsonValue& value) : m_Key(key), m_Value(value.Clone()) {

		}
	};

	JsonArray::JsonArray(const vector<int>& array) : m_ArrayType(JsonType::Number) {
		for (const auto& value : array) {
			m_Values.push_back(make_shared<JsonNumber>(value));
		}
	}
	JsonArray::JsonArray(const vector<float>& array) : m_ArrayType(JsonType::Number) {
		for (const auto& value : array) {
			m_Values.push_back(make_shared<JsonNumber>(value));
		}
	}
	JsonArray::JsonArray(const vector<bool>& array) : m_ArrayType(JsonType::Bool) {
		for (const auto& value : array) {
			m_Values.push_back(make_shared<JsonBool>(value));
		}
	}
	JsonArray::JsonArray(const vector<wstring>& array) : m_ArrayType(JsonType::String) {
		for (const auto& value : array) {
			m_Values.push_back(make_shared<JsonString>(value));
		}
	}
	JsonArray::JsonArray(const vector<JsonObject>& array) : m_ArrayType(JsonType::Object) {
		for (const auto& value : array) {
			m_Values.push_back(make_shared<JsonObject>(value));
		}
	}
	
	vector<int> JsonArray::GetIntArray() {
		if (m_ArrayType != JsonType::Number) return {};
		vector<int> array;
		for (auto& value : m_Values) {
			auto ary = dynamic_pointer_cast<JsonNumber>(value);
			if (!ary) throw BaseException(L"配列の型が異なります", L"", L"JsonArray::GetIntArray()");
			array.push_back(ary->GetIntValue());
		}
		return array;
	}
	vector<float> JsonArray::GetFloatArray() {
		if (m_ArrayType != JsonType::Number) return {};
		vector<float> array;
		for (auto& value : m_Values) {
			auto ary = dynamic_pointer_cast<JsonNumber>(value);
			if (!ary) throw BaseException(L"配列の型が異なります", L"", L"JsonArray::GetFloatArray()");
			array.push_back(ary->GetFloatValue());
		}
		return array;
	}
	vector<bool> JsonArray::GetBoolArray() {
		if (m_ArrayType != JsonType::Bool) return {};
		vector<bool> array;
		for (auto& value : m_Values) {
			auto ary = dynamic_pointer_cast<JsonBool>(value);
			if (!ary) throw BaseException(L"配列の型が異なります", L"", L"JsonArray::GetBoolArray()");
			array.push_back(ary->GetValue());
		}
		return array;
	}
	vector<wstring> JsonArray::GetStringArray() {
		if (m_ArrayType != JsonType::String) return {};
		vector<wstring> array;
		for (auto& value : m_Values) {
			auto ary = dynamic_pointer_cast<JsonString>(value);
			if (!ary) throw BaseException(L"配列の型が異なります", L"", L"JsonArray::GetStringArray()");
			array.push_back(ary->GetValue());
		}
		return array;
	}
	vector<shared_ptr<JsonObject>> JsonArray::GetObjectArray() {
		if (m_ArrayType != JsonType::Object) return {};
		vector<shared_ptr<JsonObject>> array;
		for (auto& value : m_Values) {
			auto ary = dynamic_pointer_cast<JsonObject>(value);
			if (!ary) throw BaseException(L"配列の型が異なります", L"", L"JsonArray::GetObjectArray()");
			array.push_back(ary);
		}
		return array;
	}


	JsonObject::JsonObject(initializer_list<ObjectData> list) {
		for (auto& data : list) {
			Add(data.m_Key, data.m_Value);
		}
	}

	void JsonHelper::ParseFile(const wstring& filename, const shared_ptr<JsonObject>& data) {
		wofstream ofs(filename);
		if (!ofs) throw BaseException(L"ファイルの開けませんでした。", filename, L"JsonHelper::ParseFile(const wstring&,const shared_ptr<JsonObject>&)");

		ofs << data->ToString(1);

	}
	shared_ptr<JsonObject> JsonHelper::ParseValue(const wstring& filename) {
		wifstream ifs(filename);
		if(!ifs) throw BaseException(L"ファイルの開けませんでした。", filename, L"JsonHelper::ParseValue(const wstring&)");

		wstring str,temp;
		while (getline(ifs, temp)) {
			str += temp;
		}
		size_t pos = 0;
		auto object = dynamic_pointer_cast<JsonObject>(ParseValue(str, pos));
		if (!object) throw BaseException(L"無効なJsonファイルです。", filename, L"JsonHelper::ParseValue(const wstring&)");
		return object;
	}

	shared_ptr<JsonValue> JsonHelper::ParseValue(const wstring& str, size_t& pos) {
		SkipSpace(str, pos);

		auto c = Peek(str, pos);
		if (c == L't' || c == L'f') return ParseBool(str, pos);
		else if (c == L'"') return ParseString(str, pos);
		else if (c == L'-' || isdigit(c)) return ParseNumber(str, pos);
		else if (c == L'[') return ParseArray(str, pos);
		else if (c == L'{') return ParseObject(str, pos);
		else throw BaseException(L"無効な文字列が含まれています", to_wstring(c), L"JsonHelper::ParseValue(const wstring&,size_t&)");
	}
	shared_ptr<JsonNumber> JsonHelper::ParseNumber(const wstring& str, size_t& pos) {
		SkipSpace(str, pos);

		size_t start = pos;
		if (Peek(str, pos) == L'-') Get(str,pos);
		while (isdigit(Peek(str, pos))) Get(str, pos);

		if (Peek(str, pos) == L'.') {
			Get(str, pos);
			while (isdigit(Peek(str, pos))) Get(str, pos);
		}
		auto s = str.substr(start, pos - start);
		float value = stof(s);
		return make_shared<JsonNumber>(value);
	}
	shared_ptr<JsonBool> JsonHelper::ParseBool(const wstring& str, size_t& pos) {
		SkipSpace(str, pos);

		if (str.substr(pos, 4) == L"true") {
			pos += 4;
			return make_shared<JsonBool>(true);
		}
		else if (str.substr(pos, 5) == L"false") {
			pos += 5;
			return make_shared<JsonBool>(false);
		}

		throw BaseException(L"無効な値が設定されています", L"", L"JsonHelper::ParseBool(const wstring&,size_t&)");
	}
	shared_ptr<JsonString> JsonHelper::ParseString(const wstring& str, size_t& pos) {
		if(Get(str,pos) != L'"') throw BaseException(L"無効なオブジェクトが読み込まれました", L"", L"JsonHelper::ParseString(const wstring&,size_t&)");

		wstring result;
		while (true) {
			auto c = Get(str, pos);
			if (c == L'"') break;

			result += c;
		}

		return make_shared<JsonString>(result);
	}
	shared_ptr<JsonArray> JsonHelper::ParseArray(const wstring& str, size_t& pos) {
		if (Get(str, pos) != L'[') throw BaseException(L"無効なオブジェクトが読み込まれました", L"", L"JsonHelper::ParseArray(const wstring&,size_t&)");

		auto array = make_shared<JsonArray>();

		SkipSpace(str, pos);
		if (Peek(str, pos) == L']') {
			Get(str, pos);
			return array;
		}

		while (true) {
			array->Add(ParseValue(str, pos));
			SkipSpace(str, pos);

			if (Peek(str, pos) == L',') {
				Get(str, pos);
				SkipSpace(str, pos);
				continue;
			}
			else if (Peek(str, pos) == L']') {
				Get(str, pos);
				break;
			}
		}
		return array;
	}
	shared_ptr<JsonObject> JsonHelper::ParseObject(const wstring& str, size_t& pos) {
		if (Get(str,pos) != L'{') throw BaseException(L"無効なオブジェクトが読み込まれました", L"", L"JsonHelper::ParseObject(const wstring&,size_t&)");
		SkipSpace(str, pos);

		shared_ptr<JsonObject> object = make_shared<JsonObject>();
		if (Peek(str, pos) == L'}') {
			Get(str, pos);
			return object;
		}

		while (true) {
			auto keyData = ParseString(str,pos);
			wstring key = keyData->GetValue();
			SkipSpace(str, pos);
			if (Get(str, pos) != L':') throw;

			auto value = ParseValue(str,pos);

			object->Add(key, value);
			SkipSpace(str, pos);
			auto c = Get(str, pos);
			if (c == L',') {
				SkipSpace(str, pos);
				continue;
			}
			else if (c == L'}') {
				break;
			}

		}
		return object;
	}


	vector<wstring> JsonHelper::SplitKey(const wstring& root){
		vector<wstring> keys;
		wstring key;

		for (auto& c : root) {
			if (c == L'/') {
				//キーが空でなければ追加
				if (!key.empty()) keys.push_back(key);

				key = L"";
				continue;
			}
			key += c;
		}
		//最後のキーを追加
		if (!key.empty()) keys.push_back(key);

		if (keys.size() == 0) throw BaseException(L"キーが不正です",L"keys.size() == 0",L"JsonHelper::SplitKey(const wstring&)");

		return keys;
	}
	shared_ptr<JsonObject> JsonHelper::EnsurePath(const vector<wstring>& keys, shared_ptr<JsonObject>& root) {
		auto current = root;
		for (const auto& key : keys) {
			if (current->HasKey(key)) {
				auto value = current->At(key);
				if(!value->CompareType(JsonType::Object)){
					throw BaseException(L"キーがJsonObject型ではありません", key, L"JsonHelper::EnsurePath(const vector<wstring>&, shared_ptr<JsonObject>&)");
				}
				current = dynamic_pointer_cast<JsonObject>(value);
			}
			else {
				auto value = current->Add(key, make_shared<JsonObject>());

				current = dynamic_pointer_cast<JsonObject>(value);
			}
		}

		return current;
	}

	wstring Json::s_DefaultFilePath = L"";

	shared_ptr<JsonValue> Json::At(const wstring& root) {
		vector<wstring> keys = m_Helper.SplitKey(root);

		wstring valueKey = keys.back();
		keys.pop_back();

		auto object = m_Helper.EnsurePath(keys, m_Root);

		return object->At(valueKey);
	}

	void Json::Set(const wstring& root, const shared_ptr<JsonValue>& value) {
		vector<wstring> keys = m_Helper.SplitKey(root);

		wstring valueKey = keys.back();
		keys.pop_back();

		auto object = m_Helper.EnsurePath(keys, m_Root);

		if (object->HasKey(valueKey)) {
			object->Set(valueKey, value);
		}
		else {
			object->Add(valueKey, value);
		}
	}
}
//end basecross
