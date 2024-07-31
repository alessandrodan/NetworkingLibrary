#pragma once

#include "Socket.h"
#include "SocketWatcher.h"

namespace Net
{
    class SocketWatcher;

    class CAbstractPeer
    {
        public:
            CAbstractPeer() = default;
            virtual ~CAbstractPeer() = 0;

            virtual CSocket* GetSocket() = 0;

            void AddSocketToWatcher(int fd, CAbstractPeer* client_data);
            void RemoveSocketToWatcher(int fd);

        protected:
            std::shared_ptr<SocketWatcher>	m_serverWatcher;
    };

    inline CAbstractPeer::~CAbstractPeer() {}
}
