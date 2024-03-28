#include "Animator_Controller.h"

#include "Bone.h"
#include "Avatar.h"
#include "Channel.h"
#include "Animation.h"
#include "Transform.h"
#include "State_Machine.h"

#define ROOTBONEIDX 3

CAnimator_Controller::CAnimator_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
}

//CAnimator_Controller::CAnimator_Controller(const CAnimator_Controller& rhs)
//{
//}


HRESULT CAnimator_Controller::Play_Animation(_float fTimeDelta)
{
	(*m_pAnimations)[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, *m_pBones);

	//_matrix Reverse = XMMatrixRotationY(XMConvertToRadians(180.f));

	for (auto& pBone : *m_pBones)
		pBone->Invalidate_CombinedTransformationMatrix(*m_pBones, /*Reverse*/ XMMatrixIdentity()/*XMLoadFloat4x4(&m_TransformationMatrix)*/);

	Update_RootMotion(fTimeDelta);

    return S_OK;
}

HRESULT CAnimator_Controller::Set_Animation_Index(_uint iAnimIndex)
{
	if (m_iNumAnimations <= iAnimIndex)
		return E_FAIL;

	m_iCurrentAnimIndex = iAnimIndex;
	return S_OK;
}

HRESULT CAnimator_Controller::Set_Animation_Transform(CTransform* pTargetTransform)
{
	if (nullptr == pTargetTransform)
		return E_FAIL;

	m_pTargetTransform = pTargetTransform;
	return S_OK;
}

HRESULT CAnimator_Controller::Blending_Animation(_uint iNextAnimIndex, _float fTimeDelta)
{
	if (m_iNumAnimations <= iNextAnimIndex)
		return E_FAIL;

	if (false == m_isSetInfo)
	{
		if (m_iCurrentAnimIndex != iNextAnimIndex)
			m_isChanged = true;

		m_iPrevAnimIndex = m_iCurrentAnimIndex;
		m_iCurrentAnimIndex = iNextAnimIndex;
		Set_Blending_Info();
		m_isSetInfo = true;
	}

	if(true == m_isSetInfo)
	{
		if (true == (*m_pAnimations)[m_iPrevAnimIndex]->Get_isRoot())
			m_isNeedRoot = true;

		if (S_OK == Update_Blending(fTimeDelta))
		{
			m_isSetInfo = false;
			m_isChanged = false;
			return S_OK;
		}		
	}

	return E_FAIL;
}

HRESULT CAnimator_Controller::Set_Animation_isLoop(_uint iAnimIndex, _bool isLoop)
{
	if (m_iNumAnimations <= iAnimIndex)
		return E_FAIL;

	(*m_pAnimations)[iAnimIndex]->Set_Loop(isLoop);
	return S_OK;
}

void CAnimator_Controller::Set_Animation_isRoot(_uint iAnimIndex, _bool isRoot)
{
	if (m_iNumAnimations <= iAnimIndex)
		return;

	(*m_pAnimations)[iAnimIndex]->Set_Root(isRoot);
}

void CAnimator_Controller::Set_Blend_Time(_uint iAnimIndex, _float Set_BlendTime)
{
	if (m_iNumAnimations <= iAnimIndex)
		return;

	(*m_pAnimations)[iAnimIndex]->Set_BlendTime(Set_BlendTime);
}

void CAnimator_Controller::Set_Frame_Tick(_uint iAnimIndex, _uint iStartFrame, _uint iEndFrame, _float fTickWeight)
{
	if (m_iNumAnimations <= iAnimIndex)
		return;

	(*m_pAnimations)[iAnimIndex]->Set_Frame_Tick(iStartFrame, iEndFrame, fTickWeight);
}

_uint CAnimator_Controller::Get_Current_Frame(_uint iAnimIndex)
{
	if (m_iNumAnimations <= iAnimIndex)
		return 0;

	return (*m_pAnimations)[iAnimIndex]->Get_Current_Frame();
}

_bool CAnimator_Controller::isFinished()
{
	 return (*m_pAnimations)[m_iCurrentAnimIndex]->isFinished(); 
}

_bool CAnimator_Controller::isFinished(_uint iAnimIndex)
{
	return (*m_pAnimations)[iAnimIndex]->isFinished();
}

HRESULT CAnimator_Controller::Set_Avatar(CAvatar* pAvatar)
{
	m_pAvatar = pAvatar; 

	m_pAnimations = m_pAvatar->Get_Animations_Ptr();
	m_iNumAnimations = m_pAnimations->size();

	m_pBones = m_pAvatar->Get_Bones_Ptr();
	m_iNumBones = m_pBones->size();

	m_ChannelStates[STATE_PREV].resize(m_iNumBones);
	m_ChannelStates[STATE_CUR].resize(m_iNumBones);

	return S_OK;
}

HRESULT CAnimator_Controller::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimator_Controller::Initialize(void* pArg)
{
	return S_OK;
}


