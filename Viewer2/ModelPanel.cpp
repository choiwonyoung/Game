// ModelPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Viewer2.h"
#include "ModelPanel.h"
#include "afxdialogex.h"

using namespace graphic;


// CModelPanel 대화 상자입니다.

CModelPanel::CModelPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModelPanel::IDD, pParent)
{

}

CModelPanel::~CModelPanel()
{
}

void CModelPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_MESH, m_MeshTree);
	DDX_Control(pDX, IDC_TREE_MATERIAL, m_MaterialTree);
	DDX_Control(pDX, IDC_TREE_RAWBONE, m_RawBoneTree);
	DDX_Control(pDX, IDC_TREE_BONE, m_BoneTree);
}


BEGIN_MESSAGE_MAP(CModelPanel, CDialogEx)
	ON_BN_CLICKED(IDOK, &CModelPanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CModelPanel::OnBnClickedCancel)
END_MESSAGE_MAP()


// CModelPanel 메시지 처리기입니다.

void CModelPanel::Update()
{
	UpdateMaterialInfo();
	UpdateMeshInfo();	
	UpdateBoneInfo();
	UpdateRawBoneInfo();
}


// 메쉬 정보 업데이트
void CModelPanel::UpdateMeshInfo()
{
	m_MeshTree.DeleteAllItems();

	const sRawMeshGroup *rawMeshses = cResourceManager::Get()->LoadModel( 
		cController::Get()->GetCurrentMeshFileName() );
	RET (!rawMeshses);

	const wstring rootStr = formatw( "Mehses [%d]", rawMeshses->meshes.size());
	const HTREEITEM hRoot = m_MeshTree.InsertItem(rootStr.c_str());

	for (u_int i=0; i < rawMeshses->meshes.size(); ++i)
	{
		const sRawMesh &mesh = rawMeshses->meshes[ i];
		const wstring str = formatw("%s", mesh.name.c_str());		
		const HTREEITEM hItem = m_MeshTree.InsertItem( str.c_str(), hRoot);

		const wstring name = formatw("Name = %s", mesh.name.c_str());
		const wstring materialId = formatw("Material Id = %d", mesh.mtrlId);
		const wstring vertexCount = formatw("Vertex Count = %d", mesh.vertices.size());
		const wstring faceCount = formatw("Face Count = %d", mesh.indices.size()/3);
		const wstring normalCount = formatw("Normal Count = %d", mesh.normals.size());
		const wstring textureCount = formatw("Texture UV Count = %d", mesh.tex.size());
		const wstring vertexWeightCount = formatw("Vertex Weight Count = %d", mesh.weights.size());

		m_MeshTree.InsertItem( name.c_str(), hItem);
		m_MeshTree.InsertItem( materialId.c_str(), hItem);
		m_MeshTree.InsertItem( vertexCount.c_str(), hItem);
		m_MeshTree.InsertItem( faceCount.c_str(), hItem);
		m_MeshTree.InsertItem( normalCount.c_str(), hItem);
		m_MeshTree.InsertItem( textureCount.c_str(), hItem);
		m_MeshTree.InsertItem( vertexWeightCount.c_str(), hItem);
	}

	m_MeshTree.Expand(hRoot, TVE_EXPAND);
}


// 매터리얼 정보 업데이트
void CModelPanel::UpdateMaterialInfo()
{
	m_MaterialTree.DeleteAllItems();

	const sRawMeshGroup *rawMeshses = cResourceManager::Get()->LoadModel( 
		cController::Get()->GetCurrentMeshFileName() );
	RET (!rawMeshses);

	const wstring rootStr = formatw( "Materials [%d]", rawMeshses->mtrls.size());
	const HTREEITEM hRoot = m_MaterialTree.InsertItem(rootStr.c_str());

	for (u_int i=0; i < rawMeshses->mtrls.size(); ++i )
	{
		const sMaterial &mtrl = rawMeshses->mtrls[ i];
		const wstring str = formatw("Material%d", i+1);		
		const HTREEITEM hItem = m_MaterialTree.InsertItem( str.c_str(), hRoot);

		const wstring ambient = formatw("Ambient %.3f, %.3f, %.3f, %.3f", 
			mtrl.ambient.x, mtrl.ambient.y, mtrl.ambient.z, mtrl.ambient.w );
		const wstring diffuse = formatw("Diffuse %.3f, %.3f, %.3f, %.3f", 
			mtrl.diffuse.x, mtrl.diffuse.y, mtrl.diffuse.z, mtrl.diffuse.w );
		const wstring specular = formatw("Specular %.3f, %.3f, %.3f, %.3f", 
			mtrl.specular.x, mtrl.specular.y, mtrl.specular.z, mtrl.specular.w );
		const wstring emissive = formatw("Emissive %.3f, %.3f, %.3f, %.3f", 
			mtrl.emissive.x, mtrl.emissive.y, mtrl.emissive.z, mtrl.emissive.w );
		const wstring power = formatw("Power = %.3f", mtrl.power);
		const wstring texture = formatw("texture = %s", mtrl.texture.c_str());

		m_MaterialTree.InsertItem( ambient.c_str(), hItem);
		m_MaterialTree.InsertItem( diffuse.c_str(), hItem);
		m_MaterialTree.InsertItem( specular.c_str(), hItem);
		m_MaterialTree.InsertItem( emissive.c_str(), hItem);
		m_MaterialTree.InsertItem( power.c_str(), hItem);
		m_MaterialTree.InsertItem( texture.c_str(), hItem);
	}

	m_MaterialTree.Expand(hRoot, TVE_EXPAND);
}


