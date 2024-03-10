#include "stdafx.h"
#include "Editor.h"

#include "Test_Object.h"
#include "ImGuizmo.h"

#include <locale>
#include <codecvt>

#include <fstream>
//#include "Texture.h"

#pragma region Initial

CEditor::CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEditor::CEditor(const CEditor& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CEditor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEditor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Load_Model();

	return S_OK;
}

#pragma endregion

void CEditor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEditor::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->New_Frame();

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

	// To Do
	Frame_Tab();

	ImGui::EndFrame();
}

HRESULT CEditor::Render()
{	
	m_pGameInstance->Render();

	return S_OK;
}

void CEditor::Frame_Tab()
{
	ImGui::Begin("FRAME");

	if (ImGui::BeginTabBar("MyTabBar")) {

		if (ImGui::BeginTabItem("[MODEL]")) 
		{			
			Tool_Model_List();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("[PICKING]")) 
		{
			Tool_Picking();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("[FILE]")) 
		{
			Tool_File();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();	
}

// ============================================================================================
// PICKING ====================================================================================

#pragma region PICKING
HRESULT CEditor::Test_Picking()
{
	const CVIBuffer_Terrain* pTerrainBuffer = dynamic_cast<const CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	if (nullptr == pTerrainBuffer)
		return E_FAIL;

	const CTransform* pTerrainTransform = dynamic_cast<const CTransform*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Terrain"), TEXT("Com_Transform")));
	if (nullptr == pTerrainTransform)
		return E_FAIL;

	_float3 vPickingPos = pTerrainBuffer->Compute_Picking(pTerrainTransform);

	if (!(0.f == vPickingPos.x && 0.f == vPickingPos.y && 0.f == vPickingPos.z))
	{
		CTest_Object::TEST_DESC tDesc = {};
		tDesc.vPosition = vPickingPos;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
			return E_FAIL;

		++m_iObjectCount;
	}
}

HRESULT CEditor::Test_Mesh_Picking()
{
	_uint iNumObjects = m_pGameInstance->Get_Object_Count(LEVEL_TOOL_MAP, TEXT("Layer_Object"));
	if (0 == iNumObjects)
		return E_FAIL;

	for (size_t i = 0; i < iNumObjects; ++i)
	{
		const CModel* pObjectModel = dynamic_cast<const CModel*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Com_Model"), i));
		if (nullptr == pObjectModel)
			return E_FAIL;

		const CTransform* pObjectTransform = dynamic_cast<const CTransform*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Com_Transform"), i));
		if (nullptr == pObjectTransform)
			return E_FAIL;

		if (true == pObjectModel->Check_Picking(pObjectTransform))
		{
			m_isUsingGizmo = true;
			m_pGizmoTransform = (CTransform*)pObjectTransform;
			return S_OK;
		}
	}

	return S_OK;
}

void CEditor::Gizmo(CTransform* pTransform)
{
	ImGui::Separator();

	if (ImGuizmo::IsUsing())
	{
		ImGui::Text("Using gizmo");
	}
	else
	{
		ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
	}

	m_pGameInstance->EditTransform(pTransform);
}
#pragma endregion

// ============================================================================================
// MODEL LIST =================================================================================

#pragma region MODEL LIST
HRESULT CEditor::Load_Model()
{
	// NON_ANIM_MODEL
	m_pGameInstance->Create_Prototype_Model(CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Models/Rock/"));

	// ANIM_MODEL
	m_pGameInstance->Create_Prototype_Model(CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Object/"));

	m_strModels = *m_pGameInstance->Get_Model_List(CModel::TYPE_NONANIM);

	return S_OK;
}
#pragma endregion

// ============================================================================================
// FILE =======================================================================================

#pragma region FILE
HRESULT CEditor::Save_File()
{
	ofstream fout;
	fout.open("../Bin/Resources/Data/Model/Model1.dat", ios::out | ios::binary);

	_uint iNumModels = m_pGameInstance->Get_Object_Count(LEVEL_TOOL_MAP, TEXT("Layer_Object"));
	if (0 == iNumModels)
		return E_FAIL;

	// Object Count
	fout.write(reinterpret_cast<char*>(&iNumModels), sizeof(_uint));

	for (size_t i = 0; i < iNumModels; ++i)
	{
		CModel* pObjectModel = (CModel*)(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Com_Model"), i));
		if (nullptr == pObjectModel)
			return E_FAIL;

		MODELFILE* pModelFile = pObjectModel->Get_ModelFile();

		// Type
		_uint iType = pModelFile->iType;
		fout.write(reinterpret_cast<char*>(&iType), sizeof(_uint));

		// Mesh
		fout.write(reinterpret_cast<char*>(&pModelFile->iNumMeshes), sizeof(_uint));
		for (size_t j = 0; j < pModelFile->iNumMeshes; ++j)
		{
			fout.write(reinterpret_cast<char*>(&pModelFile->Meshes[j].szName), sizeof(_char) * MAX_PATH);
			fout.write(reinterpret_cast<char*>(&pModelFile->Meshes[j].iMaterialIndex), sizeof(_uint));

			fout.write(reinterpret_cast<char*>(&pModelFile->Meshes[j].iNumFaces), sizeof(_uint));

			fout.write(reinterpret_cast<char*>(&pModelFile->Meshes[j].iNumBones), sizeof(_uint));
			for (size_t k = 0; k < pModelFile->Meshes[j].iNumBones; ++k)
				fout.write(reinterpret_cast<char*>(&pModelFile->Meshes[j].Bones[k]), sizeof(_uint));

			fout.write(reinterpret_cast<char*>(&pModelFile->Meshes[j].iNumVertices), sizeof(_uint));

			if (CModel::TYPE_NONANIM == iType)
				fout.write(reinterpret_cast<char*>(pModelFile->Meshes[j].pMeshVertices), sizeof(VTXMESH) * pModelFile->Meshes[j].iNumVertices);
			else
				fout.write(reinterpret_cast<char*>(pModelFile->Meshes[j].pAnimMeshVertices), sizeof(VTXANIMMESH) * pModelFile->Meshes[j].iNumVertices);

			fout.write(reinterpret_cast<char*>(&pModelFile->Meshes[j].iNumIndices), sizeof(_uint));
			fout.write(reinterpret_cast<char*>(pModelFile->Meshes[j].pIndices), sizeof(_uint) * pModelFile->Meshes[j].iNumIndices);

			fout.write(reinterpret_cast<char*>(&pModelFile->Meshes[j].iNumOffsetMatrices), sizeof(_uint));
			for (size_t k = 0; k < pModelFile->Meshes[j].iNumOffsetMatrices; ++k)
				fout.write(reinterpret_cast<char*>(&pModelFile->Meshes[j].OffsetMatrices[k]), sizeof(_float4x4));
		}

		// Material
		fout.write(reinterpret_cast<char*>(&pModelFile->iNumMaterials), sizeof(_uint));
		for (size_t j = 0; j < pModelFile->iNumMaterials; ++j)
			fout.write(reinterpret_cast<char*>(&pModelFile->Materials[j]), sizeof(MATERIALFILE));

		// Bone
		fout.write(reinterpret_cast<char*>(&pModelFile->TransformMatrix), sizeof(_float4x4));
		fout.write(reinterpret_cast<char*>(&pModelFile->iNumBones), sizeof(_uint));
		for (size_t j = 0; j < pModelFile->iNumBones; ++j)
			fout.write(reinterpret_cast<char*>(&pModelFile->Bones[j]), sizeof(BONEFILE));

		// Animation
		fout.write(reinterpret_cast<char*>(&pModelFile->iNumAnimations), sizeof(_uint));
		fout.write(reinterpret_cast<char*>(&pModelFile->iCurrentAnimIndex), sizeof(_uint));
		fout.write(reinterpret_cast<char*>(&pModelFile->isLoop), sizeof(_bool));
		for (size_t j = 0; j < pModelFile->iNumAnimations; ++j)
		{
			fout.write(reinterpret_cast<char*>(&pModelFile->Animations[j].szName), sizeof(_char) * MAX_PATH);

			fout.write(reinterpret_cast<char*>(&pModelFile->Animations[j].fDuration), sizeof(_float));
			fout.write(reinterpret_cast<char*>(&pModelFile->Animations[j].fTicksPerSecond), sizeof(_float));
			fout.write(reinterpret_cast<char*>(&pModelFile->Animations[j].fTrackPosition), sizeof(_float));

			// Channel
			fout.write(reinterpret_cast<char*>(&pModelFile->Animations[j].iNumChannels), sizeof(_uint));
			for (size_t k = 0; k < pModelFile->Animations[j].iNumChannels; ++k)
			{
				fout.write(reinterpret_cast<char*>(&pModelFile->Animations[j].Channels[k].szName), sizeof(_char) * MAX_PATH);
				fout.write(reinterpret_cast<char*>(&pModelFile->Animations[j].Channels[k].iBoneIndex), sizeof(_int));

				// KeyFrame
				fout.write(reinterpret_cast<char*>(&pModelFile->Animations[j].Channels[k].iNumKeyFrames), sizeof(_uint));
				for (size_t l = 0; l < pModelFile->Animations[j].Channels[k].iNumKeyFrames; ++l)
					fout.write(reinterpret_cast<char*>(&pModelFile->Animations[j].Channels[k].KeyFrames[l]), sizeof(KEYFRAME));
			}
		}

		for (size_t j = 0; j < 512; ++j)
			fout.write(reinterpret_cast<char*>(&pModelFile->MeshBoneMatrices[j]), sizeof(_float4x4));
	}

	fout.close();

	return S_OK;
}

HRESULT CEditor::Load_File()
{
	ifstream fin;
	fin.open("../Bin/Resources/Data/Model/Model1.dat", ios::in | ios::binary);

	_uint iObjectCount;
	fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(_uint));
	for (size_t i = 0; i < iObjectCount; ++i)
	{
		MODELFILE tModelFile = {};

		// Type
		fin.read(reinterpret_cast<char*>(&tModelFile.iType), sizeof(_uint));

		// Mesh
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumMeshes), sizeof(_uint));
		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
		{
			MESHFILE tMeshFile{};
			fin.read(reinterpret_cast<char*>(&tMeshFile.szName), sizeof(_char) * MAX_PATH);
			fin.read(reinterpret_cast<char*>(&tMeshFile.iMaterialIndex), sizeof(_uint));

			fin.read(reinterpret_cast<char*>(&tMeshFile.iNumFaces), sizeof(_uint));

			fin.read(reinterpret_cast<char*>(&tMeshFile.iNumBones), sizeof(_uint));
			for (size_t k = 0; k < tMeshFile.iNumBones; ++k)
			{
				_uint	iBoneIndex{};
				fin.read(reinterpret_cast<char*>(&iBoneIndex), sizeof(_uint));
				tMeshFile.Bones.push_back(iBoneIndex);
			}

			_uint iNumVertices{};
			fin.read(reinterpret_cast<char*>(&iNumVertices), sizeof(_uint));
			tMeshFile.iNumVertices = iNumVertices;

			if (CModel::TYPE_NONANIM == tModelFile.iType)
			{
				tMeshFile.pMeshVertices = new VTXMESH[iNumVertices];
				ZeroMemory(tMeshFile.pMeshVertices, sizeof(VTXMESH) * iNumVertices);
				for (size_t k = 0; k < iNumVertices; ++k)
				{
					VTXMESH tMesh{};
					fin.read(reinterpret_cast<char*>(&tMesh), sizeof(VTXMESH));
					memcpy(&tMeshFile.pMeshVertices[k], &tMesh, sizeof(VTXMESH));
				}
			}
			else
			{
				tMeshFile.pAnimMeshVertices = new VTXANIMMESH[iNumVertices];
				ZeroMemory(tMeshFile.pAnimMeshVertices, sizeof(VTXANIMMESH) * iNumVertices);
				for (size_t k = 0; k < iNumVertices; ++k)
				{
					VTXANIMMESH tMesh{};
					fin.read(reinterpret_cast<char*>(&tMesh), sizeof(VTXANIMMESH));
					memcpy(&tMeshFile.pAnimMeshVertices[k], &tMesh, sizeof(VTXANIMMESH));
				}
			}

			_uint iNumIndices{};
			fin.read(reinterpret_cast<char*>(&iNumIndices), sizeof(_uint));
			tMeshFile.iNumIndices = iNumIndices;

			tMeshFile.pIndices = new _uint[iNumIndices];
			ZeroMemory(tMeshFile.pIndices, sizeof(_uint) * iNumIndices);
			for (size_t k = 0; k < iNumIndices; ++k)
			{
				_uint iIndex{};
				fin.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));
				memcpy(&tMeshFile.pIndices[k], &iIndex, sizeof(_uint));
			}

			fin.read(reinterpret_cast<char*>(&tMeshFile.iNumOffsetMatrices), sizeof(_uint));
			for (size_t k = 0; k < tMeshFile.iNumOffsetMatrices; ++k)
			{
				_float4x4	OffsetMatrix{};
				fin.read(reinterpret_cast<char*>(&OffsetMatrix), sizeof(_float4x4));
				tMeshFile.OffsetMatrices.push_back(OffsetMatrix);
			}
			tModelFile.Meshes.push_back(tMeshFile);
		}

		// Material
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumMaterials), sizeof(_uint));
		for (size_t j = 0; j < tModelFile.iNumMaterials; ++j)
		{
			MATERIALFILE tMaterialFile{};
			fin.read(reinterpret_cast<char*>(&tMaterialFile), sizeof(MATERIALFILE));
			tModelFile.Materials.push_back(tMaterialFile);
		}

		// Bone
		fin.read(reinterpret_cast<char*>(&tModelFile.TransformMatrix), sizeof(_float4x4));
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumBones), sizeof(_uint));
		for (size_t j = 0; j < tModelFile.iNumBones; ++j)
		{
			BONEFILE tBoneFile = {};
			fin.read(reinterpret_cast<char*>(&tBoneFile), sizeof(BONEFILE));
			tModelFile.Bones.push_back(tBoneFile);
		}

		// Animation
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumAnimations), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&tModelFile.iCurrentAnimIndex), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&tModelFile.isLoop), sizeof(_bool));
		for (size_t j = 0; j < tModelFile.iNumAnimations; ++j)
		{
			ANIMFILE tAnimFile{};
			fin.read(reinterpret_cast<char*>(&tAnimFile.szName), sizeof(_char) * MAX_PATH);

			fin.read(reinterpret_cast<char*>(&tAnimFile.fDuration), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&tAnimFile.fTicksPerSecond), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&tAnimFile.fTrackPosition), sizeof(_float));

			fin.read(reinterpret_cast<char*>(&tAnimFile.iNumChannels), sizeof(_uint));
			for (size_t k = 0; k < tAnimFile.iNumChannels; ++k)
			{
				CHANNELFILE tChannelFile = {};
				fin.read(reinterpret_cast<char*>(&tChannelFile.szName), sizeof(_char) * MAX_PATH);
				fin.read(reinterpret_cast<char*>(&tChannelFile.iBoneIndex), sizeof(_int));

				fin.read(reinterpret_cast<char*>(&tChannelFile.iNumKeyFrames), sizeof(_uint));
				for (size_t l = 0; l < tChannelFile.iNumKeyFrames; ++l)
				{
					KEYFRAME tKeyFrame = {};
					fin.read(reinterpret_cast<char*>(&tKeyFrame), sizeof(KEYFRAME));
					tChannelFile.KeyFrames.push_back(tKeyFrame);
				}
				tAnimFile.Channels.push_back(tChannelFile);
			}

			tModelFile.Animations.push_back(tAnimFile);
		}

		for (size_t j = 0; j < 512; ++j)
			fin.read(reinterpret_cast<char*>(&tModelFile.MeshBoneMatrices[j]), sizeof(_float4x4));

		int a = 0;

		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
			Safe_Delete_Array(tModelFile.Meshes[j].pMeshVertices);

		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
			Safe_Delete_Array(tModelFile.Meshes[j].pAnimMeshVertices);

		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
			Safe_Delete_Array(tModelFile.Meshes[j].pIndices);
	}

	fin.close();

	return S_OK;
}
#pragma endregion

