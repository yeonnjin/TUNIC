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
{
	m_pAvatar = rhs.m_pAvatar->Clone();
	m_pAnimator_Controller = rhs.m_pAnimator_Controller->Clone();
	//Safe_AddRef(m_pAvatar);
	//Safe_AddRef(m_pAnimator_Controller);

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

// Animator_Controller : State_Machine
void CAnimator::Add_State(_uint iState, CState* pAddState)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pAnimator_Controller->Add_State(iState, pAddState);
}

void CAnimator::Update_State(_float fTimeDelta)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pAnimator_Controller->Update_State(fTimeDelta);
}

void CAnimator::Change_State(_uint iState)
{
	if (nullptr == m_pAnimator_Controller)
		return;

	m_pAnimator_Controller->Change_State(iState);
}

// Animator_Controller : Animation
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

HRESULT CAnimator::Set_Animation_isBlending(_uint iAnimIndex, _bool isBlend)
{
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	return m_pAnimator_Controller->Set_Animation_isBlending(iAnimIndex, isBlend);
}

HRESULT CAnimator::Set_Animation_isLoop(_uint iAnimIndex, _bool isLoop)
{
	if (nullptr == m_pAnimator_Controller)
		return E_FAIL;

	return m_pAnimator_Controller->Set_Animation_isLoop(iAnimIndex, isLoop);
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

	/*for (auto& pAnimation : *m_pAnimations)
		Safe_Release(pAnimation);

	m_pAnimations->clear();*/
	

}
