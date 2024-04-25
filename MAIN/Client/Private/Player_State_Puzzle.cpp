#include "stdafx.h"

#include "Player.h"
#include "Player_State_Puzzle.h"

CPlayer_State_Puzzle::CPlayer_State_Puzzle(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Puzzle::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_IDLE);
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

    _vector vOrigin = _vector{ 0.f, vPlayerPosition.m128_f32[1], -58.f, 1.f };
    _vector vDir = XMVector3Normalize(vPlayerPosition - vOrigin);
    _vector vNormal = { 0.f, 1.f, 0.f, 0.f };
    _vector vCross = XMVector3Cross(vDir, vNormal);
    //pPlayerTransform->Look_At_Dir(vCross);

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