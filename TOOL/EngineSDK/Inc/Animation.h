#pragma once

#include "Base.h"

/* 하나의 애니메이션이 몇 개의 뼈를 컨트롤 해야하는지, 그 뼈들의 정보는 뭔지 => Channel */
/* 이 애니메이션을 재생하는데 걸리는 총 거리 => Duration */
/* 이 애니메이션을 구동하는 속도 => TickPerSecond*/

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;
	
public:
	_bool	isFinished() const {
		return m_isFinished;
	}

	void	Set_Cloned() {
		m_isCloned = true;
	}

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	void	Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);

private:
	_char						m_szName[MAX_PATH] = { "" };

	_float						m_fDuration = { 0.f };			// 전체 재생 길이
	_float						m_fTicksPerSecond = { 0.f };	// 초당 얼마나 재생하는지 (속도)
	_float						m_fTrackPosition = { 0.f };		// 현재 애니메이션이 어디까지 재생되었는지

	_uint						m_iNumChannels = { 0 };
	vector<class CChannel*>		m_Channels;
	vector<_uint>				m_CurrentKeyFrameIndices;

	_bool						m_isFinished = { false };
	_bool						m_isCloned = { false };

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END