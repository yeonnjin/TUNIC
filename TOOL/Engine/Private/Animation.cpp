#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
    strcpy_s(m_szName, pAIAnimation->mName.data);

    m_fDuration = pAIAnimation->mDuration;
    m_fTicksPerSecond = pAIAnimation->mTicksPerSecond;

    /* 이 애니메이션은 몇 개의 뼈를 컨트롤 해야하는지 */
    m_iNumChannels = pAIAnimation->mNumChannels;

    for (size_t i = 0; i < m_iNumChannels; ++i)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones)
{
    m_fTrackPosition += m_fTicksPerSecond * fTimeDelta;

    for (auto* pChannel : m_Channels)
    {
        /* 이 뼈의 생태 행렬을 만들어서 CBone의 TransformationMatrix를 바꿈 */
        pChannel->Invalidate_TransformationMatrix(Bones, m_fTrackPosition);
    }
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
    {
        MSG_BOX(TEXT("Failed To Create : CAnimation"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);

    m_Channels.clear();
}
