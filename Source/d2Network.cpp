/**************************************************************************************\
** File: d2Network.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for  
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Network.h"
#include "d2StringManip.h"
#include "d2Utility.h"

namespace d2d
{
	//+--------------------------------\--------------------------------------
	//|		   Network Utility   	   |
	//\--------------------------------/--------------------------------------
	namespace
	{
		void GetIPOctets(const IPaddress& ip, std::array<unsigned, 4>& octetsOut)
		{
			Uint32 clientIPInteger;
			clientIPInteger = SDL_SwapBE32(ip.host);
			octetsOut[0] = (clientIPInteger >> 24);
			octetsOut[1] = ((clientIPInteger >> 16) & 0xff);
			octetsOut[2] = ((clientIPInteger >> 8) & 0xff);
			octetsOut[3] = (clientIPInteger & 0xff);
		}
	}
	std::string GetIPOctetsString(const IPaddress& ip)
	{
		std::array<unsigned, 4> octets;
		GetIPOctets(ip, octets);
		return d2d::ToString(octets[0]) + '.'
			+ d2d::ToString(octets[1]) + '.'
			+ d2d::ToString(octets[2]) + '.'
			+ d2d::ToString(octets[3]);
	}
	unsigned GetPort(const IPaddress& ip)
	{
		return SDL_SwapBE16(ip.port);
	}
	//+--------------------------------\--------------------------------------
	//|		   NetworkMessage   	   |
	//\--------------------------------/--------------------------------------
	NetworkMessage::NetworkMessage() 
	{
		Reset();
	}
	void NetworkMessage::Reset() 
	{
		for(int i = 0; i < 256; ++i)
			m_buffer[i] = 0;
		m_bufferState = BufferState::EMPTY;
	}
	void NetworkMessage::Finish() 
	{
		if(m_bufferState == BufferState::READING)
			m_bufferState = BufferState::FULL;
	}
	int NetworkMessage::NumToLoad() 
	{
		if(m_bufferState == BufferState::EMPTY)
			return 255;
		else
			return 0;
	}
	int NetworkMessage::NumToUnload() 
	{
		if(m_bufferState == BufferState::FULL)
			return strlen(m_buffer) + 1;
		else
			return 0;
	}
	void NetworkMessage::LoadBytes(ByteBuffer& inBuffer, int numBytes)
	{
		for(int i = 0; i < numBytes; ++i)
			m_buffer[i] = inBuffer[i];
		m_bufferState = BufferState::READING;
	}
	void NetworkMessage::UnloadBytes(ByteBuffer& outBuffer)
	{
		for(int i = 0; i < this->NumToUnload(); ++i)
			outBuffer[i] = m_buffer[i];
		Reset();
	}

	//+--------------------------------\--------------------------------------
	//|			  IpAddress			   |
	//\--------------------------------/--------------------------------------
	IpAddress::IpAddress()
	{
		m_ip.host = 0;
		m_ip.port = 0;
	}
	IpAddress::IpAddress(Uint16 port)
	{
		if(SDLNet_ResolveHost(&m_ip, NULL, port) < 0)
		{
			m_ip.host = 0;
			m_ip.port = 0;
			throw NetworkException{ std::string{"SDLNet_ResolveHost: "} + SDLNet_GetError() };
		}
	}
	IpAddress::IpAddress(const std::string& host, Uint16 port) 
	{
		if(SDLNet_ResolveHost(&m_ip, host.c_str(), port) < 0) 
		{
			m_ip.host = 0;
			m_ip.port = 0;
			throw NetworkException{ std::string{"SDLNet_ResolveHost: "} + SDLNet_GetError() };
		}
	}
	void IpAddress::Set(IPaddress sdl_IPaddress)
	{
		m_ip = sdl_IPaddress;
	}
	bool IpAddress::Valid() const
	{
		return !(m_ip.port == 0);
	}
	IPaddress IpAddress::GetSDL_IPaddress() const
	{
		return m_ip;
	}
	Uint32 IpAddress::GetHost() const 
	{
		return m_ip.host;
	}
	Uint16 IpAddress::GetPort() const 
	{
		return m_ip.port;
	}

	//+--------------------------------\--------------------------------------
	//|			  TcpSocket			   |
	//\--------------------------------/--------------------------------------
	TcpSocket::TcpSocket() 
		: m_socket{ nullptr },
		  m_socketSet{ SDLNet_AllocSocketSet(1) }
	{ 
		if(!m_socketSet)
			throw NetworkException{ std::string{"SDLNet_AllocSocketSet: "} + SDLNet_GetError() };
	}
	TcpSocket::~TcpSocket() 
	{
		if(m_socket) 
		{
			if(m_socketSet)
				SDLNet_TCP_DelSocket(m_socketSet, m_socket);
			SDLNet_TCP_Close(m_socket);
			m_socket = nullptr;
		}
		if(m_socketSet)
		{
			SDLNet_FreeSocketSet(m_socketSet);
			m_socketSet = nullptr;
		}
	}
	void TcpSocket::Set(TCPsocket sdl_TCPsocket)
	{
		if(m_socket) 
		{
			SDLNet_TCP_DelSocket(m_socketSet, m_socket);
			SDLNet_TCP_Close(m_socket);
			m_socket = nullptr;
		}
		m_socket = sdl_TCPsocket;
		if(SDLNet_TCP_AddSocket(m_socketSet, m_socket) == -1)
		{
			m_socket = nullptr;
			throw NetworkException{ std::string{"SDLNet_TCP_AddSocket: "} + SDLNet_GetError() };
		}
	}
	bool TcpSocket::Valid() const
	{
		return (m_socket != nullptr);
	}
	bool TcpSocket::Ready() const 
	{
		bool ready{ false };
		int numready{ SDLNet_CheckSockets(m_socketSet, 0) };
		if(numready == -1)
			throw NetworkException{ std::string{"SDLNet_CheckSockets: "} + SDLNet_GetError() };
		else
			if(numready)
				ready = SDLNet_SocketReady(m_socket);
		return ready;
	}
	void TcpSocket::OnReady() 
	{ }

	//+--------------------------------\--------------------------------------
	//|			ServerTcpSocket		   |
	//\--------------------------------/--------------------------------------
	ServerTcpSocket::ServerTcpSocket(const IpAddress& ip)
	{
		IPaddress SDL_IPaddress = ip.GetSDL_IPaddress();
		m_socket = SDLNet_TCP_Open(&SDL_IPaddress);
		if(!m_socket)
		{
			SDLNet_FreeSocketSet(m_socketSet);
			m_socketSet = nullptr;
			throw NetworkException{ std::string{"SDLNet_TCP_Open: "} + SDLNet_GetError() };
		}
	}
	ServerTcpSocket::ServerTcpSocket(Uint16 port) 
	{
		IpAddress iplistener(port);
		if(!iplistener.Valid()) 
			m_socket = nullptr;
		else 
		{
			IPaddress SDL_IPaddress = iplistener.GetSDL_IPaddress();
			m_socket = SDLNet_TCP_Open(&SDL_IPaddress);
			if(!m_socket) 
			{
				SDLNet_FreeSocketSet(m_socketSet);
				m_socketSet = nullptr;
				throw NetworkException{ std::string{"SDLNet_TCP_Open: "} +SDLNet_GetError() };
			}
		}
	}
	bool ServerTcpSocket::Accept(TcpSocket& clientTcpSocket)
	{
		TCPsocket clientSdl_TCPsocket;
		clientSdl_TCPsocket = SDLNet_TCP_Accept(m_socket);
		if(clientSdl_TCPsocket)
		{
			clientTcpSocket.Set(clientSdl_TCPsocket);
			return true;
		}
		else
			return false;
	}
	void ServerTcpSocket::OnReady() 
	{ }

	//+--------------------------------\--------------------------------------
	//|		   ClientTcpSocket		   |
	//\--------------------------------/--------------------------------------
	ClientTcpSocket::ClientTcpSocket() 
	{
	}
	ClientTcpSocket::ClientTcpSocket(const std::string& host, Uint16 port) 
	{
		IpAddress remoteIp(host.c_str(), port);
		if(!remoteIp.Valid())
			m_socket = nullptr;
		else 
		{
			TcpSocket();
			ConnectToRemoteHost(remoteIp);
		}
	}
	void ClientTcpSocket::ConnectToRemoteHost(const IpAddress& remoteIp)
	{
		TCPsocket clientSdl_TCPsocket;
		IPaddress SDL_IPaddress = remoteIp.GetSDL_IPaddress();
		clientSdl_TCPsocket = SDLNet_TCP_Open(&SDL_IPaddress);
		if(clientSdl_TCPsocket)
			Set(clientSdl_TCPsocket);
		else
			throw NetworkException{ std::string{"SDLNet_TCP_Open: "} + SDLNet_GetError() };
	}
	void ClientTcpSocket::ConnectToRemoteClient(ServerTcpSocket& listenerTcpSocket)
	{ }
	void ClientTcpSocket::Set(TCPsocket sdl_TCPsocket)
	{
		TcpSocket::Set(sdl_TCPsocket);
		IPaddress* sdlIPaddressPtr{ SDLNet_TCP_GetPeerAddress(m_socket) };
		if(sdlIPaddressPtr)
		{
			/* Print the address, converting it onto the host format */
			m_remoteIp.Set(*sdlIPaddressPtr);
			//Uint32 hbo = m_RemoteIp.GetHost();
			//Uint16 pbo = m_RemoteIp.GetPort();
			//std::cout << "Client connected: " << SDLNet_Read32(&hbo) << ' '
			//	<< SDLNet_Read16(&pbo) << std::endl;
			d2LogInfo << "Client connected: " << GetIPOctetsString(*sdlIPaddressPtr)
				<< " port " << sdlIPaddressPtr->port;
		}
		else
			throw NetworkException{ std::string{"SDLNet_TCP_GetPeerAddress: "} + SDLNet_GetError() };
	}
	IpAddress ClientTcpSocket::GetRemoteIpAddress() const
	{
		return m_remoteIp;
	}
	bool ClientTcpSocket::Receive(NetworkMessage& inData)
	{
		// Firstly, check if there is a socket
		if(!m_socket)
			return false;

		ByteBuffer buffer;

		// Check if the instance can receive bytes, if it can, load the number of bytes specified by NumToLoad() virtual function
		while(inData.NumToLoad() > 0)
		{
			if(SDLNet_TCP_Recv(m_socket, buffer, inData.NumToLoad()) > 0)
				inData.LoadBytes(buffer, inData.NumToLoad());
			else 
				return false;
		}
		inData.Finish();
		return true;
	}
	bool ClientTcpSocket::Send(NetworkMessage& outData)
	{
		// Check if there is a socket
		if(!m_socket)
			return false;

		ByteBuffer buffer;
		int length;

		// Check if the instance can send bytes, if it can, unload the number of bytes specified by NumToUnload() virtual function
		while((length = outData.NumToUnload()) > 0)
		{
			outData.UnloadBytes(buffer);
			if(SDLNet_TCP_Send(m_socket, (void*)buffer, length) < length)
				throw NetworkException{ std::string{"SDLNet_TCP_Send: "} + SDLNet_GetError() };
		}
		return true;
	}
	void ClientTcpSocket::OnReady()
	{ }
}