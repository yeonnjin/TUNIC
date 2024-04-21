#include "stdafx.h"
#include "Librarian_Effect_Slash.h"

#include "Player.h"

CLibrarian_Effect_Slash::CLibrarian_Effect_Slash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CLibrarian_Effect_Slash::CLibrarian_Effect_Slash(const CLibrarian_Effect_Slash& rhs)
	: CEffect{ rhs }
{
}

HRESULT CLibrarian_Effect_Slash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLibrarian_Effect_Slash::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 70.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	LIBRARIAN_EFFECT_SLASH_DESC* pDesc = (LIBRARIAN_EFFECT_SLASH_DESC*)pArg;
	m_isVertical = pDesc->isVertical;
	
	if (FAILED(Add_Components()))
		return E_FAIL;
	
	m_pTransformCom->Look_At_Dir(pDesc->vLookDir, true);
	if (false == m_isVertical)
	{
		//m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), 1.f);
		m_pTransformCom->Set_Scaled(1.2f, 1.2f, 1.2f);
	}
	else
	{
		m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vStartPosition);


	return S_OK;
}

HRESULT CLibrarian_Effect_Slash::Tick(_float fTimeDelta)
{
	m_fAccLiveTime += fTimeDelta;
	if (m_fAccLiveTime >= m_fLiveTime)
		m_isDead = true;

	if (E_FAIL == __super::Tick(fTimeDelta))
		return E_FAIL;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_Group(CCollision_Manager::GROUP_MONSTER_WEAPON, this);

	m_pTransformCom->Go_Backward(fTimeDelta);

	return S_OK;
}

void CLibrarian_Effect_Slash::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CLibrarian_Effect_Slash::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		if(true == m_isVertical)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
				return E_FAIL;

			/*if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, TEX_NORMALS)))
				return E_FAIL;*/
		}
		else
		{
			if (FAILED(m_pModelCom_Horizon->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
				return E_FAIL;

			/*if (FAILED(m_pModelCom_Horizon->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, TEX_NORMALS)))
				return E_FAIL;*/
		}

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CLibrarian_Effect_Slash::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Boss_Librarian_Effect_Slash";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	_char szModelTag1[MAX_PATH] = "Prototype_Component_Model_Boss_Librarian_Effect_Slash_Horizon";
	wstring wstr1(&szModelTag1[0], &szModelTag1[MAX_PATH]);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr1,
		TEXT("Com_Model_Horizon"), (CComponent**)&m_pModelCom_Horizon)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */

	if (m_isVertical)
	{
		ColliderDesc.vSize = _float3(3.f, 20.f, 6.f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vSize.z * -0.5f);
	}
	else
	{
		ColliderDesc.vSize = _float3(20.f, 1.f, 5.f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vSize.z * -0.5f);
	}
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLibrarian_Effect_Slash::Bind_ShaderResources()
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

CLibrarian_Effect_Slash* CLibrarian_Effect_Slash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLibrarian_Effect_Slash* pInstance = new CLibrarian_Effect_Slash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CLibrarian_Effect_Slash"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLibrarian_Effect_Slash::Clone(void* pArg)
{
	CLibrarian_Effect_Slash* pInstance = new CLibrarian_Effect_Slash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CLibrarian_Effect_Slash"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLibrarian_Effect_Slash::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom_Horizon);
	Safe_Release(m_pColliderCom);
}

void CLibrarian_Effect_Slash::Collision_Event(Engine::CGameObject* pGameObject)
{
	if (OBJ_PLAYER == pGameObject->Get_ObjectType())
	{
		pGameObject->Set_isDamage(true);
	}
}
