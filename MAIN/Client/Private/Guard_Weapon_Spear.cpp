#include "stdafx.h"
#include "Guard_Weapon_Spear.h"

#include "Bone.h"
#include "Player.h"

CGuard_Weapon_Spear::CGuard_Weapon_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CGuard_Weapon_Spear::CGuard_Weapon_Spear(const CGuard_Weapon_Spear& rhs)
    : CPartObject{ rhs }
{
}

HRESULT CGuard_Weapon_Spear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGuard_Weapon_Spear::Initialize(void* pArg)
{
    GUARD_WEAPON_SPEAR_DESC* pDesc = (GUARD_WEAPON_SPEAR_DESC*)pArg;

    m_strModelComTag = pDesc->strModelComTag;
    m_pSocketBone = pDesc->pSocketBone;
    m_eWeapon = pDesc->eWeapon;

    Safe_AddRef(m_pSocketBone);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_eType = OBJ_MONSTER_WEAPON;

    /*if (CMonster_Frog::WEAPON_SCIMITAR == m_eWeapon)
        m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.0f));*/

    return S_OK;
}

HRESULT CGuard_Weapon_Spear::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_MONSTER_WEAPON, this);

    return S_OK;
}

void CGuard_Weapon_Spear::Late_Tick(_float fTimeDelta)
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

HRESULT CGuard_Weapon_Spear::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {

        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }
    return S_OK;
}

HRESULT CGuard_Weapon_Spear::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_BEACH, m_strModelComTag,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    /* 로컬상의 정보를 셋팅한다. */
    if (CMonster_Guard::WEAPON_SPEAR == m_eWeapon)
    {
        ColliderDesc.vSize = _float3(4.5f, 2.f, 2.f);
        ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGuard_Weapon_Spear::Bind_ShaderResources()
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

    return S_OK;
}

CGuard_Weapon_Spear* CGuard_Weapon_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGuard_Weapon_Spear* pInstance = new CGuard_Weapon_Spear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CGuard_Weapon_Spear"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGuard_Weapon_Spear::Clone(void* pArg)
{
    CGuard_Weapon_Spear* pInstance = new CGuard_Weapon_Spear(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CGuard_Weapon_Spear"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGuard_Weapon_Spear::Free()
{
    __super::Free();

    Safe_Release(m_pSocketBone);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}

void CGuard_Weapon_Spear::Collision_Event(Engine::CGameObject* pGameObject)
{
    if (true == m_isAttackFrame && OBJ_PLAYER == pGameObject->Get_ObjectType())
    {
        pGameObject->Set_isDamage(true);
        dynamic_cast<CPlayer*>(pGameObject)->Change_State(CPlayer::STATE_DAMAGE);
    }
}
