#pragma once
#include "stdafx.h"

namespace basecross{
	template<class T>
	class SingletonBase {
	protected:
		SingletonBase() = default;
	public:
		~SingletonBase(){}

		static T& Get() {
			static T instance;
			return instance;
		}

	private:
		SingletonBase(const SingletonBase&) = delete;
		SingletonBase& operator=(const SingletonBase&) = delete;

	};

	///----------------------------------------------------------------
	/// Singleton ヘッダ
	/// 
	/// 使用方法
	/// ・継承によってシングルトン化
	/// ・SingletonBaseを継承したクラスをGetで取得
	/// ・friend classでSingletonBase<T>を指定する
	/// ・コンストラクタはdefaultで良い
	/// ----------------------------------------------------------------
}
