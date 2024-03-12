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
	_uint			Get_BoneIndex() { return m_iBoneIndex; }
	CHANNELSTATE*	Get_ChannelState() { return &m_tChannelState; }

public:
	HRESULT Initialize(CHANNELFILE* pChannelFile, const vector<class CBone*>& Bones);
	void	Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex);

private:
	_char				m_szName[MAX_PATH] = { "" };
	_int				m_iBoneIndex = { -1 };

	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

	// Linear
	CHANNELSTATE		m_tChannelState = {};

public:
	static CChannel* Create(CHANNELFILE* pChannelFile, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END