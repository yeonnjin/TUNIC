#pragma once

/* 클라이언트 개발자가 엔진의 기능을 사용하기 위해서 항상 접근 해야하는 클래스 */

#include "Renderer.h"
#include "Component_Manager.h"
#include "Collision_Manager.h"
#include "Input_Device.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT				Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void				Tick_Engine(_float fTimeDelta);
	void				Late_Tick_Engine(_float fTimeDelta);
	HRESULT				Begin_Draw(const _float4& vClearColor = _float4(0.f, 0.f, 1.f, 1.f));
	HRESULT				Draw();
	HRESULT				End_Draw();
	HRESULT				Clear(_uint iClearLevelIndex);

public: /* For.Input_Device */
	_bool				Get_DIKeyState(_ubyte byKeyID, KEYSTATE eState);
	_bool				Get_DIMouseState(MOUSEID eMouseID, KEYSTATE eState);
	_long				Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

public: /* For.Renderer */
	HRESULT				Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#ifdef _DEBUG
	HRESULT				Add_DebugComponent(class CComponent* pRenderComponent);
#endif

public: /* For.Level_Manager */
	HRESULT				Open_Level(_uint iNewLevelID, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT				Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT				Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT				Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pCloneObject);
	class CGameObject*	Get_GameObject_Clone(const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject*	Get_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);
	const CComponent*	Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	_uint				Get_Object_Count(_uint iLevelIndex, const wstring& strLayerTag);

public: /* For.Component_Manager */
	HRESULT				Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent*	Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Timer_Manager */
	HRESULT				Add_Timer(const wstring& strTimerTag);
	_float				Compute_TimeDelta(const wstring& strTimerTag);

public: /* For.PipeLine */
	void				Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_matrix				Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4			Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix				Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4			Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_vector				Get_CamPosition_Vector() const;
	_float4				Get_CamPosition_Float4() const;

public: /* For.Picking */
	void				Transform_Picking_To_LocalSpace(const class CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos);

public: /* For.Light_Manager */
	const LIGHT_DESC*	Get_LightDesc(_uint iIndex);
	HRESULT				Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT				Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public: /* For.Collision_Manager */
	HRESULT				Add_Group(CCollision_Manager::GROUP eCollisionGroup, class CGameObject* pGameObject);
	void				Check_Collision_Groups(CCollision_Manager::GROUP eCollisionGroupA, CCollision_Manager::GROUP eCollisionGroupB);
	HRESULT				Add_RigidGroup(class CGameObject* pGameObject);
	void				Check_Rigid_Groups();

public: /* For.Camera_Manager */
	HRESULT				Add_Camera(const wstring& strCameraTag, class CCamera* pCamera);
	HRESULT				Change_Camera(const wstring& strCameraTag, void* pArg = nullptr);
	_float				Get_Camera_Far();
	void				Set_Camera_Level(_uint iLevel);
	HRESULT				Set_Exit(const wstring& strCameraTag, _bool isExit);

public: /* For.Font_Manager */
	HRESULT				Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT				Render_Font(const wstring& strFontTag, const wstring& strOutputText, const _float2& vPosition, _fvector vColor, _float fRadian);

public: /* For.Target_Manager */
	HRESULT				Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT				Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
	HRESULT				Begin_MRT(const wstring& strMRTTag);
	HRESULT				End_MRT();
	HRESULT				Bind_RTShaderResource(class CShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT				Copy_Resource(const wstring& strRenderTargetTag, ID3D11Texture2D** ppRTTexture);
#ifdef _DEBUG
	HRESULT				Ready_RTVDebug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT				Draw_RTVDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public:	/* For.Sampler */
	_vector				Compute_WorldPos(const _float2& vViewPos, const wstring& strZRenderTargetTag, _uint iOffset = 0);

public: /* For.Frustum */
	_bool				isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);


public: /* For.ImGui_Manager */
	void				New_Frame();
	HRESULT				Render();
	void				EditTransform(CTransform* pTransformCom);

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager = { nullptr };
	class CCamera_Manager*			m_pCamera_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CSampler*					m_pSampler = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CImGui_Manager*			m_pImGui_Manager = { nullptr };

public:		
	static void Release_Engine();
	virtual void Free() override;
};

END