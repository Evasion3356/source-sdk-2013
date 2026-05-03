#ifndef ENV_ROPE_H
#define ENV_ROPE_H
#pragma once

#include "baseentity.h"

class CEnvRope : public CBaseEntity
{
public:
	DECLARE_CLASS( CEnvRope, CBaseEntity );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	void Spawn( void ) override;
	void Think( void ) override;

private:
	float		m_flRopeLength;
	float		m_flRopeWidth;
	int			m_nSubdivisions;
	float		m_flAttachRadius;
	string_t	m_iszRopeMaterial;
};

#endif // ENV_ROPE_H