// 본 정보 업데이트
void CModelPanel::UpdateRawBoneInfo()
{
	m_RawBoneTree.DeleteAllItems();

	const sRawMeshGroup *rawMeshses = cResourceManager::Get()->LoadModel( 
		cController::Get()->GetCurrentMeshFileName() );
	RET (!rawMeshses);

	const wstring rootStr = formatw( "Raw Bones [%d]", rawMeshses->bones.size());
	const HTREEITEM hRoot = m_RawBoneTree.InsertItem(rootStr.c_str());

	for (u_int i=0; i < rawMeshses->bones.size(); ++i)
	{
		const sRawBone &bone = rawMeshses->bones[ i];
		const wstring str = formatw("Bone%d [%s]", i+1, bone.name.c_str());
		const HTREEITEM hItem = m_RawBoneTree.InsertItem( str.c_str(), hRoot);

		const wstring name = formatw("Name = %s", bone.name.c_str());
		const wstring boneId = formatw("Bone Id = %d", bone.id);
		const wstring parentId = formatw("Parent Id = %d", bone.parentId);

		m_MaterialTree.InsertItem( name.c_str(), hItem);
		m_MaterialTree.InsertItem( boneId.c_str(), hItem);
		m_MaterialTree.InsertItem( parentId.c_str(), hItem);
	}

	m_RawBoneTree.Expand(hRoot, TVE_EXPAND);
}


// 본 정보 업데이트
void CModelPanel::UpdateBoneInfo()
{
	m_BoneTree.DeleteAllItems();

	const sRawMeshGroup *rawMeshses = cResourceManager::Get()->LoadModel( 
		cController::Get()->GetCurrentMeshFileName() );
	RET (!rawMeshses);

	graphic::cModel *model = cController::Get()->GetModel();
	RET(!model);

	graphic::cBoneMgr *boneMgr = model->GetBoneMgr();
	RET(!boneMgr);	

	graphic::cBoneNode *root = boneMgr->GetRoot();
	RET(!root);

	const wstring rootStr = formatw( "Bones Tree");
	const HTREEITEM hRoot = m_BoneTree.InsertItem(rootStr.c_str());

	MakeBoneTree(hRoot, root);

	//m_BoneTree.Expand(hRoot, TVE_EXPAND);
	ExpandAll(m_BoneTree);
}


// 본트리를 생성한다.
void CModelPanel::MakeBoneTree(HTREEITEM hParent,  graphic::cBoneNode *node)
{
	const wstring nodeStr = formatw( "%s [%d]", node->GetName().c_str(), node->GetId() );
	const HTREEITEM hItem = m_BoneTree.InsertItem(nodeStr.c_str(), hParent);

	BOOST_FOREACH (auto &child, node->GetChildren())
	{
		MakeBoneTree(hItem, (graphic::cBoneNode*)child);
	}
}


// 모든 트리노드를 펼친다.
void CModelPanel::ExpandAll(CTreeCtrl &treeCtrl)
{
	HTREEITEM hRoot = treeCtrl.GetRootItem();
	vector<HTREEITEM> items;
	items.reserve(treeCtrl.GetCount());

	items.push_back(hRoot);

	while (!items.empty())
	{
		HTREEITEM hItem = items.back();
		items.pop_back();
		treeCtrl.Expand(hItem, TVE_EXPAND);

		HTREEITEM hfirstChild = treeCtrl.GetChildItem(hItem);
		if (hfirstChild)
		{
			items.push_back(hfirstChild);

			while (HTREEITEM hNextItem = treeCtrl.GetNextSiblingItem(hfirstChild))
			{
				items.push_back(hNextItem);
				hfirstChild = hNextItem;
			}
		}
	}

	treeCtrl.SelectSetFirstVisible(hRoot);
}


void CModelPanel::OnBnClickedOk()
{	
}


void CModelPanel::OnBnClickedCancel()
{
}
