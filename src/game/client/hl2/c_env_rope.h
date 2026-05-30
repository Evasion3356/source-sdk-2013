#ifndef C_ENV_ROPE_H
#define C_ENV_ROPE_H
#pragma once

#include "c_baseentity.h"

#define ROPE_MAX_NODES 16

class C_EnvRope : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_EnvRope, C_BaseEntity );
	DECLARE_CLIENTCLASS();

	C_EnvRope();

	bool ShouldDraw() override;
	int  DrawModel( int flags ) override;
	void GetRenderBounds( Vector &mins, Vector &maxs ) override;
	void OnDataChanged( DataUpdateType_t updateType ) override;

private:
	Vector  m_vecNodes[ROPE_MAX_NODES];
	int     m_nActiveNodes;
	float   m_flRopeLength;
	float   m_flRopeWidth;

};

#endif // C_ENV_ROPE_H
