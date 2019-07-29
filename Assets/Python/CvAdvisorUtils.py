##
## CvAdvisorUtils

##  - Unoficial Patch Added
##  - Platyping's efficency code modifications added



from CvPythonExtensions import *

gc = CyGlobalContext()
localText = CyTranslator()


g_iAdvisorNags = 0
g_listNoLiberateCities = []
lPopulation = [	[2000000000, FeatTypes.FEAT_POPULATION_2_BILLION, "TXT_KEY_FEAT_2_BILLION"],
		[1000000000, FeatTypes.FEAT_POPULATION_1_BILLION, "TXT_KEY_FEAT_1_BILLION"],
		[500000000, FeatTypes.FEAT_POPULATION_500_MILLION, "TXT_KEY_FEAT_500_MILLION"],
		[200000000, FeatTypes.FEAT_POPULATION_200_MILLION, "TXT_KEY_FEAT_200_MILLION"],
		[100000000, FeatTypes.FEAT_POPULATION_100_MILLION, "TXT_KEY_FEAT_100_MILLION"],
		[50000000, FeatTypes.FEAT_POPULATION_50_MILLION, "TXT_KEY_FEAT_50_MILLION"],
		[20000000, FeatTypes.FEAT_POPULATION_20_MILLION, "TXT_KEY_FEAT_20_MILLION"],
		[10000000, FeatTypes.FEAT_POPULATION_10_MILLION, "TXT_KEY_FEAT_10_MILLION"],
		[5000000, FeatTypes.FEAT_POPULATION_5_MILLION, "TXT_KEY_FEAT_5_MILLION"],
		[2000000, FeatTypes.FEAT_POPULATION_2_MILLION, "TXT_KEY_FEAT_2_MILLION"],
		[1000000, FeatTypes.FEAT_POPULATION_1_MILLION, "TXT_KEY_FEAT_1_MILLION"],
		[500000, FeatTypes.FEAT_POPULATION_HALF_MILLION, "TXT_KEY_FEAT_HALF_MILLION"]]
lCorporations = []
lBonus = []
lUnitCombat = []

def featPopup(iPlayer):
	if not gc.getPlayer(iPlayer).isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
		return False
	if not gc.getPlayer(iPlayer).isHuman():
		return False
	if CyGame().isNetworkMultiPlayer():
		return False
	if CyGame().getElapsedGameTurns() == 0:
		return False
	return True


def resetAdvisorNags():
	global g_iAdvisorNags
	g_iAdvisorNags = 0