HRESULT CAnimator_Controller::Set_Blending_Info()
{
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		// BONE MATRIX
		_float4x4 BoneFloat4x4 = (*m_pBones)[i]->Get_TransformationMatrix();
		_matrix BoneMatrix = XMLoadFloat4x4(&BoneFloat4x4);

		_vector vScale, vRotation, vTranslation;

		bool result = XMMatrixDecompose(&vScale, &vRotation, &vTranslation, BoneMatrix);

		vScale = { 1.f, 1.f, 1.f };

		XMStoreFloat3(&m_ChannelStates[STATE_PREV][i].vScale, vScale);
		XMStoreFloat4(&m_ChannelStates[STATE_PREV][i].vRotation, vRotation);
		XMStoreFloat3(&m_ChannelStates[STATE_PREV][i].vTranslation, vTranslation);

		XMStoreFloat3(&m_ChannelStates[STATE_CUR][i].vScale, vScale);
		XMStoreFloat4(&m_ChannelStates[STATE_CUR][i].vRotation, vRotation);
		XMStoreFloat3(&m_ChannelStates[STATE_CUR][i].vTranslation, vTranslation);
	}

	// PREV
	_uint iPrevNumChannels = (*m_pAnimations)[m_iPrevAnimIndex]->Get_NumChannels();
	for (size_t i = 0; i < iPrevNumChannels; ++i)
	{
		CChannel* pChannel = (*m_pAnimations)[m_iPrevAnimIndex]->Get_Channel(i);
		_uint iBoneIndex = pChannel->Get_BoneIndex();
		m_ChannelBoneIndex.push_back(iBoneIndex);
		m_ChannelStates[STATE_PREV][pChannel->Get_BoneIndex()] = *pChannel->Get_ChannelState();
	}

	// CUR
	(*m_pAnimations)[m_iCurrentAnimIndex]->Invalidate_Blending(*m_pBones, true);
	_uint iCurNumChannels = (*m_pAnimations)[m_iCurrentAnimIndex]->Get_NumChannels();

	for (size_t i = 0; i < iCurNumChannels; ++i)
	{
		CChannel* pChannel = (*m_pAnimations)[m_iCurrentAnimIndex]->Get_Channel(i);
		m_ChannelStates[STATE_CUR][pChannel->Get_BoneIndex()] = *pChannel->Get_ChannelState_First();
	}

	return S_OK;
}

HRESULT CAnimator_Controller::Update_Blending(_float fTimeDelta)
{
	_float fBlendTime = (*m_pAnimations)[m_iPrevAnimIndex]->Get_BlendTime();

	_float3     vScale;
	_float4     vRotation;
	_float3     vTranslation;

	m_fTime += fTimeDelta;
	if (m_fTime > fBlendTime)
	{
		m_fTime = 0.f;
		(*m_pAnimations)[m_iPrevAnimIndex]->Set_AnimationData_Initialize();
		(*m_pAnimations)[m_iCurrentAnimIndex]->Set_AnimationData_Initialize();

		return S_OK;
	}

	_float fRatio = m_fTime / fBlendTime;

	// Blending
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&m_ChannelStates[STATE_PREV][i].vScale), XMLoadFloat3(&m_ChannelStates[STATE_CUR][i].vScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&m_ChannelStates[STATE_PREV][i].vRotation), XMLoadFloat4(&m_ChannelStates[STATE_CUR][i].vRotation), fRatio));
		XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&m_ChannelStates[STATE_PREV][i].vTranslation), XMLoadFloat3(&m_ChannelStates[STATE_CUR][i].vTranslation), fRatio));

		_matrix     TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f));

		(*m_pBones)[i]->Set_TransformationMatrix(TransformationMatrix);
	}

	for (auto& pBone : (*m_pBones))
		pBone->Invalidate_CombinedTransformationMatrix((*m_pBones), /*XMLoadFloat4x4(&m_TransformationMatrix)*/ XMMatrixIdentity());

	Update_RootMotion(fTimeDelta);

	return E_FAIL;
}

void CAnimator_Controller::Update_RootMotion(_float fTimeDelta)
{
	// Root Bone의 현재 로컬 위치
	memcpy(&m_vRootDistance[STATE_CUR], (*m_pBones)[ROOTBONEIDX]->Get_TransformationMatrix().m[3], sizeof(_float3));

	// 애니메이션이 끝나면 이전 로컬 위치 갱신
	if (true == isFinished(m_iPrevAnimIndex) || true == m_isNeedRoot)
	{
		m_isNeedRoot = false;
		m_vRootDistance[STATE_PREV] = m_vRootDistance[STATE_CUR];
	}

	// 이동한 거리를 월드로 변환
	_float3 vPre, vCur;
	XMStoreFloat3(&vPre, XMVector3TransformCoord(XMLoadFloat3(&m_vRootDistance[STATE_PREV]), XMLoadFloat4x4(m_pTargetTransform->Get_WorldFloat4x4_Ptr())));
	XMStoreFloat3(&vCur, XMVector3TransformCoord(XMLoadFloat3(&m_vRootDistance[STATE_CUR]), XMLoadFloat4x4(m_pTargetTransform->Get_WorldFloat4x4_Ptr())));
	_vector vDiffDistance = XMLoadFloat3(&vCur) - XMLoadFloat3(&vPre);

	// 타겟 오브젝트의 월드 위치 값 변경
	_vector vObjectPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
	vObjectPosition += vDiffDistance;
	m_pTargetTransform->Set_State(CTransform::STATE_POSITION, vObjectPosition);

	// Root Bone의 이전 로컬 위치
	m_vRootDistance[STATE_PREV] = m_vRootDistance[STATE_CUR];

	// Root Bone의 CombinedTransformationMatrix 초기화
	_vector vOriginPosition = { 0.f, 0.f, 0.f };
	(*m_pBones)[ROOTBONEIDX]->Set_CombinedTransformationPosition(vOriginPosition);
}

CAnimator_Controller* CAnimator_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimator_Controller* pInstance = new CAnimator_Controller(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CAnimator_Controller"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimator_Controller* CAnimator_Controller::Clone(void* pArg)
{
	CAnimator_Controller* pInstance = new CAnimator_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CAnimator_Controller"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimator_Controller::Free()
{
	__super::Free();

	Safe_Release(m_pAvatar);
}
