#include "stdafx.h"
#include "NPC_Merchant.h"

#include "Merchant_State_Idle.h"
#include "Merchant_State_Hidden.h"
#include "Merchant_State_Thankyou.h"

#include "Player.h"

#include "UI_Purchase.h"
#include "UI_Obtain.h"

CNPC_Merchant::CNPC_Merchant(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CNPC_Merchant::CNPC_Merchant(const CNPC_Merchant& rhs)
    : CGameObject{ rhs }
{
}

void CNPC_Merchant::Select_Item(CItem* pItem)
{
    m_isSelect = true;

    // ±¸¸Å UI ¶ç¿ì±â
    m_pUIPurchase->Set_Using(true, pItem->Get_Item());
}

void CNPC_Merchant::Buy_Item(CItem* pItem)
{
    m_isSelect = false;
    // ¾ò±â UI ¶ç¿ì±â
    m_pUIPurchase->Set_Using(false);
    m_isLookPurchaseUI = true;
}

_bool CNPC_Merchant::Get_isOK()
{
    return m_pUIPurchase->Get_isOK();
}

void CNPC_Merchant::Exit_Shop()
{
    m_isSelect = false;
    m_pUIPurchase->Set_Using(false);
}

void CNPC_Merchant::Change_State(STATE eState)
{
    m_pModelCom->Change_State(eState);
    m_eState = eState;
}

HRESULT CNPC_Merchant::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNPC_Merchant::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_States()))
        return E_FAIL;

    if (FAILED(Add_Items()))
        return E_FAIL;

    m_pModelCom->Set_Animation_Transform(m_pTransformCom);
    Set_Animation();

    _float4 vPosition = _float4(0.f, 18.f, 56.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pModelCom->Set_isUseTransformaion(true);

    m_pUIPurchase = dynamic_cast<CUI_Purchase*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_UI_Purchase")));
    if (nullptr == m_pUIPurchase)
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Merchant::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;

    if (true == m_isLookPurchaseUI)
    {
        CUI_Obtain* pUIObtain = dynamic_cast<CUI_Obtain*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Obtain")));
        if (false == pUIObtain->Get_Using())
        {
            Change_State(STATE_THANKYOU);
            m_isLookPurchaseUI = false;
        }
    }
    
    m_pModelCom->Update_State(fTimeDelta);
    Update_State();

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

    for (auto& pItem : m_pItems)
        pItem->Tick(fTimeDelta);

    m_pUIPurchase->Tick(fTimeDelta);

    return S_OK;
}

void CNPC_Merchant::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

    for (auto& pItem : m_pItems)
        pItem->Late_Tick(fTimeDelta);

    m_pUIPurchase->Late_Tick(fTimeDelta);
}

HRESULT CNPC_Merchant::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
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

void CNPC_Merchant::Update_State()
{
}

HRESULT CNPC_Merchant::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_NPC_Merchant";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_SHOP, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC_Merchant::Add_States()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
    m_pModelCom->Add_State(STATE_HIDDEN, CMerchant_State_Hidden::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_IDLE, CMerchant_State_Idle::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_THANKYOU, CMerchant_State_Thankyou::Create(this, pPlayer));

    m_pModelCom->Change_State(STATE_HIDDEN);
    m_eState = STATE_HIDDEN;

    return S_OK;
}

HRESULT CNPC_Merchant::Add_Items()
{
    CItem::ITEM_DESC tDesc = {};
    tDesc.vPosition = { -15.f, 18.f, 47.f, 1.f };
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Item_Berry_HP";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.isUseMaterial = false;
    tDesc.eItem = CItem::ITEM_HP;
    tDesc.iPrice = 100;
    CItem* pItem = dynamic_cast<CItem*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Object_Item"), &tDesc));
    if (nullptr == pItem)
        return E_FAIL;

    pItem->Set_ItemType(CItem::TYPE_USE);
    pItem->Set_ShopItem(this);
    m_pItems.emplace_back(pItem);

    tDesc = {};
    tDesc.vPosition = { -5.f, 18.f, 47.f, 1.f };
    _char szModelTag1[MAX_PATH] = "Prototype_Component_Model_Item_Berry_MP";
    wstring wstr1(&szModelTag1[0], &szModelTag1[MAX_PATH]);
    tDesc.strModelComTag = wstr1;
    tDesc.isUseMaterial = false;
    tDesc.eItem = CItem::ITEM_MP;
    tDesc.iPrice = 100;
    pItem = dynamic_cast<CItem*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Object_Item"), &tDesc));
    if (nullptr == pItem)
        return E_FAIL;

    pItem->Set_ItemType(CItem::TYPE_USE);
    pItem->Set_ShopItem(this);
    m_pItems.emplace_back(pItem);

    tDesc = {};
    tDesc.vPosition = { 5.f, 18.f, 47.f, 1.f };
    _char szModelTag2[MAX_PATH] = "Prototype_Component_Model_Item_Laurel";
    wstring wstr2(&szModelTag2[0], &szModelTag2[MAX_PATH]);
    tDesc.strModelComTag = wstr2;
    tDesc.isUseMaterial = true;
    tDesc.vMtrlDiffuse = { 0.858f, 0.504f, 1.f, 1.f };
    tDesc.eItem = CItem::ITEM_DASH;
    tDesc.iPrice = 2000;
    pItem = dynamic_cast<CItem*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Object_Item"), &tDesc));
    if (nullptr == pItem)
        return E_FAIL;

    pItem->Set_ItemType(CItem::TYPE_UTILE);
    pItem->Set_ShopItem(this);
    m_pItems.emplace_back(pItem);

    tDesc = {};
    tDesc.vPosition = { 15.f, 18.f, 47.f, 1.f };
    _char szModelTag3[MAX_PATH] = "Prototype_Component_Model_Item_Potion";
    wstring wstr3(&szModelTag3[0], &szModelTag3[MAX_PATH]);
    tDesc.strModelComTag = wstr3;
    tDesc.isUseMaterial = true;
    tDesc.isPotion = true;
    tDesc.vMtrlDiffuse = { 1.f, 0.303f, 0.433f, 1.f };
    tDesc.eItem = CItem::ITEM_POTION;
    tDesc.iPrice = 1000;
    pItem = dynamic_cast<CItem*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Object_Item"), &tDesc));
    if (nullptr == pItem)
        return E_FAIL;

    pItem->Set_ItemType(CItem::TYPE_USE);
    pItem->Set_ShopItem(this);
    m_pItems.emplace_back(pItem);

    return S_OK;
}

HRESULT CNPC_Merchant::Bind_ShaderResources()
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

void CNPC_Merchant::Set_Animation()
{
    // LOOP
    m_pModelCom->Set_Animation_isLoop(ANIM_IDLE, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_HIDDEN, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_BLINK, true);

    // ROOT
    m_pModelCom->Set_Animation_isRoot(ANIM_IDLE, true);
}

CNPC_Merchant* CNPC_Merchant::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Merchant* pInstance = new CNPC_Merchant(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CNPC_Merchant"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNPC_Merchant::Clone(void* pArg)
{
    CNPC_Merchant* pInstance = new CNPC_Merchant(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CNPC_Merchant"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Merchant::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);

    Safe_Release(m_pUIPurchase);

    for (auto& pItem : m_pItems)
        Safe_Release(pItem);

    m_pItems.clear();
}
