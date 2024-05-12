#include "stdafx.h"
#include "Effect_WandBeam.h"

CEffect_WandBeam::CEffect_WandBeam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_WandBeam::CEffect_WandBeam(const CEffect_WandBeam& rhs)
	: CEffect{ rhs }
{
}

HRESULT CEffect_WandBeam::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_WandBeam::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 150.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(10.f, 10.f, 10.f);

	m_pTransformCom->Rotation(_vector{0.f, 1.f, 0.f, 0.f}, XMConvertToRadians(180.f));
	
	if (nullptr != pArg)
	{
		EFFECT_WANDBEAM_DESC* pDesc = (EFFECT_WANDBEAM_DESC*)pArg;

		m_pTransformCom->Look_At_Dir(pDesc->vLookDir);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vStartPosition);
	}

	return S_OK;
}

HRESULT CEffect_WandBeam::Tick(_float fTimeDelta)
{
	m_fAccLiveTime += fTimeDelta;
	if (m_fAccLiveTime >= m_fLiveTime)
		m_isDead = true;

	if (E_FAIL == __super::Tick(fTimeDelta))
		return E_FAIL;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER_WEAPON, this);

	m_pTransformCom->Go_Backward(fTimeDelta);

	return S_OK;
}

void CEffect_WandBeam::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CEffect_WandBeam::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEffect_WandBeam::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Weapon_Wandbow_Beam";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, wstr,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */

	ColliderDesc.vSize = _float3(0.2f, 0.2f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vSize.z * -0.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_WandBeam::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_Camera_Far();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CEffect_WandBeam* CEffect_WandBeam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_WandBeam* pInstance = new CEffect_WandBeam(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CEffect_WandBeam"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_WandBeam::Clone(void* pArg)
{
	CEffect_WandBeam* pInstance = new CEffect_WandBeam(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CEffect_WandBeam"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_WandBeam::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}

void CEffect_WandBeam::Collision_Event(Engine::CGameObject* pGameObject)
{
	if (OBJ_MONSTER == pGameObject->Get_ObjectType())
	{
		pGameObject->Set_isDamage(true);
		m_isDead = true;
	}
}
