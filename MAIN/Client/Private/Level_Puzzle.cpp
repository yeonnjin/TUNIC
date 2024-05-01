#include "stdafx.h"
#include "Level_Puzzle.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Map.h"

#include "Player.h"

#include "Object_ColliderBox.h"
#include "Object_Ladder.h"

CLevel_Puzzle::CLevel_Puzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Puzzle::Initialize()
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

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, TEXT("Layer_Editor"), TEXT("Prototype_GameObject_Editor"))))
		return E_FAIL;

    return S_OK;
}

void CLevel_Puzzle::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (GetKeyState(VK_RETURN) & 0x8000)
    {
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS))))
            return;
    }
}

HRESULT CLevel_Puzzle::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("LEVEL : PUZZLE"));

    return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_Map(const wstring& strLayerTag)
{
    CMap::MAP_DESC tDesc = {};
	tDesc.isRotation = false;
	tDesc.vPosition = _float3(0.f, 0.f, 0.f);
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Map_Puzzle";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eLevel = LEVEL_PUZZLE;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, strLayerTag, TEXT("Prototype_GameObject_Map"), &tDesc)))
        return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_UI()
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, TEXT("Layer_UI_Arrow"), TEXT("Prototype_GameObject_UI_Arrow"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_Object(const wstring& strLayerTag)
{
	// ColliderBox
	vector<_vector> vColliderPosition;
	vColliderPosition.push_back(_vector{ 6.9f, 0.02f, -50.4f });
	vColliderPosition.push_back(_vector{ 6.8f, 0.02f, -64.4f });
	vColliderPosition.push_back(_vector{ 6.923f, 5.020f, -64.538f });
	vColliderPosition.push_back(_vector{ 6.878f, 10.020f, -50.734f });
	vColliderPosition.push_back(_vector{ -6.816f, 5.020f, -50.650f });
	vColliderPosition.push_back(_vector{ -6.937f, 10.020f, -64.521f });
	vColliderPosition.push_back(_vector{ -6.954f, 15.020f, -64.480f });
	vColliderPosition.push_back(_vector{ -6.983f, 15.020f, -50.594f });
	vColliderPosition.push_back(_vector{ 6.834f, 15.020f, -50.665f });
	vColliderPosition.push_back(_vector{ 6.830f, 20.020f, -50.678f });

	vColliderPosition.push_back(_vector{ -0.094f, 0.02f, -61.900f });
	vColliderPosition.push_back(_vector{ 4.392f, 5.020f, -57.407f }); // 11
	vColliderPosition.push_back(_vector{ -0.094f, 5.020f, -53.144f });
	vColliderPosition.push_back(_vector{ -4.419f, 5.020f, -57.552f }); // 13
	vColliderPosition.push_back(_vector{ -0.094f, 10.020f, -61.880f });
	vColliderPosition.push_back(_vector{ 4.267f, 15.020f, -57.533f }); // 15
	vColliderPosition.push_back(_vector{ -0.094f, 20.020f, -53.295f });

	vColliderPosition.push_back(_vector{ -0.094f, 7.000f, -63.332f });
	vColliderPosition.push_back(_vector{ 5.774f, 12.000f, -57.407f }); // 18
	vColliderPosition.push_back(_vector{ -0.094f, 12.000f, -51.688f });
	vColliderPosition.push_back(_vector{ -5.825f, 12.000f, -57.552f }); // 20
	vColliderPosition.push_back(_vector{ -0.094f, 17.000f, -63.281f });
	vColliderPosition.push_back(_vector{ 5.714f, 22.000f, -57.533f }); // 22
	vColliderPosition.push_back(_vector{ -0.094f, 27.000f, -51.720f });

	vColliderPosition.push_back(_vector{ 0.f, 25.020f, -58.f }); // 24

	for (size_t i = 0; i < vColliderPosition.size(); i++)
	{
		CObject_ColliderBox::COLLIDERBOX_DESC tColliderBoxDesc{};
		tColliderBoxDesc.vPosition = vColliderPosition[i];

		if (i < 10)
			tColliderBoxDesc.isRotation = true;
		else if (24 == i)
			tColliderBoxDesc.vSize = { 2.f, 2.f, 2.f };
		else
		{
			tColliderBoxDesc.vSize = { 2.f, 1.f, 1.f };

			if (11 == i || 13 == i || 15 == i || 18 == i || 20 == i || 22 == i)
			{
				tColliderBoxDesc.isRotation = true;
				tColliderBoxDesc.fAngle = 90.f;
			}
		}

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, TEXT("Layer_Object_ColliderBox"), TEXT("Prototype_GameObject_Object_ColliderBox"), &tColliderBoxDesc)))
			return E_FAIL;
	}

	// Ladder
	vector<_vector> vLadderPosition;

	vLadderPosition.push_back(_vector{ -0.094f, 0.02f, -63.072f });
	vLadderPosition.push_back(_vector{ -0.094f, 5.220f, -62.332f });

	vLadderPosition.push_back(_vector{ 5.692f, 5.020f, -57.407f });
	vLadderPosition.push_back(_vector{ 4.774f, 10.220f, -57.407f });

	vLadderPosition.push_back(_vector{ -0.094f, 5.020f, -51.644f });
	vLadderPosition.push_back(_vector{ -0.094f, 10.220f, -52.688f });

	vLadderPosition.push_back(_vector{ -5.919f, 5.020f, -57.552f });
	vLadderPosition.push_back(_vector{ -4.825f, 10.220f, -57.552f });

	vLadderPosition.push_back(_vector{ -0.094f, 10.020f, -63.280f });
	vLadderPosition.push_back(_vector{ -0.094f, 15.220f, -62.281f });

	vLadderPosition.push_back(_vector{ 5.667f, 15.020f, -57.533f });
	vLadderPosition.push_back(_vector{ 4.714f, 20.220f, -57.533f });

	vLadderPosition.push_back(_vector{ -0.094f, 20.020f, -51.995f });
	vLadderPosition.push_back(_vector{ -0.094f, 25.220f, -52.720f });

	for (size_t i = 0; i < vLadderPosition.size(); i++)
	{
		CObject_Ladder::LADDER_DESC tLadderDesc{};
		tLadderDesc.vPosition = vLadderPosition[i];
		tLadderDesc.isUpper = i % 2 == 0 ? true : false;
		tLadderDesc.isRotation = (i % 4 == 2 || i % 4 == 3) ? true : false;
		tLadderDesc.iIndex = i;

		if (13 == i)
			tLadderDesc.isEnd = true;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_PUZZLE, TEXT("Layer_Object_Ladder"), TEXT("Prototype_GameObject_Object_Ladder"), &tLadderDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Puzzle::Ready_Layer_Player()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
	pPlayer->Set_Level(LEVEL_PUZZLE);
	if (FAILED(pPlayer->Set_Navigation(LEVEL_PUZZLE)))
		return E_FAIL;
}

HRESULT CLevel_Puzzle::Ready_Layer_Camera()
{
	m_pGameInstance->Change_Camera(TEXT("Camera_Follow"));
	m_pGameInstance->Set_Camera_Level(LEVEL_PUZZLE);

	return S_OK;
}

CLevel_Puzzle* CLevel_Puzzle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Puzzle* pInstance = new CLevel_Puzzle(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CLevel_Puzzle"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Puzzle::Free()
{
    __super::Free();
}
