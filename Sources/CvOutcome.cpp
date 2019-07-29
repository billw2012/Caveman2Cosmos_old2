//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvOutcome.cpp
//
//  PURPOSE: A single outcome from an outcome list
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include <boost/algorithm/string.hpp>


CvOutcome::CvOutcome(): m_eUnitType(NO_UNIT),
						m_iChance(NULL),
						m_eType(NO_OUTCOME),
						m_ePromotionType(NO_PROMOTION),
						m_iGPP(0),
						m_eGPUnitType(NO_UNIT),
						m_eBonusType(NO_BONUS),
						m_bUnitToCity(NULL),
						m_eEventTrigger(NO_EVENTTRIGGER),
						m_pPlotCondition(NULL),
						m_pUnitCondition(NULL),
						m_bKill(false),
						m_iChancePerPop(0),
						m_iHappinessTimer(0),
						m_iPopulationBoost(0),
						m_iReduceAnarchyLength(NULL),
						m_pPythonAIFunc(NULL),
						m_pPythonDisplayFunc(NULL),
						m_pPythonExecFunc(NULL),
						m_pPythonPossibleFunc(NULL)
{
	for (int i=0; i<NUM_YIELD_TYPES; i++)
	{
		m_aiYield[i] = NULL;
	}

	for (int i=0; i<NUM_COMMERCE_TYPES; i++)
	{
		m_aiCommerce[i] = NULL;
	}
}

CvOutcome::~CvOutcome()
{
	SAFE_DELETE(m_iChance);
	SAFE_DELETE(m_bUnitToCity);
	SAFE_DELETE(m_iReduceAnarchyLength);
	SAFE_DELETE(m_pPlotCondition);
	SAFE_DELETE(m_pUnitCondition);

	for (int i=0; i<NUM_YIELD_TYPES; i++)
	{
		SAFE_DELETE(m_aiYield[i]);
	}
	for (int i=0; i<NUM_COMMERCE_TYPES; i++)
	{
		SAFE_DELETE(m_aiCommerce[i]);
	}

	Py_XDECREF(m_pPythonAIFunc);
	Py_XDECREF(m_pPythonDisplayFunc);
	Py_XDECREF(m_pPythonExecFunc);
	Py_XDECREF(m_pPythonPossibleFunc);
}

int CvOutcome::getYield(YieldTypes eYield, const CvUnit& kUnit) const
{
	FAssert(0 <= eYield);
	FAssert(eYield < NUM_YIELD_TYPES);
	if (m_aiYield[eYield])
	{
		return m_aiYield[eYield]->evaluate(kUnit.getGameObjectConst());
	}
	else
	{
		return 0;
	}
}

int CvOutcome::getCommerce(CommerceTypes eCommerce, const CvUnit& kUnit) const
{
	FAssert(0 <= eCommerce);
	FAssert(eCommerce < NUM_COMMERCE_TYPES);
	if (m_aiCommerce[eCommerce])
	{
		return m_aiCommerce[eCommerce]->evaluate(kUnit.getGameObjectConst());
	}
	else
	{
		return 0;
	}
}

OutcomeTypes CvOutcome::getType() const
{
	return m_eType;
}

UnitTypes CvOutcome::getUnitType() const
{
	return m_eUnitType;
}

bool CvOutcome::getUnitToCity(const CvUnit& kUnit) const
{
	if (m_bUnitToCity)
		return m_bUnitToCity->evaluate(kUnit.getGameObjectConst());
	else
		return false;
}

PromotionTypes CvOutcome::getPromotionType() const
{
	return m_ePromotionType;
}

BonusTypes CvOutcome::getBonusType() const
{
	return m_eBonusType;
}

UnitTypes CvOutcome::getGPUnitType() const
{
	return m_eGPUnitType;
}

int CvOutcome::getGPP() const
{
	return m_iGPP;
}

CvProperties* CvOutcome::getProperties()
{
	return &m_Properties;
}

int CvOutcome::getHappinessTimer()
{
	return m_iHappinessTimer;
}

int CvOutcome::getPopulationBoost()
{
	return m_iPopulationBoost;
}

int CvOutcome::getReduceAnarchyLength(const CvUnit &kUnit) const
{
	if (m_iReduceAnarchyLength)
	{
		return m_iReduceAnarchyLength->evaluate(kUnit.getGameObjectConst());
	}
	return 0;
}

EventTriggerTypes CvOutcome::getEventTrigger()
{
	return m_eEventTrigger;
}

int CvOutcome::getChancePerPop() const
{
	return m_iChancePerPop;
}

bool CvOutcome::isKill() const
{
	return m_bKill;
}

void preparePython(CvString& szCode)
{
	//boost::replace_all(szCode, "\r\n", "\n");
	//boost::replace_all(szCode, "\n", "\r\n");
	//boost::replace_all(szCode, "\t", "  ");
	/*size_t firstNL = szCode.find_first_of('\n');
	if (firstNL != CvString::npos)
	{
		szCode.erase(0, firstNL + 1);
	}*/

	// remove the last line so the closing tag can be separate without caring for whitespace
	size_t lastNL = szCode.find_last_of('\n');
	if (lastNL != CvString::npos)
	{
		szCode.erase(lastNL, szCode.length() - lastNL);
	}

	// we want to remove the amount of space at the start of first code line from all code lines as there might be arbitrary indentation from the XML
	CvString szXMLSpace;
	bool bFinished = false;

	// read the string line by line
	while (!bFinished)
	{
		int iPos = 0;
		bool bComment = false;
		bool bLineFinished = false;
		szXMLSpace.clear();
		while (!bFinished && !bLineFinished)
		{
			if (iPos >= (int)szCode.length())
			{
				// this only happens if there is only whitespace and no code
				bFinished = true;
				szXMLSpace.clear();
				break;
			}
			char c = szCode[iPos];
			switch (c)
			{
				case ' ':
				case '\t':
					if (!bComment)
					{
						szXMLSpace.append(1, c);
					}
					break;

				case '\r':
					break;

				case '\n':
					// end of line, erase it from the string
					szCode.erase(0, iPos + 1);
					bLineFinished = true;
					break;

				case '#':
					// comment
					bComment = true;
					break;

				default:
					// something else, assume actual code, remove line until this char without itself
					szCode.erase(0, iPos);
					bFinished = true;
					break;
			}
			iPos++;
		}
	}

	// now remove this amount of white space from every line
	boost::replace_all(szCode, "\n"+szXMLSpace, "\n");
	
}

void CvOutcome::compilePython()
{
	if (m_szPythonCode.empty())
	{
		return;
	}

	// compile the code and add it as a new module
	PyObject* pCode = Py_CompileString(m_szPythonCode.c_str(), m_szPythonModuleName.c_str(), Py_file_input);
	if (!pCode)
	{
		return;
	}

	PyObject* pModule = PyImport_ExecCodeModule((char*)m_szPythonModuleName.c_str(), pCode);
	Py_XDECREF(pCode);
	if (!pModule)
	{
		return;
	}
	
	PyObject* pDictionary = PyModule_GetDict(pModule);   // borrowed reference

	PyObject* pFunc = PyDict_GetItemString(pDictionary, "isPossible");     // borrowed reference
	if (pFunc)
	{
		Py_INCREF(pFunc);
		m_pPythonPossibleFunc = pFunc;
	}

	pFunc = PyDict_GetItemString(pDictionary, "doOutcome");     // borrowed reference
	if (pFunc)
	{
		Py_INCREF(pFunc);
		m_pPythonExecFunc = pFunc;
	}

	pFunc = PyDict_GetItemString(pDictionary, "getDisplay");     // borrowed reference
	if (pFunc)
	{
		Py_INCREF(pFunc);
		m_pPythonDisplayFunc = pFunc;
	}

	pFunc = PyDict_GetItemString(pDictionary, "getAIValue");     // borrowed reference
	if (pFunc)
	{
		Py_INCREF(pFunc);
		m_pPythonAIFunc = pFunc;
	}

	Py_XDECREF(pModule);

}

