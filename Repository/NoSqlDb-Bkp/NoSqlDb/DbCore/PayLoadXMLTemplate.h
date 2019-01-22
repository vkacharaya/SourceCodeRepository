#pragma once
/////////////////////////////////////////////////////////////////////////////////
// PayLoadXMLTemplate.h - Provides the partially specialized template functions //
// needed for converting various objects to XML and then read from the same file//
// ver 1.0																        //
// Author: Vishnu Karthik.R, CSE687 - Object Oriented Design, Spring 2018       //
/////////////////////////////////////////////////////////////////////////////////
/*
/*

* Package Operations:
* -------------------
* This package reads the XML value and stores the value into the object. It also
* writes contents to XML by reading the object. This file contains partially specialized
* member functions needed for the reading the value.

* Required Files:
* ---------------
* XmlDocument.h, XmlDocument.cpp
* XmlElement.h, XmlElement.cpp
* DbCore.h, DbCore.cpp

* Maintenance History:
* --------------------
*
* ver 1.0 : 16 Jan 2018
* - Initial Implementation

*/

#include "PayLoad.h"
#include "../XmlDocument/XmlDocument.h"
#include "../XmlElement/XmlElement.h"

using namespace XmlProcessing;

namespace NoSqlDb
{
	//////////////////////////////////////////////////////////////////////////
	// Contains template member functions for retrieving and loading the value
	// into XML
	//////////////////////////////////////////////////////////////////////////
	class PayLoadXMLTemplate
	{
	public:
		/////////////////////////////////////////////////////////////////////
		// Creates XML by reading the object passed
		template<typename T>
		void createPayLoadXML(std::shared_ptr<XmlProcessing::AbstractXmlElement> &docElem_dbRec, T &elem);

		/////////////////////////////////////////////////////////////////////
		// Loads the xml value into the templated object
		template<typename T>
		void RetrivePayLoadfromXML(std::shared_ptr<XmlProcessing::AbstractXmlElement> &pValueChild, T &elem);
	};

	/////////////////////////////////////////////////////////////////////
	// Loads the xml value into the templated object with specialization
	// on Payload
	template<>
	void PayLoadXMLTemplate::RetrivePayLoadfromXML(std::shared_ptr<AbstractXmlElement> &pValueChild, PayLoad &payLoad)
	{
		using sPtr = std::shared_ptr<AbstractXmlElement>;
		std::vector<sPtr> payloadList = pValueChild->children();
		for (auto payloadElem : payloadList)
		{
			if (payloadElem->tag() == "filepath")
			{
				payLoad.setFilePath(payloadElem->children()[0]->value());
			}
			if (payloadElem->tag() == "status")
			{

				ApplicationConstants::CheckinStatus statusValue = payloadElem->children().size() != 0 ?
					(ApplicationConstants::CheckinStatus)payloadElem->children()[0]->value()[0] :
					ApplicationConstants::CheckinStatus::Open;
				payLoad.setCheckInStatus(statusValue);
			}
			if (payloadElem->tag() == "author")
			{
				payLoad.setUser(payloadElem->children()[0]->value());
			}
			if (payloadElem->tag() == "categoryList")
			{
				std::vector<sPtr> categList = payloadElem->children();
				for (auto categElem : categList)
				{
					if (categElem->tag() == "category")
					{
						payLoad.pushFileCategory(categElem->children()[0]->value());
					}
				}
			}
		}
	}

	/////////////////////////////////////////////////////////////////////
	// Loads the xml value into the templated object
	template<typename T>
	void PayLoadXMLTemplate::RetrivePayLoadfromXML(std::shared_ptr<AbstractXmlElement> &pValueChild, T &payLoad)
	{
		payLoad = pValueChild->children()[0]->value();
	}

	/////////////////////////////////////////////////////////////////////
	// Creates XML by reading the object passed with specialization
	// on Payload
	template<>
	void PayLoadXMLTemplate::createPayLoadXML(std::shared_ptr<XmlProcessing::AbstractXmlElement> &docElem_dbRec, PayLoad &payLoad)
	{
		using sPtr = std::shared_ptr<AbstractXmlElement>;
		sPtr docElem_PayLoad = makeTaggedElement("payload");
		docElem_dbRec->addChild(docElem_PayLoad);
		sPtr docElem_fp = makeTaggedElement("filepath", payLoad.getFilePath());
		docElem_PayLoad->addChild(docElem_fp);
		std::string checkInStatus = (char)payLoad.getCheckInStatus() != '\0' ? std::string(1, (char)payLoad.getCheckInStatus()) : "";
		sPtr docElem_st = makeTaggedElement("status", checkInStatus);
		docElem_PayLoad->addChild(docElem_st);
		sPtr docElem_auth = makeTaggedElement("author", payLoad.getUser());
		docElem_PayLoad->addChild(docElem_auth);
		sPtr docElem_categlist = makeTaggedElement("categoryList");
		docElem_PayLoad->addChild(docElem_categlist);

		for (auto categ : payLoad.getCategories())
		{
			sPtr docElem_categ = makeTaggedElement("category", categ);
			docElem_categlist->addChild(docElem_categ);
		}
	}

	/////////////////////////////////////////////////////////////////////
	// Creates XML by reading the object passed
	template<typename T>
	void PayLoadXMLTemplate::createPayLoadXML(std::shared_ptr<XmlProcessing::AbstractXmlElement> &docElem_dbRec, T &payLoad)
	{
		using sPtr = std::shared_ptr<AbstractXmlElement>;
		sPtr docElem_PayLoad = makeTaggedElement("payload");
		docElem_dbRec->addChild(docElem_PayLoad);
		sPtr docElem_fp = makeTextElement(payLoad);
		docElem_PayLoad->addChild(docElem_fp);
	}
}
