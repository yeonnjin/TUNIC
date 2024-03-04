#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
    // 해당 노드가 가르키는 뼈를 찾아서 이름과 인덱스 저장
    strcpy_s(m_szName, pAIChannel->mNodeName.data);

    auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
    {
        ++m_iBoneIndex;
        return pBone->Compare_Name(m_szName);
    });

    // 3가지 조건 중에 가장 큰 값을 찾아 KeyFrame의 개수로 저장
    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

    _float3     vScale;
    _float4     vRotation;
    _float3     vTranslation;
    _float      fTime;          // 3개 중 하나만 걸릴 수 있으므로 매 번 값을 받아줌

    for (size_t i = 0; i < m_iNumKeyFrames; ++i)
    {
        KEYFRAME    tKeyFrame{};

        if (i < pAIChannel->mNumScalingKeys)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));

            fTime = pAIChannel->mScalingKeys[i].mTime;
        }

        if (i < pAIChannel->mNumRotationKeys)
        {
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

            fTime = pAIChannel->mRotationKeys[i].mTime;
        }

        if (i < pAIChannel->mNumPositionKeys)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            
            fTime = pAIChannel->mPositionKeys[i].mTime;
        }

        tKeyFrame.vScale = vScale;
        tKeyFrame.vRotation = vRotation;
        tKeyFrame.vTranslation = vTranslation;
        tKeyFrame.fTime = fTime;

        m_KeyFrames.push_back(tKeyFrame);
    }

    return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition)
{
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
        if (fTrackPosition >= m_KeyFrames[m_iCurrentKeyFrame + 1].fTime)
            ++m_iCurrentKeyFrame;

        // 앞 뒤 KeyFrame의 Time 사이에서 얼마나 지났는지 비율을 구함
        _float  fRatio =  (fTrackPosition - m_KeyFrames[m_iCurrentKeyFrame].fTime)
                        / (m_KeyFrames[m_iCurrentKeyFrame + 1].fTime - m_KeyFrames[m_iCurrentKeyFrame].fTime);

        // 선형보간
        XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame].vScale), XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1].vScale), fRatio));
        XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vRotation), XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vRotation), fRatio));
        XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame].vTranslation), XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame + 1].vTranslation), fRatio));
    }

    // 스 * 자 * 이 행렬
    _matrix     TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f));

    // Bone의 TransformationMatrix 세팅
    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
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