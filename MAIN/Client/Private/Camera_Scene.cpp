#include "stdafx.h"
#include "Camera_Scene.h"

#include "Player.h"
#include "Easing.h"

#include "Monster_Librarian.h"

CCamera_Scene::CCamera_Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Scene::CCamera_Scene(const CCamera_Scene& rhs)
    : CCamera{ rhs }
{
#ifndef _DEBUG
    Safe_AddRef(m_pEasing);
#endif // _DEBUG   
}

HRESULT CCamera_Scene::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Scene::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Scene::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(false == m_isFinish)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
		_vector vPlayerPosition = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);

		_vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

		m_fAccEasingTime += fTimeDelta;
		_float fRatio = m_fAccEasingTime / m_fEasingTime;
		if (m_fAccEasingTime >= m_fEasingTime)
		{
			m_fAccEasingTime = 0.f;
			fRatio = 1.f;
			m_isFinish = true;

			CMonster_Librarian* pBoss = dynamic_cast<CMonster_Librarian*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, TEXT("Layer_Boss")));
			pBoss->Set_isTrigger(true);
		}

		_vector vPosition;
		vPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, m_vEnterPosition.m128_f32[0], m_vTargetPosition.m128_f32[0], fRatio);
		vPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, m_vEnterPosition.m128_f32[1], m_vTargetPosition.m128_f32[1], fRatio);
		vPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, m_vEnterPosition.m128_f32[2], m_vTargetPosition.m128_f32[2], fRatio);
		vPosition.m128_f32[3] = 1.f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		m_pTransformCom->Look_At(vPlayerPosition);
	}

	__super::Bind_PipeLines();

	return S_OK;
}

void CCamera_Scene::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Scene::Render()
{
	return S_OK;
}

void CCamera_Scene::Set_Level(_uint iLevel)
{
}

void CCamera_Scene::OnEnter(void* pArg)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
	_vector vPlayerPosition = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);

	m_vTargetPosition = _vector{ vPlayerPosition.m128_f32[0], vPlayerPosition.m128_f32[1] + 3.7f, vPlayerPosition.m128_f32[2] + 17.f };

	m_vEnterPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
}

void CCamera_Scene::OnExit()
{
}

CCamera_Scene* CCamera_Scene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Scene* pInstance = new CCamera_Scene(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CCamera_Scene"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Scene::Clone(void* pArg)
{
	CCamera_Scene* pInstance = new CCamera_Scene(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CCamera_Scene"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Scene::Free()
{
	__super::Free();

	if (true == m_isClone)
		Safe_Release(m_pEasing);
}
