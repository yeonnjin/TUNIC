#include "stdafx.h"
#include "Librarian_Effect_Orb.h"

#include "Player.h"

CLibrarian_Effect_Orb::CLibrarian_Effect_Orb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffect{ pDevice, pContext }
{
}

CLibrarian_Effect_Orb::CLibrarian_Effect_Orb(const CLibrarian_Effect_Orb& rhs)
    : CEffect{ rhs }
{
}

HRESULT CLibrarian_Effect_Orb::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLibrarian_Effect_Orb::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 40.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CLibrarian_Effect_Orb::Set_Start_Position(_vector vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

HRESULT CLibrarian_Effect_Orb::Tick(_float fTimeDelta)
{
	if (true == m_isActive)
	{
		if (E_FAIL == __super::Tick(fTimeDelta))
			return E_FAIL;

		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

		m_pGameInstance->Add_Group(CCollision_Manager::GROUP_MONSTER_WEAPON, this);

		if (true == m_isMove)
		{
			m_fAccStayTime += fTimeDelta;
			if(m_fAccStayTime > m_fStayTime)
				m_pTransformCom->Go_Backward(fTimeDelta);
		}

		if (true == m_isInactive)
		{
			m_fAccInactiveTime += fTimeDelta;
			if (m_fAccInactiveTime > m_fInactiveTime)
			{
				m_isActive = false;
				m_isInactive = false;
			}
		}
	}
	else
	{
		m_fAccStayTime = 0.f;
		m_fAccInactiveTime = 0.f;
		m_isMove = false;
	}

	return S_OK;
}

void CLibrarian_Effect_Orb::Late_Tick(_float fTimeDelta)
{
	if (true == m_isActive)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CLibrarian_Effect_Orb::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
			return E_FAIL;*/

		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i, TEX_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	//#ifdef _DEBUG
	//	m_pColliderCom->Render();
	//#endif

	return S_OK;
}

HRESULT CLibrarian_Effect_Orb::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Boss_Librarian_Effect_Orb";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC		ColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */

	ColliderDesc.fRadius = 0.6f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLibrarian_Effect_Orb::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CLibrarian_Effect_Orb* CLibrarian_Effect_Orb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLibrarian_Effect_Orb* pInstance = new CLibrarian_Effect_Orb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CLibrarian_Effect_Orb"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLibrarian_Effect_Orb::Clone(void* pArg)
{
	CLibrarian_Effect_Orb* pInstance = new CLibrarian_Effect_Orb(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CLibrarian_Effect_Orb"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLibrarian_Effect_Orb::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}

void CLibrarian_Effect_Orb::Collision_Event(Engine::CGameObject* pGameObject)
{
	if (OBJ_PLAYER == pGameObject->Get_ObjectType())
	{
		pGameObject->Set_isDamage(true);
	}
}