// ============================================================================================
// TOOL =======================================================================================

#pragma region TOOL
void CEditor::Tool_Model_List()
{
	static _int	iCurrentIndex = { 0 };

	// vector<string>
	vector<string> strString;
	for (auto& strModel : m_strModels)
		strString.push_back(WStringToString(strModel));

	// const char*
	vector<const _char*> strChar;
	for (auto& strModel : strString)
		strChar.push_back(strModel.c_str());

	ImGui::ListBox("List Box", &iCurrentIndex, strChar.data(), strChar.size());
}

void CEditor::Tool_Picking()
{
	ImGui::Checkbox("Using Picking", &m_isUsingPicking);
	ImGui::Text("LBUTTON : Terrain, RBUTTON : Mesh");

	if (m_isUsingPicking && !m_isUsingGizmo)
	{
		if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
			Test_Picking();

		else if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_DOWN))
			Test_Mesh_Picking();
	}

	ImGui::Checkbox("Using Gizmo", &m_isUsingGizmo);
	if (m_pGameInstance->Get_DIKeyState(DIK_G, KEY_DOWN))
		m_isUsingGizmo = !m_isUsingGizmo;

	if(m_isUsingGizmo)
		m_isUsingPicking = false;
	else
		m_isUsingPicking = true;

	if (m_isUsingGizmo && nullptr != m_pGizmoTransform)
		Gizmo(m_pGizmoTransform);
}

