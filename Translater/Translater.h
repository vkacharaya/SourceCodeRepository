#pragma once
/////////////////////////////////////////////////////////////////////////
// Translater.h - Translates messages to/from managed and native types //
// ver 1.0                                                             //
// Source:Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018    //
// Author: Vishnu Karthik Ravindran, Spring 2018					   //
/////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*  This C++\Cli Package contains one class, Translater.  It's purpose is to convert
*  managed messages, CsMessage, to native messages, Message, and back.
*
*  Required Files:
* -----------------
*  Translater.h, Translater.cpp
*  CsMessage.h
*  Required References:
* ----------------------
*  CommLibWrapper.dll
*  Message.lib
*
*
*  Maintenance History:
* ----------------------
*  ver 1.0 : 3/27/2018
*  - first release
*  ver 1.1 : 4/10/2018 - Removed comments
*/

// Project > Properties > C/C++ > Language > Conformance mode set to No
// Add references to System, System.Core

#include <string>
#include "../CppCommWithFileXfer/MsgPassingComm/IComm.h"
#include "CsMessage.h"
#include "../CommLibWrapper/CommLibWrapper.h"

using namespace System;
using namespace System::Text;

namespace MsgPassingCommunication
{
	public ref class Translater
	{
	public:
		Translater();
		void listen(CsEndPoint^ ep);
		void disconnect();
		void connect();
		void postMessage(CsMessage^ msg);
		CsMessage^ getMessage();
		CsMessage^ fromMessage(Message& msg);
		Message fromCsMessage(CsMessage^ csMsg);
		void setPathContext(String^ sendFp, String^ receiveFP);
	private:
		CsEndPoint ^ ep_;
		MsgPassingCommunication::CommFactory* pFactory;
		MsgPassingCommunication::IComm* pComm;
		bool disconnectFg = false;
	};
	//----< initialize endpoint >----------------------------------------

	Translater::Translater()
	{
		ep_ = gcnew CsEndPoint;
	}
	//----< set client listen endpoint and start Comm >------------------

	void Translater::listen(CsEndPoint^ ep)
	{
		if (pComm != nullptr)
			pComm->stop();
		ep_->machineAddress = ep->machineAddress;
		ep_->port = ep->port;
		pFactory = new CommFactory;
		pComm = pFactory->create(Sutils::MtoNstr(ep_->machineAddress), ep_->port);
		pComm->start();
		delete pFactory;
	}

	////////////////////////////////////////////////////////////////////
    // Sets the save file path and send file path
	void Translater::setPathContext(String^ sendFp, String^ receiveFP)
	{
		pComm->setSendFilePath(Sutils::MtoNstr(sendFp));
		pComm->setSaveFilePath(Sutils::MtoNstr(receiveFP));
	}

	//----< Used to set disconnect flag to block the connection >------------------

	void Translater::disconnect() { disconnectFg = true; }

	//----< Used to reset disconnect flag to block the connection  >------------------

	void Translater::connect() { disconnectFg = false; }

	//----< convert native message to managed message >------------------

	CsMessage^ Translater::fromMessage(Message& msg)
	{
		CsMessage^ csMsg = gcnew CsMessage;
		Message::Attributes attribs = msg.attributes();
		for (auto attrib : attribs)
		{
			csMsg->add(Sutils::NtoMstr(attrib.first), Sutils::NtoMstr(attrib.second));
		}
		return csMsg;
	}
	//----< convert managed message to native message >------------------

	Message Translater::fromCsMessage(CsMessage^ csMsg)
	{
		Message msg;
		auto enumer = csMsg->attributes->GetEnumerator();
		while (enumer.MoveNext())
		{
			String^ key = enumer.Current.Key;
			String^ value = enumer.Current.Value;
			msg.attribute(Sutils::MtoNstr(key), Sutils::MtoNstr(value));
		}
		return msg;
	}
	//----< use Comm to post message >-----------------------------------

	inline void Translater::postMessage(CsMessage^ csMsg)
	{
		if (!disconnectFg)
		{
			Message msg = this->fromCsMessage(csMsg);
			pComm->postMessage(msg);
		}
	}
	//----< get message from Comm >--------------------------------------

	inline CsMessage^ Translater::getMessage()
	{
		Message msg = pComm->getMessage();
		return fromMessage(msg);
	}
}