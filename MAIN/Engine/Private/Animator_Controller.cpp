#include "Animator_Controller.h"

#include "Bone.h"
#include "Avatar.h"
#include "Animation.h"
#include "State_Machine.h"

CAnimator_Controller::CAnimator_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
}

void CAnimator_Controller::Add_State(_uint iState, CState* pAddState)
{
}

void CAnimator_Controller::Update_State(_float fTimeDelta)
{
}

void CAnimator_Controller::Change_State(_uint iState)
{
}

HRESULT CAnimator_Controller::Play_Animation(_float fTimeDelta)
{
	(*m_pAnimations)[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, *m_pBones);

	for (auto& pBone : *m_pBones)
		pBone->Invalidate_CombinedTransformationMatrix(*m_pBones, XMMatrixIdentity()/*XMLoadFloat4x4(&m_TransformationMatrix)*/);

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

HRESULT CAnimator_Controller::Set_Animation_isBlending(_uint iAnimIndex, _bool isBlend)
{
	if (m_iNumAnimations <= iAnimIndex)
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimator_Controller::Set_Animation_isLoop(_uint iAnimIndex, _bool isLoop)
{
	if (m_iNumAnimations <= iAnimIndex)
		return E_FAIL;

	(*m_pAnimations)[iAnimIndex]->Set_Loop(true);
	return S_OK;
}

HRESULT CAnimator_Controller::Set_Avatar(CAvatar* pAvatar)
{
	m_pAvatar = pAvatar; 

	m_pAnimations = m_pAvatar->Get_Animations_Ptr();
	m_iNumAnimations = m_pAnimations->size();

	m_pBones = m_pAvatar->Get_Bones_Ptr();

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
	Safe_Release(m_pState_Machine);
}
