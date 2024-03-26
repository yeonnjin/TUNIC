#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(BONEFILE* pBoneFile)
{
    m_iParentBoneIndex = pBoneFile->iParentBoneIndex;
    strcpy_s(m_szName, pBoneFile->szName);
    memcpy(&m_TransformationMatrix, &pBoneFile->TransformationMatrix, sizeof(_float4x4));
    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
  
    return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix TransformationMatrix)
{
    // 3 == Root Bone Index
    if (false == m_isUse)
    {
        if (m_iCustomBoneIndex == m_iParentBoneIndex || -1 == m_iParentBoneIndex)
        {
            XMStoreFloat4x4(&m_CombinedTransformationMatrix,
                TransformationMatrix);
        }
        else
        {
            XMStoreFloat4x4(&m_CombinedTransformationMatrix,
                XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
        }
    }
    else
    {
        if (-1 == m_iParentBoneIndex)
        {
            XMStoreFloat4x4(&m_CombinedTransformationMatrix,
                TransformationMatrix);
        }
        else
        {
            XMStoreFloat4x4(&m_CombinedTransformationMatrix,
                XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
        }
    } 
}

CBone* CBone::Create(BONEFILE* pBoneFile)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize(pBoneFile)))
    {
        MSG_BOX(TEXT("Failed To Create : CBone"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CBone* CBone::Clone()
{
    return new CBone(*this);
}


void CBone::Free()
{
    __super::Free();
}
