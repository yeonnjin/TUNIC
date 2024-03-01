#pragma once

#include "Base.h"

/* 특정 애니메이션이 사용하고 있는 뼈들 중, 하나의 정보를 표현 */
/* 이 뼈는 시간에 따라 어떤 상태(KeyFrame)을 취해야 하는가? */
/* aiNodeAnim == Channel */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void	Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition);

private:
	_char				m_szName[MAX_PATH] = { "" };
	_int				m_iBoneIndex = { -1 };

	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;
	_uint				m_iCurrentKeyFrame = { 0 };		// 현재 뼈는 어떤 키 프레임을 취해야 하는가?

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END