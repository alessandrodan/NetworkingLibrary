#include "StdAfx.h"
#include <iostream>
#include "AbstractServer.h"

namespace Net
{
	bool CAbstractServer::Initialize(const char* c_szAddr, int port)
	{
		try
		{
			if (!listenSocket.Create())
			{
				std::cerr << "Failed to create socket" << std::endl;
				return false;
			}

			CNetAddress netAddress;
			if (!netAddress.Set(c_szAddr, port))
			{
				std::cerr << "Failed to set address" << std::endl;
				return false;
			}

			unsigned long val = 1;
			ioctlsocket(listenSocket.GetSocket(), FIONBIO, &val);

			if (!listenSocket.Listen(netAddress))
			{
				std::cerr << "Failed to listen socket" << std::endl;
				return false;
			}
		}
		catch (NetException ex)
		{
			std::cerr << "Exception: " << ex.what() << std::endl;
			return false;
		}

		watcher = std::make_unique<SocketWatcher>(4096);
		watcher->add_fd(listenSocket.GetSocket(), NULL, FDW_READ, false);

		OnSocketListening();
		return true;
	}

	void CAbstractServer::Process()
	{
		int num_events = watcher->monitor(0);
		if (num_events < 0)
			return;

		std::shared_ptr<CSocket> d;

		for (int event_idx = 0; event_idx < num_events; ++event_idx)
		{
			d = watcher->get_client_data(event_idx);
			if (!d)
			{
				if (FDW_READ == watcher->get_event_status(listenSocket.GetSocket(), event_idx))
				{
					HandleNewConnection();
					watcher->clear_event(listenSocket.GetSocket(), event_idx);
				}

				continue;
			}

			const auto dataStream = d->GetDataStream();
			if (!dataStream)
				continue;

			int iRet = watcher->get_event_status(d->GetSocket(), event_idx);
			switch (iRet)
			{
				case FDW_READ:
				{
					if (!m_packetManager->ProcessRecv(d))
					{
						OnDisconnectClient(d);
						watcher->remove_fd(d->GetSocket());
					}
				}
				break;

				case FDW_WRITE:
				{
					if (!dataStream->ProcessSend(d->GetSocket()))
					{
						OnDisconnectClient(d);
						watcher->remove_fd(d->GetSocket());
					}
				}
				break;

				case FDW_EOF:
				{
					std::cerr << "SetPhase(PHASE_CLOSE)" << std::endl;
				}
				break;

				default:
					printf("watcher->get_event_status returned unknown %d", iRet);
					break;
			}
		}
	}

	void CAbstractServer::HandleNewConnection()
	{
		auto newClientSocket = std::make_shared<CSocket>();
		if (!listenSocket.Accept(*newClientSocket))
		{
			std::cerr << "Failed to accept new client" << std::endl;
			newClientSocket->Close();
			return;
		}

		watcher->add_fd(newClientSocket->GetSocket(), newClientSocket, FDW_READ, false);
		OnConnectClient(newClientSocket);
	}
}
