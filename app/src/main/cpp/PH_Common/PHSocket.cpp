#ifdef MULTIPLAYER

#include "StdAfx.h"
#include "PHSocket.h"

PHSocketHandler::PHSocketHandler(StdLog *p): SocketHandler(p) {}
PHSocketHandler::~PHSocketHandler() {}

void PHSocket::SendBuf(const char *buf,size_t len)
{
  m_logic->OnUpload(len);
  TcpSocket::SendBuf(buf, len);
}

void PHSocket::Send(const iStringA& str)
{	
	if(ErasedByHandler())
		return;
	m_logic->OnUpload(str.Size());
	TcpSocket::Send(str.CStr());	
	//delete [] buf;
}

void PHSocket::OnConnect()
{	
	m_logic->OnConnect();
}

void PHSocket::OnDisconnect()
{
	m_logic->OnDisconnect();
}

void PHSocket::OnConnectTimeout()
{
	m_logic->OnConnectTimeout();
}

void PHSocket::OnConnectFailed()
{
	m_logic->OnConnectFailed();
}

void PHSocket::OnLine(const std::string& str) 
{	  
  m_logic->OnDownload(str.size());
  m_logic->OnLine(str.c_str());	
}

void PHSocket::OnRawData(const char *buf, size_t n)
{  
  m_logic->OnDownload(n);
  m_logic->OnRawData(buf, n);
}

bool PHSocket::IsBlocking() { return false; }



#endif //MULTIPLAYER