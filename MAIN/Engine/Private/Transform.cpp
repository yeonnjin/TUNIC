#include "Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& rhs)
    : CComponent{ rhs }
    , m_WorldMatrix{ rhs.m_WorldMatrix }
{
}

void CTransform::Set_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ)
{
    Set_State(STATE_RIGHT, XMVector3Normalize(Get_State_Vector(STATE_RIGHT)) * fScaleX);
    Set_State(STATE_UP, XMVector3Normalize(Get_State_Vector(STATE_UP)) * fScaleY);
    Set_State(STATE_LOOK, XMVector3Normalize(Get_State_Vector(STATE_LOOK)) * fScaleZ);
}

HRESULT CTransform::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        TRANSFORM_DESC* pDesc = (TRANSFORM_DESC*)pArg;

        m_fSpeedPerSec = pDesc->fSpeedPerSec;
        m_fRotationPerSec = pDesc->fRotationPerSec;
    }

    return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

void CTransform::Go_Look(_float fTimeDelta, _fvector vLook)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * -1.f;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight(_float fTimeDelta, class CNavigation* pNavigation)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vLook = Get_State_Vector(STATE_LOOK);

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    if (nullptr == pNavigation)
        Set_State(STATE_POSITION, vPosition);
    else
    {
        if (true == pNavigation->isMove(vPosition))
            Set_State(STATE_POSITION, vPosition);
    }
}

void CTransform::Go_Backward(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vLook = Get_State_Vector(STATE_LOOK);

    vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vRight = Get_State_Vector(STATE_RIGHT);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vRight = Get_State_Vector(STATE_RIGHT);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Look_At(_fvector vAt)
{
    _vector vLook = vAt - Get_State_Vector(STATE_POSITION);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    _float3 vScaled = Get_Scaled();

    Set_State(STATE_RIGHT,  XMVector3Normalize(vRight) * vScaled.x);
    Set_State(STATE_UP,     XMVector3Normalize(vUp) * vScaled.y);
    Set_State(STATE_LOOK,   XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Look_At_For_LandOject(_fvector vAt, _bool isReverse)
{
    // XMMatrixDecompose();
    // : Transform 이 들고 있는 WorldMatrix 로 부터 값을 뽑아옴 (회전 정보 들고 오기 가능(쿼터니언))

    _vector vUp = Get_State_Vector(STATE_UP);
    _vector vLook;
    if (false == isReverse)
        vLook = vAt - Get_State_Vector(STATE_POSITION);
    else
        vLook = Get_State_Vector(STATE_POSITION) - vAt;

    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    vLook = XMVector3Cross(vRight, vUp);

    _float3 vScaled = Get_Scaled();

    Set_State(STATE_RIGHT,  XMVector3Normalize(vRight) * vScaled.x);
    Set_State(STATE_LOOK,   XMVector3Normalize(vLook) * vScaled.z);
}

_bool CTransform::Move_To_Target(_fvector vTargetPos, _float fTimeDelta, _float fMinDistance)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vLook = vTargetPos - vPosition;

    // XMVector3Length() 하면 길이가 { 5.f, 5.f, 5.f, 5.f } 이런 식으로 채워짐 => 128을 32로 나눈 것 중 0번째
    if (fMinDistance <= XMVector3Length(vLook).m128_f32[0])
    {
        vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
        Set_State(STATE_POSITION, vPosition);
        return false;
    }

    return true;
}

// 현재 각도에서 회전
void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

    for (size_t i = 0; i < STATE_POSITION; ++i)
    {
        Set_State(STATE(i), XMVector4Transform(Get_State_Vector((STATE)i), RotationMatrix));
    }
}

_bool CTransform::Turn_Look(_Out_ _bool* isFirst, _Out_ _float3* vLerpLook, _fvector vTargetLook, _float fTimeDelta)
{
    static _vector vLook, vFinishLook, vUp;
    static _float fTime = 0.f;
    _bool isFinish = false;

    if (*isFirst == true)
    {
        vLook = XMVector3Normalize(Get_State_Vector(STATE_LOOK));
        vLook.m128_f32[1] = 0.f;
        vFinishLook = XMVector3Normalize(vTargetLook);
        vFinishLook.m128_f32[1] = 0.f;
        *isFirst = false;
        fTime = 0.f;
    }
  
    fTime += fTimeDelta;
    _float  fRatio = fTime / 0.2f;
    if (fRatio >= 1.f)
    {
        fTime = 0.f;
        fRatio = 1.f;
        isFinish = true;
    }

    vUp = { 0.f, 1.f, 0.f };
    //_vector vUp = Get_State_Vector(STATE_UP);
   /* if (false == XMVector3Equal(vUp, _vector{ 0.f, 1.f, 0.f }))
    {
        int a = 0;
    }*/

     _vector vLerp = XMVectorLerp(vLook, vFinishLook, fRatio);
    vLerp.m128_f32[1] = 0.f;
    //_vector vLerpLook = vLerp - Get_State_Vector(STATE_POSITION);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerp);
    //vLerp = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

    _float3 vScaled = Get_Scaled();
    Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
    Set_State(STATE_LOOK, XMVector3Normalize(vLerp) * vScaled.z);
    Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);

    XMStoreFloat3(vLerpLook, Get_State_Vector(STATE_LOOK));

    return isFinish;
}

