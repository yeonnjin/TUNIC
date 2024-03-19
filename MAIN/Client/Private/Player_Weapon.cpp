#include "stdafx.h"
#include "Player_Weapon.h"

#include "Bone.h"

CPlayer_Weapon::CPlayer_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CPlayer_Weapon::CPlayer_Weapon(const CPlayer_Weapon& rhs)
    : CPartObject{ rhs }
{
}

HRESULT CPlayer_Weapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer_Weapon::Initialize(void* pArg)
{
    PLAYER_WEAPON_DESC* pDesc = (PLAYER_WEAPON_DESC*)pArg;

    m_pSocketBone = pDesc->pSocketBone;
    Safe_AddRef(m_pSocketBone);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;


    // Shield
    //m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.0f));


    //m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);   
    //m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.0f));
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.0f, 0.f, 0.f, 1.f));

    return S_OK;
}

void CPlayer_Weapon::Tick(_float fTimeDelta)
{
}

void CPlayer_Weapon::Late_Tick(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBone->Get_CombinedTransformationMatrix());

    SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
    SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
    SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

    // 본인의 월드 행렬 * 소켓 본의 최종 행렬 * 부모 본의 행렬
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

    if(true == m_isRender)
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer_Weapon::Render()
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

    return S_OK;
}

HRESULT CPlayer_Weapon::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    // "Prototype_Component_Model_Map_Object"
    // "Prototype_Component_Model_Weapon_Sword"
    // "Prototype_Component_Model_Weapon_Shield"
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Weapon_Shield";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_Weapon::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    /*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;*/

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    /*if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if(FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
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
        return E_FAIL;*/

    return S_OK;
}

CPlayer_Weapon* CPlayer_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer_Weapon* pInstance = new CPlayer_Weapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_Weapon"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer_Weapon::Clone(void* pArg)
{
    CPlayer_Weapon* pInstance = new CPlayer_Weapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CPlayer_Weapon"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Weapon::Free()
{
    __super::Free();

    Safe_Release(m_pSocketBone);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
