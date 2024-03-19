#include "stdafx.h"

#include "Player.h"
#include "Player_State_Attack.h"

CPlayer_State_Attack::CPlayer_State_Attack()
{
}

void CPlayer_State_Attack::OnStateEnter()
{
    // TODO:무기 별 상태 처리 해줘야 함
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_SWING_STICK1);
    //m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon"), true);
}

void CPlayer_State_Attack::OnStateUpdate(_float fTimeDelta)
{
    
}

void CPlayer_State_Attack::OnStateExit()
{
   // m_pPlayer->Set_Weapon_Render(TEXT("Part_Player_Weapon"), false);
}

void CPlayer_State_Attack::Free()
{
    __super::Free();
}