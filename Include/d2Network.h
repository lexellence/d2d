/**************************************************************************************\
** File: d2Network.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for
**
\**************************************************************************************/
#pragma once
namespace d2d
{
	//+--------------------------------\--------------------------------------
	//|		   Network Utility   	   |
	//\--------------------------------/--------------------------------------
	std::string GetIPOctetsString(const IPaddress& ip);
	unsigned GetPort(const IPaddress& ip);

	//+--------------------------------\--------------------------------------
	//|		   NetworkMessage   	   |
	//\--------------------------------/--------------------------------------
	const int bufferBytes{ 256 };
	typedef char ByteBuffer[bufferBytes];
	class NetworkMessage
	{
	public:
		NetworkMessage();
		virtual int NumToLoad();
		virtual int NumToUnload();

		void LoadBytes(ByteBuffer& inBuffer, int numBytes);
		void UnloadBytes(ByteBuffer& outBuffer);

		//set the state object to full. No more data to be loaded
		void Finish();

	protected:
		ByteBuffer m_buffer;
		enum class BufferState
		{
			EMPTY,
			READING,
			WRITING,
			FULL
		} m_bufferState;

		// reset message: fulfill it with zeroes and change its state to EMPTY
		void Reset();
	};

	//+--------------------------------\--------------------------------------
	//|			  IpAddress			   |
	//\--------------------------------/--------------------------------------
	class IpAddress
	{
	public:
		IpAddress();
		IpAddress(Uint16 port);
		IpAddress(const std::string& host, Uint16 port);
		void Set(IPaddress sdl_IPaddress);
		bool Valid() const;
		IPaddress GetSDL_IPaddress() const;
		Uint32 GetHost() const;
		Uint16 GetPort() const;

	private:
		IPaddress m_ip;
	};

	//+--------------------------------\--------------------------------------
	//|			  TcpSocket			   |
	//\--------------------------------/--------------------------------------
	class TcpSocket
	{
	public:
		TcpSocket();
		virtual ~TcpSocket();
		virtual void Set(TCPsocket sdl_TCPsocket);
		bool Valid() const;
		bool Ready() const;
		virtual void OnReady(); // Pure virtual
	protected:
		TCPsocket m_socket;
		SDLNet_SocketSet m_socketSet;
	};

	//+--------------------------------\--------------------------------------
	//|			ServerTcpSocket		   |
	//\--------------------------------/--------------------------------------
	class ClientTcpSocket;
	class ServerTcpSocket : public TcpSocket
	{
	public:
		//create and open a new socket, with an existing IpAddress object
		ServerTcpSocket(const IpAddress& ip);

		//create and open a new socket with the desired port
		ServerTcpSocket(Uint16 port);

		//set a client TcpSocket object after listening to the port
		bool Accept(TcpSocket& clientTcpSocket);

		virtual void OnReady(); // Pure virtual
	};

	//+--------------------------------\--------------------------------------
	//|		   ClientTcpSocket		   |
	//\--------------------------------/--------------------------------------
	class ClientTcpSocket : public TcpSocket
	{
	public:
		ClientTcpSocket();

		// Create the object and connect to a host, in a given port
		ClientTcpSocket(const std::string& server, Uint16 port);

		// Make a connection to communicate with a remote host
		void ConnectToRemoteHost(const IpAddress& remoteIp);

		// Make a connection to communicate with a client
		void ConnectToRemoteClient(ServerTcpSocket& listenerTcpSocket);

		void Set(TCPsocket sdl_TCPsocket);
		IpAddress GetRemoteIpAddress() const;

		virtual void OnReady(); // Pure virtual

		bool Receive(NetworkMessage& inData);
		bool Send(NetworkMessage& outData);
	private:
		IpAddress m_remoteIp;
	};
}