int CvOutcome::getChance(const CvUnit &kUnit) const
{
	int iChance = m_iChance->evaluate(kUnit.getGameObjectConst());
	CvOutcomeInfo& kInfo = GC.getOutcomeInfo(m_eType);

	CvCity* pCity = kUnit.plot()->getPlotCity();

	if (pCity)
	{
		iChance += getChancePerPop() * pCity->getPopulation();
	}
	
	for (int i=0; i<kInfo.getNumExtraChancePromotions(); i++)
	{
		if (kUnit.isHasPromotion(kInfo.getExtraChancePromotion(i)))
		{
			iChance += kInfo.getExtraChancePromotionChance(i);
		}
	}
	return iChance > 0 ? iChance : 0;
}

bool CvOutcome::isPossible(const CvUnit& kUnit) const
{
	CvTeam& kTeam = GET_TEAM(kUnit.getTeam());
	CvOutcomeInfo& kInfo = GC.getOutcomeInfo(m_eType);

	if (!kTeam.isHasTech(kInfo.getPrereqTech()))
	{
		return false;
	}

	if (kInfo.getObsoleteTech() != NO_TECH)
	{
		if (kTeam.isHasTech(kInfo.getObsoleteTech()))
		{
			return false;
		}
	}

	if (kInfo.getPrereqCivic() != NO_CIVIC)
	{
		if (!GET_PLAYER(kUnit.getOwnerINLINE()).isCivic(kInfo.getPrereqCivic()))
		{
			return false;
		}
	}

	if (kInfo.getCity())
	{
		if (!kUnit.plot()->isCity())
		{
			return false;
		}
	}

	if (kInfo.getNotCity())
	{
		if (kUnit.plot()->isCity())
		{
			return false;
		}
	}

	if (kInfo.isCapture())
	{
		if (kUnit.isNoCapture())
		{
			return false;
		}
	}

	TeamTypes eOwnerTeam = GET_PLAYER(kUnit.getOwnerINLINE()).getTeam();
	CvTeam& kOwnerTeam = GET_TEAM(eOwnerTeam);
	PlayerTypes ePlotOwner = kUnit.plot()->getOwnerINLINE();
	if (ePlotOwner == NO_PLAYER)
	{
		if (!kInfo.getNeutralTerritory())
		{
			return false;
		}
	}
	else
	{
		if (GET_PLAYER(ePlotOwner).isBarbarian())
		{
			if (!kInfo.getBarbarianTerritory())
			{
				return false;
			}
		}
		else
		{
			TeamTypes ePlotOwnerTeam = GET_PLAYER(ePlotOwner).getTeam();
			CvTeam& kPlotOwnerTeam = GET_TEAM(ePlotOwnerTeam);
			if (kOwnerTeam.isAtWar(ePlotOwnerTeam))
			{
				if (!kInfo.getHostileTerritory())
				{
					return false;
				}
			}
			else if ((eOwnerTeam == ePlotOwnerTeam) || (kPlotOwnerTeam.isVassal(eOwnerTeam)))
			{
				if (!kInfo.getFriendlyTerritory())
				{
					return false;
				}
			}
			else
			{
				if (!kInfo.getNeutralTerritory())
				{
					return false;
				}
			}
		}
	}

	int iPrereqBuildings = kInfo.getNumPrereqBuildings();
	if (iPrereqBuildings > 0)
	{
		CvCity* pCity = kUnit.plot()->getPlotCity();
		if (!pCity)
		{
			return false;
		}

		for (int i=0; i<iPrereqBuildings; i++)
		{
			if (pCity->getNumBuilding(kInfo.getPrereqBuilding(i)) <= 0)
			{
				return false;
			}
		}
	}

	// Removed because outcome has its own prereq and obsolete tech
	/*if (m_ePromotionType != NO_PROMOTION)
	{
		CvPromotionInfo& kPromotion = GC.getPromotionInfo(m_ePromotionType);
		if (!kTeam.isHasTech((TechTypes)kPromotion.getTechPrereq()))
		{
			return false;
		}
		if ((TechTypes)kPromotion.getObsoleteTech() != NO_TECH)
		{
			if (kTeam.isHasTech((TechTypes)kPromotion.getObsoleteTech()))
			{
				return false;
			}
		}
	}*/

	if (m_eBonusType != NO_BONUS)
	{
		CvBonusInfo& kBonus = GC.getBonusInfo(m_eBonusType);
		if (!kTeam.isHasTech((TechTypes)kBonus.getTechReveal()))
		{
			return false;
		}
		if ((TechTypes)kBonus.getTechObsolete() != NO_TECH)
		{
			if (kTeam.isHasTech((TechTypes)kBonus.getTechObsolete()))
			{
				return false;
			}
		}
		if (kUnit.plot()->getBonusType() != NO_BONUS)
		{
			return false;
		}
		if (kUnit.plot()->getFeatureType() == NO_FEATURE)
		{
			if (!kBonus.isTerrain(kUnit.plot()->getTerrainType()))
			{
				return false;
			}
		}
		else
		{
			if (!kBonus.isFeature(kUnit.plot()->getFeatureType()))
			{
				return false;
			}
			if (!kBonus.isFeatureTerrain(kUnit.plot()->getTerrainType()))
			{
				return false;
			}
		}

		int iCount = 0;
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(kUnit.plot()->getX_INLINE(), kUnit.plot()->getY_INLINE(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (pAdjacentPlot->getBonusType() == m_eBonusType)
				{
					iCount++;
				}
			}
		}
		if (!(iCount == 0 || (iCount == 1 && kUnit.plot()->isWater())))
		{
			return false;
		}
	}

	if (m_eEventTrigger != NO_EVENTTRIGGER)
	{
		CvPlayer& kOwner = GET_PLAYER(kUnit.getOwnerINLINE());
		CvEventTriggerInfo& kTriggerInfo = GC.getEventTriggerInfo(m_eEventTrigger);
		if (!kOwner.isEventTriggerPossible(m_eEventTrigger, true))
		{
			return false;
		}

		if (kTriggerInfo.isPickCity())
		{
			CvCity* pCity = kUnit.plot()->getPlotCity();
			if (pCity)
			{
				if (!pCity->isEventTriggerPossible(m_eEventTrigger))
				{
					return false;
				}
			}
		}

		if (isPlotEventTrigger(m_eEventTrigger))
		{
			if (!kUnit.plot()->canTrigger(m_eEventTrigger, kUnit.getOwnerINLINE()))
			{
				return false;
			}
		}
	}

	if (m_pPlotCondition)
	{
		if (!m_pPlotCondition->evaluate(kUnit.plot()->getGameObject()))
		{
			return false;
		}
	}

	if (m_pUnitCondition)
	{
		if (!m_pUnitCondition->evaluate(kUnit.getGameObjectConst()))
		{
			return false;
		}
	}

	if (m_pPythonPossibleFunc)
	{
		CyUnit cyUnit(const_cast<CvUnit*>(&kUnit));
		PyObject* pyUnit = gDLL->getPythonIFace()->makePythonObject(&cyUnit);
		CyPlot cyPlot(kUnit.plot());
		PyObject* pyPlot = gDLL->getPythonIFace()->makePythonObject(&cyPlot);
		
		PyObject* pyResult = PyObject_CallFunctionObjArgs(m_pPythonPossibleFunc, pyUnit, pyPlot, NULL);
		bool bResult = boost::python::extract<bool>(pyResult);
		
		Py_XDECREF(pyResult);
		Py_DECREF(pyUnit);
		Py_DECREF(pyPlot);

		if (!bResult)
		{
			return false;
		}
	}

	return getChance(kUnit) > 0;
}

