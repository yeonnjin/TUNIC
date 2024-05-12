#include "stdafx.h"
#include "Level_Boss.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Map.h"
#include "Sky.h"

#include "Player.h"
#include "Monster.h"

#include "Camera_Follow.h"

#include "Object_ColliderBox.h"
#include "Object_Ladder.h"

#include "UI_Loading.h"

CLevel_Boss::CLevel_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

void CLevel_Boss::Set_NextLevel(LEVEL eNextLevel)
{
    m_isNext = true;
    CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
    pUILoading->Set_Using(true);

    m_eNextLevel = eNextLevel;
}

HRESULT CLevel_Boss::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, TEXT("Layer_Editor"), TEXT("Prototype_GameObject_Editor"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Boss::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    static _bool isEnd = false;
    if (false == isEnd && false == m_pGameInstance->Sound_isPlaying(CSound_Manager::BGM))
    {
        m_pGameInstance->PlayBGM(TEXT("BGM_Boss_Loop.wav"));
        isEnd = true;
    }

    if (true == m_pGameInstance->Get_DIKeyState(DIK_RCONTROL, KEY_DOWN))
    {
        CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
        pUILoading->Set_Using(true);

        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BEACH))))
            return;
    }

    if (true == m_isNext)
    {
        CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
        if (true == pUILoading->Get_isFinish())
        {
            if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_eNextLevel))))
                return;
        }
    }
}

HRESULT CLevel_Boss::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("LEVEL : BOSS"));

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Map(const wstring& strLayerTag)
{
    CMap::MAP_DESC tDesc = {};
    tDesc.isRotation = false;
    tDesc.vPosition = _float3(0.f, 0.f, 0.f);
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Map_Librarian";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eLevel = LEVEL_BOSS;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, strLayerTag, TEXT("Prototype_GameObject_Map"), &tDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const wstring& strLayerTag)
{
    // BOSS_LIBRARIAN
    CMonster::Monster_Desc tDesc = {};
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Boss_Librarian";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eLevel = LEVEL_BOSS;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Monster_Librarian"), &tDesc)))
    	return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    CSky::SKY_DESC tDesc{};
    tDesc.strSkyComTag = TEXT("Prototype_Component_Texture_Sky_Boss");

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, strLayerTag, TEXT("Prototype_GameObject_Sky"), &tDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_UI()
{
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, TEXT("Layer_UI_BossHP"), TEXT("Prototype_GameObject_UI_BossHP"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Object(const wstring& strLayerTag)
{
    // ColliderBox : Trigger
    vector<_vector> vTriggerPosition;
    vTriggerPosition.push_back(_vector{ -0.4f, -40.503f, 40.6f });

    CObject_ColliderBox::COLLIDERBOX_DESC tColliderBoxDesc{};
    tColliderBoxDesc.vPosition = vTriggerPosition[0];
    tColliderBoxDesc.vSize = { 14.f, 2.f, 3.f };
    tColliderBoxDesc.isUseCollider = true;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, TEXT("Layer_Object_ColliderBox"), TEXT("Prototype_GameObject_Object_ColliderBox"), &tColliderBoxDesc)))
        return E_FAIL;

    vector<_vector> vColliderPosition;
    vColliderPosition.push_back(_vector{ 0.031f, -40.911f, 15.098f });
    vColliderPosition.push_back(_vector{ 0.031f, 20.905f, 15.598f }); // vColliderPosition.push_back(_vector{ 0.031f, -3.905f, 15.598f });
    vColliderPosition.push_back(_vector{ 0.031f, -4.603f, 11.197f });
    vColliderPosition.push_back(_vector{ 0.031f, 20.78f, 11.698f }); // vColliderPosition.push_back(_vector{ 0.031f, 1.78f, 11.698f });

    for (size_t i = 0; i < vColliderPosition.size(); i++)
    {
        CObject_ColliderBox::COLLIDERBOX_DESC tColliderBoxDesc{};
        tColliderBoxDesc.vPosition = vColliderPosition[i];

        tColliderBoxDesc.vSize = { 4.f, 1.f, 1.f };

        if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, TEXT("Layer_Object_ColliderBox"), TEXT("Prototype_GameObject_Object_ColliderBox"), &tColliderBoxDesc)))
            return E_FAIL;
    }

    // Ladder
    vector<_vector> vLadderPosition;

    vLadderPosition.push_back(_vector{ 0.031f, -40.911f, 16.098f });
    vLadderPosition.push_back(_vector{ 0.031f, -5.305f, 15.398f });

    vLadderPosition.push_back(_vector{ 0.031f, -4.603f, 12.197f });
    vLadderPosition.push_back(_vector{ 0.031f, 0.38f, 11.498f });

    for (size_t i = 0; i < vLadderPosition.size(); i++)
    {
        CObject_Ladder::LADDER_DESC tLadderDesc{};
        tLadderDesc.vPosition = vLadderPosition[i];
        tLadderDesc.isUpper = i % 2 == 0 ? true : false;
        //tLadderDesc.isRotation = (i % 4 == 2 || i % 4 == 3) ? true : false;
        tLadderDesc.iIndex = i;

        /*if (13 == i)
            tLadderDesc.isEnd = true;*/

        if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, TEXT("Layer_Object_Ladder"), TEXT("Prototype_GameObject_Object_Ladder"), &tLadderDesc)))
            return E_FAIL;
    }

    // Trigger
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, TEXT("Layer_Trigger_Scene"), TEXT("Prototype_GameObject_Object_Trigger_Scene"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Player()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    pPlayer->Set_Level(LEVEL_BOSS);
    if (FAILED(pPlayer->Set_Navigation(LEVEL_BOSS)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Camera()
{
    m_pGameInstance->Change_Camera(TEXT("Camera_Follow"));
    m_pGameInstance->Set_Camera_Level(LEVEL_BOSS);

    CCamera_Follow* pCamera = dynamic_cast<CCamera_Follow*>(m_pGameInstance->Get_Camera(TEXT("Camera_Follow")));
    CTransform* pCameraTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(g_strTransformTag));

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
    _vector vPlayerPosition = pCameraTransform->Get_State_Vector(CTransform::STATE_POSITION);

   _vector vCamPosition = { vPlayerPosition.m128_f32[0], vPlayerPosition.m128_f32[1] + 12.8f, vPlayerPosition.m128_f32[2] + 13.f, 1.f };
    pCameraTransform->Set_State(CTransform::STATE_POSITION, vCamPosition);
    pCameraTransform->Look_At(vPlayerPosition);

    return S_OK;
}

CLevel_Boss* CLevel_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Boss* pInstance = new CLevel_Boss(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CLevel_Boss"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Boss::Free()
{
    __super::Free();

    m_pGameInstance->Stop_Sound(CSound_Manager::BGM);
}
