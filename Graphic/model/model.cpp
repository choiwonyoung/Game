
#include "stdafx.h"
#include "model.h"
#include "node.h"
#include "../manager/resourcemanager.h"
#include "mesh.h"
#include "bonemgr.h"


using namespace graphic;



cModel::cModel(const int id) :
	cNode(id, "model" )
,	m_bone(NULL)
,	m_isRenderMesh(true)
,	m_isRenderBone(false)
,	m_isRenderBoundingBox(false)
,	m_isRenderShadow(false)
,	m_type(MODEL_TYPE::RIGID)
,	m_curAni(NULL)
{
	m_nodeType = MODEL;
	
}

cModel::~cModel()
{
	Clear();
}


bool cModel::Create(const string &modelName, MODEL_TYPE::TYPE type )
	// type = MODEL_TYPE::AUTO
{
	sRawMeshGroup *rawMeshes = cResourceManager::Get()->LoadModel(modelName);
	RETV(!rawMeshes, false);

	Clear();

	m_fileName = modelName;
	const bool isSkinnedMesh = !rawMeshes->bones.empty();

	// 스키닝 애니메이션이면 Bone을 생성한다.
	switch (type)
	{
	case MODEL_TYPE::RIGID:
		m_type = MODEL_TYPE::RIGID;
		break;

	case MODEL_TYPE::AUTO:
	case MODEL_TYPE::SKIN:
		if (isSkinnedMesh)
		{
			m_bone = new cBoneMgr(0, *rawMeshes);
			m_type = MODEL_TYPE::SKIN;
		}
		else
		{
			m_type = MODEL_TYPE::RIGID;
		}
		break;
	}

	// 메쉬 생성.
	int id = 0;
	BOOST_FOREACH (auto &mesh, rawMeshes->meshes)
	{
		cMesh *p = NULL;
		if (MODEL_TYPE::SKIN == m_type)
		{
			p = new cSkinnedMesh(id++, &m_bone->GetPalette(), mesh);
		}
		else
		{
			p = new cRigidMesh(id++, mesh);
		}

		if (p)
			m_meshes.push_back(p);
	}
	
	if (MODEL_TYPE::RIGID == m_type)
	{
		SetShader( cResourceManager::Get()->LoadShader("hlsl_rigid_phong.fx") );
	}
	else
	{
		SetShader( cResourceManager::Get()->LoadShader("hlsl_skinning_no_light.fx") );
	}


	// 모델 충돌 박스를 생성한다.
	GetCollisionBox();

	return true;
}


// 애니메이션 시작.
bool cModel::SetAnimation( const string &aniFileName, 
	const int nAniFrame, // = 0
	const bool isLoop, // = false
	const bool isBlend // = true
	)
{
	sRawAniGroup *rawAnies = cResourceManager::Get()->LoadAnimation(aniFileName);
	RETV(!rawAnies, false);

	m_curAni = rawAnies;

	if (MODEL_TYPE::SKIN == m_type)
	{
		m_bone->SetAnimation(*rawAnies, nAniFrame, isLoop, isBlend);
	}
	else
	{
		for (u_int i=0; i < m_meshes.size(); ++i)
		{
			((cRigidMesh*)m_meshes[ i])->LoadAnimation(rawAnies->anies[0]);
		}
	}

	return true;
}


bool cModel::Move(const float elapseTime)
{
	BOOST_FOREACH (auto node, m_meshes)
		node->Move(elapseTime);

	if (m_bone)
		m_bone->Move(elapseTime);

	return true;
}


void cModel::Render(const Matrix44 &tm)
{

	// 셰이더가 설정되지 않았다면 업데이트 한다.
	if (m_shader &&  !m_meshes.empty()) // check shader setting
	{
		if (m_meshes[ 0]->GetShader() == NULL)
		{
			BOOST_FOREACH (auto node, m_meshes)
				node->SetShader(m_shader);
		}		
	}

	GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&Matrix44::Identity);

	if (m_isRenderMesh)
	{
		BOOST_FOREACH (auto node, m_meshes)
			node->Render(m_TM * tm);
	}

	if (m_isRenderBone && m_bone)
		m_bone->Render(m_TM * tm);

	if (m_bone && m_isRenderBoundingBox)
		m_bone->RenderBoundingBox(m_TM * tm);

	// 그림자 업데이트.
	if (m_isRenderShadow)
	{
		// 그림자맵 생성.
		if (!m_shadow.IsLoaded())
			m_shadow.Create(256, 256);

		m_shadow.UpdateShadow(*this);
	}
}


