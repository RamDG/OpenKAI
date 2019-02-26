/*
 * _HiphenServer.cpp
 *
 *  Created on: Feb 19, 2019
 *      Author: yankai
 */

#include "_HiphenServer.h"

namespace kai
{

_HiphenServer::_HiphenServer()
{
	m_dir = "/home/";
	m_subDir = "";

}

_HiphenServer::~_HiphenServer()
{
}

bool _HiphenServer::init(void* pKiss)
{
	IF_F(!_TCPserver::init(pKiss));
	Kiss* pK = (Kiss*) pKiss;

	KISSm(pK,dir);
	KISSm(pK,subDir);

	if(m_subDir.empty())
		m_subDir = m_dir + tFormat() + "/";
	else
		m_subDir = m_dir + m_subDir;

	string cmd = "mkdir " + m_subDir;
	system(cmd.c_str());

	return true;
}

bool _HiphenServer::start(void)
{
	IF_T(m_bThreadON);

	m_bThreadON = true;
	int retCode = pthread_create(&m_threadID, 0, getUpdateThread, this);
	if (retCode != 0)
	{
		LOG(ERROR)<< retCode;
		m_bThreadON = false;
		return false;
	}

	return true;
}

void _HiphenServer::update(void)
{
	while (m_bThreadON)
	{
		if(!handler())
		{
			this->sleepTime(USEC_1SEC);
		}
	}
}

bool _HiphenServer::handler(void)
{
	IF_F(!setup());

	int socketNew;
	struct sockaddr_in clientAddr;
	int c = sizeof(struct sockaddr_in);

	while ((socketNew = accept(m_socket, (struct sockaddr *) &clientAddr, (socklen_t*) &c)) >= 0)
	{
		if (m_lSocket.size() >= m_nSocket)
		{
			cleanupClient();
			IF_CONT(m_lSocket.size() >= m_nSocket);
		}

		_HiphenIMG* pImgSock = new _HiphenIMG();
		IF_CONT(!pImgSock);
		pImgSock->init(m_pKiss);
		struct sockaddr_in *pAddr = (struct sockaddr_in *) &clientAddr;
		pImgSock->m_strAddr = inet_ntoa(pAddr->sin_addr);
		pImgSock->m_socket = socketNew;
		pImgSock->m_ioStatus = io_opened;
		pImgSock->m_bClient = false;
		pImgSock->m_dir = m_subDir;

		if (!pImgSock->start())
		{
			delete pImgSock;
			continue;
		}

		m_lSocket.push_back(pImgSock);
	}

	close(m_socket);
	return true;
}

void _HiphenServer::cleanupClient(void)
{
	auto itr = m_lSocket.begin();
	while (itr != m_lSocket.end())
	{
		_HiphenIMG* pSocket = (_HiphenIMG*)*itr;
		if(pSocket->bComplete())
		{
			itr = m_lSocket.erase(itr);
			delete pSocket;
		}
		else
		{
			itr++;
		}
	}
}

}
