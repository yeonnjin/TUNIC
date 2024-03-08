#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(CHANNELFILE* pChannelFile, const vector<class CBone*>& Bones)
{
    // 해당 노드가 가르키는 뼈를 찾아서 이름과 인덱스 저장
    strcpy_s(m_szName, pChannelFile->szName);

    auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
    {
        ++m_iBoneIndex;
        return pBone->Compare_Name(m_szName);
    });

    // 3가지 조건 중에 가장 큰 값을 찾아 KeyFrame의 개수로 저장
    m_iNumKeyFrames = pChannelFile->iNumKeyFrames;

    _float3     vScale;
    _float4     vRotation;
    _float3     vTranslation;
    _float      fTime;          // 3개 중 하나만 걸릴 수 있으므로 매 번 값을 받아줌

    for (size_t i = 0; i < m_iNumKeyFrames; ++i)
        m_KeyFrames.push_back(pChannelFile->KeyFrames[i]);

    return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex)
{
    if (0.f == fTrackPosition)
        (*pCurrentKeyFrameIndex) = 0;

    KEYFRAME    tKeyFrame = m_KeyFrames.back();

    _float3     vScale;
    _float4     vRotation;
    _float3     vTranslation;

    // 마지막 KeyFrame 을 지났을 땐 같은 상태 유지
    if (tKeyFrame.fTime <= fTrackPosition)
    {
        vScale = tKeyFrame.vScale;
        vRotation = tKeyFrame.vRotation;
        vTranslation = tKeyFrame.vTranslation;
    }
    // 이전일 경우 
    else
    {
        // 정확한 CurrentKeyFrame 찾아주기
        while (fTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime)
            ++(*pCurrentKeyFrameIndex);

        // 앞 뒤 KeyFrame의 Time 사이에서 얼마나 지났는지 비율을 구함
        _float  fRatio =  (fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime)
                        / (m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime);

        // 선형보간
        XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio));
        XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation), XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio));
        XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio));
    }

    // 스 * 자 * 이 행렬
    _matrix     TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f));

    // Bone의 TransformationMatrix 세팅
    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(CHANNELFILE* pChannelFile, const vector<class CBone*>& Bones)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pChannelFile, Bones)))
    {
        MSG_BOX(TEXT("Failed To Create : CChannel"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CChannel::Free()
{
    m_KeyFrames.clear();
}