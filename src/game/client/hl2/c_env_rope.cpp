#include "cbase.h"
#include "c_env_rope.h"
#include "view.h"
#include "model_types.h"

// memdbgon must be the last include file in a .cpp file
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_EnvRope, DT_EnvRope, CEnvRope )
	RecvPropInt(   RECVINFO( m_nActiveNodes ) ),
	RecvPropFloat( RECVINFO( m_flRopeLength ) ),
	RecvPropFloat( RECVINFO( m_flRopeWidth  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[0]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[1]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[2]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[3]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[4]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[5]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[6]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[7]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[8]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[9]  ) ),
	RecvPropVector( RECVINFO( m_vecNodes[10] ) ),
	RecvPropVector( RECVINFO( m_vecNodes[11] ) ),
	RecvPropVector( RECVINFO( m_vecNodes[12] ) ),
	RecvPropVector( RECVINFO( m_vecNodes[13] ) ),
	RecvPropVector( RECVINFO( m_vecNodes[14] ) ),
	RecvPropVector( RECVINFO( m_vecNodes[15] ) ),
END_RECV_TABLE()

C_EnvRope::C_EnvRope()
{
	m_nActiveNodes = 0;
	m_flRopeLength = 128.0f;
	m_flRopeWidth  = 2.0f;
}

void C_EnvRope::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_RopeMaterial.Init( "cable/cable", TEXTURE_GROUP_OTHER );
		AddToLeafSystem( RENDER_GROUP_OPAQUE_ENTITY );
	}
}

bool C_EnvRope::ShouldDraw()
{
	return ( m_nActiveNodes >= 2 ) && !IsEffectActive( EF_NODRAW );
}

void C_EnvRope::GetRenderBounds( Vector &mins, Vector &maxs )
{
	float r = m_flRopeLength;
	mins.Init( -r, -r, -r );
	maxs.Init(  r,  r,  r );
}

int C_EnvRope::DrawModel( int flags )
{
	if ( flags & STUDIO_SHADOWDEPTHTEXTURE )
		return 0;

	int nSegs = m_nActiveNodes - 1;
	if ( nSegs <= 0 )
		return 0;

	IMaterial *pMat = m_RopeMaterial;
	if ( !pMat )
		return 0;

	Vector viewOrigin = CurrentViewOrigin();
	float  halfWidth  = m_flRopeWidth * 0.5f;

	CMatRenderContextPtr pRenderContext( g_pMaterialSystem );
	pRenderContext->Bind( pMat );

	IMesh *pMesh = pRenderContext->GetDynamicMesh();
	CMeshBuilder mb;
	mb.Begin( pMesh, MATERIAL_QUADS, nSegs );

	float totalLength = m_flRopeLength;
	float accumU = 0.0f;

	for ( int i = 0; i < nSegs; i++ )
	{
		const Vector &posA = m_vecNodes[i];
		const Vector &posB = m_vecNodes[i + 1];

		// Billboard the segment perpendicular to the view direction
		Vector seg   = posB - posA;
		float  segLen = seg.Length();
		if ( segLen < 0.001f )
			continue;

		Vector segDir = seg / segLen;
		Vector toView = ( ( posA + posB ) * 0.5f ) - viewOrigin;
		VectorNormalize( toView );

		Vector right = CrossProduct( segDir, toView );
		float rightLen = right.Length();
		if ( rightLen < 0.001f )
			continue;
		right /= rightLen;
		right *= halfWidth;

		float u0 = accumU / totalLength;
		float u1 = ( accumU + segLen ) / totalLength;
		accumU += segLen;

		// Quad: A-left, A-right, B-right, B-left
		mb.Position3fv( ( posA - right ).Base() );
		mb.TexCoord2f( 0, u0, 0.0f );
		mb.Color4ub( 255, 255, 255, 255 );
		mb.AdvanceVertex();

		mb.Position3fv( ( posA + right ).Base() );
		mb.TexCoord2f( 0, u0, 1.0f );
		mb.Color4ub( 255, 255, 255, 255 );
		mb.AdvanceVertex();

		mb.Position3fv( ( posB + right ).Base() );
		mb.TexCoord2f( 0, u1, 1.0f );
		mb.Color4ub( 255, 255, 255, 255 );
		mb.AdvanceVertex();

		mb.Position3fv( ( posB - right ).Base() );
		mb.TexCoord2f( 0, u1, 0.0f );
		mb.Color4ub( 255, 255, 255, 255 );
		mb.AdvanceVertex();
	}

	mb.End();
	pMesh->Draw();
	return 1;
}
