#include "stdafx.h"
#include "Librarian_Weapon.h"

#include "Bone.h"

CLibrarian_Weapon::CLibrarian_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CLibrarian_Weapon::CLibrarian_Weapon(const CLibrarian_Weapon& rhs)
    : CPartObject{ rhs }
{
}

HRESULT CLibrarian_Weapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLibrarian_Weapon::Initialize(void* pArg)
{
    LIBRARIAN_WEAPON_DESC* pDesc = (LIBRARIAN_WEAPON_DESC*)pArg;

    m_strModelComTag = pDesc->strModelComTag;
    m_pSocketBone = pDesc->pSocketBone;
    m_eWeapon = pDesc->eWeapon;

    Safe_AddRef(m_pSocketBone);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_eType = OBJ_MONSTER_WEAPON;

    if (CMonster_Librarian::WEAPON_SWORD == m_eWeapon)
        m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.0f));
    /*else if (CMonster_Librarian::WEAPON_SHIELD == m_eWeapon)
        m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.0f));*/

    return S_OK;
}

HRESULT CLibrarian_Weapon::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_MONSTER_WEAPON, this);

    return S_OK;
}

void CLibrarian_Weapon::Late_Tick(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBone->Get_CombinedTransformationMatrix());

    SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
    SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
    SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

    // 본인의 월드 행렬 * 소켓 본의 최종 행렬 * 부모 본의 행렬
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

    if (true == m_isRender)
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CLibrarian_Weapon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        /*if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", i, TEX_NORMALS)))
            return E_FAIL;*/

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    return S_OK;
}

HRESULT CLibrarian_Weapon::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelComTag,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    /* 로컬상의 정보를 셋팅한다. */
    if (CMonster_Librarian::WEAPON_SWORD == m_eWeapon)
    {
        ColliderDesc.vSize = _float3(1.2f, 1.2f, 9.f);
        ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vSize.z * 0.5f);
    }
    //else if (CMonster_Librarian::WEAPON_SHIELD == m_eWeapon)
    //{
    //    ColliderDesc.vSize = _float3(1.5f, 1.5f, 0.6f);
    //    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    //}

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLibrarian_Weapon::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fCamFar = m_pGameInstance->Get_Camera_Far();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
        return E_FAIL;
}

CLibrarian_Weapon* CLibrarian_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLibrarian_Weapon* pInstance = new CLibrarian_Weapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_Weapon"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLibrarian_Weapon::Clone(void* pArg)
{
    CLibrarian_Weapon* pInstance = new CLibrarian_Weapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CLibrarian_Weapon"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_Weapon::Free()
{
    __super::Free();

    Safe_Release(m_pSocketBone);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}

void CLibrarian_Weapon::Collision_Event(Engine::CGameObject* pGameObject)
{
    if (OBJ_PLAYER == pGameObject->Get_ObjectType())
    {
        pGameObject->Set_isDamage(true);
    }
}
