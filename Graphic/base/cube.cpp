
#include "stdafx.h"
#include "cube.h"

using namespace graphic;


cCube::cCube()
{
}

cCube::cCube(const Vector3 &vMin, const Vector3 &vMax )
{
	SetCube(Vector3(-1,-1,-1), Vector3(1,1,1));
}


void cCube::InitCube()
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return;

	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	Vector3 vertices[8] = {
		Vector3(-1,1,-1), Vector3(1,1,-1), Vector3(-1,-1,-1), Vector3(1,-1,-1),
		Vector3(-1,1, 1), Vector3(1,1, 1), Vector3(-1,-1,1), Vector3(1,-1,1),
	};
	WORD indices[36] = {
		// front
		0, 3, 2,
		0 ,1, 3,
		// back
		5, 6, 7,
		5, 4, 6,
		// top
		4, 1, 0,
		4, 5, 1,
		// bottom
		2, 7, 6,
		2, 3, 7,
		// left
		4, 2, 6,
		4, 0, 2,
		// right
		1, 7, 3,
		1, 5, 7,
	};

	m_vtxBuff.Create(8, sizeof(sVertexDiffuse), sVertexDiffuse::FVF);
	m_idxBuff.Create(12);

	sVertexDiffuse *vbuff = (sVertexDiffuse*)m_vtxBuff.Lock();
	WORD *ibuff = (WORD*)m_idxBuff.Lock();

	for (int i=0; i < 8; ++i)
		vbuff[ i].p = vertices[ i];

	for (int i=0; i < 36; ++i)
		ibuff[ i] = indices[ i];

	m_vtxBuff.Unlock();
	m_idxBuff.Unlock();
}


void cCube::SetCube(const Vector3 &vMin, const Vector3 &vMax )
{
	if (m_vtxBuff.GetVertexCount() <= 0)
		InitCube();

	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	// min = index 2
	// max = index 5

	Vector3 vertices[8] = {
		Vector3(vMin.x, vMax.y, vMin.z), 
		Vector3(vMax.x, vMax.y, vMin.z), 
		Vector3(vMin.x, vMin.y, vMin.z), 
		Vector3(vMax.x, vMin.y, vMin.z),
		Vector3(vMin.x, vMax.y, vMax.z), 
		Vector3(vMax.x, vMax.y, vMax.z),
		Vector3(vMin.x, vMin.y, vMax.z), 
		Vector3(vMax.x, vMin.y, vMax.z),
	};

	sVertexDiffuse *vbuff = (sVertexDiffuse*)m_vtxBuff.Lock();
	for (int i=0; i < 8; ++i)
		vbuff[ i].p = vertices[ i];
	m_vtxBuff.Unlock();
}


void cCube::Render(const Matrix44 &tm)
{
	DWORD cullMode;
	DWORD fillMode;
	graphic::GetDevice()->GetRenderState(D3DRS_CULLMODE, &cullMode);
	graphic::GetDevice()->GetRenderState(D3DRS_FILLMODE, &fillMode);
	graphic::GetDevice()->SetRenderState(D3DRS_CULLMODE, FALSE);
	graphic::GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	
	Matrix44 mat = m_tm * tm;
	GetDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&mat );
	m_vtxBuff.Bind();
	m_idxBuff.Bind();
	GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 
		m_vtxBuff.GetVertexCount(), 0, 12);

	graphic::GetDevice()->SetRenderState(D3DRS_CULLMODE, cullMode);
	graphic::GetDevice()->SetRenderState(D3DRS_FILLMODE, fillMode );
}