def resetNoLiberateCities():
	global g_listNoLiberateCities
	g_listNoLiberateCities = []

	global lCorporations
	lCorporations = []
	for iI in xrange(gc.getNumBuildingInfos()):
		Info = gc.getBuildingInfo(iI)
		eCorporation = Info.getFoundsCorporation()
		if eCorporation > -1 and not CyGame().isCorporationFounded(eCorporation):
			lTechs = []
			iTech = Info.getPrereqAndTech()
			if iTech > -1:
				lTechs.append(iTech)
			for iPrereq in xrange(gc.getDefineINT("NUM_BUILDING_AND_TECH_PREREQS")):
				iTech = Info.getPrereqAndTechs(iPrereq)
				if iTech > -1:
					lTechs.append(iTech)

			iUnit = -1
			for i in xrange(gc.getNumUnitInfos()):
				if gc.getUnitInfo(i).getBuildings(iI):
					iUnit = i
					break
			if iUnit == -1: continue

			lTemp = []
			for iPrereq in xrange(gc.getDefineINT("NUM_CORPORATION_PREREQ_BONUSES")):
				eBonus = gc.getCorporationInfo(eCorporation).getPrereqBonus(iPrereq)
				if eBonus > -1:
					lTemp.append(eBonus)
			if len(lTemp) == 0: continue

			lCorporations.append([eCorporation, lTechs, iUnit, lTemp])
	global lBonus
	lBonus = []
	lLuxury = []
	lFood = []
	for i in xrange(gc.getNumBonusInfos()):
		if gc.getBonusInfo(i).getHappiness() > 0:
			lLuxury.append(i)
		if gc.getBonusInfo(i).getHealth() > 0:
			lFood.append(i)
	iBonus = gc.getInfoTypeForString("BONUS_COPPER")
	if iBonus > -1:
		lBonus.append([FeatTypes.FEAT_COPPER_CONNECTED, [iBonus], "TXT_KEY_FEAT_COPPER_CONNECTED"])
	iBonus = gc.getInfoTypeForString("BONUS_HORSE")
	if iBonus > -1:
		lBonus.append([FeatTypes.FEAT_HORSE_CONNECTED, [iBonus], "TXT_KEY_FEAT_HORSE_CONNECTED"])
	iBonus = gc.getInfoTypeForString("BONUS_IRON")
	if iBonus > -1:
		lBonus.append([FeatTypes.FEAT_IRON_CONNECTED, [iBonus], "TXT_KEY_FEAT_IRON_CONNECTED"])
	if len(lLuxury) > 0:
		lBonus.append([FeatTypes.FEAT_LUXURY_CONNECTED, lLuxury, "TXT_KEY_FEAT_LUXURY_CONNECTED"])
	if len(lFood) > 0:
		lBonus.append([FeatTypes.FEAT_FOOD_CONNECTED, lFood, "TXT_KEY_FEAT_FOOD_CONNECTED"])

	global lUnitCombat
	lUnitCombat = []
	for i in xrange(gc.getNumUnitCombatInfos()):
		lUnitCombat.append([-1, ""])

	iCombat = gc.getInfoTypeForString("UNITCOMBAT_ARCHER")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_ARCHER, "TXT_KEY_FEAT_UNITCOMBAT_ARCHER"]
	iCombat = gc.getInfoTypeForString("UNITCOMBAT_MOUNTED")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_MOUNTED, "TXT_KEY_FEAT_UNITCOMBAT_MOUNTED"]
	iCombat = gc.getInfoTypeForString("UNITCOMBAT_MELEE")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_MELEE, "TXT_KEY_FEAT_UNITCOMBAT_MELEE"]
	iCombat = gc.getInfoTypeForString("UNITCOMBAT_SIEGE")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_SIEGE, "TXT_KEY_FEAT_UNITCOMBAT_SIEGE"]
	iCombat = gc.getInfoTypeForString("UNITCOMBAT_GUN")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_GUN, "TXT_KEY_FEAT_UNITCOMBAT_GUN"]
	iCombat = gc.getInfoTypeForString("UNITCOMBAT_ARMOR")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_ARMOR, "TXT_KEY_FEAT_UNITCOMBAT_ARMOR"]
	iCombat = gc.getInfoTypeForString("UNITCOMBAT_HELICOPTER")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_HELICOPTER, "TXT_KEY_FEAT_UNITCOMBAT_HELICOPTER"]
	iCombat = gc.getInfoTypeForString("UNITCOMBAT_NAVAL")
	if iCombat > -1:
		lUnitCombat[iCombat] = [FeatTypes.FEAT_UNITCOMBAT_NAVAL, "TXT_KEY_FEAT_UNITCOMBAT_NAVAL"]


