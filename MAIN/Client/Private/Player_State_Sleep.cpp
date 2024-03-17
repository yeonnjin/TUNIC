#include "stdafx.h"

#include "Player.h"
#include "Player_State_Sleep.h"

CPlayer_State_Sleep::CPlayer_State_Sleep(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Sleep::OnStateEnter()
{
    // TODO:무기 별 상태 처리 해줘야 함
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_SLEEPING);
}

void CPlayer_State_Sleep::OnStateUpdate(_float fTimeDelta)
{
    static _bool isGetUp = false;

    if (m_pGameInstance->Get_DIKeyState(DIK_P, KEY_DOWN))
    {        
        isGetUp = true;
        m_pPlayer->Set_Blending(true, CPlayer::ANIM_WAKEUP);
    }
    else if (true == isGetUp && true == m_pPlayer->Get_isFinished(CPlayer::ANIM_WAKEUP))
    {
        isGetUp = false;
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }
}

void CPlayer_State_Sleep::OnStateExit()
{
}

CPlayer_State_Sleep* CPlayer_State_Sleep::Create(CPlayer* pPlayer)
{
    CPlayer_State_Sleep* pInstance = new CPlayer_State_Sleep(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Sleep"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Sleep::Free()
{
    __super::Free();
}
