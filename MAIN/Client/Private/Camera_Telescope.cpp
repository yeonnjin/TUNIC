#include "stdafx.h"
#include "Camera_Telescope.h"

#include "Easing.h"

#include "Player.h"

CCamera_Telescope::CCamera_Telescope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Telescope::CCamera_Telescope(const CCamera_Telescope& rhs)
    : CCamera{ rhs }
	, m_pEasing{ CEasing::Get_Instance() }
{
#ifndef _DEBUG
	Safe_AddRef(m_pEasing);
#endif // _DEBUG   
}

HRESULT CCamera_Telescope::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Telescope::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	//m_vTargetLook = { 0.008f, -0.327f, 0.904f, 0.f };
	m_vTargetLook = { 10.2f, 2.f, -82.f, 0.f };

    return S_OK;
}

HRESULT CCamera_Telescope::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	// 최초 진입 시, 타겟 위치로 이동
	if (true == m_isMove && false == m_isStop)
	{
		m_fAccMoveTime += fTimeDelta;
		_float fRatio = m_fAccMoveTime / m_fMoveTime;
		if (fRatio >= 1.f)
		{
			m_fAccMoveTime = 0.f;
			fRatio = 1.f;
			m_isMove = false;
			m_isStop = true;
		}

		_vector vPosition;
		vPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vCamPosition.m128_f32[0], m_vTargetPosition.m128_f32[0], fRatio);
		vPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vCamPosition.m128_f32[1], m_vTargetPosition.m128_f32[1], fRatio);
		vPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vCamPosition.m128_f32[2], m_vTargetPosition.m128_f32[2], fRatio);
		vPosition.m128_f32[3] = 1.f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

		_vector vLook;
		vLook.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vCamLook.m128_f32[0], m_vTargetLook.m128_f32[0], fRatio);
		vLook.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vCamLook.m128_f32[1], m_vTargetLook.m128_f32[1], fRatio);
		vLook.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vCamLook.m128_f32[2], m_vTargetLook.m128_f32[2], fRatio);
		vLook.m128_f32[3] = 1.f;

		m_pTransformCom->Look_At(vLook);

		_float fFovy;
		fFovy = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_fCamFovy, m_fTargetFovy, fRatio);
		m_fFovy = fFovy;
	}
	// 타겟 위치로 이동 후 멈추기
	else if (false == m_isMove && true == m_isStop)
	{
		m_fAccStopTime += fTimeDelta;
		if (m_fAccStopTime >= m_fStopTime)
		{
			m_fAccStopTime = 0.f;
			m_isStop = false;
		}
	}
	// 원래 위치로 돌아가기
	else if (false == m_isMove && false == m_isStop)
	{
		m_fAccOriginTime += fTimeDelta;
		_float fRatio = m_fAccOriginTime / m_fOriginTime;
		if (fRatio >= 1.f)
		{
			m_isFinish = true;
			m_fAccOriginTime = 0.f;
		}

		_vector vPosition;
		vPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vTargetPosition.m128_f32[0], m_vCamPosition.m128_f32[0], fRatio);
		vPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vTargetPosition.m128_f32[1], m_vCamPosition.m128_f32[1], fRatio);
		vPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vTargetPosition.m128_f32[2], m_vCamPosition.m128_f32[2], fRatio);
		vPosition.m128_f32[3] = 1.f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

		_vector vLook;
		vLook.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vTargetLook.m128_f32[0], m_vCamLook.m128_f32[0], fRatio);
		vLook.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vTargetLook.m128_f32[1], m_vCamLook.m128_f32[1], fRatio);
		vLook.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_vTargetLook.m128_f32[2], m_vCamLook.m128_f32[2], fRatio);
		vLook.m128_f32[3] = 1.f;

		m_pTransformCom->Look_At(vLook);

		_float fFovy;
		fFovy = m_pEasing->Get_Ease(CEasing::Ease_InQuart, m_fTargetFovy, m_fCamFovy, fRatio);
		m_fFovy = fFovy;
	}

    __super::Bind_PipeLines();

	if (true == m_isFinish)
		m_pGameInstance->Change_Camera(TEXT("Camera_Follow"));

    return S_OK;
}

void CCamera_Telescope::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Telescope::Render()
{
    return S_OK;
}

void CCamera_Telescope::Set_Level(_uint iLevel)
{
}

void CCamera_Telescope::OnEnter(void* pArg)
{
	m_vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
	m_vCamLook = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
	m_fCamFovy = m_fFovy;

	m_pGameInstance->Play_Once(TEXT("INTERACT_Telescope_Use.wav"), CSound_Manager::EFFECT1);
}

void CCamera_Telescope::OnExit()
{
	m_isFinish = false;
	m_isMove = true;
	m_isStop = false;

	m_fAccMoveTime = 0.f;
	m_fAccStopTime = 0.f;
	m_fAccOriginTime = 0.f;

	m_pGameInstance->Play_Once(TEXT("INTERACT_Telescope_Reset.wav"), CSound_Manager::EFFECT1);
}

CCamera_Telescope* CCamera_Telescope::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Telescope* pInstance = new CCamera_Telescope(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CCamera_Telescope"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Telescope::Clone(void* pArg)
{
	CCamera_Telescope* pInstance = new CCamera_Telescope(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CCamera_Telescope"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Telescope::Free()
{
	__super::Free();

	if (true == m_isClone)
		Safe_Release(m_pEasing);
}
