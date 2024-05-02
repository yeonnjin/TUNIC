#include "stdafx.h"
#include "Level_Shop.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Map.h"

#include "Player.h"
#include "Camera_Follow.h"

CLevel_Shop::CLevel_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Shop::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI()))
        return E_FAIL;

    if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;

    return S_OK;
}

void CLevel_Shop::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (true == m_pGameInstance->Get_DIKeyState(DIK_RCONTROL, KEY_DOWN))
    {
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_PUZZLE))))
            return;
    }
}

HRESULT CLevel_Shop::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("LEVEL : SHOP"));

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SHOP, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_UI()
{
    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_NPC(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SHOP, strLayerTag, TEXT("Prototype_GameObject_NPC_Merchant"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_Map(const wstring& strLayerTag)
{
    CMap::MAP_DESC tDesc = {};
    tDesc.isRotation = false;
    tDesc.vPosition = _float3(0.f, 0.f, 0.f);
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Map_Shop";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eLevel = LEVEL_SHOP;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SHOP, strLayerTag, TEXT("Prototype_GameObject_Map"), &tDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_Player()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    pPlayer->Set_Level(LEVEL_SHOP);
    if (FAILED(pPlayer->Set_Navigation(LEVEL_SHOP)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_Camera()
{
    m_pGameInstance->Change_Camera(TEXT("Camera_Follow"));
    m_pGameInstance->Set_Camera_Level(LEVEL_SHOP);

    CCamera_Follow* pCamera = dynamic_cast<CCamera_Follow*>(m_pGameInstance->Get_Camera(TEXT("Camera_Follow")));
    CTransform* pCameraTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(g_strTransformTag));

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
    _vector vPlayerPosition = pCameraTransform->Get_State_Vector(CTransform::STATE_POSITION);

    _vector vCamPosition = { vPlayerPosition.m128_f32[0], vPlayerPosition.m128_f32[1] + 13.f, vPlayerPosition.m128_f32[2] - 13.f, 1.f };
    pCameraTransform->Set_State(CTransform::STATE_POSITION, vCamPosition);
    pCameraTransform->Look_At(vPlayerPosition);

    return S_OK;
}

CLevel_Shop* CLevel_Shop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Shop* pInstance = new CLevel_Shop(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CLevel_Shop"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Shop::Free()
{
    __super::Free();
}
