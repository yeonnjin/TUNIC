#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CPlayer_State_Climb final : public CState
{
private:
	CPlayer_State_Climb(class CPlayer* pPlayer);
	virtual ~CPlayer_State_Climb() = default;

public:
	// 상태 진입 시 최초에 한 번만 호출
	virtual void OnStateEnter() override;
	// 상태 진입 상태에서 매 tick 마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태 변경 시 호출
	virtual void OnStateExit() override;

private:
	class CPlayer*		m_pPlayer = { nullptr };

	_bool				m_isStart = { true };
	_bool				m_isStartEndClimb = { false };
	_bool				m_isEndClimb = { false };
	_bool				m_isEndLadder = { false };

	_bool				m_isPuzzle = { true };

	_float				m_fBlendAccTime = { 0.f };
	_float				m_fBlendTime = { 0.21f };

	_float				m_fAccSoundTime = { 0.f };
	_float				m_fSoundTime = { 0.4f };

	CPlayer::ANIMATION	m_eAnimation = { CPlayer::ANIM_END };

private:
	void				Compute_Dir();

public:
	static CPlayer_State_Climb* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

END