// Can return a false positive if an outcome requires a building combination
bool CvOutcome::isPossibleSomewhere(const CvUnit& kUnit) const
{
	CvTeam& kTeam = GET_TEAM(kUnit.getTeam());
	CvOutcomeInfo& kInfo = GC.getOutcomeInfo(m_eType);

	if (!kTeam.isHasTech(kInfo.getPrereqTech()))
	{
		return false;
	}

	if (kInfo.getObsoleteTech() != NO_TECH)
	{
		if (kTeam.isHasTech(kInfo.getObsoleteTech()))
		{
			return false;
		}
	}

	if (kInfo.getPrereqCivic() != NO_CIVIC)
	{
		if (!GET_PLAYER(kUnit.getOwnerINLINE()).isCivic(kInfo.getPrereqCivic()))
		{
			return false;
		}
	}

	//TeamTypes eOwnerTeam = GET_PLAYER(kUnit.getOwnerINLINE()).getTeam();
	//CvTeam& kOwnerTeam = GET_TEAM(eOwnerTeam);

	int iPrereqBuildings = kInfo.getNumPrereqBuildings();
	if (iPrereqBuildings > 0)
	{
		for (int i=0; i<iPrereqBuildings; i++)
		{
			if (GET_PLAYER(kUnit.getOwnerINLINE()).getBuildingClassCount((BuildingClassTypes)GC.getBuildingInfo(kInfo.getPrereqBuilding(i)).getBuildingClassType()) <= 0)
			{
				return false;
			}
		}
	}

	// Removed because outcome has its own prereq and obsolete tech
	/*if (m_ePromotionType != NO_PROMOTION)
	{
		CvPromotionInfo& kPromotion = GC.getPromotionInfo(m_ePromotionType);
		if (!kTeam.isHasTech((TechTypes)kPromotion.getTechPrereq()))
		{
			return false;
		}
		if ((TechTypes)kPromotion.getObsoleteTech() != NO_TECH)
		{
			if (kTeam.isHasTech((TechTypes)kPromotion.getObsoleteTech()))
			{
				return false;
			}
		}
	}*/

	if (m_eBonusType != NO_BONUS)
	{
		CvBonusInfo& kBonus = GC.getBonusInfo(m_eBonusType);
		if (!kTeam.isHasTech((TechTypes)kBonus.getTechReveal()))
		{
			return false;
		}
		if ((TechTypes)kBonus.getTechObsolete() != NO_TECH)
		{
			if (kTeam.isHasTech((TechTypes)kBonus.getTechObsolete()))
			{
				return false;
			}
		}
	}

	if (m_eEventTrigger != NO_EVENTTRIGGER)
	{
		CvPlayer& kOwner = GET_PLAYER(kUnit.getOwnerINLINE());
		if (!kOwner.isEventTriggerPossible(m_eEventTrigger, true))
		{
			return false;
		}
	}

	if (m_pUnitCondition)
	{
		if (!m_pUnitCondition->evaluate(kUnit.getGameObjectConst()))
		{
			return false;
		}
	}

	return getChance(kUnit) > 0;
}

bool CvOutcome::isPossibleInPlot(const CvUnit& kUnit, const CvPlot& kPlot, bool bForTrade) const
{
	CvTeam& kTeam = GET_TEAM(kUnit.getTeam());
	CvOutcomeInfo& kInfo = GC.getOutcomeInfo(m_eType);

	if (!kTeam.isHasTech(kInfo.getPrereqTech()))
	{
		return false;
	}

	if (kInfo.getObsoleteTech() != NO_TECH)
	{
		if (kTeam.isHasTech(kInfo.getObsoleteTech()))
		{
			return false;
		}
	}

	if (kInfo.getPrereqCivic() != NO_CIVIC)
	{
		if (!GET_PLAYER(kUnit.getOwnerINLINE()).isCivic(kInfo.getPrereqCivic()))
		{
			return false;
		}
	}

	if (kInfo.getCity())
	{
		if (!kPlot.isCity())
		{
			return false;
		}
	}

	if (kInfo.getNotCity())
	{
		if (kPlot.isCity())
		{
			return false;
		}
	}

	TeamTypes eOwnerTeam = GET_PLAYER(kUnit.getOwnerINLINE()).getTeam();
	CvTeam& kOwnerTeam = GET_TEAM(eOwnerTeam);
	PlayerTypes ePlotOwner = kPlot.getOwnerINLINE();
	if (ePlotOwner == NO_PLAYER)
	{
		if (!kInfo.getNeutralTerritory())
		{
			return false;
		}
	}
	else
	{
		if (GET_PLAYER(ePlotOwner).isBarbarian())
		{
			if (!kInfo.getBarbarianTerritory())
			{
				return false;
			}
		}
		else
		{
			TeamTypes ePlotOwnerTeam = GET_PLAYER(ePlotOwner).getTeam();
			CvTeam& kPlotOwnerTeam = GET_TEAM(ePlotOwnerTeam);
			if (kOwnerTeam.isAtWar(ePlotOwnerTeam))
			{
				if (!kInfo.getHostileTerritory())
				{
					return false;
				}
			}
			else if ((eOwnerTeam == ePlotOwnerTeam) || (kPlotOwnerTeam.isVassal(eOwnerTeam)))
			{
				if (!kInfo.getFriendlyTerritory())
				{
					return false;
				}
			}
			else
			{
				if (!kInfo.getNeutralTerritory())
				{
					return false;
				}
			}
		}
	}

	int iPrereqBuildings = kInfo.getNumPrereqBuildings();
	if (iPrereqBuildings > 0)
	{
		CvCity* pCity = kPlot.getPlotCity();
		if (!pCity)
		{
			return false;
		}

		for (int i=0; i<iPrereqBuildings; i++)
		{
			if (pCity->getNumBuilding(kInfo.getPrereqBuilding(i)) <= 0)
			{
				return false;
			}
		}
	}

	// Removed because outcome has its own prereq and obsolete tech
	/*if (m_ePromotionType != NO_PROMOTION)
	{
		CvPromotionInfo& kPromotion = GC.getPromotionInfo(m_ePromotionType);
		if (!kTeam.isHasTech((TechTypes)kPromotion.getTechPrereq()))
		{
			return false;
		}
		if ((TechTypes)kPromotion.getObsoleteTech() != NO_TECH)
		{
			if (kTeam.isHasTech((TechTypes)kPromotion.getObsoleteTech()))
			{
				return false;
			}
		}
	}*/

	if (m_eBonusType != NO_BONUS)
	{
		CvBonusInfo& kBonus = GC.getBonusInfo(m_eBonusType);
		if (!kTeam.isHasTech((TechTypes)kBonus.getTechReveal()))
		{
			return false;
		}
		if ((TechTypes)kBonus.getTechObsolete() != NO_TECH)
		{
			if (kTeam.isHasTech((TechTypes)kBonus.getTechObsolete()))
			{
				return false;
			}
		}
		if (kPlot.getBonusType() != NO_BONUS)
		{
			return false;
		}
		if (kPlot.getFeatureType() == NO_FEATURE)
		{
			if (!kBonus.isTerrain(kPlot.getTerrainType()))
			{
				return false;
			}
		}
		else
		{
			if (!kBonus.isFeature(kPlot.getFeatureType()))
			{
				return false;
			}
			if (!kBonus.isFeatureTerrain(kPlot.getTerrainType()))
			{
				return false;
			}
		}

		int iCount = 0;
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(kPlot.getX_INLINE(), kPlot.getY_INLINE(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (pAdjacentPlot->getBonusType() == m_eBonusType)
				{
					iCount++;
				}
			}
		}
		if (!(iCount == 0 || (iCount == 1 && kPlot.isWater())))
		{
			return false;
		}
	}

	if (m_eEventTrigger != NO_EVENTTRIGGER)
	{
		CvPlayer& kOwner = GET_PLAYER(kUnit.getOwnerINLINE());
		CvEventTriggerInfo& kTriggerInfo = GC.getEventTriggerInfo(m_eEventTrigger);
		if (!kOwner.isEventTriggerPossible(m_eEventTrigger, true))
		{
			return false;
		}

		if (kTriggerInfo.isPickCity())
		{
			CvCity* pCity = kPlot.getPlotCity();
			if (pCity)
			{
				if (!pCity->isEventTriggerPossible(m_eEventTrigger))
				{
					return false;
				}
			}
		}

		if (isPlotEventTrigger(m_eEventTrigger))
		{
			if (!kPlot.canTrigger(m_eEventTrigger, kUnit.getOwnerINLINE()))
			{
				return false;
			}
		}
	}

	if (m_pPlotCondition)
	{
		if (!m_pPlotCondition->evaluate(kPlot.getGameObjectConst()))
		{
			return false;
		}
	}

	if (m_pUnitCondition)
	{
		if (!m_pUnitCondition->evaluate(kUnit.getGameObjectConst()))
		{
			return false;
		}
	}

	if (m_pPythonPossibleFunc)
	{
		CyUnit cyUnit(const_cast<CvUnit*>(&kUnit));
		PyObject* pyUnit = gDLL->getPythonIFace()->makePythonObject(&cyUnit);
		CyPlot cyPlot(const_cast<CvPlot*>(&kPlot));
		PyObject* pyPlot = gDLL->getPythonIFace()->makePythonObject(&cyPlot);

		PyObject* pyResult = PyObject_CallFunctionObjArgs(m_pPythonPossibleFunc, pyUnit, pyPlot, NULL);
		bool bResult = boost::python::extract<bool>(pyResult);

		Py_XDECREF(pyResult);
		Py_DECREF(pyUnit);
		Py_DECREF(pyPlot);

		if (!bResult)
		{
			return false;
		}
	}

	return getChance(kUnit) > 0;
}

