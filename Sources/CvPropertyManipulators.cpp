//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertyManipulators.cpp
//
//  PURPOSE: Stores manipulators of generic properties for Civ4 classes (sources, interactions, propagators)
//
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"

CvPropertyManipulators::~CvPropertyManipulators()
{
	for (std::vector<CvPropertySource*>::iterator it = m_apSources.begin(); it != m_apSources.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	for (std::vector<CvPropertyInteraction*>::iterator it = m_apInteractions.begin(); it != m_apInteractions.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	for (std::vector<CvPropertyPropagator*>::iterator it = m_apPropagators.begin(); it != m_apPropagators.end(); it++)
	{
		SAFE_DELETE(*it);
	}
}

int CvPropertyManipulators::getNumSources() const
{
	return (int) m_apSources.size();
}

const CvPropertySource *const CvPropertyManipulators::getSource(int index) const
{
	FAssert(0 <= index);
	FAssert(index < (int)m_apSources.size());
	return m_apSources.at(index);
}

bool CvPropertyManipulators::addSource(PropertySourceTypes eType)
{
	switch (eType)
	{
		case PROPERTYSOURCE_CONSTANT:
			m_apSources.push_back(new CvPropertySourceConstant());
			return true;
			
		case PROPERTYSOURCE_CONSTANT_LIMITED:
			m_apSources.push_back(new CvPropertySourceConstantLimited());
			return true;
			
		case PROPERTYSOURCE_DECAY:
			m_apSources.push_back(new CvPropertySourceDecay());
			return true;
			
		case PROPERTYSOURCE_ATTRIBUTE_CONSTANT:
			m_apSources.push_back(new CvPropertySourceAttributeConstant());
			return true;
			
	}
	return false;
}

int CvPropertyManipulators::getNumInteractions() const
{
	return (int) m_apInteractions.size();
}

const CvPropertyInteraction *const CvPropertyManipulators::getInteraction(int index) const
{
	FAssert(0 <= index);
	FAssert(index < (int)m_apInteractions.size());
	return m_apInteractions.at(index);
}

bool CvPropertyManipulators::addInteraction(PropertyInteractionTypes eType)
{
	switch (eType)
	{
		case PROPERTYINTERACTION_CONVERT_CONSTANT:
			m_apInteractions.push_back(new CvPropertyInteractionConvertConstant());
			return true;

		case PROPERTYINTERACTION_INHIBITED_GROWTH:
			m_apInteractions.push_back(new CvPropertyInteractionInhibitedGrowth());
			return true;

		case PROPERTYINTERACTION_CONVERT_PERCENT:
			m_apInteractions.push_back(new CvPropertyInteractionConvertPercent());
			return true;
	}
	return false;
}

int CvPropertyManipulators::getNumPropagators() const
{
	return (int) m_apPropagators.size();
}

const CvPropertyPropagator *const CvPropertyManipulators::getPropagator(int index) const
{
	FAssert(0 <= index);
	FAssert(index < (int)m_apPropagators.size());
	return m_apPropagators.at(index);
}

bool CvPropertyManipulators::addPropagator(PropertyPropagatorTypes eType)
{
	switch (eType)
	{
		case PROPERTYPROPAGATOR_SPREAD:
			m_apPropagators.push_back(new CvPropertyPropagatorSpread());
			return true;

		case PROPERTYPROPAGATOR_GATHER:
			m_apPropagators.push_back(new CvPropertyPropagatorGather());
			return true;

		case PROPERTYPROPAGATOR_DIFFUSE:
			m_apPropagators.push_back(new CvPropertyPropagatorDiffuse());
			return true;
	}
	return false;
}

void CvPropertyManipulators::buildDisplayString(CvWStringBuffer &szBuffer) const
{
	for (std::vector<CvPropertySource*>::const_iterator it = m_apSources.begin(); it != m_apSources.end(); it++)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		(*it)->buildDisplayString(szBuffer);
	}
	for (std::vector<CvPropertyInteraction*>::const_iterator it = m_apInteractions.begin(); it != m_apInteractions.end(); it++)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		(*it)->buildDisplayString(szBuffer);
	}
	for (std::vector<CvPropertyPropagator*>::const_iterator it = m_apPropagators.begin(); it != m_apPropagators.end(); it++)
	{
		szBuffer.append(NEWLINE);
		szBuffer.append(gDLL->getSymbolID(BULLET_CHAR));
		(*it)->buildDisplayString(szBuffer);
	}
}

