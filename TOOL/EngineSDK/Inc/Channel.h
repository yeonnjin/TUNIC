#pragma once

#include "Base.h"

/* 특정 애니메이션이 사용하고 있는 뼈들 중, 하나의 정보를 표현 */
/* 이 뼈는 시간에 따라 어떤 상태(KeyFrame)을 취해야 하는가? */
/* aiNodeAnim == Channel */

BEGIN(Engine)

class ENGINE_DLL CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	// File
	CHANNELFILE* Get_ChannelFile() { Ready_ChannelFile(); return &m_tChannelFile; }

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void	Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex);

private:
	_char				m_szName[MAX_PATH] = { "" };
	_int				m_iBoneIndex = { -1 };

	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

	CHANNELFILE			m_tChannelFile = {};

private:
	HRESULT				Ready_ChannelFile();

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END