_bool CTransform::Turn_Angle(_fvector vAxis, _float fAngle, _float fTimeDelta)
{
    static _bool isFinished = false;
    static _float fRadian = 0.f;
    _matrix RotationMatrix;

    if (0 > fAngle)
    {
        fRadian += m_fRotationPerSec * fTimeDelta * -1.f;

        if (fRadian < XMConvertToRadians(fAngle))
        {
            fRadian = XMConvertToRadians(fAngle);
            isFinished = true;
            fRadian = 0.f;
        }
        else
            isFinished = false;

        RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta * -1.f);
    }
    else
    {
        fRadian += m_fRotationPerSec * fTimeDelta;

        if (fRadian > XMConvertToRadians(fAngle))
        {
            fRadian = XMConvertToRadians(fAngle);
            isFinished = true;
            fRadian = 0.f;
        }
        else
            isFinished = false;

        RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);
    }
  
    for (size_t i = 0; i < STATE_POSITION; ++i)
    {
        Set_State(STATE(i), XMVector4Transform(Get_State_Vector((STATE)i), RotationMatrix));
    }

    return isFinished;
}

// 원점으로부터 회전
void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
    _float3 vScaled = Get_Scaled();

    _vector vState[] = {
        XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x,
        XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y,
        XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z
    };

    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

    for (size_t i = 0; i < STATE_POSITION; ++i)
    {
        // XMVector4Transform() : 벡터 x 변환 매트릭스 (Coord, Normal 같은 친구)
        Set_State(STATE(i), XMVector4Transform(vState[(STATE)i], RotationMatrix));
        /*Set_State(STATE(i), XMVector3TransformNormal(Get_State_Vector((STATE)i), RotationMatrix));*/
    }
}

void CTransform::Go_Front(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vUp = { 0.f, 0.f, 1.f };

    vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_FL(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vDir = { -1.f, 0.f, -1.f };

    vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_L(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vLeft = { 1.f, 0.f, 0.f };

    vPosition -= XMVector3Normalize(vLeft) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_BL(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vDir = { -1.f, 0.f, 1.f };

    vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Back(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vUp = { 0.f, 0.f, 1.f };

    vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_BR(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vDir = { 1.f, 0.f, 1.f };

    vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_R(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vLeft = { 1.f, 0.f, 0.f };

    vPosition += XMVector3Normalize(vLeft) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_FR(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vDir = { 1.f, 0.f, -1.f };

    vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTransform* pInstance = new CTransform(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CTransform"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
    CTransform* pInstance = new CTransform(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CTransform"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTransform::Free()
{
    __super::Free();
}