bool CvPropertyManipulators::read(CvXMLLoadUtility *pXML, const wchar_t* szTagName)
{
	if(pXML->TryMoveToXmlFirstChild(szTagName))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertySource"))
			{
				do
				{
					CvString szTextVal;
					pXML->GetChildXmlValByName(szTextVal, L"PropertySourceType");
					if (addSource((PropertySourceTypes)pXML->GetInfoClass(szTextVal)))
					{
						m_apSources.back()->read(pXML);
					}
				} while(pXML->TryMoveToXmlNextSibling(L"PropertySource"));
			}
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertyInteraction"))
			{
				do
				{
					CvString szTextVal;
					pXML->GetChildXmlValByName(szTextVal, L"PropertyInteractionType");
					if(addInteraction((PropertyInteractionTypes)pXML->GetInfoClass(szTextVal)))
					{
						m_apInteractions.back()->read(pXML);
					}
				} while(pXML->TryMoveToXmlNextSibling(L"PropertyInteraction"));
			}
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertyPropagator"))
			{
				do
				{
					CvString szTextVal;
					pXML->GetChildXmlValByName(szTextVal, L"PropertyPropagatorType");
					if (addPropagator((PropertyPropagatorTypes)pXML->GetInfoClass(szTextVal)))
					{
						m_apPropagators.back()->read(pXML);
					}
				} while(pXML->TryMoveToXmlNextSibling(L"PropertyPropagator"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvPropertyManipulators::copyNonDefaults(CvPropertyManipulators *pProp, CvXMLLoadUtility *pXML)
{
	int iSizeOrig = getNumSources();
	m_apSources.reserve(iSizeOrig + pProp->getNumSources());
	while (pProp->getNumSources())
	{
		bool bExists = false;
		CvPropertySource* pSource = pProp->m_apSources.back();
		for (std::vector<CvPropertySource*>::iterator it = m_apSources.begin();
				it - m_apSources.begin() < iSizeOrig; it++)
		{
			if ((*it)->getProperty() == pSource->getProperty())
			{
				bExists = true;
				break;
			}
		}
		if (!bExists)
		{
			m_apSources.push_back(pSource);
		}
		pProp->m_apSources.pop_back();
	}
	
	iSizeOrig = getNumInteractions();
	m_apInteractions.reserve(iSizeOrig + pProp->getNumInteractions());
	while (pProp->getNumInteractions())
	{
		bool bExists = false;
		CvPropertyInteraction* pInteraction = pProp->m_apInteractions.back();
		for (std::vector<CvPropertyInteraction*>::iterator it = m_apInteractions.begin();
				it - m_apInteractions.begin() < iSizeOrig; it++)
		{
			if ((*it)->getSourceProperty() == pInteraction->getSourceProperty() &&
				(*it)->getTargetProperty() == pInteraction->getTargetProperty())
			{
				bExists = true;
				break;
			}
		}
		if (!bExists)
		{
			m_apInteractions.push_back(pInteraction);
		}
		pProp->m_apInteractions.pop_back();
	}
	
	iSizeOrig = getNumPropagators();
	m_apPropagators.reserve(iSizeOrig + pProp->getNumPropagators());
	while (pProp->getNumPropagators())
	{
		bool bExists = false;
		CvPropertyPropagator* pPropagator = pProp->m_apPropagators.back();
		for (std::vector<CvPropertyPropagator*>::iterator it = m_apPropagators.begin();
				it - m_apPropagators.begin() < iSizeOrig; it++)
		{
			if ((*it)->getProperty() == pPropagator->getProperty())
			{
				bExists = true;
				break;
			}
		}
		if (!bExists)
		{
			m_apPropagators.push_back(pPropagator);
		}
		pProp->m_apPropagators.pop_back();
	}
}

void CvPropertyManipulators::getCheckSum(unsigned int &iSum) const
{
	for (std::vector<CvPropertySource*>::const_iterator it = m_apSources.begin(); it != m_apSources.end(); it++)
	{
		(*it)->getCheckSum(iSum);
	}
	for (std::vector<CvPropertyInteraction*>::const_iterator it = m_apInteractions.begin(); it != m_apInteractions.end(); it++)
	{
		(*it)->getCheckSum(iSum);
	}
	for (std::vector<CvPropertyPropagator*>::const_iterator it = m_apPropagators.begin(); it != m_apPropagators.end(); it++)
	{
		(*it)->getCheckSum(iSum);
	}
}