void CEditor::Tool_File()
{
	if (ImGui::Button("SAVE", ImVec2(50.f, 30.f)))
		Save_File();

	ImGui::SameLine();

	if (ImGui::Button("LOAD", ImVec2(50.f, 30.f)))
		Load_File();
}
#pragma endregion

// ============================================================================================
// ETC ========================================================================================

#pragma region ETC
string CEditor::WStringToString(wstring& wstr)
{
	wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(wstr);
}

void CEditor::Test()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		static float fCharacterX = 0.0f;
		static float fCharacterY = 0.0f;
		static float fCharacterZ = 0.0f;

		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");

		ImGui::SliderFloat("float", &fCharacterX, 0.0f, 300.0f);
		ImGui::SliderFloat("float", &fCharacterY, 0.0f, 300.0f);
		ImGui::SliderFloat("float", &fCharacterZ, 0.0f, 1.0f);

		if (ImGui::Button("Save Me"))
			// File Save
			if (ImGui::Button("Close Me"))
				show_another_window = false;
		ImGui::End();
	}
}
#pragma endregion

// ============================================================================================
// CREATE =====================================================================================

#pragma region CREATE
CEditor* CEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEditor* pInstance = new CEditor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CEditor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEditor::Clone(void* pArg)
{
	CEditor* pInstance = new CEditor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CEditor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditor::Free()
{
	__super::Free();

	Safe_Release(m_pGizmoTransform);
}
#pragma endregion

// ============================================================================================