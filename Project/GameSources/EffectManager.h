/*!
@file EffectManager.h
@brief Effectなど
*/

#pragma once
#include "stdafx.h"


#include <Effekseer.h>
#include <EffekseerRendererDX11.h>


#pragma comment(lib, "Effekseer.lib" )
#pragma comment(lib, "EffekseerRendererDX11.lib" )


namespace basecross {
	//--------------------------------------------------------------------------------------
	///	@class	EffectManager
	///	@brief	Effekseerライブラリを使用したエフェクトの再生、管理、描画を行うクラス
	//--------------------------------------------------------------------------------------
	class EffectManager : public MultiParticle {
	/// @brief 読み込んだエフェクトリソースを管理するマップ (キー: wstring, 値: EffectRef)
	map<wstring, Effekseer::EffectRef> m_ResMap;

	/// @brief 再生中のエフェクトハンドル (サンプル用、または一時利用)
	Effekseer::Handle handle;

	/// @brief Effekseerの管理マネージャー
	Effekseer::ManagerRef m_Manager;

	/// @brief Effekseerの描画レンダラー (DirectX11用)
	EffekseerRendererDX11::RendererRef m_renderer;

	/// @brief 経過時間
	float m_TotalTime;

	/// @brief 単一のエフェクトリソースへの参照 (サンプル用、または一時利用)
	Effekseer::EffectRef m_Effect;
	public:
		static shared_ptr<EffectManager> g_Instance;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	stage	ゲームのステージオブジェクトなど、初期化に必要な情報を持つオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		EffectManager(const shared_ptr<Stage>& stage);

		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		@details	Effekseer関連のリソースを解放します。
		*/
		//--------------------------------------------------------------------------------------
		virtual ~EffectManager();

		//--------------------------------------------------------------------------------------
		/*!
		@brief	初期化処理
		@details	Effekseerのマネージャーとレンダラーを生成し、基本的な設定を行います。
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	更新処理
		@details	毎フレーム呼び出され、Effekseerマネージャーを更新してエフェクトのアニメーションを進めます。
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	描画処理。
		@details	毎フレーム呼び出され、管理しているエフェクトを描画します。
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDraw()override;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	終了処理
		@details	オブジェクトの破棄時に、確保したリソースを解放します。
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDestroy()override;

		/*!
		@brief	bsm::Mat4x4 から Effekseer::Matrix44 へ行列を変換します。
		@param[in]	src		変換元の行列 (bsm::Mat4x4)
		@param[out]	dest	変換先の行列 (Effekseer::Matrix44)
		*/
		void Mat4x4ToMatrix44(const bsm::Mat4x4& src, Effekseer::Matrix44& dest);

		/*!
		@brief	ビュー行列とプロジェクション行列をレンダラーに設定します。
		@param[in]	view	ビュー行列
		@param[in]	proj	プロジェクション行列
		*/
		void SetViewProj(const bsm::Mat4x4& view, const bsm::Mat4x4& proj);

		/*!
		@brief 指定したエフェクトの再生速度を設定します。
		@param[in] handle 対象のエフェクトハンドル
		@param[in] speed 再生速度 (1.0fが標準速)
		*/
		void SetEffectSpeed(Effekseer::Handle& handle, const float& speed);

		/*!
		@brief	マネージャーが管理する全てのエフェクトの更新を一時停止/再開します。
		@param[in]	pause	trueで一時停止、falseで再開
		*/
		void SetEffectPause(const bool& pause);

		/*!
		@brief	指定したキーのエフェクトを再生します。
		@param[out]	handle	再生されたエフェクトのハンドルがこの引数に格納されます。
		@param[in]	Key		リソースマップに登録したエフェクトのキー
		@param[in]	Emitter	エフェクトの発生座標
		@param[in]	freme	エフェクトの開始フレーム (0フレーム目から再生する場合は0.0f)
		*/
		void PlayEffect(Effekseer::Handle& handle, const wstring& Key, const bsm::Vec3& Emitter, const float freme);

		/*!
		@brief	Effekseerのマネージャーとレンダラーを生成し、初期設定を行います。
		*/
		void CreateEffectInterface();

		/*!
		@brief	エフェクトファイル(.efkefc)を読み込み、リソースマップに登録します。
		@param[in]	Key			登録するキー
		@param[in]	FileName	エフェクトファイルのパス
		*/
		void RegisterResource(const wstring& Key, const  wstring& FileName);

		/*!
		@brief	登録済みのエフェクトリソースを取得します。
		@param[in]	Key		取得したいエフェクトのキー
		@return	Effekseer::EffectRef	エフェクトリソースへの参照。見つからない場合はnullptr。
		*/
		Effekseer::EffectRef GetEffectResource(const wstring& Key);

		/*!
		@brief	エフェクトの座標を相対的に移動させます。
		@param[in]	handle		対象のエフェクトハンドル
		@param[in]	Location	現在の座標に加算する移動量
		*/
		void AddLocation(Effekseer::Handle& handle, const bsm::Vec3& Location);

		/*!
		@brief	エフェクトの座標を絶対座標で設定します。
		@param[in]	handle		対象のエフェクトハンドル
		@param[in]	Location	設定する座標
		*/
		void SetLocation(Effekseer::Handle& handle, const bsm::Vec3& Location);

		/*!
		@brief	エフェクトの回転を軸と角度で設定します。
		@param[in]	handle		対象のエフェクトハンドル
		@param[in]	Rotation	回転軸
		@param[in]	angle		回転角度 (ラジアン)
		*/
		void SetRotation(Effekseer::Handle& handle, const bsm::Vec3& Rotation, const float angle);

		/*!
		@brief	エフェクトの拡大縮小率を設定します。
		@param[in]	handle	対象のエフェクトハンドル
		@param[in]	Scale	XYZ各軸の拡大率
		*/
		void SetScale(Effekseer::Handle& handle, const bsm::Vec3& Scale);

		/*!
		@brief	エフェクト全体の色を設定します。
		@param[in]	handle	対象のエフェクトハンドル
		@param[in]	color	設定する色 (RGBA)
		*/
		void SetAllColor(Effekseer::Handle& handle, const bsm::Col4 color);

		/*!
		@brief	指定したエフェクトの強制破棄をします。
		@param[in]	handle	停止するエフェクトのハンドル
		*/
		void StopEffect(Effekseer::Handle& handle);

		/*!
		@brief	エフェクトの描画レイヤーを設定します。
		@details	レイヤーごとに描画タイミングを制御したい場合などに使用します。
		@param[in]	handle	対象のエフェクトハンドル
		@param[in]	layer	設定するレイヤー番号
		*/
		void SetLayer(Effekseer::Handle& handle, int32_t layer);

		/*!
		@brief	エフェクトの描画レイヤーを取得します。
		@param[in]	handle	対象のエフェクトハンドル
		@return	レイヤー番号
		*/
		int32_t GetLayer(Effekseer::Handle& handle)
		{
			return m_Manager->GetLayer(handle);
		}
	};
}
//end basecross