bool CvOutcome::isPossible(const CvPlayerAI& kPlayer) const
{
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());
	CvOutcomeInfo& kInfo = GC.getOutcomeInfo(m_eType);

	if (!kTeam.isHasTech(kInfo.getPrereqTech()))
	{
		return false;
	}

	if (kInfo.getObsoleteTech() != NO_TECH)
	{
		if (kTeam.isHasTech(kInfo.getObsoleteTech()))
		{
			return false;
		}
	}

	if (kInfo.getPrereqCivic() != NO_CIVIC)
	{
		if (!kPlayer.isCivic(kInfo.getPrereqCivic()))
		{
			return false;
		}
	}

	// Removed because outcome has its own prereq and obsolete tech
	/*if (m_ePromotionType != NO_PROMOTION)
	{
		CvPromotionInfo& kPromotion = GC.getPromotionInfo(m_ePromotionType);
		if (!kTeam.isHasTech((TechTypes)kPromotion.getTechPrereq()))
		{
			return false;
		}
		if ((TechTypes)kPromotion.getObsoleteTech() != NO_TECH)
		{
			if (kTeam.isHasTech((TechTypes)kPromotion.getObsoleteTech()))
			{
				return false;
			}
		}
	}*/

	if (m_eBonusType != NO_BONUS)
	{
		CvBonusInfo& kBonus = GC.getBonusInfo(m_eBonusType);
		if (!kTeam.isHasTech((TechTypes)kBonus.getTechReveal()))
		{
			return false;
		}
		if ((TechTypes)kBonus.getTechObsolete() != NO_TECH)
		{
			if (kTeam.isHasTech((TechTypes)kBonus.getTechObsolete()))
			{
				return false;
			}
		}
	}

	if (m_eEventTrigger != NO_EVENTTRIGGER)
	{
		if (!kPlayer.isEventTriggerPossible(m_eEventTrigger, true))
		{
			return false;
		}
	}

	return true;
}

