#include "StdAfx.h"
#include "Peer.h"
#include <Network/Utils.hpp>

CPeer::CPeer(std::shared_ptr<Net::SocketWatcher> serverWatcher) :
	Net::CAbstractPeer(serverWatcher)
{
	m_packetHandlerServerHandshake = std::make_unique<ServerHandshake>();
	RegisterPhaseHandler(PHASE_HANDSHAKE, m_packetHandlerServerHandshake.get());
}

void CPeer::OnSetupCompleted()
{
	SetPhase(PHASE_HANDSHAKE);
	StartHandshake();
}

void CPeer::StartHandshake()
{
	SendHandshake(Net::Utils::GetTime(), 0);
}

void CPeer::SendHandshake(uint32_t curTime, long delta)
{
	Net::TPacketGCHandshake pack;

	pack.handshake = m_dwHandshake;
	pack.time = curTime;
	pack.delta = delta;

	Packet(&pack, sizeof(Net::TPacketGCHandshake));
}

bool CPeer::HandshakeProcess(uint32_t time, long delta)
{
	// Simulation of the handshake process. not concerned now
	return true;
}