def unitBuiltFeats(pCity, pUnit):
	iCombat = pUnit.getUnitCombatType()
	iPlayer = pCity.getOwner()
	pPlayer = gc.getPlayer(iPlayer)
	if iCombat > -1:
		iFeat = lUnitCombat[iCombat][0]
		if iFeat > -1:
			if not pPlayer.isFeatAccomplished(lUnitCombat[iCombat][0]):
				pPlayer.setFeatAccomplished(lUnitCombat[iCombat][0], True)
				if featPopup(iPlayer) and gc.getGame().getStartYear() == gc.getDefineINT("START_YEAR"):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
					popupInfo.setData1(lUnitCombat[iCombat][0])
					popupInfo.setData2(pCity.getID())
					popupInfo.setText(localText.getText(lUnitCombat[iCombat][1], (pUnit.getNameKey(), pCity.getNameKey(),)))
					popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
					popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
					popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
					popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
					popupInfo.addPopup(iPlayer)

	if not pPlayer.isFeatAccomplished(FeatTypes.FEAT_UNIT_PRIVATEER):
		if gc.getUnitInfo(pUnit.getUnitType()).isHiddenNationality() and pUnit.getDomainType() == DomainTypes.DOMAIN_SEA:
			pPlayer.setFeatAccomplished(FeatTypes.FEAT_UNIT_PRIVATEER, True)
			if featPopup(iPlayer) and CyGame().getStartYear() == gc.getDefineINT("START_YEAR"):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(FeatTypes.FEAT_UNIT_PRIVATEER)
				popupInfo.setData2(pCity.getID())
				popupInfo.setText(localText.getText("TXT_KEY_FEAT_UNIT_PRIVATEER", (pUnit.getNameKey(), pCity.getNameKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(iPlayer)

	if not pPlayer.isFeatAccomplished(FeatTypes.FEAT_UNIT_SPY):
		if gc.getUnitInfo(pUnit.getUnitType()).isSpy():
			pPlayer.setFeatAccomplished(FeatTypes.FEAT_UNIT_SPY, True)
			if featPopup(iPlayer) and CyGame().getStartYear() == gc.getDefineINT("START_YEAR"):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(FeatTypes.FEAT_UNIT_SPY)
				popupInfo.setData2(pCity.getID())
				popupInfo.setText(localText.getText("TXT_KEY_FEAT_UNIT_SPY", (pUnit.getNameKey(), pCity.getNameKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(iPlayer)

def buildingBuiltFeats(pCity, iBuildingType):
	if not gc.getPlayer(pCity.getOwner()).isFeatAccomplished(FeatTypes.FEAT_NATIONAL_WONDER):
		if isNationalWonderClass(gc.getBuildingInfo(iBuildingType).getBuildingClassType()):
			gc.getPlayer(pCity.getOwner()).setFeatAccomplished(FeatTypes.FEAT_NATIONAL_WONDER, True)

			if (featPopup(pCity.getOwner()) and (gc.getGame().getStartYear() == gc.getDefineINT("START_YEAR"))):
				popupInfo = CyPopupInfo()
				popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
				popupInfo.setData1(FeatTypes.FEAT_NATIONAL_WONDER)
				popupInfo.setData2(pCity.getID())
				popupInfo.setText(localText.getText("TXT_KEY_FEAT_NATIONAL_WONDER", (gc.getBuildingInfo(iBuildingType).getTextKey(), pCity.getNameKey(), )))
				popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
				popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
				popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
				popupInfo.addPopup(pCity.getOwner())

def populationFeat(iPlayer, eFeat, szText):
	gc.getPlayer(iPlayer).setFeatAccomplished(eFeat, True)
	if featPopup(iPlayer):
		popupInfo = CyPopupInfo()
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
		popupInfo.setData1(eFeat)
		popupInfo.setText(localText.getText(szText, (gc.getPlayer(iPlayer).getCivilizationDescriptionKey(), )))
		popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
		popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
		popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
		popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
		popupInfo.addPopup(iPlayer)


def endTurnFeats(iPlayer):
	global g_listPopulationFeats
	lRealPopulation = gc.getPlayer(iPlayer).getRealPopulation()
	pPlayer = gc.getPlayer(iPlayer)
	for item in lPopulation:
		if pPlayer.isFeatAccomplished(item[1]): break
		if lRealPopulation > item[0]:
			populationFeat(iPlayer, item[1], item[2])

	pCapitalCity = pPlayer.getCapitalCity()
	if pCapitalCity.isNone(): return

	if not pPlayer.isFeatAccomplished(FeatTypes.FEAT_TRADE_ROUTE):
		(pCity, iter) = pPlayer.firstCity(False)
		while(pCity):
			if not pCity.isCapital():
				if pCity.isConnectedToCapital(iPlayer):
					pPlayer.setFeatAccomplished(FeatTypes.FEAT_TRADE_ROUTE, True)
					if featPopup(iPlayer) and CyGame().getStartYear() == gc.getDefineINT("START_YEAR"):
						popupInfo = CyPopupInfo()
						popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
						popupInfo.setData1(FeatTypes.FEAT_TRADE_ROUTE)
						popupInfo.setData2(pCity.getID())
						popupInfo.setText(localText.getText("TXT_KEY_FEAT_TRADE_ROUTE", (pCity.getNameKey(), )))
						popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
						popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
						popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
						popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
						popupInfo.addPopup(iPlayer)

					break
			(pCity, iter) = pPlayer.nextCity(iter, False)

	for item in lBonus:
		if pPlayer.isFeatAccomplished(item[0]): continue
		for iBonus in item[1]:
			if pCapitalCity.hasBonus(iBonus):
				pPlayer.setFeatAccomplished(item[0], True)
				if featPopup(iPlayer) and CyGame().getStartYear() == gc.getDefineINT("START_YEAR"):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
					popupInfo.setData1(item[0])
					popupInfo.setData2(pCapitalCity.getID())
					popupInfo.setText(localText.getText(item[2], (gc.getBonusInfo(iBonus).getTextKey(),)))
					popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
					popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
					popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
					popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
					popupInfo.addPopup(iPlayer)
					break

	if not pPlayer.isFeatAccomplished(FeatTypes.FEAT_CORPORATION_ENABLED):
		global lCorporations
		eTeam = pPlayer.getTeam()
		pTeam = gc.getTeam(eTeam)
		i = 0
		while i < len(lCorporations):
			item = lCorporations[i]
			if CyGame().isCorporationFounded(item[0]):
				del lCorporations[i]
			else:
				bValid = True
				for iTech in item[1]:
					if not pTeam.isHasTech(iTech):
						bValid = False
						break
				if bValid:
					pPlayer.setFeatAccomplished(FeatTypes.FEAT_CORPORATION_ENABLED, True)
					szBonusList = u""
					for j in xrange(len(item[3])):
						eBonus = item[3][j]
						szBonusList += gc.getBonusInfo(eBonus).getDescription()
						if j != len(item[3]) - 1:
							szBonusList += CyTranslator().getText("TXT_KEY_OR", ())

					szFounder = gc.getUnitInfo(item[2]).getTextKey()

					if featPopup(iPlayer) and CyGame().getStartYear() == gc.getDefineINT("START_YEAR"):
						popupInfo = CyPopupInfo()
						popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
						popupInfo.setData1(FeatTypes.FEAT_CORPORATION_ENABLED)
						popupInfo.setData2(pCapitalCity.getID())
						popupInfo.setText(localText.getText("TXT_KEY_FEAT_CORPORATION_ENABLED", (item[0], szFounder, szBonusList)))
						popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
						popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
						popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
						popupInfo.addPythonButton(localText.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
						popupInfo.addPopup(iPlayer)
					break
				i += 1

def cityAdvise(pCity, iPlayer):

	global g_iAdvisorNags

	if (g_iAdvisorNags >= 2):
		return

	if (pCity.isDisorder()):
		return

	if (gc.getPlayer(iPlayer).isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS) and gc.getPlayer(iPlayer).isHuman() and not gc.getGame().isNetworkMultiPlayer()):

		if (gc.getGame().getGameTurn() % 40 == pCity.getGameTurnFounded() % 40):
			if (not pCity.getID() in g_listNoLiberateCities):
				eLiberationPlayer = pCity.getLiberationPlayer(False)
				if (eLiberationPlayer != -1):
					# UNOFFICIAL_PATCH begin
					if( gc.getTeam(gc.getPlayer(iPlayer).getTeam()).isHasMet(gc.getPlayer(eLiberationPlayer).getTeam()) ) :
						if not gc.getTeam(gc.getPlayer(eLiberationPlayer).getTeam()).isAtWar(CyGame().getActiveTeam()) :
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_LIBERATION_DEMAND", (pCity.getNameKey(), gc.getPlayer(eLiberationPlayer).getCivilizationDescriptionKey(), gc.getPlayer(eLiberationPlayer).getNameKey())))
							popupInfo.setOnClickedPythonCallback("liberateOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_listNoLiberateCities.append(pCity.getID())
							g_iAdvisorNags += 1
					# UNOFFICIAL_PATCH end

				elif (gc.getPlayer(iPlayer).canSplitEmpire() and gc.getPlayer(iPlayer).canSplitArea(pCity.area().getID()) and pCity.AI_cityValue() < 0):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
					popupInfo.setData1(pCity.getID())
					popupInfo.setText(localText.getText("TXT_KEY_POPUP_COLONY_DEMAND", (pCity.getNameKey(), )))
					popupInfo.setOnClickedPythonCallback("colonyOnClickedCallback")
					popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
					popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
					popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
					popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
					popupInfo.addPopup(iPlayer)
					g_listNoLiberateCities.append(pCity.getID())
					g_iAdvisorNags += 1

		if (pCity.isProduction()):

			if (not pCity.isProductionUnit() and (pCity.getOrderQueueLength() <= 1)):

				if (gc.getGame().getGameTurn() + 3) % 40 == pCity.getGameTurnFounded() % 40:

					if ((gc.getGame().getElapsedGameTurns() < 200) and (pCity.getPopulation() > 2) and (gc.getPlayer(iPlayer).AI_totalAreaUnitAIs(pCity.area(), UnitAITypes.UNITAI_SETTLE) == 0) and not gc.getPlayer(iPlayer).AI_isFinancialTrouble() and (pCity.area().getBestFoundValue(iPlayer) > 0)):

						iBestValue = 0
						eBestUnit = UnitTypes.NO_UNIT

						for iI in range(gc.getNumUnitClassInfos()):

							if (not isLimitedUnitClass(iI)):

								eLoopUnit = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationUnits(iI)

								if (eLoopUnit != UnitTypes.NO_UNIT):

									if (gc.getUnitInfo(eLoopUnit).getDomainType() == DomainTypes.DOMAIN_LAND):

										if pCity.canTrain(eLoopUnit, False, False, False, False):

											if (pCity.getFirstUnitOrder(eLoopUnit) == -1):

												iValue = gc.getPlayer(iPlayer).AI_unitValue(eLoopUnit, UnitAITypes.UNITAI_SETTLE, pCity.area())

												if (iValue > iBestValue):

													iBestValue = iValue
													eBestUnit = eLoopUnit

						if (eBestUnit != UnitTypes.NO_UNIT):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_TRAIN)
							popupInfo.setData3(eBestUnit)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_UNIT_SETTLE_DEMAND", (gc.getUnitInfo(eBestUnit).getTextKey(), )))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (gc.getGame().getGameTurn() + 15) % 40 == pCity.getGameTurnFounded() % 40:

					if ((pCity.getPopulation() > 1) and (pCity.countNumImprovedPlots() == 0) and (pCity.AI_countBestBuilds(pCity.area()) > 3)):

						iBestValue = 0
						eBestUnit = UnitTypes.NO_UNIT

						for iI in range(gc.getNumUnitClassInfos()):

							if (not isLimitedUnitClass(iI)):

								eLoopUnit = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationUnits(iI)

								if (eLoopUnit != UnitTypes.NO_UNIT):

									if (gc.getUnitInfo(eLoopUnit).getDomainType() == DomainTypes.DOMAIN_LAND):

										if pCity.canTrain(eLoopUnit, False, False, False, False):

											if (pCity.getFirstUnitOrder(eLoopUnit) == -1):

												iValue = gc.getPlayer(iPlayer).AI_unitValue(eLoopUnit, UnitAITypes.UNITAI_WORKER, pCity.area())

												if (iValue > iBestValue):

													iBestValue = iValue
													eBestUnit = eLoopUnit

						if (eBestUnit != UnitTypes.NO_UNIT):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_TRAIN)
							popupInfo.setData3(eBestUnit)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_UNIT_WORKER_DEMAND", (pCity.getNameKey(), gc.getUnitInfo(eBestUnit).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (gc.getGame().getGameTurn() + 27) % 40 == pCity.getGameTurnFounded() % 40:

					if (pCity.plot().getNumDefenders(iPlayer) == 0):

						iBestValue = 0
						eBestUnit = UnitTypes.NO_UNIT

						for iI in range(gc.getNumUnitClassInfos()):

							if (not isLimitedUnitClass(iI)):

								eLoopUnit = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationUnits(iI)

								if (eLoopUnit != UnitTypes.NO_UNIT):

									if (gc.getUnitInfo(eLoopUnit).getDomainType() == DomainTypes.DOMAIN_LAND):

										if pCity.canTrain(eLoopUnit, False, False, False, False):

											iValue = (gc.getPlayer(iPlayer).AI_unitValue(eLoopUnit, UnitAITypes.UNITAI_CITY_DEFENSE, pCity.area()) * 2)
											iValue += gc.getPlayer(iPlayer).AI_unitValue(eLoopUnit, UnitAITypes.UNITAI_ATTACK, pCity.area())

											if (iValue > iBestValue):

												iBestValue = iValue
												eBestUnit = eLoopUnit

						if (eBestUnit != UnitTypes.NO_UNIT):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_TRAIN)
							popupInfo.setData3(eBestUnit)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_UNIT_DEFENSE_DEMAND", (pCity.getNameKey(), gc.getUnitInfo(eBestUnit).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (gc.getGame().getGameTurn() + 36) % 40 == pCity.getGameTurnFounded() % 40:

					if ((gc.getPlayer(iPlayer).AI_totalAreaUnitAIs(pCity.area(), UnitAITypes.UNITAI_MISSIONARY) == 0) and (gc.getTeam(gc.getPlayer(iPlayer).getTeam()).getAtWarCount(True) == 0)):

						eStateReligion = gc.getPlayer(iPlayer).getStateReligion()

						if (eStateReligion != ReligionTypes.NO_RELIGION):

							if (gc.getPlayer(iPlayer).getHasReligionCount(eStateReligion) < (gc.getPlayer(iPlayer).getNumCities() / 2)):

								iBestValue = 0
								eBestUnit = UnitTypes.NO_UNIT

								for iI in range(gc.getNumUnitClassInfos()):

									eLoopUnit = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationUnits(iI)

									if (eLoopUnit != UnitTypes.NO_UNIT):

										if (gc.getUnitInfo(eLoopUnit).getDomainType() == DomainTypes.DOMAIN_LAND):

											if (gc.getUnitInfo(eLoopUnit).getReligionSpreads(eStateReligion)):

												if pCity.canTrain(eLoopUnit, False, False, False, False):

													iValue = gc.getPlayer(iPlayer).AI_unitValue(eLoopUnit, UnitAITypes.UNITAI_MISSIONARY, pCity.area())

													if (iValue > iBestValue):

														iBestValue = iValue
														eBestUnit = eLoopUnit

								if (eBestUnit != UnitTypes.NO_UNIT):
									popupInfo = CyPopupInfo()
									popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
									popupInfo.setData1(pCity.getID())
									popupInfo.setData2(OrderTypes.ORDER_TRAIN)
									popupInfo.setData3(eBestUnit)
									popupInfo.setText(localText.getText("TXT_KEY_POPUP_MISSIONARY_DEMAND", (gc.getReligionInfo(eStateReligion).getTextKey(), gc.getUnitInfo(eBestUnit).getTextKey(), pCity.getNameKey())))
									popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
									popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
									popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
									popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
									popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
									popupInfo.addPopup(iPlayer)
									g_iAdvisorNags += 1

			if (not pCity.isProductionBuilding() and (pCity.getOrderQueueLength() <= 1)):

				if (pCity.healthRate(False, 0) < 0):

					if (gc.getGame().getGameTurn() + 6) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							if (not isLimitedWonderClass(iI)):

								eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

								if (eLoopBuilding != BuildingTypes.NO_BUILDING):

									if (gc.getBuildingInfo(eLoopBuilding).getHealth() > 0):

										if pCity.canConstruct(eLoopBuilding, False, False, False):

											iValue = gc.getBuildingInfo(eLoopBuilding).getHealth()

											if (iValue > iBestValue):

												iBestValue = iValue
												eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_UNHEALTHY_CITIZENS_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_UNHEALTHY_DO_SO_NEXT", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_UNHEALTHY_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_UNHEALTHY_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (pCity.angryPopulation(0) > 0):

					if (gc.getGame().getGameTurn() + 9) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							if (not isLimitedWonderClass(iI)):

								eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

								if (eLoopBuilding != BuildingTypes.NO_BUILDING):

									if (gc.getBuildingInfo(eLoopBuilding).getHappiness() > 0):

										if pCity.canConstruct(eLoopBuilding, False, False, False):

											iValue = gc.getBuildingInfo(eLoopBuilding).getHappiness()

											if (iValue > iBestValue):

												iBestValue = iValue
												eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_UNHAPPY_CITIZENS_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_UNHAPPY_DO_SO_NEXT", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_UNHAPPY_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_UNHEALTHY_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if ((gc.getGame().getGameTurn < 100) and (gc.getTeam(gc.getPlayer(iPlayer).getTeam()).getHasMetCivCount(True) > 0) and (pCity.getBuildingDefense() == 0)):

					if (gc.getGame().getGameTurn() + 12) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							if (not isLimitedWonderClass(iI)):

								eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

								if (eLoopBuilding != BuildingTypes.NO_BUILDING):

									if (gc.getBuildingInfo(eLoopBuilding).getDefenseModifier() > pCity.getNaturalDefense()):

										if pCity.canConstruct(eLoopBuilding, False, False, False):

											iValue = gc.getBuildingInfo(eLoopBuilding).getDefenseModifier()

											if (iValue > iBestValue):

												iBestValue = iValue
												eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_BUILDING_DEFENSE_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (pCity.getMaintenance() >= 8):

					if (gc.getGame().getGameTurn() + 18) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							if (not isLimitedWonderClass(iI)):

								eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

								if (eLoopBuilding != BuildingTypes.NO_BUILDING):

									if (gc.getBuildingInfo(eLoopBuilding).getMaintenanceModifier() < 0):

										if pCity.canConstruct(eLoopBuilding, False, False, False):

											iValue = gc.getBuildingInfo(eLoopBuilding).getMaintenanceModifier()

											if (iValue < iBestValue):

												iBestValue = iValue
												eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_MAINTENANCE_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (pCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE) == 0 and not pCity.isOccupation()):

					if (gc.getGame().getGameTurn() + 21) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							if (not isLimitedWonderClass(iI)):

								eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

								if (eLoopBuilding != BuildingTypes.NO_BUILDING):

									if (gc.getBuildingInfo(eLoopBuilding).getObsoleteSafeCommerceChange(CommerceTypes.COMMERCE_CULTURE) > 0):

										if pCity.canConstruct(eLoopBuilding, False, False, False):

											iValue = gc.getBuildingInfo(eLoopBuilding).getObsoleteSafeCommerceChange(CommerceTypes.COMMERCE_CULTURE)

											if (iValue > iBestValue):

												iBestValue = iValue
												eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_CULTURE_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (pCity.getBaseCommerceRate(CommerceTypes.COMMERCE_GOLD) > 10):

					if (gc.getGame().getGameTurn() + 24) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							if (not isLimitedWonderClass(iI)):

								eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

								if (eLoopBuilding != BuildingTypes.NO_BUILDING):

									if (gc.getBuildingInfo(eLoopBuilding).getCommerceModifier(CommerceTypes.COMMERCE_GOLD) > 0):

										if pCity.canConstruct(eLoopBuilding, False, False, False):

											iValue = gc.getBuildingInfo(eLoopBuilding).getCommerceModifier(CommerceTypes.COMMERCE_GOLD)

											if (iValue > iBestValue):

												iBestValue = iValue
												eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_GOLD_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (pCity.getBaseCommerceRate(CommerceTypes.COMMERCE_RESEARCH) > 10):

					if (gc.getGame().getGameTurn() + 30) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							if (not isLimitedWonderClass(iI)):

								eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

								if (eLoopBuilding != BuildingTypes.NO_BUILDING):

									if (gc.getBuildingInfo(eLoopBuilding).getCommerceModifier(CommerceTypes.COMMERCE_RESEARCH) > 0):

										if pCity.canConstruct(eLoopBuilding, False, False, False):

											iValue = gc.getBuildingInfo(eLoopBuilding).getCommerceModifier(CommerceTypes.COMMERCE_RESEARCH)

											if (iValue > iBestValue):

												iBestValue = iValue
												eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_RESEARCH_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1

				if (pCity.countNumWaterPlots() > 10):

					if (gc.getGame().getGameTurn() + 33) % 40 == pCity.getGameTurnFounded() % 40:

						iBestValue = 0
						eBestBuilding = BuildingTypes.NO_BUILDING

						for iI in range(gc.getNumBuildingClassInfos()):

							if (not isLimitedWonderClass(iI)):

								eLoopBuilding = gc.getCivilizationInfo(gc.getPlayer(iPlayer).getCivilizationType()).getCivilizationBuildings(iI)

								if (eLoopBuilding != BuildingTypes.NO_BUILDING):

									if (gc.getBuildingInfo(eLoopBuilding).getSeaPlotYieldChange(YieldTypes.YIELD_FOOD) > 0):

										if pCity.canConstruct(eLoopBuilding, False, False, False):

											iValue = gc.getBuildingInfo(eLoopBuilding).getSeaPlotYieldChange(YieldTypes.YIELD_FOOD)

											if (iValue > iBestValue):

												iBestValue = iValue
												eBestBuilding = eLoopBuilding

						if (eBestBuilding != BuildingTypes.NO_BUILDING):
							popupInfo = CyPopupInfo()
							popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
							popupInfo.setData1(pCity.getID())
							popupInfo.setData2(OrderTypes.ORDER_CONSTRUCT)
							popupInfo.setData3(eBestBuilding)
							popupInfo.setText(localText.getText("TXT_KEY_POPUP_WATER_FOOD_DEMAND", (pCity.getNameKey(), gc.getBuildingInfo(eBestBuilding).getTextKey())))
							popupInfo.setOnClickedPythonCallback("cityWarningOnClickedCallback")
							popupInfo.setOnFocusPythonCallback("cityWarningOnFocusCallback")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_AGREE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_REFUSE", ()), "")
							popupInfo.addPythonButton(localText.getText("TXT_KEY_POPUP_DEMAND_EXAMINE", ()), "")
							popupInfo.addPopup(iPlayer)
							g_iAdvisorNags += 1
