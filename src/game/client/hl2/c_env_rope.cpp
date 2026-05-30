#include "cbase.h"
#include "c_env_rope.h"
#include "view.h"
#include "model_types.h"
#include "datacache/imdlcache.h"
#include "tier3/tier3.h"
#include "c_baseanimating.h"

// memdbgon must be the last include file in a .cpp file
#include "tier0/memdbgon.h"

// The model's geometry runs along its local +Y axis and spans ~10 Source units.
static const float ROPE_MODEL_LENGTH = 10.0f;

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
		// Force GPU hardware data upload that DrawModelSetup requires.
		// C_BaseEntity never calls into the C_BaseAnimating OnNewModel path,
		// so without this GetHardwareData returns NULL and DrawModelSetup fails.
		const model_t *pModel = GetModel();
		if ( pModel )
		{
			MDLHandle_t hMDL = modelinfo->GetCacheHandle( pModel );
			if ( hMDL != MDLHANDLE_INVALID )
				g_pMDLCache->GetHardwareData( hMDL );
		}

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

	const model_t *pModel = GetModel();
	if ( !pModel )
		return 0;

	float white[3] = { 1.0f, 1.0f, 1.0f };
	render->SetColorModulation( white );
	render->SetBlend( 1.0f );

	// All bones share the same per-segment transform, bypassing SetupBones
	// (which would return the entity's anchor origin for every segment).
	matrix3x4_t boneToWorld[MAXSTUDIOBONES];

	for ( int i = 0; i < nSegs; i++ )
	{
		Vector posA   = m_vecNodes[i];
		Vector posB   = m_vecNodes[i + 1];
		Vector seg    = posB - posA;
		float  segLen = seg.Length();
		if ( segLen < 0.001f )
			continue;

		// Align model's local +Y axis to the segment direction and scale to fit.
		Vector along = seg / segLen;
		Vector ref   = ( fabsf( along.z ) < 0.9f ) ? Vector( 0, 0, 1 ) : Vector( 1, 0, 0 );
		Vector worldX = CrossProduct( ref, along );
		VectorNormalize( worldX );
		Vector worldZ = CrossProduct( worldX, along );
		VectorNormalize( worldZ );

		float s = segLen / ROPE_MODEL_LENGTH;
		matrix3x4_t mat;
		mat[0][0] = worldX.x;  mat[0][1] = along.x * s;  mat[0][2] = worldZ.x;  mat[0][3] = posA.x;
		mat[1][0] = worldX.y;  mat[1][1] = along.y * s;  mat[1][2] = worldZ.y;  mat[1][3] = posA.y;
		mat[2][0] = worldX.z;  mat[2][1] = along.z * s;  mat[2][2] = worldZ.z;  mat[2][3] = posA.z;

		for ( int b = 0; b < MAXSTUDIOBONES; b++ )
			MatrixCopy( mat, boneToWorld[b] );

		QAngle angles;
		MatrixAngles( mat, angles );

		ClientModelRenderInfo_t info;
		info.flags           = STUDIO_RENDER;
		info.pRenderable     = this;
		info.instance        = MODEL_INSTANCE_INVALID;
		info.entity_index    = entindex();
		info.pModel          = pModel;
		info.origin          = posA;
		info.angles          = angles;
		info.skin            = 0;
		info.body            = 0;
		info.hitboxset       = 0;
		info.pLightingOffset = NULL;
		info.pLightingOrigin = NULL;
		info.modelToWorld    = mat;
		info.pModelToWorld   = &info.modelToWorld;

		DrawModelState_t state;
		matrix3x4_t *pBoneOut;
		if ( modelrender->DrawModelSetup( info, &state, boneToWorld, &pBoneOut ) )
			modelrender->DrawModelExecute( state, info, boneToWorld );
	}

	return 1;
}
