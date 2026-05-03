#ifndef ENV_ROPE_H
#define ENV_ROPE_H
#pragma once

#include "baseentity.h"

#define ROPE_MAX_NODES 16

class CEnvRope : public CBaseEntity
{
public:
	DECLARE_CLASS( CEnvRope, CBaseEntity );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	void Spawn() override;
	void Think() override;
	int  UpdateTransmitState() override;

private:
	void InitNodes();
	void StepVerlet();
	void SolveConstraints();

	// Keyfields (server-only)
	int		m_nSubdivisions;
	float	m_flAttachRadius;
	string_t m_iszRopeMaterial;

	// Networked
	CNetworkVar( float,  m_flRopeLength );
	CNetworkVar( float,  m_flRopeWidth );
	CNetworkVar( int,    m_nActiveNodes );
	CNetworkArray( Vector, m_vecNodes, ROPE_MAX_NODES );

	// Server-only physics state
	Vector	m_vecPrevNodes[ROPE_MAX_NODES];
	float	m_flSegmentLength;
};

#endif // ENV_ROPE_H