bool CvOutcome::execute(CvUnit &kUnit, PlayerTypes eDefeatedUnitPlayer, UnitTypes eDefeatedUnitType) const
{
	if (!isPossible(kUnit))
	{
		return false;
	}

	CvWStringBuffer szBuffer;

	CvPlayer& kPlayer = GET_PLAYER(kUnit.getOwnerINLINE());
	bool bToCoastalCity = GC.getOutcomeInfo(getType()).getToCoastalCity();
	CvUnitInfo* pUnitInfo;
	if (eDefeatedUnitType > NO_UNIT)
		pUnitInfo = &GC.getUnitInfo(eDefeatedUnitType);
	else
		pUnitInfo = &kUnit.getUnitInfo();

	CvWString& szMessage = GC.getOutcomeInfo(getType()).getMessageText();
	bool bNothing = true;
	if (!szMessage.empty())
	{
		szBuffer.append(gDLL->getText(szMessage, kUnit.getNameKey(), pUnitInfo->getDescription()));
		szBuffer.append(L" ( ");
		bNothing = false;
	}

	bool bFirst = true;

	if (m_ePromotionType > NO_PROMOTION)
	{
		kUnit.setHasPromotion(m_ePromotionType, true);
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(GC.getPromotionInfo(m_ePromotionType).getDescription());
	}

	bool bUnitToCity = getUnitToCity(kUnit);
	if (GC.getGame().isOption(GAMEOPTION_TELEPORT_HUNTING_AWARDS) && 
		m_eUnitType > NO_UNIT && 
		(GC.getUnitInfo(m_eUnitType).hasUnitCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SUBDUED")) ||
		GC.getUnitInfo(m_eUnitType).hasUnitCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_IDEA"))))
	{
		bUnitToCity = true;
	}
	if (m_eUnitType > NO_UNIT && !bUnitToCity)
	{
		CvUnit* pUnit = kPlayer.initUnit(m_eUnitType, kUnit.plot()->getX_INLINE(), kUnit.plot()->getY_INLINE(), (UnitAITypes)GC.getUnitInfo(m_eUnitType).getDefaultUnitAIType(), NO_DIRECTION, GC.getGameINLINE().getSorenRandNum(10000, "AI Unit Birthmark"));
		FAssertMsg(pUnit != NULL, "pUnit is expected to be assigned a valid unit object");
		int iDmg = GC.getDefineINT("ANIMAL_DAMAGE_PERCENT_AFTER_SUBDUE");
		iDmg = (iDmg * pUnit->maxHitPoints())/100;
		pUnit->setDamage(iDmg, NO_PLAYER, false);
		pUnit->finishMoves();

		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(GC.getUnitInfo(m_eUnitType).getDescription());
	}

	// Calculate the actual yields and commerces
	int aiYield[NUM_YIELD_TYPES];
	int aiCommerce[NUM_COMMERCE_TYPES];

	for (int i=0; i<NUM_YIELD_TYPES; i++)
	{
		aiYield[i] = getYield((YieldTypes)i, kUnit);
	}

	for (int i=0; i<NUM_COMMERCE_TYPES; i++)
	{
		aiCommerce[i] = getCommerce((CommerceTypes)i, kUnit);
	}

	if (aiYield[YIELD_COMMERCE])
	{
		aiCommerce[COMMERCE_CULTURE] += aiYield[YIELD_COMMERCE] * kPlayer.getCommercePercent(COMMERCE_CULTURE);
	}

	if (aiYield[YIELD_PRODUCTION] || aiYield[YIELD_FOOD] || aiCommerce[COMMERCE_CULTURE] || m_iGPP || (bUnitToCity && m_eUnitType > NO_UNIT) || m_iHappinessTimer || m_iPopulationBoost || m_iReduceAnarchyLength)
	{
		CvCity* pCity = GC.getMapINLINE().findCity(kUnit.plot()->getX_INLINE(), kUnit.plot()->getY_INLINE(), kUnit.getOwnerINLINE(), NO_TEAM, true, bToCoastalCity);
		if (!pCity)
			pCity = GC.getMapINLINE().findCity(kUnit.plot()->getX_INLINE(), kUnit.plot()->getY_INLINE(), kUnit.getOwnerINLINE(), NO_TEAM, false, bToCoastalCity);

		if (pCity)
		{
			if (!bFirst)
			{
				szBuffer.append(L", ");
			}
			else
			{
				bFirst = false;
			}
			if (aiYield[YIELD_PRODUCTION])
			{
				pCity->changeProduction(aiYield[YIELD_PRODUCTION]);
				CvWString szTemp;
				szTemp.Format(L" %d%c", aiYield[YIELD_PRODUCTION], GC.getYieldInfo(YIELD_PRODUCTION).getChar());
				szBuffer.append(szTemp);
			}
			if (aiYield[YIELD_FOOD])
			{
				pCity->changeFood(aiYield[YIELD_FOOD]);
				CvWString szTemp;
				szTemp.Format(L" %d%c", aiYield[YIELD_FOOD], GC.getYieldInfo(YIELD_FOOD).getChar());
				szBuffer.append(szTemp);
			}

			if (aiCommerce[COMMERCE_CULTURE])
			{
				pCity->changeCulture(kUnit.getOwnerINLINE(), aiCommerce[COMMERCE_CULTURE], true, true);
				CvWString szTemp;
				szTemp.Format(L" %d%c", aiCommerce[COMMERCE_CULTURE], GC.getCommerceInfo(COMMERCE_CULTURE).getChar());
				szBuffer.append(szTemp);
			}

			if (m_iGPP)
			{
				pCity->changeGreatPeopleProgress(m_iGPP);
				if (m_eGPUnitType > NO_UNIT)
				{
					pCity->changeGreatPeopleUnitProgress(m_eGPUnitType, m_iGPP);
				}
				CvWString szTemp;
				szTemp.Format(L" %d%c", m_iGPP, gDLL->getSymbolID(GREAT_PEOPLE_CHAR));
				szBuffer.append(szTemp);
			}

			if (!m_Properties.isEmpty())
			{
				pCity->getProperties()->addProperties(&m_Properties);
				m_Properties.buildCompactChangesString(szBuffer);
			}

			if (m_iHappinessTimer)
			{
				pCity->changeHappinessTimer(m_iHappinessTimer);
				int iHappy = GC.getDefineINT("TEMP_HAPPY");
				szBuffer.append(L" ");
				szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_TEMP_HAPPY", iHappy, m_iHappinessTimer));
			}

			if (m_iPopulationBoost)
			{
				pCity->changePopulation(m_iPopulationBoost);
				szBuffer.append(L" ");
				szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_TEMP_POPULATION_BOOST", m_iPopulationBoost));
			}

			int iReduce = getReduceAnarchyLength(kUnit);
			if (iReduce)
			{
				int iOccupation = pCity->getOccupationTimer();
				iReduce = std::min(iReduce, iOccupation);
				if (iReduce)
				{
					pCity->changeOccupationTimer(-iReduce);
					szBuffer.append(L" ");
					szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_LESS_ANARCHY", iReduce));
				}
			}

			if (bUnitToCity && m_eUnitType > NO_UNIT)
			{
				CvUnit* pUnit = kPlayer.initUnit(m_eUnitType, pCity->getX_INLINE(), pCity->getY_INLINE(), (UnitAITypes)GC.getUnitInfo(m_eUnitType).getDefaultUnitAIType(), NO_DIRECTION, GC.getGameINLINE().getSorenRandNum(10000, "AI Unit Birthmark"));
				FAssertMsg(pUnit != NULL, "pUnit is expected to be assigned a valid unit object");
				int iDmg = GC.getDefineINT("ANIMAL_DAMAGE_PERCENT_AFTER_SUBDUE");
				iDmg = (iDmg * pUnit->maxHitPoints())/100;
				pUnit->setDamage(iDmg, NO_PLAYER, false);
				pUnit->finishMoves();

				szBuffer.append(L" ");
				szBuffer.append(GC.getUnitInfo(m_eUnitType).getDescription());
			}

			szBuffer.append(L" ");
			szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_TO"));
			szBuffer.append(" ");
			szBuffer.append(pCity->getName());
		}
	}

	int iGoldTimes100 = 0;
	int iResearchTimes100 = 0;
	int iEspionageTimes100 = 0;

	if (aiYield[YIELD_COMMERCE])
	{
		iGoldTimes100 = aiYield[YIELD_COMMERCE] * kPlayer.getCommercePercent(COMMERCE_GOLD);
		iResearchTimes100 = aiYield[YIELD_COMMERCE] * kPlayer.getCommercePercent(COMMERCE_RESEARCH);
		iEspionageTimes100 = aiYield[YIELD_COMMERCE] * kPlayer.getCommercePercent(COMMERCE_ESPIONAGE);
	}

	iGoldTimes100 += aiCommerce[COMMERCE_GOLD] * 100;
	iResearchTimes100 += aiCommerce[COMMERCE_RESEARCH] * 100;
	iEspionageTimes100 += aiCommerce[COMMERCE_ESPIONAGE] * 100;
	
	if (iGoldTimes100)
	{
		kPlayer.changeGold(iGoldTimes100 / 100);
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		CvWString szTemp;
		szTemp.Format(L" %d%c", iGoldTimes100 / 100, GC.getCommerceInfo(COMMERCE_GOLD).getChar());
		szBuffer.append(szTemp);
	}
	CvTeam& kTeam = GET_TEAM(kUnit.getTeam());
	if (iResearchTimes100)
	{
		TechTypes eCurrentTech = kPlayer.getCurrentResearch();
		if (eCurrentTech != NO_TECH)
		{
			kTeam.changeResearchProgress(eCurrentTech, iResearchTimes100 / 100, kUnit.getOwnerINLINE());
			if (!bFirst)
			{
				szBuffer.append(L", ");
			}
			else
			{
				bFirst = false;
			}
			CvWString szTemp;
			szTemp.Format(L" %d%c", iResearchTimes100 / 100, GC.getCommerceInfo(COMMERCE_RESEARCH).getChar());
			szBuffer.append(szTemp);
		}
	}
	if (iEspionageTimes100 && (eDefeatedUnitPlayer != NO_PLAYER))
	{
		kTeam.changeEspionagePointsEver(iEspionageTimes100 / 100);
		kTeam.changeEspionagePointsAgainstTeam(GET_PLAYER(eDefeatedUnitPlayer).getTeam(), iEspionageTimes100 / 100);
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		CvWString szTemp;
		szTemp.Format(L" %d%c", iEspionageTimes100 / 100, GC.getCommerceInfo(COMMERCE_ESPIONAGE).getChar());
		szBuffer.append(szTemp);
	}

	if (m_eBonusType != NO_BONUS)
	{
		kUnit.plot()->setBonusType(m_eBonusType);
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(GC.getBonusInfo(m_eBonusType).getDescription());
	}

	if (m_bKill)
	{
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_KILLS_UNIT"));
	}

	if (m_eEventTrigger != NO_EVENTTRIGGER)
	{
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(GC.getEventTriggerInfo(m_eEventTrigger).getDescription());
	}

	szBuffer.append(L" )");

	if (!bNothing)
	{
		MEMORY_TRACK_EXEMPT();

		AddDLLMessage(kUnit.getOwnerINLINE(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer.getCString(), NULL, MESSAGE_TYPE_INFO, pUnitInfo->getButton(), NO_COLOR, kUnit.plot()->getX_INLINE(), kUnit.plot()->getY_INLINE(), true, true);
	}

	if (m_eEventTrigger != NO_EVENTTRIGGER)
	{
		CvEventTriggerInfo& kTriggerInfo = GC.getEventTriggerInfo(m_eEventTrigger);
		if (kTriggerInfo.isPickCity() && (kUnit.plot()->getPlotCity() != NULL))
		{
			kPlayer.initTriggeredData(m_eEventTrigger, true, kUnit.plot()->getPlotCity()->getID());
		}
		else
		{
			kPlayer.initTriggeredData(m_eEventTrigger, true, -1, kUnit.plot()->getX_INLINE(), kUnit.plot()->getY_INLINE());
		}
	}

	if (!m_szPythonCallback.empty())
	{
		PYTHON_ACCESS_LOCK_SCOPE

		long lResult;

		CyUnit* pyUnit = new CyUnit(&kUnit);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyUnit));
		argsList.add((int)eDefeatedUnitPlayer);
		argsList.add((int)eDefeatedUnitType);

		PYTHON_CALL_FUNCTION4(__FUNCTION__, PYRandomEventModule, m_szPythonCallback, argsList.makeFunctionArgs(), &lResult);
		delete pyUnit;
	}

	if (m_pPythonExecFunc)
	{
		CyUnit cyUnit(&kUnit);
		PyObject* pyUnit = gDLL->getPythonIFace()->makePythonObject(&cyUnit);
		CyPlot cyPlot(kUnit.plot());
		PyObject* pyPlot = gDLL->getPythonIFace()->makePythonObject(&cyPlot);
		PyObject* pyDefeatedPlayer = gDLL->getPythonIFace()->makePythonObject(&eDefeatedUnitPlayer);
		PyObject* pyDefeatedUnitType = gDLL->getPythonIFace()->makePythonObject(&eDefeatedUnitType);

		PyObject* pyResult = PyObject_CallFunctionObjArgs(m_pPythonExecFunc, pyUnit, pyPlot, pyDefeatedPlayer, pyDefeatedUnitType, NULL);
		//bool bResult = boost::python::extract<bool>(pyResult);

		Py_XDECREF(pyResult);
		Py_DECREF(pyUnit);
		Py_DECREF(pyPlot);
		Py_DECREF(pyDefeatedPlayer);
		Py_DECREF(pyDefeatedUnitType);
	}

	if (m_bKill)
	{
		kUnit.kill(true);
	}

	return true;
}

int CvOutcome::AI_getValueInPlot(const CvUnit &kUnit, const CvPlot &kPlot, bool bForTrade) const
{
	if (!isPossibleInPlot(kUnit, kPlot, bForTrade))
	{
		return 0;
	}

	int iValue = 0;

	CvPlayerAI& kPlayer = GET_PLAYER(kUnit.getOwnerINLINE());
	bool bToCoastalCity = GC.getOutcomeInfo(getType()).getToCoastalCity();
	//CvUnitInfo* pUnitInfo = &kUnit.getUnitInfo();

	if (m_ePromotionType > NO_PROMOTION)
	{
		iValue += kPlayer.AI_promotionValue(m_ePromotionType, kUnit.getUnitType(), &kUnit, kUnit.AI_getUnitAIType());
	}

	if ( m_eEventTrigger != NO_EVENTTRIGGER )
	{
		int iTempValue;
		EventTriggeredData* pTriggerData;

		CvEventTriggerInfo& kTriggerInfo = GC.getEventTriggerInfo(m_eEventTrigger);
		if (kTriggerInfo.isPickCity() && (kPlot.getPlotCity() != NULL))
		{
			pTriggerData = kPlayer.initTriggeredData(m_eEventTrigger, false, kPlot.getPlotCity()->getID());
		}
		else
		{
			pTriggerData = kPlayer.initTriggeredData(m_eEventTrigger, false, -1, kPlot.getX_INLINE(), kPlot.getY_INLINE());
		}

		if ( NO_EVENT != kPlayer.AI_chooseEvent(pTriggerData->getID(), &iTempValue) )
		{
			iValue += iTempValue;
		}
		
		kPlayer.deleteEventTriggered(pTriggerData->getID());
	}

	if (m_eUnitType > NO_UNIT)
	{
		iValue += kPlayer.AI_unitValue(m_eUnitType, (UnitAITypes)GC.getUnitInfo(m_eUnitType).getDefaultUnitAIType(), kPlot.area());
	}

	// Calculate the actual yields and commerces, if the expression tries to include the plot the result will be incorrect
	int aiYield[NUM_YIELD_TYPES];
	int aiCommerce[NUM_COMMERCE_TYPES];

	for (int i=0; i<NUM_YIELD_TYPES; i++)
	{
		aiYield[i] = getYield((YieldTypes)i, kUnit);
	}

	for (int i=0; i<NUM_COMMERCE_TYPES; i++)
	{
		aiCommerce[i] = getCommerce((CommerceTypes)i, kUnit);
	}

	// We go the easy way and use AI_yieldValue for all the yields and commerces despite that city multipliers do not apply for some
	if (aiYield[YIELD_PRODUCTION] || aiYield[YIELD_FOOD] || aiYield[YIELD_COMMERCE] || aiCommerce[COMMERCE_GOLD] || aiCommerce[COMMERCE_RESEARCH] || aiCommerce[COMMERCE_CULTURE] || aiCommerce[COMMERCE_ESPIONAGE] || m_iGPP)
	{
		// short circuit plot city as this method will be called for city plots most of the time
		CvCityAI* pCity = (CvCityAI*) kPlot.getPlotCity();
		if (!pCity || (bToCoastalCity && (!pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))))
			pCity = (CvCityAI*) GC.getMapINLINE().findCity(kPlot.getX_INLINE(), kPlot.getY_INLINE(), kUnit.getOwnerINLINE(), NO_TEAM, true, bToCoastalCity);
		if (!pCity)
			pCity = (CvCityAI*) GC.getMapINLINE().findCity(kPlot.getX_INLINE(), kPlot.getY_INLINE(), kUnit.getOwnerINLINE(), NO_TEAM, false, bToCoastalCity);

		if (pCity)
		{
			if (aiYield[YIELD_PRODUCTION] || aiYield[YIELD_FOOD] || aiYield[YIELD_COMMERCE] || aiCommerce[COMMERCE_GOLD] || aiCommerce[COMMERCE_RESEARCH] || aiCommerce[COMMERCE_CULTURE] || aiCommerce[COMMERCE_ESPIONAGE])
			{
				// We need shorts and not ints
				short aiYields[NUM_YIELD_TYPES];
				short aiCommerces[NUM_COMMERCE_TYPES];
				for (int i=0; i<NUM_YIELD_TYPES; i++)
					aiYields[i] = (short)aiYield[i];
				for (int i=0; i<NUM_COMMERCE_TYPES; i++)
					aiCommerces[i] = (short)aiCommerce[i];
				iValue += pCity->AI_yieldValue(aiYields, aiCommerces, false, false);
			}

			if (m_iGPP)
			{
				// Currently there is no use of that feature so go the easy way. If it is used more, code similar to AI_specialistValue should be used
				iValue += m_iGPP * 4;
			}

			if (m_iHappinessTimer)
			{
				if (pCity->happyLevel() - pCity->unhappyLevel(1) < 0)
				{
					iValue += m_iHappinessTimer * 10;
				}
			}

			if (m_iPopulationBoost)
			{
				iValue += m_iPopulationBoost * 50;
			}

			int iReduce = getReduceAnarchyLength(kUnit);
			if (iReduce)
			{
				int iOccupation = pCity->getOccupationTimer();
				iReduce = std::min(iReduce, iOccupation);
				iValue += iReduce * 50; // pure guess, might be more or less valuable or some more complex code might be needed
			}
		}

	}

	if (m_pPythonAIFunc)
	{
		CyUnit cyUnit(const_cast<CvUnit*>(&kUnit));
		PyObject* pyUnit = gDLL->getPythonIFace()->makePythonObject(&cyUnit);
		CyPlot cyPlot(const_cast<CvPlot*>(&kPlot));
		PyObject* pyPlot = gDLL->getPythonIFace()->makePythonObject(&cyPlot);

		PyObject* pyResult = PyObject_CallFunctionObjArgs(m_pPythonAIFunc, pyUnit, pyPlot, NULL);
		iValue += boost::python::extract<int>(pyResult);

		Py_XDECREF(pyResult);
		Py_DECREF(pyUnit);
		Py_DECREF(pyPlot);
	}

	return iValue;
}