// 그림자 출력.
void cModel::RenderShadow(const Matrix44 &viewProj, 
	const Vector3 &lightPos, const Vector3 &lightDir, const Matrix44 &parentTm)
{
	GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&Matrix44::Identity);

	if (m_isRenderMesh)
	{
		const Matrix44 tm = m_TM * parentTm;
		BOOST_FOREACH (auto node, m_meshes)
			node->RenderShadow(viewProj, lightDir, lightDir, tm);
	}
}


// remove all data
void cModel::Clear()
{
	BOOST_FOREACH (auto mesh, m_meshes)
	{
		SAFE_DELETE(mesh);
	}
	m_meshes.clear();
	m_curAni = NULL;

	SAFE_DELETE(m_bone);
}


// 메쉬를 찾아서 리턴한다.
cMesh* cModel::FindMesh(const string &meshName)
{
	BOOST_FOREACH (auto &mesh, m_meshes)
	{
		if (mesh->GetName() == meshName)
			return (cMesh*)mesh;
	}
	return NULL;
}


// 행렬파레트를 동기화 한다.
bool cModel::SharePalette(vector<Matrix44> *palette)
{
	RETV(m_type != MODEL_TYPE::SKIN, false);

	BOOST_FOREACH (auto &mesh, m_meshes)
	{
		if (cSkinnedMesh *skinMesh = dynamic_cast<cSkinnedMesh*>(mesh))
		{
			skinMesh->SetPalette(palette);
		}
	}

	return true;
}


void cModel::SetRenderMesh(const bool isRenderMesh) 
{ 
	m_isRenderMesh = isRenderMesh; 
}

void cModel::SetRenderBone(const bool isRenderBone) 
{ 
	m_isRenderBone = isRenderBone; 
}

void cModel::SetRenderBoundingBox(const bool isRenderBoundingBox)
{
	m_isRenderBoundingBox = isRenderBoundingBox;
}


int cModel::GetCollisionId() 
{
	return GetId();
}

bool cModel::IsTest( int testNum )
{
	return true;
}


void cModel::UpdateCollisionBox()
{
	m_boundingBox.SetTransform(m_TM);
}


cBoundingBox* cModel::GetCollisionBox()
{
	sMinMax mm;
	BOOST_FOREACH (auto &mesh, m_meshes)
	{
		if (const cBoundingBox *box = mesh->GetBoundingBox())
		{
			mm.Update(box->m_min);
			mm.Update(box->m_max);
		}
	}

	m_boundingBox.SetBoundingBox(mm._min, mm._max);
	if (m_boundingBox.Length() >= FLT_MAX)
		m_boundingBox.SetBoundingBox(Vector3(0,0,0), Vector3(0,0,0));

	return &m_boundingBox;
}


void cModel::Collision( int testNum, ICollisionable *obj )
{

}


// 모델을 복사해서 리턴한다.
cModel* cModel::Clone() const
{
	cModel *clone = new cModel(GenerateId());
	clone->Create(m_fileName, m_type);

	clone->SetTM(m_TM);
	clone->SetShader(m_shader);
	clone->SetRenderMesh(m_isRenderMesh);
	clone->SetRenderBone(m_isRenderBone);
	clone->SetRenderBoundingBox(m_isRenderBoundingBox);

	return clone;
}


// 스크린 좌표 x,y 로 모델이 선택이 되었는지 판단한다. 피킹되었다면 true를 리턴한다.
bool cModel::Pick(const Vector3 &orig, const Vector3 &dir)
{
	m_boundingBox.SetTransform(m_TM);
	return m_boundingBox.Pick(orig, dir);
}


cMesh* cModel::GetMesh(const string &name)
{
	BOOST_FOREACH (auto &mesh, m_meshes)
	{
		if (mesh->GetName() == name)
			return mesh;
	}
	return NULL;	
}


cMesh* cModel::GetMesh(const int idx)
{
	if ((int)m_meshes.size() > idx)
		return m_meshes[ idx];
	return NULL;
}

