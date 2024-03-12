#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(ANIMFILE* pAnimFile, const vector<class CBone*>& Bones)
{
    strcpy_s(m_szName, pAnimFile->szName);

    m_fDuration = pAnimFile->fDuration;
    m_fTicksPerSecond = pAnimFile->fTicksPerSecond;

    /* 이 애니메이션은 몇 개의 뼈를 컨트롤 해야하는지 */
    m_iNumChannels = pAnimFile->iNumChannels;

    /* 각 채널의 CurrentKeyFrame을 0으로 초기화 */
    m_CurrentKeyFrameIndices.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; ++i)
    {
        CChannel* pChannel = CChannel::Create(&pAnimFile->Channels[i], Bones);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

void CAnimation::Invalidate_Blending(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop)
{
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        /* 이 뼈의 생태 행렬을 만들어서 CBone의 TransformationMatrix를 바꿈 */
        m_Channels[i]->Invalidate_TransformationMatrix(Bones, 0, &m_CurrentKeyFrameIndices[i]);
    }
}


void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop)
{
    m_isFinished = false;

    m_fTrackPosition += m_fTicksPerSecond * fTimeDelta;

    if (m_fDuration <= m_fTrackPosition)
    {
        // 반복 상태가 아닐 때
        if (false == isLoop)
        {
            m_isFinished = true;
            return;
        }

        // 반복 상태일 때 초기화
        m_fTrackPosition = 0.f;
    }

    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        /* 이 뼈의 생태 행렬을 만들어서 CBone의 TransformationMatrix를 바꿈 */
        m_Channels[i]->Invalidate_TransformationMatrix(Bones, m_fTrackPosition, &m_CurrentKeyFrameIndices[i]);
    }
}

CAnimation* CAnimation::Create(ANIMFILE* pAnimFile, const vector<class CBone*>& Bones)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(pAnimFile, Bones)))
    {
        MSG_BOX(TEXT("Failed To Create : CAnimation"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Clone()
{
    CAnimation* pInstance = new CAnimation(*this);
    pInstance->Set_Cloned();
    return pInstance;
}

void CAnimation::Free()
{
    __super::Free();

    if (!m_isCloned)
    {
        for (auto& pChannel : m_Channels)
            Safe_Release(pChannel);

        m_Channels.clear();
    }
}
