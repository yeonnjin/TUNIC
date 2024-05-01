#include "stdafx.h"

#include "Player.h"
#include "Player_State_Puzzle.h"

CPlayer_State_Puzzle::CPlayer_State_Puzzle(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
    Set_Height();
    Set_Axis();
}

void CPlayer_State_Puzzle::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_IDLE);

    _uint iLadderIndex = m_pPlayer->Get_Ladder_Index();
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
    _vector vPlayerPosition = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    vPlayerPosition.m128_f32[1] = m_fHeight[iLadderIndex];
    pPlayerTransform->Set_State(CTransform::STATE_POSITION, vPlayerPosition);

    m_pGameInstance->Change_Camera(TEXT("Camera_Puzzle"));
}

void CPlayer_State_Puzzle::OnStateUpdate(_float fTimeDelta)
{
    m_isPreMove = m_isMove;

    // 애니메이션 제어
    IF_PLAYER_ISMOVE
        m_isMove = true;
    else
        m_isMove = false;

    if(m_isPreMove != m_isMove)
    {
        if(true == m_isMove)
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
        else
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_IDLE);
    }

    // 움직임 제어
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
    _vector vPlayerPosition = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vCamPosition = m_pGameInstance->Get_CamPosition_Vector();

    // 플레이어와 맵 중점의 각도에 따라서 vOrigin이 바뀜
    _vector vOrigin = _vector{ 0.f, vPlayerPosition.m128_f32[1], -58.f, 1.f };
    _vector vDiff = vPlayerPosition - vOrigin;
    _vector vDir = XMVector3Normalize(vDiff);

    
    //_float fAngle = acosf(vDot.m128_f32[0]);
    _vector vCross;
    _float fRadian = XMConvertToRadians(45.f);
    _float fRadian2 = XMConvertToRadians(-45.f);
    _float fCosf = cosf(XMConvertToRadians(45.f));
    _float fCosf2 = cosf(XMConvertToRadians(-45.f));

    for(size_t i = 0; i < 4; i++)
    {
        _vector vDot = XMVector3Dot(m_vAxis[i], vDir);

        if (cosf(XMConvertToRadians(45.f)) <= vDot.m128_f32[0] && cosf(XMConvertToRadians(-45.f)) < vDot.m128_f32[0])
        {
            //vCross = { -1.f, 0.f, 0.f, 0.f };
            vCross = { -1.f * m_vAxis[i].m128_f32[2], 0.f, m_vAxis[i].m128_f32[0], 0.f };
            vCross = XMVector3Normalize(vCross);
            break;
        }
    }

    if(true == m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS) && true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
        pPlayerTransform->Puzzle_FL(fTimeDelta, vCross);
    else if(true == m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS) && true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
        pPlayerTransform->Puzzle_FR(fTimeDelta, vCross);
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS) && true == m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
        pPlayerTransform->Puzzle_BL(fTimeDelta, vCross);
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS) && true == m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
        pPlayerTransform->Puzzle_BR(fTimeDelta, vCross);
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS))
        pPlayerTransform->Puzzle_Straight(fTimeDelta, vCross);
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))
        pPlayerTransform->Puzzle_Backward(fTimeDelta, vCross);
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
        pPlayerTransform->Puzzle_Right(fTimeDelta, vCross);
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
        pPlayerTransform->Puzzle_Left(fTimeDelta, vCross);
}

void CPlayer_State_Puzzle::OnStateExit()
{
    //m_pGameInstance->Change_Camera(TEXT("Camera_Follow"));

    m_isPreMove = m_isMove = false;
}

void CPlayer_State_Puzzle::Set_Height()
{
    m_fHeight[0] = 0.02f;
    m_fHeight[1] = 5.020f;

    m_fHeight[2] = 5.020f;
    m_fHeight[3] = 10.020f;

    m_fHeight[4] = 5.020f;
    m_fHeight[5] = 10.020f;

    m_fHeight[6] = 5.020f;
    m_fHeight[7] = 10.020f;

    m_fHeight[8] = 10.020f;
    m_fHeight[9] = 15.020f;

    m_fHeight[10] = 15.020f;
    m_fHeight[11] = 20.020f;

    m_fHeight[12] = 20.020f;
    m_fHeight[13] = 25.020f;
}

void CPlayer_State_Puzzle::Set_Axis()
{
    _vector vAxisZPlus = { 0.f, 0.f, 1.f, 0.f };
    _vector vAxisXPlus = { 1.f, 0.f, 0.f, 0.f };
    _vector vAxisZMinus = { 0.f, 0.f, -1.f, 0.f };
    _vector vAxisXMinus = { -1.f, 0.f, 0.f, 0.f };

    m_vAxis.push_back(vAxisZPlus);
    m_vAxis.push_back(vAxisXPlus);
    m_vAxis.push_back(vAxisZMinus);
    m_vAxis.push_back(vAxisXMinus);
}

CPlayer_State_Puzzle* CPlayer_State_Puzzle::Create(CPlayer* pPlayer)
{
    CPlayer_State_Puzzle* pInstance = new CPlayer_State_Puzzle(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Puzzle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Puzzle::Free()
{
    __super::Free();
}