#include "Animator.h"

#include "State.h"
#include "State_Machine.h"
#include "Avatar.h"
#include "Animation.h"
#include "Animator_Controller.h"

CAnimator::CAnimator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CAnimator::CAnimator(const CAnimator& rhs)
	: CComponent{ rhs }
	/*, m_iNumAnimations{ rhs.m_iNumAnimations }
	, m_pAnimations{ rhs.m_pAnimations }*/
	/*, m_pAvatar{ rhs.m_pAvatar }*/
	/*, m_pAnimator_Controller{ rhs.m_pAnimator_Controller }*/
	, m_pState_Machine{ rhs.m_pState_Machine }
{
	m_pAvatar = rhs.m_pAvatar->Clone();
	m_pAnimator_Controller = rhs.m_pAnimator_Controller->Clone();
	//Safe_AddRef(m_pAvatar);
	//Safe_AddRef(m_pAnimator_Controller);
	Safe_AddRef(m_pState_Machine);

	m_isCloned = true;
}

HRESULT CAnimator::Initialize_Prototype(MODELFILE* pModelFile)
{
	if (nullptr == pModelFile)
		return E_FAIL;

	m_pAvatar = CAvatar::Create(m_pDevice, m_pContext, CAvatar::TYPE_ANIM, pModelFile);
	if (nullptr == m_pAvatar)
		return E_FAIL;

	/*m_pAnimations = m_pAvatar->Get_Animations_Ptr();
	m_iNumAnimations = m_pAnimations->size();*/

	m_pAnimator_Controller = CAnimator_Controller::Create(m_pDevice, m_pContext);
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	m_pState_Machine = CState_Machine::Create(m_pDevice, m_pContext);
	if (nullptr == m_pState_Machine)
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimator::Initialize(void* pArg)
{
	m_pAnimator_Controller->Set_Avatar(m_pAvatar);
	Safe_AddRef(m_pAvatar);

	return S_OK;
}

// Avatar
_uint CAnimator::Get_NumMeshes() const
{
	if (nullptr == m_pAvatar)
		return 0;

	return m_pAvatar->Get_NumMeshes();
}

CBone* CAnimator::Get_Bone_Ptr(_uint iBoneIndex) const
{
	if (nullptr == m_pAvatar)
		return nullptr;

	return m_pAvatar->Get_Bone_Ptr(iBoneIndex);
}

HRESULT CAnimator::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (nullptr == m_pAvatar)
		return E_FAIL;

	return m_pAvatar->Bind_BoneMatrices(pShader, pConstantName, iMeshIndex);
}

HRESULT CAnimator::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, AITEXTURETYPE eTextureType)
{
	if (nullptr == m_pAvatar)
		return E_FAIL;

	return m_pAvatar->Bind_ShaderResource(pShader, pConstantName, iMeshIndex, eTextureType);
}

HRESULT CAnimator::Render(_uint iMeshIndex)
{
	if (nullptr == m_pAvatar)
		return E_FAIL;

	return m_pAvatar->Render(iMeshIndex);
}

// State_Machine
void CAnimator::Add_State(_uint iState, CState* pAddState)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pState_Machine->Add_State(iState, pAddState);
}

void CAnimator::Update_State(_float fTimeDelta)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pState_Machine->Update_State(fTimeDelta);
}

void CAnimator::Change_State(_uint iState)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pState_Machine->Change_State(iState);
}

// Animator_Controller
HRESULT CAnimator::Play_Animation(_float fTimeDelta)
{
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	return m_pAnimator_Controller->Play_Animation(fTimeDelta);
}

HRESULT CAnimator::Set_Animation_Index(_uint iAnimIndex)
{
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	return m_pAnimator_Controller->Set_Animation_Index(iAnimIndex);
}

HRESULT CAnimator::Set_Animation_Transform(CTransform* pObjectTransform)
{
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	return m_pAnimator_Controller->Set_Animation_Transform(pObjectTransform);
}

HRESULT CAnimator::Blending_Animation(_uint iNextAnimIndex, _float fTimeDelta)
{
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	return m_pAnimator_Controller->Blending_Animation(iNextAnimIndex, fTimeDelta);
}

HRESULT CAnimator::Set_Animation_isLoop(_uint iAnimIndex, _bool isLoop)
{
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	return m_pAnimator_Controller->Set_Animation_isLoop(iAnimIndex, isLoop);
}

void CAnimator::Set_TargetTransform(CTransform* pTargetTransform)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pAnimator_Controller->Set_Animation_Transform(pTargetTransform);
}

void CAnimator::Set_Animation_isRoot(_uint iAnimIndex, _bool isRoot)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	return m_pAnimator_Controller->Set_Animation_isRoot(iAnimIndex, isRoot);
}

void CAnimator::Set_Blend_Time(_uint iAnimIndex, _float fBlendTime)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pAnimator_Controller->Set_Blend_Time(iAnimIndex, fBlendTime);
}

void CAnimator::Set_Frame_Tick(_uint iAnimIndex, _uint iStartFrame, _uint iEndFrame, _float fTickWeight)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pAnimator_Controller->Set_Frame_Tick(iAnimIndex, iStartFrame, iEndFrame, fTickWeight);
}

void CAnimator::Set_SlowMotion(_uint iAnimIndex, _uint iStartFrame, _uint iEndFrame, _float fSlowTime)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pAnimator_Controller->Set_SlowMotion(iAnimIndex, iStartFrame, iEndFrame, fSlowTime);
}

_bool CAnimator::isFinished()
{
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	return m_pAnimator_Controller->isFinished();
}

_bool CAnimator::isFinished(_uint iAnimIndex)
{
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	return m_pAnimator_Controller->isFinished(iAnimIndex);
}

CAnimator* CAnimator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELFILE* pModelFile)
{
	CAnimator* pInstance = new CAnimator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFile)))
	{
		MSG_BOX(TEXT("Failed To Create : CAnimator"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CAnimator::Clone(void* pArg)
{
	CAnimator* pInstance = new CAnimator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CAnimator"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimator::Free()
{
	m_pAvatar->Free();
	Safe_Release(m_pAvatar);
	Safe_Release(m_pAnimator_Controller);
	Safe_Release(m_pState_Machine);

	/*for (auto& pAnimation : *m_pAnimations)
		Safe_Release(pAnimation);

	m_pAnimations->clear();*/
	

}
