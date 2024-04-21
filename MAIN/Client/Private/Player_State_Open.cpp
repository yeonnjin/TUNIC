#include "stdafx.h"

#include "Player.h"
#include "Player_State_Open.h"

CPlayer_State_Open::CPlayer_State_Open(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Open::OnStateEnter()
{
    // 패턴
    // 0. 상자 여는 모션 (51)
    // 1. 종료 시 획득 UI (다시 스페이스 바를 누르기 전까지 모두 멈춰있는 상태)
    // 2. 인벤토리에 넣기
    // 3. 상태 종료

    // TODO: 추가 인벤토리 작업
    // 0. 선택된 상태에서 J, K, L 중 하나를 누르면 슬롯에 장착

    m_pPlayer->Set_Blending(true, CPlayer::ANIM_OPEN_CHEST);
}

void CPlayer_State_Open::OnStateUpdate(_float fTimeDelta)
{
    if (49 == m_pPlayer->Get_Current_Frame(CPlayer::ANIM_OPEN_CHEST))
        m_pPlayer->Set_ChestOpen();

    if (true == m_pPlayer->Get_isFinished(CPlayer::ANIM_OPEN_CHEST))
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
}

void CPlayer_State_Open::OnStateExit()
{
}

CPlayer_State_Open* CPlayer_State_Open::Create(CPlayer* pPlayer)
{
    CPlayer_State_Open* pInstance = new CPlayer_State_Open(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Open"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Open::Free()
{
    __super::Free();
}