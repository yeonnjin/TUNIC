#pragma once

#include "Base.h"

/*
1. Animation : 애니메이션을 관리하고 애니메이션을 실행
2. Animation State : 하나의 애니메이션의 상태 정보와 애니메이션 클립 정보를 관리
3. Animation Clip : 애니메이션 데이터(Idle, Walk) 커브들의 집합
4. Clip Property : 변화되는 속성 (애니메이션 커브를 가짐)
5. Animation Curve : 시간에 대한 움직임 정보 (키 프레임들의 집합 (키 프레임 애니메이션))
6. keyFrame : 위치(시간)
*/

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;
	
public:
	
	// Set
	void	Set_Cloned() { m_isCloned = true; }

	void	Set_Loop(_bool isLoop) { m_isLoop = isLoop; }

	void	Set_AnimationData_Initialize();

	void	Invalidate_Blending(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);

	// Get
	_bool	isFinished() const {
		return m_isFinished;
	}

	_bool			Get_isLoop() { return m_isLoop; }
	_uint			Get_NumChannels() { return m_iNumChannels; }
	class CChannel* Get_Channel(_uint iChannelIndex) { return m_Channels[iChannelIndex]; }

public:
	HRESULT Initialize(ANIMFILE* pAnimFile, const vector<class CBone*>& Bones);
	void	Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones);

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
	_bool						m_isLoop = { false };
	
	/* 프로퍼티 */
	// 1. Start :	클립의 시작 프레임.
	// 2. End :	클립의 종료 프레임.
	// 3. Loop Time : 애니메이션 클립이 재생되다가 끝날 때 다시 시작하도록 설정하려면 이 옵션을 활성화해야 합니다.
	//		- 1) Loop Pose : 모션 루프를 끊기지 않게 하려면 활성화해야 합니다.
	//		- 2) Cycle Offset : 루핑 애니메이션의 사이클에 대한 오프셋, 애니메이션을 다른 시간에 시작하고 싶은 경우 설정해야 합니다.
	// 4. Mask : 현재 애니메이션 클립에 적용된 바디 마스크나 트랜스폼 마스크입니다(아바타 마스크 섹션 참조).
	// 5. Events : 클립에 새로운 이벤트를 생성할 때 사용합니다(애니메이션 이벤트 사용 참조).
	// 

public:
	static CAnimation* Create(ANIMFILE* pAnimFile, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END 