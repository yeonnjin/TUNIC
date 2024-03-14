#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 3.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		PLAYER_DESC* pDesc = (PLAYER_DESC*)pArg;
		m_strModelComTag = pDesc->strModelComTag;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	_float4 vPosition = _float4(0.f, 2.f, 0.3f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pModelCom->Set_Animation(0, true);

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	static _uint iIndex = 0;
	if (m_pGameInstance->Get_DIKeyState(DIK_Z, KEY_DOWN))
	{
		iIndex++;
		if (iIndex > 60)
			iIndex = 0;

		if (iIndex == 39)
		{
			int a = 0;
		}

		m_pModelCom->Set_Animation(iIndex, true);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_X, KEY_DOWN))
	{
		iIndex--;
		if (iIndex < 0)
			iIndex = 60;

		m_pModelCom->Set_Animation(iIndex, true);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_UP, KEY_PRESS))
	{
		// TODO: 전진
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_LEFT, KEY_PRESS))
	{
		_fvector vec = { 0.f, 1.f, 0.f };
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_DOWN, KEY_PRESS))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT, KEY_PRESS))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_1, KEY_DOWN))
	{
		m_pModelCom->Set_Animation(25, true);	// IDLE
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_2, KEY_DOWN))
	{
		m_pModelCom->Set_Animation(23, true);	// HURT
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_3, KEY_DOWN))
	{
		m_pModelCom->Set_Animation(37, true);	// sprint
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_4, KEY_DOWN))
	{
		m_pModelCom->Set_Animation(28, true);	// open chest
	}

	// 한 번만 넘기게 수정 필요!!!
	m_pModelCom->Set_ObjectTransform(m_pTransformCom);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
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

	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelComTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
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

	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CPlayer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CPlayer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}