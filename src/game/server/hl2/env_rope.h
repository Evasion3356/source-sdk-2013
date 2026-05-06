#ifndef ENV_ROPE_H
#define ENV_ROPE_H
#pragma once

#include "baseentity.h"

#define ROPE_MAX_NODES          16
#define ROPE_CONSTRAINT_ITERS   10

class CEnvRope : public CBaseEntity
{
public:
	DECLARE_CLASS( CEnvRope, CBaseEntity );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	void Spawn() override;
	void Think() override;
	int  UpdateTransmitState() override;

	// Player attachment interface
	int    GetNodeCount()      const { return m_nActiveNodes; }
	float  GetSegmentLength()  const { return m_flSegmentLength; }
	float  GetAttachRadius()   const { return m_flAttachRadius; }
	Vector GetNodePos( int i ) const { return m_vecNodes[i]; }
	Vector GetNodeVelocity( int i ) const { return ( m_vecNodes[i] - m_vecPrevNodes[i] ) / TICK_INTERVAL; }
	static constexpr float MAX_SWING_SPEED = 8.0f; // ~530 u/s at 66 tick

	void   ApplyNodeImpulse( int i, const Vector &impulse )
	{
		if ( i <= 0 || i >= m_nActiveNodes )
			return;
		// Modify prev (not pos) so velocity builds over multiple Verlet steps.
		Vector vel = m_vecNodes[i] - m_vecPrevNodes[i];
		vel += impulse;
		if ( vel.Length() > MAX_SWING_SPEED )
			vel = vel.Normalized() * MAX_SWING_SPEED;
		m_vecPrevNodes[i] = m_vecNodes[i] - vel;
	}
	void   ZeroNodeVelocity( int i )
	{
		if ( i < 0 || i >= m_nActiveNodes ) return;
		m_vecPrevNodes[i] = m_vecNodes[i];
	}
	// Snap a node to a new position, stripping the velocity component going into
	// the surface (wallNormal) while preserving tangential velocity.
	void   SnapNodeTo( int i, const Vector &pos, const Vector &wallNormal )
	{
		if ( i <= 0 || i >= m_nActiveNodes )
			return;
		Vector vel = m_vecNodes[i] - m_vecPrevNodes[i];
		// Remove only the component driving the node into the surface
		float dot = vel.Dot( wallNormal );
		if ( dot < 0.0f )
			vel -= wallNormal * dot;
		m_vecNodes.Set( i, pos );
		m_vecPrevNodes[i] = pos - vel;
	}

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
