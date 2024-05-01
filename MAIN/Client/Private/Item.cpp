#include "stdafx.h"
#include "Item.h"

#include "Player.h"
#include "NPC_Merchant.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CItem::CItem(const CItem& rhs)
    : CInteractiveObject{ rhs }
    , m_eItemType{ rhs.m_eItemType }
    , m_eItem{ rhs.m_eItem }
    , m_iTextureIndex{ rhs.m_iTextureIndex }
{
}

_bool CItem::Get_isOK()
{
    return m_pNPC->Get_isOK();
}

HRESULT CItem::Use_Item()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));

    if (ITEM_HP == m_eItem)
    {
        _uint iHP = pPlayer->Get_HP();
        _uint iMaxHP = pPlayer->Get_MaxHP();
        // 이미 풀일 때 : 아이템 사용 실패
        if (iMaxHP <= iHP)
            return E_FAIL;
        // 회복 가능한 양보다 회복될 양이 많을 때 : 최대 게이지로
        else if(m_iPlusHP >= iMaxHP - iHP)
        {
            pPlayer->Set_HP(iMaxHP - iHP);
        }
        // 일반 상태일 때 : + 회복양
        else
        {
            pPlayer->Set_HP(m_iPlusHP);
        }
    }
    else if (ITEM_MP == m_eItem)
    {
        _float fMP = pPlayer->Get_MP();
        _float fMaxMP = pPlayer->Get_MaxMP();
        // 이미 풀일 때 : 아이템 사용 실패
        if (fMaxMP <= fMP)
            return E_FAIL;
        // 회복 가능한 양보다 회복될 양이 많을 때 : 최대 게이지로
        else if (m_fPlusMP >= fMaxMP - fMP)
        {
            pPlayer->Set_MP(fMaxMP - fMP);
        }
        // 일반 상태일 때 : + 회복양
        else
        {
            pPlayer->Set_MP(m_fPlusMP);
        }
    }

    return S_OK;
}

void CItem::Set_ShopItem(CNPC_Merchant* pNPC)
{
    m_isShop = true;
    m_pNPC = pNPC;
    //Safe_AddRef(m_pNPC);
}

void CItem::Select_Item()
{
    m_pNPC->Select_Item(this);
}

CItem* CItem::Buy_Item(_uint* iNumCubic)
{
    if(m_iPrice <= *iNumCubic)
    {
        *iNumCubic -= m_iPrice;
        m_pNPC->Buy_Item(this);
        return dynamic_cast<CItem*>(CItem::Clone(nullptr));
    }

    return nullptr;
}

void CItem::Exit_Shop()
{
    m_pNPC->Exit_Shop();
}

HRESULT CItem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{   
    if (nullptr != pArg)
    {
        ITEM_DESC* pDesc = (ITEM_DESC*)pArg;
        m_strModelComTag = pDesc->strModelComTag;    
        m_vPosition = pDesc->vPosition;
        m_isUseMaterial = pDesc->isUseMaterial;
        m_isPotion = pDesc->isPotion;
        m_eItem = pDesc->eItem;

        if(true == m_isUseMaterial)
            m_vMtrlDiffuse = pDesc->vMtrlDiffuse;

        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);

        Compute_ColliderMatrix();
    }

    m_eInteractiveType = INTERACTIVE_ITEM;

    m_iTextureIndex = (_uint)m_eItem;

    return S_OK;
}

HRESULT CItem::Tick(_float fTimeDelta)
{
    if (true == m_isShop)
    {
        m_pTransformCom->Turn(_vector{ 0.f, 1.f, 0.f, 0.f }, fTimeDelta * 0.3f);

        m_pColliderCom->Tick(m_ColliderMatrix);

        m_pGameInstance->Add_Group(CCollision_Manager::GROUP_INTERACTIVE, this);
    }

    return S_OK;
}

void CItem::Late_Tick(_float fTimeDelta)
{
    if(true == m_isShop)
    {
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

//#ifdef _DEBUG
//        m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif
    }
}

HRESULT CItem::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if(false == m_isPotion)
    {
        _uint iNumMeshes = m_pModelItemCom->Get_NumMeshes();
        for (size_t i = 0; i < iNumMeshes; ++i)
        {

            if (FAILED(m_pModelItemCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin((_uint)m_isUseMaterial)))
                return E_FAIL;

            m_pModelItemCom->Render(i);
        }
    }
    else
    {
        if (FAILED(m_pModelItemCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelItemCom->Render(0);

        if (FAILED(m_pModelItemCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 1, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelItemCom->Render(1);
    }


    return S_OK;
}

void CItem::Set_Information()
{
    m_iTextureIndex = (_uint)m_eItem;  

    if (ITEM_SHIELD == m_eItem || ITEM_DASH == m_eItem)
        m_eItemType = TYPE_UTILE;
    else if (ITEM_HP == m_eItem || ITEM_BOMB == m_eItem || ITEM_ICE == m_eItem /*|| ITEM_POTION == m_eItem*/)
        m_eItemType = TYPE_USE;
    else if (ITEM_STICK == m_eItem || ITEM_SWORD == m_eItem || ITEM_WAND == m_eItem)
        m_eItemType = TYPE_WEAPON;
}

HRESULT CItem::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelComTag,
        TEXT("Com_Model"), (CComponent**)&m_pModelItemCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(2.f, 1.f, 2.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fCamFar = m_pGameInstance->Get_Camera_Far();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
        return E_FAIL;

    if (true == m_isUseMaterial)
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_vMtrlDiffuse", &m_vMtrlDiffuse, sizeof(_vector))))
            return E_FAIL;
    }

    return S_OK;
}

CItem* CItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CItem* pInstance = new CItem(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CItem"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
    CItem* pInstance = new CItem(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CItem"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItem::Free()
{
    __super::Free();

    Safe_Release(m_pModelItemCom);
}

void CItem::Collision_Event(Engine::CGameObject* pGameObject)
{
    __super::Collision_Event(pGameObject);
}

void CItem::Damage_Event()
{
}
