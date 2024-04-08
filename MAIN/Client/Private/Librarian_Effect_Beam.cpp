#include "stdafx.h"

#include "Librarian_Effect_Beam.h"

CLibrarian_Effect_Beam::CLibrarian_Effect_Beam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffect{ pDevice, pContext }
{	
}

CLibrarian_Effect_Beam::CLibrarian_Effect_Beam(const CLibrarian_Effect_Beam& rhs)
    : CEffect{ rhs }
{
}

void CLibrarian_Effect_Beam::Start_Pattern(_fvector vStartPosition, _fvector vDir)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPosition);
	m_pTransformCom->Look_At_Dir(vDir);
	m_isActive = true;

	Set_Blending(true, ANIM_DEPLOY);
}

HRESULT CLibrarian_Effect_Beam::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLibrarian_Effect_Beam::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 70.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(0.5f, 0.5f, 0.5f);

	m_pModelCom->Set_ParentBoneIndex(-1);
	m_pModelCom->Set_Animation_Index(ANIM_OFF);
	m_pModelCom->Set_Animation_Transform(m_pTransformCom);
	Set_Animation();

	return S_OK;
}

HRESULT CLibrarian_Effect_Beam::Tick(_float fTimeDelta)
{
	/*static _int iIndex = -1;
	if (m_pGameInstance->Get_DIKeyState(DIK_I, KEY_DOWN))
	{
		iIndex++;
		if (iIndex > 3)
			iIndex = 0;
		m_pModelCom->Set_Animation_Index(iIndex);
	}

	m_pModelCom->Play_Animation(fTimeDelta);*/


	if (true == m_isActive)
	{
		if (E_FAIL == __super::Tick(fTimeDelta))
			return E_FAIL;

		Play_Pattern(fTimeDelta);

		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

		m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER, this);
	}
	else
	{
		m_isBlend = false;
	}
	

	return S_OK;
}

void CLibrarian_Effect_Beam::Late_Tick(_float fTimeDelta)
{
	if (true == m_isActive)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CLibrarian_Effect_Beam::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CLibrarian_Effect_Beam::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Boss_Librarian_Effect_Beam";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */

	ColliderDesc.vSize = _float3(0.2f, 0.2f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vSize.z * -0.5f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLibrarian_Effect_Beam::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CLibrarian_Effect_Beam::Set_Animation()
{
	// LOOP
	m_pModelCom->Set_Animation_isLoop(ANIM_SUSTAIN, true);
}

void CLibrarian_Effect_Beam::Play_Pattern(_float fTimeDelta)
{
	if (true == Get_isFinished(ANIM_DEPLOY))
	{
		int a = 0;
	}

	if (0 == m_iPattern && true == Get_isFinished(ANIM_DEPLOY))
	{
		++m_iPattern;
		Set_Blending(true, ANIM_SUSTAIN);
	}

	else if (1 == m_iPattern && true == m_isInactive)
	{
		++m_iPattern;
		Set_Blending(true, ANIM_OFF);
	}

	else if (2 == m_iPattern && true == Get_isFinished(ANIM_OFF))
	{
		m_iPattern = 0;
		m_isActive = false;
		m_isInactive = false;
	}

	// Blending
	if (true == m_isBlend)
	{
		if (S_OK == m_pModelCom->Blending_Animation(m_eBlendAnimIndex, fTimeDelta))
		{
			m_isBlend = false;
			m_pModelCom->Set_Animation_Index(m_eBlendAnimIndex);
			m_eAnimationIndex = m_eBlendAnimIndex;
		}
	}
	else
		m_pModelCom->Play_Animation(fTimeDelta);
}

CLibrarian_Effect_Beam* CLibrarian_Effect_Beam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLibrarian_Effect_Beam* pInstance = new CLibrarian_Effect_Beam(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CLibrarian_Effect_Beam"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLibrarian_Effect_Beam::Clone(void* pArg)
{
	CLibrarian_Effect_Beam* pInstance = new CLibrarian_Effect_Beam(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CLibrarian_Effect_Beam"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLibrarian_Effect_Beam::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