bool CvOutcome::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, L"OutcomeType");
	GC.addDelayedResolution((int*)&m_eType, szTextVal);
	//m_eType = (OutcomeTypes) pXML->FindInInfoClass(szTextVal);
	if (pXML->TryMoveToXmlFirstChild(L"iChance"))
	{
		m_iChance = IntExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	pXML->GetOptionalChildXmlValByName(&m_iChancePerPop, L"iChancePerPop");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"UnitType");
	GC.addDelayedResolution((int*)&m_eUnitType, szTextVal);
	//m_eUnitType = (UnitTypes) pXML->FindInInfoClass(szTextVal);
	if (pXML->TryMoveToXmlFirstChild(L"bUnitToCity"))
	{
		m_bUnitToCity = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PromotionType");
	m_ePromotionType = (PromotionTypes) pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusType");
	m_eBonusType = (BonusTypes) pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iGPP, L"iGPP");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"GPUnitType");
	GC.addDelayedResolution((int*)&m_eGPUnitType, szTextVal);
	//m_eGPUnitType = (UnitTypes) pXML->FindInInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iHappinessTimer, L"iHappinessTimer");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationBoost, L"iPopulationBoost");
	if (pXML->TryMoveToXmlFirstChild(L"iReduceAnarchyLength"))
	{
		m_iReduceAnarchyLength = IntExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	pXML->GetOptionalChildXmlValByName(szTextVal, L"EventTrigger");
	GC.addDelayedResolution((int*)&m_eEventTrigger, szTextVal);
	pXML->GetOptionalChildXmlValByName(m_szPythonCallback, L"PythonCallback");
	pXML->GetOptionalChildXmlValByName(&m_bKill, L"bKill");
	pXML->GetOptionalChildXmlValByName(m_szPythonModuleName, L"PythonName");
	pXML->GetOptionalChildXmlValByName(m_szPythonCode, L"Python");
	
	if (!m_szPythonCode.empty())
	{
		preparePython(m_szPythonCode);
		compilePython();
	}
	
	if(pXML->TryMoveToXmlFirstChild(L"Yields"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"iYield"))
			{
				int iI = 0;
				do
				{
					//pXML->GetXmlVal(&m_aiYield[iI]);
					m_aiYield[iI] = IntExpr::read(pXML);
					iI++;
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"Commerces"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"iCommerce"))
			{
				int iI = 0;
				do
				{
					//pXML->GetXmlVal(&m_aiCommerce[iI]);
					m_aiCommerce[iI] = IntExpr::read(pXML);
					iI++;
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	m_Properties.read(pXML);

	if (pXML->TryMoveToXmlFirstChild(L"PlotCondition"))
	{
		m_pPlotCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"UnitCondition"))
	{
		m_pUnitCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvOutcome::copyNonDefaults(CvOutcome* pOutcome, CvXMLLoadUtility* pXML )
{
	GC.copyNonDefaultDelayedResolution((int*)&m_eType, (int*)&(pOutcome->m_eType));
	//if (m_eType == NO_OUTCOME)
	//{
	//	m_eType = pOutcome->getType();
	//}
	if (!m_iChance)
	{
		m_iChance = pOutcome->m_iChance;
		pOutcome->m_iChance = NULL;
	}
	GC.copyNonDefaultDelayedResolution((int*)&m_eUnitType, (int*)&(pOutcome->m_eUnitType));
	//if (m_eUnitType == NO_UNIT)
	//{
	//	m_eUnitType = pOutcome->getUnitType();
	//}
	if (!m_bUnitToCity)
	{
		m_bUnitToCity = pOutcome->m_bUnitToCity;
		pOutcome->m_bUnitToCity = NULL;
	}
	if(m_ePromotionType == -1){	m_ePromotionType = pOutcome->m_ePromotionType;}
	//if (m_ePromotionType == NO_PROMOTION)
	//{
	//	m_ePromotionType = pOutcome->getPromotionType();
	//}
	if(m_eBonusType == -1){	m_eBonusType = pOutcome->m_eBonusType;}
	//if (m_eBonusType == NO_BONUS)
	//{
	//	m_eBonusType = pOutcome->getBonusType();
	//}
	GC.copyNonDefaultDelayedResolution((int*)&m_eGPUnitType, (int*)&(pOutcome->m_eGPUnitType));
	if (m_iGPP == 0)
	{
		m_iGPP = pOutcome->getGPP();
		//m_eGPUnitType = pOutcome->getGPUnitType();
	}
	if (m_iHappinessTimer == 0)
	{
		m_iHappinessTimer = pOutcome->getHappinessTimer();
	}

	if (m_iPopulationBoost == 0)
	{
		m_iPopulationBoost = pOutcome->getPopulationBoost();
	}

	if (!m_iReduceAnarchyLength)
	{
		m_iReduceAnarchyLength = pOutcome->m_iReduceAnarchyLength;
		pOutcome->m_iReduceAnarchyLength = NULL;
	}

	GC.copyNonDefaultDelayedResolution((int*)&m_eEventTrigger,(int*)&(pOutcome->m_eEventTrigger));

	bool bDefault = true;
	for (int i=0; i<NUM_YIELD_TYPES; i++)
	{
		bDefault = bDefault && !m_aiYield[i];
	}
	if (bDefault)
	{
		for (int i=0; i<NUM_YIELD_TYPES; i++)
		{
			m_aiYield[i] = pOutcome->m_aiYield[i];
			pOutcome->m_aiYield[i] = NULL;
		}
	}
	bDefault = true;
	for (int i=0; i<NUM_COMMERCE_TYPES; i++)
	{
		bDefault = bDefault && !m_aiCommerce[i];
	}
	if (bDefault)
	{
		for (int i=0; i<NUM_COMMERCE_TYPES; i++)
		{
			m_aiCommerce[i] = pOutcome->m_aiCommerce[i];
			pOutcome->m_aiCommerce[i] = NULL;
		}
	}

	m_Properties.copyNonDefaults(pOutcome->getProperties(), pXML);

	if (!m_pPlotCondition)
	{
		m_pPlotCondition = pOutcome->m_pPlotCondition;
		pOutcome->m_pPlotCondition = NULL;
	}

	if (!m_pUnitCondition)
	{
		m_pUnitCondition = pOutcome->m_pUnitCondition;
		pOutcome->m_pUnitCondition = NULL;
	}

	if (m_szPythonCallback.empty()) m_szPythonCallback = pOutcome->m_szPythonCallback;
	if (!m_bKill) m_bKill = pOutcome->m_bKill;
	if (m_szPythonModuleName.empty()) m_szPythonModuleName = pOutcome->m_szPythonModuleName;
	if (m_szPythonCode.empty()) m_szPythonCode = pOutcome->m_szPythonCode;
	if (!m_pPythonAIFunc)
	{
		m_pPythonAIFunc = pOutcome->m_pPythonAIFunc;
		pOutcome->m_pPythonAIFunc = NULL;
	}
	if (!m_pPythonDisplayFunc)
	{
		m_pPythonDisplayFunc = pOutcome->m_pPythonDisplayFunc;
		pOutcome->m_pPythonDisplayFunc = NULL;
	}
	if (!m_pPythonExecFunc)
	{
		m_pPythonExecFunc = pOutcome->m_pPythonExecFunc;
		pOutcome->m_pPythonExecFunc = NULL;
	}
	if (!m_pPythonPossibleFunc)
	{
		m_pPythonPossibleFunc = pOutcome->m_pPythonPossibleFunc;
		pOutcome->m_pPythonPossibleFunc = NULL;
	}

}

void CvOutcome::buildDisplayString(CvWStringBuffer &szBuffer, const CvUnit& kUnit) const
{

	//CvPlayer& kPlayer = GET_PLAYER(kUnit.getOwnerINLINE());
	bool bToCoastalCity = GC.getOutcomeInfo(getType()).getToCoastalCity();
	//CvUnitInfo* pUnitInfo = &kUnit.getUnitInfo();

	szBuffer.append(GC.getOutcomeInfo(getType()).getText());
	szBuffer.append(L" ( ");

	bool bFirst = true;

	if (m_ePromotionType > NO_PROMOTION)
	{
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(GC.getPromotionInfo(m_ePromotionType).getDescription());
	}

	bool bUnitToCity = getUnitToCity(kUnit);
	if (GC.getGame().isOption(GAMEOPTION_TELEPORT_HUNTING_AWARDS) && 
		m_eUnitType > NO_UNIT && 
		(GC.getUnitInfo(m_eUnitType).hasUnitCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SUBDUED")) ||
		GC.getUnitInfo(m_eUnitType).hasUnitCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_IDEA"))))
	{
		bUnitToCity = true;
	}
	if (m_eUnitType > NO_UNIT && !bUnitToCity)
	{
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(GC.getUnitInfo(m_eUnitType).getDescription());
	}

	if ((m_aiYield[YIELD_PRODUCTION] && !m_aiYield[YIELD_PRODUCTION]->isConstantZero()) || (m_aiYield[YIELD_FOOD] && !m_aiYield[YIELD_FOOD]->isConstantZero()) ||
		(m_aiYield[YIELD_COMMERCE] && !m_aiYield[YIELD_COMMERCE]->isConstantZero()) || m_aiCommerce[COMMERCE_CULTURE] || m_iGPP || (bUnitToCity && m_eUnitType > NO_UNIT))
	{
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		if (m_aiYield[YIELD_PRODUCTION])
		{
			if (!m_aiYield[YIELD_PRODUCTION]->isConstantZero())
			{
				CvWString szTemp;
				szBuffer.append(L" ");
				m_aiYield[YIELD_PRODUCTION]->buildDisplayString(szBuffer);
				szTemp.Format(L"%c", GC.getYieldInfo(YIELD_PRODUCTION).getChar());
				szBuffer.append(szTemp);
			}
		}
		if (m_aiYield[YIELD_FOOD])
		{
			if (!m_aiYield[YIELD_FOOD]->isConstantZero())
			{
				CvWString szTemp;
				szBuffer.append(L" ");
				m_aiYield[YIELD_FOOD]->buildDisplayString(szBuffer);
				szTemp.Format(L"%c", GC.getYieldInfo(YIELD_FOOD).getChar());
				szBuffer.append(szTemp);
			}
		}

		if (m_aiYield[YIELD_COMMERCE])
		{
			if (!m_aiYield[YIELD_COMMERCE]->isConstantZero())
			{
				CvWString szTemp;
				szBuffer.append(L" ");
				m_aiYield[YIELD_COMMERCE]->buildDisplayString(szBuffer);
				szTemp.Format(L"%c", GC.getYieldInfo(YIELD_COMMERCE).getChar());
				szBuffer.append(szTemp);
			}
		}

		if (m_aiCommerce[COMMERCE_CULTURE])
		{
			if (!m_aiCommerce[COMMERCE_CULTURE]->isConstantZero())
			{
				CvWString szTemp;
				szBuffer.append(L" ");
				m_aiCommerce[COMMERCE_CULTURE]->buildDisplayString(szBuffer);
				szTemp.Format(L"%c", GC.getCommerceInfo(COMMERCE_CULTURE).getChar());
				szBuffer.append(szTemp);
			}
		}

		if (m_iGPP)
		{
			CvWString szTemp;
			szTemp.Format(L" %d%c", m_iGPP, gDLL->getSymbolID(GREAT_PEOPLE_CHAR));
			szBuffer.append(szTemp);
		}

		if (!m_Properties.isEmpty())
		{
			m_Properties.buildCompactChangesString(szBuffer);
		}

		if (m_iHappinessTimer)
		{
			int iHappy = GC.getDefineINT("TEMP_HAPPY");
			szBuffer.append(L" ");
			szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_TEMP_HAPPY", iHappy, m_iHappinessTimer));
		}

		if (m_iPopulationBoost)
		{
			szBuffer.append(L" ");
			szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_TEMP_POPULATION_BOOST", m_iPopulationBoost));
		}


		if (m_iReduceAnarchyLength)
		{
			szBuffer.append(L" -");
			m_iReduceAnarchyLength->buildDisplayString(szBuffer);
			szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_LESS_ANARCHY_DISPLAY"));
		}

		if (bUnitToCity && m_eUnitType > NO_UNIT)
		{
			szBuffer.append(L" ");
			szBuffer.append(GC.getUnitInfo(m_eUnitType).getDescription());
		}

		if (bToCoastalCity)
		{
			szBuffer.append(L" ");
			szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_NEAREST_COASTAL"));
		}
		else
		{
			szBuffer.append(L" ");
			szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_NEAREST_CITY"));
		}
	}

	//int iGoldTimes100 = 0;
	//int iResearchTimes100 = 0;

	//if (m_aiYield[YIELD_COMMERCE])
	//{
	//	iGoldTimes100 = m_aiYield[YIELD_COMMERCE] * kPlayer.getCommercePercent(COMMERCE_GOLD);
	//	iResearchTimes100 = m_aiYield[YIELD_COMMERCE] * kPlayer.getCommercePercent(COMMERCE_RESEARCH);
	//}

	//iGoldTimes100 += m_aiCommerce[COMMERCE_GOLD] * 100;
	//iResearchTimes100 += m_aiCommerce[COMMERCE_RESEARCH] * 100;
	
	//if (iGoldTimes100)
	if (m_aiCommerce[COMMERCE_GOLD])
	{
		if (!m_aiCommerce[COMMERCE_GOLD]->isConstantZero())
		{
			if (!bFirst)
			{
				szBuffer.append(L", ");
			}
			else
			{
				bFirst = false;
			}
			CvWString szTemp;
			szBuffer.append(L" ");
			m_aiCommerce[COMMERCE_GOLD]->buildDisplayString(szBuffer);
			szTemp.Format(L"%c", GC.getCommerceInfo(COMMERCE_GOLD).getChar());
			szBuffer.append(szTemp);
		}
	}
	//CvTeam& kTeam = GET_TEAM(kUnit.getTeam());
	//if (iResearchTimes100)
	if (m_aiCommerce[COMMERCE_RESEARCH])
	{
		if (!m_aiCommerce[COMMERCE_RESEARCH]->isConstantZero())
		{
			if (!bFirst)
			{
				szBuffer.append(L", ");
			}
			else
			{
				bFirst = false;
			}
			CvWString szTemp;
			szBuffer.append(L" ");
			m_aiCommerce[COMMERCE_RESEARCH]->buildDisplayString(szBuffer);
			szTemp.Format(L"%c", GC.getCommerceInfo(COMMERCE_RESEARCH).getChar());
			szBuffer.append(szTemp);
		}
	}

	if (m_eBonusType != NO_BONUS)
	{
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(GC.getBonusInfo(m_eBonusType).getDescription());
	}

	if (m_eEventTrigger != NO_EVENTTRIGGER)
	{
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(GC.getEventTriggerInfo(m_eEventTrigger).getDescription());
	}

	if (m_bKill)
	{
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}
		szBuffer.append(gDLL->getText("TXT_KEY_OUTCOME_KILLS_UNIT"));
	}

	if (m_pPythonDisplayFunc)
	{
		if (!bFirst)
		{
			szBuffer.append(L", ");
		}
		else
		{
			bFirst = false;
		}

		CyUnit cyUnit(const_cast<CvUnit*>(&kUnit));
		PyObject* pyUnit = gDLL->getPythonIFace()->makePythonObject(&cyUnit);
		CyPlot cyPlot(kUnit.plot());
		PyObject* pyPlot = gDLL->getPythonIFace()->makePythonObject(&cyPlot);

		PyObject* pyResult = PyObject_CallFunctionObjArgs(m_pPythonDisplayFunc, pyUnit, pyPlot, NULL);
		szBuffer.append(CvWString(boost::python::extract<std::wstring>(pyResult)));

		Py_XDECREF(pyResult);
		Py_DECREF(pyUnit);
		Py_DECREF(pyPlot);
	}

	szBuffer.append(L" )");
}

void CvOutcome::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_eType);
	m_iChance->getCheckSum(iSum);
	CheckSum(iSum, m_eUnitType);
	if (m_bUnitToCity)
		m_bUnitToCity->getCheckSum(iSum);
	CheckSum(iSum, m_ePromotionType);
	CheckSum(iSum, m_eBonusType);
	CheckSum(iSum, m_iGPP);
	CheckSum(iSum, m_eGPUnitType);
	for (int i=0; i<NUM_YIELD_TYPES; i++)
	{
		if (m_aiYield[i])
			m_aiYield[i]->getCheckSum(iSum);
	}
	for (int i=0; i<NUM_COMMERCE_TYPES; i++)
	{
		if (m_aiCommerce[i])
			m_aiCommerce[i]->getCheckSum(iSum);
	}
	CheckSum(iSum, m_iHappinessTimer);
	CheckSum(iSum, m_iPopulationBoost);
	if (m_iReduceAnarchyLength)
		m_iReduceAnarchyLength->getCheckSum(iSum);
	m_Properties.getCheckSum(iSum);
	if (m_pPlotCondition)
		m_pPlotCondition->getCheckSum(iSum);
	if (m_pUnitCondition)
		m_pUnitCondition->getCheckSum(iSum);
	CheckSumC(iSum, m_szPythonCallback);
	CheckSum(iSum, m_bKill);
	CheckSum(iSum, m_szPythonCode);
	CheckSum(iSum, m_szPythonModuleName);
}