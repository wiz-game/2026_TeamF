/*!
@file Sprite.cpp
@brief スプライト
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	Sprite::Sprite(const shared_ptr<Stage>& ptr, const wstring& texKey, Vec3 pos, Vec2 size, Anchor anchor) : Sprite(ptr, texKey, pos, size, {0,0}) {		
		SetPivot(GetAnchorNormalize(anchor));
	}


	void Sprite::OnCreate() {
		CreateAnimationUV(Vec2(1, 1), 1);
		CreateVertex(m_Size, m_AnimationUV[0]);
		vector<uint16_t> indices = {
			0, 1, 2,
			2, 1, 3
		};

		m_Draw = AddComponent<PCTSpriteDraw>(m_Vertices, indices);
		if (m_TexKey != L"") {
			m_Draw->SetTextureResource(m_TexKey);
		}
		SetAlphaActive(true);
		m_Draw->SetSamplerState(SamplerState::LinearWrap);
		m_Draw->SetDiffuse(Col4(1, 1, 1, 1));
		m_Transform = GetComponent<Transform>();
		m_Transform->SetPosition(m_Pos);

		m_ScreenHalfSize.x = App::GetApp()->GetGameWidth() * 0.5f;
		m_ScreenHalfSize.y = App::GetApp()->GetGameHeight() * 0.5f;
	}
	void Sprite::OnUpdate() {
		if (m_IsAnimation) {
			Animation();
		}
		//情報更新
		m_Pos = m_Transform->GetPosition();
	}
	void Sprite::Animation() {
		float elapsedTime = App::GetApp()->GetElapsedTime();

		if (m_CurrentAnimation.Update(elapsedTime)) {
			UpdateUV(m_AnimationUV[m_CurrentAnimation.m_CurrentOrder]);
		}
	}
	vector<vector<Vec2>> Sprite::CreateAnimationUV(Vec2 cut, const size_t& maxIndex) {

		if (cut.x == 0.0f || cut.y == 0.0f) return {};

		vector<vector<Vec2>> uv;
		uv.reserve(maxIndex);
		m_AnimationUV.reserve(maxIndex);

		size_t cutX = cut.x, cutY = cut.y;
		size_t maxImageCount = cutX * cutY;

		float stepX = 1.0f / cut.x, stepY = 1.0f / cut.y;

		for (size_t i = 0; i < maxIndex && i < maxImageCount; i++) {
			size_t x = i % cutX;
			size_t y = i / cutX;

			uv.push_back({
				{stepX *  x,		stepY *  y		},
				{stepX * (x + 1),	stepY *  y		},
				{stepX *  x,		stepY * (y + 1)	},
				{stepX * (x + 1),	stepY * (y + 1)	}
			});
		}
		m_AnimationUV = uv;
		m_IsAnimation = uv.size() > 1 ? true : false;
		m_MaxAnimationFrame = uv.size();

		return uv;
	}
	void Sprite::CreateVertex(Vec2 size, vector<Vec2> uv) {

		m_Vertices = {
			{Vec3(-size.x *			m_Pivot.x,	 size.y *			m_Pivot.y ,0),Col4(1,1,1,1),uv[0]},
			{Vec3( size.x *	(1.0f - m_Pivot.x),	 size.y *			m_Pivot.y ,0),Col4(1,1,1,1),uv[1]},
			{Vec3(-size.x *			m_Pivot.x,	-size.y *	(1.0f - m_Pivot.y),0),Col4(1,1,1,1),uv[2]},
			{Vec3( size.x * (1.0f - m_Pivot.x),	-size.y *	(1.0f - m_Pivot.y),0),Col4(1,1,1,1),uv[3]}
		};
	}
	void Sprite::SetVertex(vector<Vec3> positions) {
		for (int i = 0; i < m_Vertices.size(); i++) {
			if (positions.size() <= i) break;
			m_Vertices[i].position = positions[i];
		}
		m_Draw->UpdateVertices(m_Vertices);
	}
	void Sprite::UpdateUV(vector<Vec2> uv) {
		if (uv.empty()) return;
		if (m_Draw) {
			for (int i = 0; i < m_Vertices.size(); i++)
			{
				m_Vertices[i].textureCoordinate = uv[i];
			}

			m_Draw->UpdateVertices(m_Vertices);
		}
	}
	void Sprite::SetSize(Vec2 size) {
		if (m_Draw) {
			m_Size = size;
			if (m_IsAnimation) {
				CreateVertex(m_Size, m_AnimationUV[m_CurrentAnimation.m_CurrentOrder]);
			}
			else {
				CreateVertex(m_Size, m_AnimationUV[0]);
			}
			m_Draw->UpdateVertices(m_Vertices);
		}
	}

	void Sprite::SetPosition(Vec3 pos) {
		m_Transform->SetPosition(pos);
		m_Pos = pos;
	}
	void Sprite::VectorToward(Vec2 vec) {
		if (vec.length() == 0) return;
		float rad = atan2f(-vec.x, vec.y);
		auto rot = XMMatrixRotationAxis(Vec3(0,0,1), rad);
		auto world = m_Transform->GetWorldMatrix();
		world.rotation((Quat)XMQuaternionRotationMatrix(rot));

		m_Transform->SetQuaternion(world.quatInMatrix());
	}

	void Sprite::SetDiffuse(Col4 color) {
		m_Draw->SetDiffuse(color);
	}
	Col4 Sprite::GetDiffuse() const{
		return m_Draw->GetDiffuse();
	}
	Vec2 Sprite::GetAnchorNormalize(Anchor anchor) {
		switch (anchor)
		{
		case Anchor::Center:	 return Vec2(0.5f, 0.5f); 
		case Anchor::Top:		 return Vec2(0.5f, 0.0f);
		case Anchor::Bottom:	 return Vec2(0.5f, 1.0f);
		case Anchor::Left:		 return Vec2(0.0f, 0.5f);
		case Anchor::Right:		 return Vec2(1.0f, 0.5f);
		case Anchor::TopLeft:	 return Vec2(0.0f, 0.0f);
		case Anchor::TopRight:	 return Vec2(1.0f, 0.0f);
		case Anchor::BottomLeft: return Vec2(0.0f, 1.0f);
		case Anchor::BottomRight:return Vec2(1.0f, 1.0f);
		default: return Vec2();
		}
	}
	void Sprite::ScreenAnchor(Anchor anchor, const Vec3& offset) {

		Vec3 pos;

		switch (anchor)
		{
		case Anchor::Center:		pos = Vec3(); break;
		case Anchor::Top:			pos = Vec3(0.0f, m_ScreenHalfSize.y, 0.0f); break;
		case Anchor::Bottom:		pos = Vec3(0.0f, -m_ScreenHalfSize.y, 0.0f); break;
		case Anchor::TopLeft:		pos = Vec3(-m_ScreenHalfSize.x, m_ScreenHalfSize.y, 0.0f); break;
		case Anchor::TopRight:		pos = Vec3(m_ScreenHalfSize.x, m_ScreenHalfSize.y, 0.0f); break;
		case Anchor::BottomLeft:	pos = Vec3(-m_ScreenHalfSize.x, -m_ScreenHalfSize.y, 0.0f); break;
		case Anchor::BottomRight:	pos = Vec3(m_ScreenHalfSize.x, -m_ScreenHalfSize.y, 0.0f); break;
		case Anchor::Left:			pos = Vec3(-m_ScreenHalfSize.x, 0.0f, 0.0f); break;
		case Anchor::Right:			pos = Vec3(m_ScreenHalfSize.x, 0.0f, 0.0f); break;
		default:					pos = Vec3(); break;
		}

		m_Transform->SetPosition(pos + offset);
	}
	void Sprite::ChangeWindowSize(float width, float height) {
		float rateW = width - m_ScreenHalfSize.x * 2.0f;
		float rateH = height - m_ScreenHalfSize.y * 2.0f;

		Vec3 position = GetPosition();
		SetPosition(Vec3(position.x * rateW, position.y * rateH, position.z));
		SetSize(Vec2(m_Size.x * rateW, m_Size.y * rateH));

		m_ScreenHalfSize = Vec2(width * 0.5f, height * 0.5f);
	}
	Vec2 Sprite::GetAnchorPosition(Anchor anchor) {
		Vec3 pos = GetPosition();
		Vec2 size = GetSize();
		Vec2 leftTop = Vec2(pos.x - m_Pivot.x * size.x, pos.y + m_Pivot.y * size.y);
		Vec2 offset;
		offset = GetAnchorNormalize(anchor) * GetSize();
		offset.y *= -1;
		return leftTop + offset;
	}
	void Sprite::SetAnchorPosition(Vec3 pos, Anchor anchor) {
		Vec3 offset;
		offset = GetAnchorNormalize(anchor) * GetSize();
		offset.y *= -1;
		Vec3 leftTop = pos - offset;
		
		Vec3 newPosition = leftTop + static_cast<Vec3>(Vec2(m_Pivot.x,-m_Pivot.y) * GetSize());

		SetPosition(newPosition);
	}
	void NumberSprite::OnCreate() {
		int digits = static_cast<int>(pow(10, m_DisplayDigit - 1));
		float sizeX = m_Size.x / m_DisplayDigit;
		m_Numbers.reserve(m_DisplayDigit);
		m_DrawActives.resize(m_DisplayDigit, 0);
		bool isFirst = true;
		for (int i = 0; i < m_DisplayDigit; i++) {
			shared_ptr<Sprite> number = GetStage()->AddGameObject<Sprite>(m_TexKey, Vec3(m_Pos.x + i * sizeX, m_Pos.y, m_Pos.z), Vec2(sizeX, m_Size.y),Anchor::TopLeft);//ObjectFactory::Create<Sprite>(GetStage(), m_TexKey, Vec3(0,0,0)/*m_Pos + i * sizeX*/, Vec2(sizeX, m_Size.y));
			int singleDigit = m_DisplayNumber / digits % 10;
			digits /= 10;
			auto trans = number->GetComponent<Transform>();
			trans->SetParent(GetThis<NumberSprite>());
			m_Numbers.push_back(number);

			if (isFirst && singleDigit == 0) continue;

			isFirst = false;
			number->UpdateUV(GetUV(singleDigit));
			m_DrawActives[i] = 1;
		}
		auto trans = GetComponent<Transform>();
		
		trans->SetPosition(0, 0, 0);
	}

	void NumberSprite::OnUpdate() {
		for (int i = 0; i < m_Numbers.size(); i++) {
			bool isActive = GetDrawActive() && m_DrawActives[i];
			m_Numbers[i]->SetDrawActive(isActive);
		}
	}

	vector<Vec2> NumberSprite::GetUV(int displayDigit) {
		float uvX = 1.0f / m_CutNum;

		vector<Vec2> uv = {
				{uvX * displayDigit,0.0f},
				{uvX * (displayDigit + 1),0.0f},
				{uvX * displayDigit,1.0f},
				{uvX * (displayDigit + 1),1.0f}
		};

		return uv;
	}

	void NumberSprite::UpdateNumber(int number) {
		m_DisplayNumber = number;
		bool isFirst = true;

		int digits = static_cast<int>(pow(10, m_DisplayDigit - 1));

		for (int i = 0; i < m_Numbers.size(); i++) {
			int singleDigit = m_DisplayNumber / digits % 10;
			digits /= 10;
			if (isFirst && singleDigit == 0) {
				m_DrawActives[i] = 0;
				continue;
			}

			isFirst = false;
			m_Numbers[i]->UpdateUV(GetUV(singleDigit));
			m_DrawActives[i] = 1;
		}
	}
	void NumberSprite::Destroy() {
		for (int i = 0; i < m_Numbers.size(); i++) {
			GetStage()->RemoveGameObject<Sprite>(m_Numbers[i]);
		}
		GetStage()->RemoveGameObject<Sprite>(GetThis<NumberSprite>());
	}

	void SpriteAction::OnCreate() {
		m_Draw = GetGameObject()->GetComponent<SpriteBaseDraw>();
		m_Trans = GetGameObject()->GetComponent<Transform>();
	}

	void SpriteFlash::Reset() {
		if (m_Draw != nullptr) {
			Col4 color = m_Draw->GetDiffuse();
			color.w = 1.0f;
			m_Draw->SetDiffuse(color);
		}
	}
	void SpriteFlash::OnUpdate() {
		if (m_Draw != nullptr && IsPlay()) {
			float elapsed = App::GetApp()->GetElapsedTime();
			Col4 color = m_Draw->GetDiffuse();
			color.w += m_FlashSpeed * elapsed;
			if (color.w < 0 || color.w > 1) {
				m_FlashSpeed *= -1;
				if (color.w < 0) {
					color.w = 0;
				}
				if (color.w > 1) {
					color.w = 1;
				}
			}
			m_Draw->SetDiffuse(color);
		}
	}
	void SpriteScaling::OnCreate() {
		SpriteAction::OnCreate();

		defaultSize = m_Trans->GetScale();
	}
	void SpriteScaling::OnUpdate() {
		if (m_Trans != nullptr && IsPlay()) {
			float elapsed = App::GetApp()->GetElapsedTime();
			m_Ratio += m_ScalingSpeed * elapsed;
			if (m_Ratio < m_MinRatio || m_Ratio > m_MaxRatio) {
				m_ScalingSpeed *= -1;
			}
			m_Trans->SetScale(defaultSize * m_Ratio);
		}
	}

	void SpriteFade::OnUpdate() {
		if (m_Draw != nullptr && IsPlay()) {
			float elapsed = App::GetApp()->GetElapsedTime();
			if (!m_IsFadeOut) {
				elapsed *= -1;
			}
			Col4 color = m_Draw->GetDiffuse();
			if (!IsFinish()) {
				color.w += elapsed * m_FadeSpeed;
			}
			if (color.w < 0.0f) {
				if (m_FadeState == FadeState::OutToIn || m_FadeState == FadeState::In) m_IsFinished = true;
				else if (m_FadeState == FadeState::InToOut) {
					m_IsFadeOut = true;
					m_HalfFinished = true;
				}
			}
			else if (color.w > 1.0f) {
				if (m_FadeState == FadeState::InToOut || m_FadeState == FadeState::Out) m_IsFinished = true;
				else if (m_FadeState == FadeState::OutToIn) {
					m_IsFadeOut = false;
					m_HalfFinished = true;
				}

			}
			else {
				m_IsFinished = false;
			}
			m_Draw->SetDiffuse(color);
		}
	}
	void SpriteButton::OnCreate() {
		m_SpriteDraw = GetGameObject()->GetComponent<SpriteBaseDraw>();
		m_UnSelectColor = m_SpriteDraw->GetDiffuse();
		if (m_SelectedColor.w == 1.0f) {
			m_UnSelectColor.w = 0.0f;
		}
		
		ButtonManager::instance->Register(m_BelongGroup, GetThis<SpriteButton>());
	}
	void SpriteButton::OnUpdate() {

		if (m_AddFunction != nullptr) {
			m_AddFunction(GetThis<SpriteButton>());
		}
		if (m_IsSelect) {
			if (m_SelectedTexture != L"") {
				m_SpriteDraw->SetTextureResource(m_SelectedTexture);
			}
			if (m_SelectedColor != Col4(0, 0, 0, 0)) {
				m_SpriteDraw->SetDiffuse(m_SelectedColor);
			}
			if (m_Effect != nullptr) {
				m_Effect->Play();
			}
		}
		else {
			m_SpriteDraw->SetTextureResource(m_UnSelectTexture);
			m_SpriteDraw->SetDiffuse(m_UnSelectColor);
			if (m_Effect != nullptr) {
				m_Effect->Reset();
				m_Effect->Stop();
			}
		}
	}


	shared_ptr<ButtonManager> ButtonManager::instance = nullptr;

	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, selectedTex, Col4(), pos, size, nullptr, func);
	}
	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Col4 selectedColor, Vec3 pos, Vec2 size, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, L"", selectedColor, pos, size, nullptr, func);
	}
	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, selectedTex, Col4(), pos, size, object, func);
	}
	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, Col4 selectedColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		return ButtonManager::instance->Create(stage, group, defaultTex, L"", selectedColor, pos, size, object, func);
	}



	shared_ptr<Sprite> ButtonManager::Create(shared_ptr<Stage>& stage, const wstring& group, const wstring& defaultTex, const wstring& selectedTex, Col4 selectedColor, Vec3 pos, Vec2 size, const shared_ptr<ObjectInterface>& object, function<void(shared_ptr<ObjectInterface>&)> func) {
		auto sprite = stage->AddGameObject<Sprite>(defaultTex, pos, size, Anchor::TopLeft);
		sprite->AddTag(L"Button");
		sprite->SetLayer(11);
		shared_ptr<SpriteButton> button = nullptr;

		if (selectedTex != L"") {
			button = sprite->AddComponent<SpriteButton>(defaultTex, group, selectedTex);
		}
		else if (selectedColor != Col4()) {
			button = sprite->AddComponent<SpriteButton>(defaultTex, group, selectedColor);
		}

		button->SetFunction(func, object);
		return sprite;
	}

	void ButtonManager::InitGroup(const wstring& group) {
		if (m_ButtonGroup.find(group) != end(m_ButtonGroup)) {
			for (auto& button : m_ButtonGroup[group]) {
				button->UnSelect();
				//m_SelectIndexes[group] = 0;
			}
		}
	}

	void ButtonManager::OnCreate() {
		AddTag(L"Manager");
		instance = GetThis<ButtonManager>();
	}
	void ButtonManager::OnUpdate() {
		if (!CheckUpdate()) return;

		size_t& selectIndex = m_SelectIndexes[m_UsingGroup];

		//選択中のボタンが非アクティブ状態なら移動する
		if (!m_ButtonGroup[m_UsingGroup][selectIndex]->GetActive()) {
			//基本的には前に移動する。移動できない時は次に移動
			selectIndex = selectIndex > 0 ? selectIndex - 1 : selectIndex + 1;
		}

		InputData data(0, 0);
		for (auto& pressdSelect : m_SelectPressed) {
			pressdSelect.second = data;
		}
		//選択
		if (PressSelect(m_UsingGroup,data)) {
			int checkButton = static_cast<int>(selectIndex) + data.m_MoveAmount;
			if (LimitIndex(checkButton)) {
				m_SelectPressed[m_UsingGroup] = data;
			}
			if (m_ButtonGroup[m_UsingGroup][checkButton]->GetActive()) {
				if (m_SelectSound != L"") {
					SoundManager::Get().PlaySE(m_SelectSound);
				}
				selectIndex = checkButton;
			}
		}
		m_SelectIndexes[m_UsingGroup] = selectIndex;
		//選択状態の適用
		for (int i = 0; i < m_ButtonGroup[m_UsingGroup].size(); i++) {
			if (i == m_SelectIndexes[m_UsingGroup]) {
				m_ButtonGroup[m_UsingGroup][i]->Select();
			}
			else {
				m_ButtonGroup[m_UsingGroup][i]->UnSelect();
			}
		}
		//決定
		if (PressAccept(m_UsingGroup, m_PressedAccept[m_UsingGroup])) {
			if (m_ClickSound != L"") {
				SoundManager::Get().PlaySE(m_ClickSound);
			}
			m_ButtonGroup[m_UsingGroup][m_SelectIndexes[m_UsingGroup]]->Func();
		}

		//移動
		for (auto& groupMovementAmount : m_GroupMovementAmount) {
			Vec3 movementAmount = groupMovementAmount.second;
			if (movementAmount.length() != 0) {
				movementAmount = movementAmount.normalize();
				movementAmount *= m_GroupMovementSpeed;//移動速度
				if (groupMovementAmount.second.length() < movementAmount.length()) {
					movementAmount = groupMovementAmount.second;
					m_IsMoveStop = true;
				}
				for (auto& button : m_ButtonGroup[groupMovementAmount.first]) {
					auto trans = button->GetGameObject()->GetComponent<Transform>();
					Vec3 pos = trans->GetPosition();
					pos += movementAmount;
					trans->SetPosition(pos);
				}
				groupMovementAmount.second -= movementAmount;
			}
		}

	}

	void ButtonManager::OnDestroy() {
		instance = nullptr;
	}
	bool ButtonManager::CheckUpdate() {
		if (!m_IsActive) return false;
		if (!ExistOpenGroup()) {
			m_IsActive = false;
			return false;
		}
		if (m_SelectIndexes.size() == 0 && m_ButtonGroup.size() == 0) return false;

		return true;
	}
	bool ButtonManager::CheckMoveInput(InputData& input) {

		auto& inputState = App::GetApp()->GetInputDevice().GetControlerVec()[0];
		if (input.m_Mode == InputMode::Button) {
			return input.CheckInput(inputState.wPressedButtons);
		}
		float inputThumb = 0.0f;
		switch (input.m_StickMode) {
		case StickMode::LX:
			inputThumb = inputState.fThumbLX;
			break;
		case StickMode::LY:
			inputThumb = -inputState.fThumbLY;
			break;
		case StickMode::RX:
			inputThumb = inputState.fThumbRX;
			break;
		case StickMode::RY:
			inputThumb = -inputState.fThumbRY;
			break;
		}
		return input.CheckInput(inputThumb);

	}
	void ButtonManager::SetSound(const wstring& sound) {
		m_ClickSound = sound;
	}
	void ButtonManager::SetSelectSound(const wstring& sound) {
		m_SelectSound = sound;
	}

	void ButtonManager::SetMoveAmount(const wstring& group, Vec3 target) {
		if (FindGroup(m_GroupMovementAmount, group)) {
			m_GroupMovementSpeed = 30.0f;
			m_IsMoveStop = false;
			if (m_GroupMovementAmount[group].length() == 0) {
				m_GroupMovementAmount[group] = target;
			}
		}
	}


	void ButtonManager::SetMoveAmount(const wstring& group, Vec3 target, float speed) {
		if (FindGroup(m_GroupMovementAmount, group)) {
			m_GroupMovementSpeed = speed;
			m_IsMoveStop = false;
			if (m_GroupMovementAmount[group].length() == 0) {
				m_GroupMovementAmount[group] = target;
			}
		}
	}
}
//end basecross
