#pragma once

#include "AbstractEntity.h"
#include "AbstractClientPacketManager.h"

namespace Net
{
	class CAbstractClient : public AbstractEntity
	{
		public:
			CAbstractClient();
			~CAbstractClient() = default;

			bool Initialize(const char* c_szAddr, int port) override;
			void Process() override;
			bool IsConnected();

			virtual void OnSocketCreated() = 0;
			virtual void OnConnect() = 0;
			virtual void OnConnectFail() = 0;
			virtual void OnDisconnect() = 0;
			virtual bool Analyze(TPacketHeader header, std::shared_ptr<CSocket> socket) = 0;

		protected:
			std::shared_ptr<Net::CSocket> connectSocket;
			bool isConnected;
			time_t	m_connectLimitTime;
			std::shared_ptr<CAbstractClientPacketManager> m_packetManager;
	};
}
