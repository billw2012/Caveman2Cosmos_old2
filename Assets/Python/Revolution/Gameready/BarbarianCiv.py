#-#
# BARBARIAN_CIV_MOD
'''
New world policy controls handling of barbcivs starting away from the original civs in the game.
	RevOpt.getNewWorldPolicy()
	1: A minor civ in the New World needs contact with a major civ to become a major civ, each contact increase the odds for it.
	2: Like 1, but a major civ must have units in the New World for major civs to emerge there.
	3: No civs from barbarian in the New World unless an Old World civ have units on the landmass.
	4: Like 3, but the requirement is cities instead of units.
'''

from CvPythonExtensions import *
import SdToolKit as SDTK

# globals
GC = CyGlobalContext()
MAP = GC.getMap()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

class BarbarianCiv:

	def __init__(self, customEM, RevOpt):
		self.RevOpt = RevOpt
		self.customEM = customEM
		self.BARBARIAN_PLAYER = GC.getBARBARIAN_PLAYER()
		self.MAX_PC_PLAYERS = GC.getMAX_PC_PLAYERS()
		self.NUM_UNIT_AND_TECH_PREREQS = GC.getDefineINT("NUM_UNIT_AND_TECH_PREREQS")

		self.customEM.addEventHandler("BeginGameTurn", self.onBeginGameTurn)

	def removeEventHandlers(self):
		self.customEM.removeEventHandler("BeginGameTurn", self.onBeginGameTurn)

	# Called at the beginning of the end of each turn
	def onBeginGameTurn(self, argsList):
		#iGameTurn, = argsList
		'''
		Check if any minor civs will become major.
		'''
		MAX_PC_PLAYERS = self.MAX_PC_PLAYERS

		for iPlayerX in xrange(MAX_PC_PLAYERS):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive() and CyPlayerX.isMinorCiv():
				self.checkMinorCivs(iPlayerX, CyPlayerX, MAX_PC_PLAYERS)
		'''
		Check if any barbarian cities will become a minor civ.
		'''
		fMod = self.RevOpt.getFormMinorModifier()
		if not fMod:
			return
		maxCivs = self.RevOpt.getBarbCivMaxCivs()
		if maxCivs < 1:
			maxCivs = MAX_PC_PLAYERS
		if GAME.countCivPlayersAlive() >= maxCivs: return

		iPlayerBarb = self.BARBARIAN_PLAYER
		CyPlayerBarb = GC.getPlayer(iPlayerBarb)
		iNumCities = CyPlayerBarb.getNumCities()
		if iNumCities < 2: return

		# Increase odds per barb city within reason.
		fMod *= iNumCities ** .5
		# Gamespeed factor
		iFactorGS = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getGrowthPercent()
		iRange = 10*iFactorGS
		iEra = GAME.getCurrentEra()

		iPolicy = self.RevOpt.getNewWorldPolicy()
		iMinPop = self.RevOpt.getMinPopulation() + iEra

		bNoGo = True
		CyCity, i = CyPlayerBarb.firstCity(False)
		while CyCity:
			iPop = CyCity.getPopulation()
			if iPop >= iMinPop:

				CyArea = CyCity.area()

				aList = self.getScenario(CyArea)
				if not aList: continue
				bNewWorld, bMajorCivCities, bMajorCivUnits = aList

				if bNewWorld:
					if iPolicy > 2 and not bMajorCivUnits:
						continue
					elif iPolicy > 3 and not bMajorCivCities:
						continue

				fOdds = 3*(1.0 + iPop - iMinPop)

				iTemp = CyCity.getCultureLevel()
				if iTemp > 0:
					fOdds += 16*iTemp

				if CyCity.getOriginalOwner() != iPlayerBarb:
					fOdds += 50

				iTemp = CyArea.getNumCities()
				if iTemp > 0:
					fOdds += iTemp

				if bNewWorld and not bMajorCivUnits:
					fOdds /= 4.0

				fOdds *= fMod
				if GAME.getSorenRandNum(int(iRange+fOdds), 'Barbarian city evolve') < fOdds:
					bNoGo = False
					break
			CyCity, i = CyPlayerBarb.nextCity(i, False)

		if bNoGo: return
		del iRange, fOdds, bNoGo
		'''
		Create Minor Civ
		'''
		POST_FIX = "\n\tBarbarianCiv.onBeginGameTurn"
		iPlayer = None
		iCivType = -1
		# Pick a vacant player slot
		CyPlayerCulture = None
		iCulture = 0
		iDeadCulture = 0
		aList = []
		# The dead civ with the highest culture in the city gets precedence.
		for iPlayerX in xrange(MAX_PC_PLAYERS):
			CyPlayerX = GC.getPlayer(iPlayerX)
			# Find player with highest culture.
			iCult = CyCity.getCulture(iPlayerX)
			if iCult > iCulture:
				iCulture = iCult
				CyPlayerCulture = CyPlayerX
			if CyPlayerX.isAlive(): continue
			if CyPlayerX.isEverAlive():
				if iCult > iDeadCulture:
					iDeadCulture = iCult
					aList = [iPlayerX, CyPlayerX]
			elif iPlayer is None and not SDTK.sdObjectExists('Revolution', CyPlayerX):
				# Empty slot
				iPlayer = iPlayerX
				CyPlayer = CyPlayerX
		if aList:
			iPlayer, CyPlayer = aList
			iCivType = CyPlayer.getCivilizationType()
			print "[INFO] Reincarnating dead player" + POST_FIX

		elif iPlayer is None:
			print "[WARNING] No available player slot found." + POST_FIX
			return

		if iCivType < 0:
			# Choose a civ for the new player
			aList = [] # Claimed civs
			for iPlayerX in xrange(MAX_PC_PLAYERS):
				if iPlayerX == iPlayer: continue
				iCivType = GC.getPlayer(iPlayerX).getCivilizationType()
				if iCivType > -1:
					aList.append(iCivType)

			civs = [] # Available civs
			for iCivType in xrange(GC.getNumCivilizationInfos()):
				if iCivType in aList: continue
				CvCivInfo = GC.getCivilizationInfo(iCivType)
				if not CvCivInfo.isPlayable(): continue
				civs.append(iCivType)

			if civs:
				# Civs with similar style to CyPlayerCulture, if they exist.
				if CyPlayerCulture:
					aList = []
					iArtStyle = GC.getCivilizationInfo(CyPlayerCulture.getCivilizationType()).getArtStyleType()
					for iCivType in civs:
						if GC.getCivilizationInfo(iCivType).getArtStyleType() == iArtStyle:
							aList.append(iCivType)
					if aList:
						civs = aList
				iCivType = civs[GAME.getSorenRandNum(len(civs),'Pick civ')]

		if iCivType < 0:
			print "[WARNING] Unexpected lack of unused civ types." + POST_FIX
			return
		del civs, CyPlayerCulture, iCult, iCulture, iDeadCulture

		# Choose a leader for the new civ
		aList = [] # Claimed Leaders
		for iPlayerX in xrange(MAX_PC_PLAYERS):
			if iPlayerX == iPlayer: continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if CyPlayerX.isAlive():
				iLeader = CyPlayerX.getLeaderType()
				if iLeader > -1: aList.append(iLeader)

		bLeadAnyCiv = GAME.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV)
		leaders = []
		for iLeader in xrange(GC.getNumLeaderHeadInfos()):
			if iLeader in aList: continue
			if bLeadAnyCiv:
				if not GC.getLeaderHeadInfo(iLeader).isNPC(): continue
			elif not GC.getCivilizationInfo(iCivType).isLeaders(iLeader): continue
			leaders.append(iLeader)

		if not leaders:
			print "[ERROR] Unexpected lack of possible leaders." + POST_FIX
			return
		iLeader = leaders[GAME.getSorenRandNum(len(leaders), 'Pick leader')]
		del leaders, bLeadAnyCiv

		iX = CyCity.getX()
		iY = CyCity.getY()
		CyPlot = CyCity.plot()

		print "[INFO] Adding new player in slot %d.%s" %(iPlayer, POST_FIX)
		szCityName = CyCity.getName()

		# Add player to game
		GAME.addPlayer(iPlayer, iLeader, iCivType, False)

		CyTeam = GC.getTeam(CyPlayer.getTeam())

		CyPlayer.setNewPlayerAlive(True)

		civName = CyPlayer.getCivilizationDescription(0)

		# Add replay message
		mess = TRNSLTR.getText("TXT_KEY_BARBCIV_FORM_MINOR", ()) %(civName, szCityName)
		mess = mess[0].capitalize() + mess[1:]
		GAME.addReplayMessage(ReplayMessageTypes.REPLAY_MESSAGE_MAJOR_EVENT, iPlayer, mess, iX, iY, GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"))

		# Using following method to acquire city produces 'revolted and joined' replay messages
		CyPlot.setOwner(iPlayer)

		# Note: city acquisition may invalidate previous city pointer, so have to create new list of cities
		CyCity = CyPlayer.getCapitalCity()

		iTemp = iFactorGS
		if iEra and not (bNewWorld and not bMajorCivUnits):
			iTemp *= iEra + 1
		CyPlayer.changeGold(iTemp)

		closeTeams = self.getCloseCivs(iPlayer, CyArea, iX, iY)
		# Give techs to new player, with variables for extra techs for builders.
		if bNewWorld:
			iMinEra = iEra - self.RevOpt.getNewWorldErasBehind()
			if iMinEra < 0:
				iMinEra = 0
			for iTech in xrange(GC.getNumTechInfos()):
				if CyTeam.isHasTech(iTech) or not CyPlayer.canEverResearch(iTech): continue
				if GC.getTechInfo(iTech).getEra() <= iMinEra:
					CyTeam.setHasTech(iTech, True, iPlayer, False, False)
		else:
			fNumTeams = GAME.countCivTeamsAlive() * 1.0
			fTechFrac = self.RevOpt.getBarbTechFrac()

			for iTech in xrange(GC.getNumTechInfos()):
				if CyTeam.isHasTech(iTech) or not CyPlayer.canEverResearch(iTech): continue

				fKnownRatio = GAME.countKnownTechNumTeams(iTech) / fNumTeams
				if fKnownRatio < 1 and closeTeams:
					iCount = 0
					iTemp = 0
					for iTeamX in closeTeams:
						iCount += 1
						CyTeamX = GC.getTeam(iTeamX)
						if CyTeamX.isHasTech(iTech):
							iTemp += 1

					fKnownRatio = fKnownRatio/2 + iTemp/(2.0*iCount)

				if fKnownRatio >= fTechFrac:
					CyTeam.setHasTech(iTech, True, iPlayer, False, False)

		CyTeam.setIsMinorCiv(True, False)
		# Remove initial units
		CyUnit, i = CyPlayer.firstUnit(False)
		while CyUnit:
			CyUnit.kill(False, -1)
			CyUnit, i = CyPlayer.nextUnit(i, False)

		# Units
		iNumBarbDefenders = GC.getHandicapInfo(GAME.getHandicapType()).getBarbarianInitialDefenders()

		iDefender, iCounter, iAttack, iMobile, iAttackCity, iWorker, iSettler, iExplorer, iMerchant = self.getUnitsForPlayer(iPlayer, CyTeam)

		# Put stuff in city
		fMilitaryMod = self.RevOpt.getMilitaryStrength()
		self.setupFormerBarbCity(CyCity, iPlayer, iDefender, int(iNumBarbDefenders*fMilitaryMod + 1))

		# Extra units
		iBaseOffensiveUnits = 2 + iEra + iNumBarbDefenders

		if bNewWorld:
			if bMajorCivCities:
				iBaseOffensiveUnits *= 4
			elif bMajorCivUnits:
				iBaseOffensiveUnits *= 2
			else: iBaseOffensiveUnits /= 3
		else:
			if iSettler > -1:
				CyPlayer.initUnit(iSettler, iX, iY, UnitAITypes.UNITAI_SETTLE, DirectionTypes.DIRECTION_SOUTH)
			if iWorker > -1:
				CyPlayer.initUnit(iWorker, iX, iY, UnitAITypes.UNITAI_WORKER, DirectionTypes.DIRECTION_SOUTH)
				CyPlayer.initUnit(iWorker, iX, iY, UnitAITypes.UNITAI_WORKER, DirectionTypes.DIRECTION_SOUTH)
			if iExplorer > -1:
				CyPlayer.initUnit(iExplorer, iX, iY, UnitAITypes.UNITAI_EXPLORE, DirectionTypes.DIRECTION_SOUTH)
				CyPlayer.initUnit(iExplorer, iX, iY, UnitAITypes.UNITAI_EXPLORE, DirectionTypes.DIRECTION_SOUTH)
			if iMerchant > -1:
				iTemp = 2 + 2*(iEra + 1)
				for i in xrange(iTemp):
					CyPlayer.initUnit(iMerchant, iX, iY, UnitAITypes.UNITAI_MERCHANT, DirectionTypes.DIRECTION_SOUTH)

		aList = [iCounter, iAttack, iAttackCity, iMobile]
		iTemp = int(iBaseOffensiveUnits*fMilitaryMod)
		for j in xrange(iTemp):
			iUnit = aList[GAME.getSorenRandNum(4, 'BC give offensive')]
			iUnitAI = GC.getUnitInfo(iUnit).getDefaultUnitAIType()
			CyUnit = CyPlayer.initUnit(iUnit, iX, iY, UnitAITypes(iUnitAI), DirectionTypes.DIRECTION_SOUTH)
			CyUnit.changeExperience(iEra + GAME.getSorenRandNum(2*(iEra+1), 'Experience'), -1, False, False, False)

		szTxt = TRNSLTR.getText("TXT_KEY_BARBCIV_WORD_SPREADS", ()) + " "
		szTxt += TRNSLTR.getText("TXT_KEY_BARBCIV_FORM_MINOR", ()) % (civName, szCityName)
		MSG_TIME = GC.getDefineINT("EVENT_MESSAGE_TIME")
		for iPlayerX in xrange(MAX_PC_PLAYERS):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isHuman(): continue
			iTeamX = CyPlayerX.getTeam()
			if GAME.isDebugMode() or iTeamX in closeTeams or CyCity.plot().isRevealed(iTeamX, False):
				CyInterface().addMessage(iPlayerX, False, MSG_TIME, szTxt, None, InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), -1, -1, False, False)


	def getScenario(self, CyArea):
		if CyArea == None:
			print "Error!  Passed a None area!"
			return None

		bMajorCivCities = False
		bMajorCivUnits = False
		iCount = 0
		iPowerArea = 0
		fPowerPlayer = 0.0

		# Smaller landmasses require less relative civ power to be considered Old World...
		fSizeMod = 1.0 * MAP.getLandPlots()/CyArea.getNumTiles()
		fSizeMod **= 0.9 # ...within reason.

		iAreaID = CyArea.getID()
		for iPlayerX in xrange(self.MAX_PC_PLAYERS):
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isAlive(): continue

			iPowerArea += CyArea.getPower(iPlayerX)
			fPowerPlayer += CyPlayerX.getPower()
			iCount += 1

			if CyPlayerX.isMinorCiv(): continue

			if not bMajorCivCities and CyArea.getCitiesPerPlayer(iPlayerX) > 0:
				bMajorCivCities = True
				bMajorCivUnits = True

			elif not bMajorCivUnits and CyArea.getUnitsPerPlayer(iPlayerX) > 0:
				bMajorCivUnits = True

		if iCount:
			bNewWorld = iPowerArea * fSizeMod < fPowerPlayer / iCount
		else: bNewWorld = True

		return [bNewWorld, bMajorCivCities, bMajorCivUnits]


	def getCloseCivs(self, iPlayer, CyArea, iX, iY):
		closeTeams = []
		iTemp = self.RevOpt.getCloseDist()
		iTemp *= GC.getWorldInfo(MAP.getWorldSize()).getDefaultPlayers() - 1
		for iPlayerX in xrange(self.MAX_PC_PLAYERS):
			if iPlayerX == iPlayer: continue
			if CyArea.getCitiesPerPlayer(iPlayerX) > 0:
				CyPlayerX = GC.getPlayer(iPlayerX)
				iTeamX = CyPlayerX.getTeam()
				if iTeamX in closeTeams: continue
				CyCity = CyPlayerX.getCapitalCity()
				iDist = plotDistance(iX, iY, CyCity.getX(), CyCity.getY())
				if iDist < iTemp: closeTeams.append(iTeamX)
		return closeTeams

	def getUnitsForPlayer(self, iPlayer, CyTeam):

		CyPlayer = GC.getPlayer(iPlayer)

		aList = [-1, -1, -1, -1, -1]

		iDefenderStr = 0
		iCounterStr = 0
		iAttackStr = 0
		iAttackCityStr = 0
		iMobileVal = 0
		for i in xrange(GC.getNumUnitClassInfos()):

			if GC.getUnitClassInfo(i).getMaxGlobalInstances() > 0 or GC.getUnitClassInfo(i).getMaxPlayerInstances() > 0 or GC.getUnitClassInfo(i).getMaxTeamInstances() > 0:
				continue
			iUnit = GC.getUnitClassInfo(i).getDefaultUnitIndex()
			if iUnit < 0 or not CyPlayer.canTrain(iUnit, False, False): continue
			CvUnitInfo = GC.getUnitInfo(iUnit)
			if CvUnitInfo.getDomainType() != DomainTypes.DOMAIN_LAND: continue

			iStr = CvUnitInfo.getCombat()
			if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_CITY_DEFENSE):
				if iStr > iDefenderStr:
					aList[0] = iUnit
					iDefenderStr = iStr
			if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_COUNTER):
				if iStr >= iCounterStr:
					aList[1] = iUnit
					iCounterStr = iStr
			if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_ATTACK):
				if iStr > iAttackStr:
					aList[2] = iUnit
					iAttackStr = iStr
			if CvUnitInfo.getUnitAIType(UnitAITypes.UNITAI_ATTACK_CITY):
				if iStr > iAttackCityStr:
					aList[3] = iUnit
					iAttackCityStr = iStr
			iVal = iStr * CvUnitInfo.getMoves()
			if iVal > iMobileVal:
				aList[4] = iUnit
				iMobileVal = iVal

		iStd = -1
		if -1 in aList:
			for iUnit in aList:
				if iUnit > -1:
					iStd = iUnit
					break
			if iStd == -1: iStd = GC.getInfoTypeForString("UNIT_CAPTIVE_MILITARY")
			for i in xrange(5): # len(aList)
				if aList[i] == -1:
					aList[i] = iStd

		aList.append(CyPlayer.getBestUnitType(UnitAITypes.UNITAI_WORKER))
		aList.append(CyPlayer.getBestUnitType(UnitAITypes.UNITAI_SETTLE))
		aList.append(CyPlayer.getBestUnitType(UnitAITypes.UNITAI_EXPLORE))

		aMerchantList = [
			GC.getInfoTypeForString("UNIT_FREIGHT"),
			GC.getInfoTypeForString("UNIT_SUPPLY_TRAIN"),
			GC.getInfoTypeForString("UNIT_TRADE_CARAVAN"),
			GC.getInfoTypeForString("UNIT_EARLY_MERCHANT_C2C")
		]
		for iUnit in aMerchantList:
			if iUnit < 0: continue
			CvUnitInfo = GC.getUnitInfo(iUnit)
			# Tech Prereq
			iTech = CvUnitInfo.getPrereqAndTech()
			if iTech > -1 and not CyTeam.isHasTech(iTech): continue
			for i in range(self.NUM_UNIT_AND_TECH_PREREQS):
				iTech = CvUnitInfo.getPrereqAndTechs(i)
				if iTech > -1 and not CyTeam.isHasTech(iTech): break
			else:
				aList.append(iUnit); break
		else: aList.append(iStd)

		return aList


	def setupFormerBarbCity(self, CyCity, iPlayer, iDefender, iDefenders):
		# Change name
		CyPlayer = GC.getPlayer(iPlayer)
		CyCity.setName(CyPlayer.getNewCityName(), True)

		# List plots within city range.
		X = CyCity.getX()
		Y = CyCity.getY()
		CyPlot = CyCity.plot()

		bWrapX = MAP.isWrapX()
		bWrapY = MAP.isWrapY()
		iWidth = MAP.getGridWidth()
		iHeight = MAP.getGridHeight()
		aList = [[0, CyPlot]]
		for iX in xrange(X-3,X+4):
			for iY in xrange(Y-3, Y+4):
				if iX == X and iY == Y: continue
				# Get raw radius
				iRadX = X - iX
				iRadY = Y - iY
				# Check if coordinates are valid and translate wrap edge crossing.
				if iX < 0 or iX >= iWidth:
					if bWrapX:
						if iX < 0:
							iX += iWidth
						else: iX -= iWidth
					else: continue

				if iY < 0 or iY >= iHeight:
					if bWrapY:
						if iY < 0:
							iY += iHeight
						else: iY -= iHeight
					else : continue
				# Get efective radius
				if iRadX < 0:
					iRadX = -iRadX
				if iRadY < 0:
					iRadY = -iRadY
				if iRadX >= iRadY:
					iRadius = iRadX
				else: iRadius = iRadY

				aList.append([iRadius, MAP.plot(iX, iY)])
		# Convert nearby barbarian
		iPlayerBarb = self.BARBARIAN_PLAYER
		# City Culture
		iCult = CyCity.getCultureTimes100(iPlayerBarb)
		CyCity.setCultureTimes100(iPlayerBarb, 0, False)
		CyCity.setCultureTimes100(iPlayer, iCult, False)
		for iRadius, CyPlotX in aList:
			# Plot Culture
			iCult = CyPlotX.getCulture(iPlayerBarb)
			if iCult > 0:
				if iRadius:
					iCult *= 2/(2+iRadius*iRadius)
				CyPlotX.changeCulture(iPlayerBarb, -iCult, False)
				CyPlotX.setCulture(iPlayer, iCult, True)
			# Units
			for i in xrange(CyPlotX.getNumUnits()):
				CyUnit = CyPlotX.getUnit(i)
				if CyUnit.getOwner() == iPlayerBarb:
					if iRadius and GAME.getSorenRandNum(iRadius + 1, 'Convert Barbarian'): continue
					iUnit = CyUnit.getUnitType()
					CyUnit.kill(False, -1)
					iUnitAI = GC.getUnitInfo(iUnit).getDefaultUnitAIType()
					CyPlayer.initUnit(iUnit, X, Y, UnitAITypes(iUnitAI), DirectionTypes.NO_DIRECTION)
		# Free city defenders
		if iDefender > -1:
			for i in range(iDefenders):
				CyPlayer.initUnit(iDefender, X, Y, UnitAITypes.UNITAI_CITY_DEFENSE, DirectionTypes.NO_DIRECTION)


	def checkMinorCivs(self, iPlayer, CyPlayer, MAX_PC_PLAYERS):
		# Check minor civs for accomplishments which warrant settling into full civ.
		iCities = CyPlayer.getNumCities()
		if iCities < 1: return
		POST_FIX = "\n\tBarbarianCiv.checkMinorCivs"
		CyCity1 = CyPlayer.getCapitalCity()

		CyArea = CyCity1.area()
		aList = self.getScenario(CyArea)
		if not aList: return
		bNewWorld, bMajorCivCities, bMajorCivUnits = aList
		iAreaID = CyArea.getID()

		if bNewWorld:
			iPolicy = self.RevOpt.getNewWorldPolicy()

			if iPolicy > 3:
				if not bMajorCivCities: return

			elif iPolicy > 1:
				if not bMajorCivUnits: return

		iTeam = CyPlayer.getTeam()
		CyTeam = GC.getTeam(iTeam)

		# Check for accomplishments
		odds = 0; iTemp = 0
		hasMet = []
		for iPlayerX in range(MAX_PC_PLAYERS):
			if iPlayerX == iPlayer: continue
			CyPlayerX = GC.getPlayer(iPlayerX)
			if not CyPlayerX.isAlive(): continue
			iTeamX = CyPlayerX.getTeam()
			if iTeamX != iTeam and not CyTeam.isHasMet(iTeamX): continue
			hasMet.append([iPlayerX, CyPlayerX])
			if CyPlayerX.isMinorCiv():
				iTemp += 2
			else: odds += 10

		# New world requires contact with a major civ.
		if bNewWorld and not odds: return

		odds += iTemp # Value from contact with other minor civs.
		odds += 10*iCities + CyPlayer.getTotalPopulation() + 40*CyPlayer.countHolyCities()
		odds += CyPlayer.getNumMilitaryUnits()/(4*GC.getWorldInfo(GC.getMap().getWorldSize()).getTargetNumCities())
		odds += 5*CyPlayer.getWondersScore() # 25 points per wonder, see getWonderScore in CvGameCoreUtils.cpp.
		if odds < 128: return

		iFactorGS = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getGrowthPercent()
		if not GAME.getSorenRandNum(10*iFactorGS + odds, 'minor2major') < odds: return

		# Turn a minor BarbCiv into a full civ, give more bonuses to launch into the world
		civName = CyPlayer.getCivilizationShortDescription(0)
		print "[INFO] Minor civ %s becomes a major civ.%s" %(civName, POST_FIX)

		# Units
		iDefender, iCounter, iAttack, iMobile, iAttackCity, iWorker, iSettler, iExplorer, iMerchant = self.getUnitsForPlayer(iPlayer, CyTeam)
		iEra = CyPlayer.getCurrentEra()

		# Pickup nearby barb cities
		iX = CyCity1.getX(); iY = CyCity1.getY()

		iPlayerBarb = self.BARBARIAN_PLAYER
		iNumBarbDefenders = GC.getHandicapInfo(GAME.getHandicapType()).getBarbarianInitialDefenders()
		fMilitaryMod = self.RevOpt.getMilitaryStrength()

		iMaxDistance = 8 * GC.getWorldInfo(MAP.getWorldSize()).getDefaultPlayers()
		CyPlayerBarb = GC.getPlayer(iPlayerBarb)
		aList = ()
		CyCityX, i = CyPlayerBarb.firstCity(False)
		while CyCityX:
			CyPlotX = CyCityX.plot()
			if CyPlotX.getArea() == iAreaID or CyPlotX.isAdjacentRevealed(iTeam):
				x = CyCityX.getX()
				y = CyCityX.getY()
				iDist = plotDistance(iX, iY, x, y)

				if iDist <= iMaxDistance and GAME.getSorenRandNum(2, "fifty fifty"):
					iCities += 1
					aList += ((CyPlotX, x, y),) # No point in including the CyCityX pointer...
			CyCityX, i = CyPlayerBarb.nextCity(i, False)

		for CyPlotX, x, y in aList:
			CyPlotX.setOwner(iPlayer) # ...because this invalidates the CyCityX pointer.
			CyCityX = CyPlotX.getPlotCity()
			self.setupFormerBarbCity(CyCityX, iPlayer, iDefender, int(iNumBarbDefenders*fMilitaryMod + 1))
			CyCityX.changePopulation(1)
			if iWorker > -1:
				CyPlayer.initUnit(iWorker, x, y, UnitAITypes.UNITAI_WORKER, DirectionTypes.DIRECTION_SOUTH)
			if iExplorer > -1:
				CyPlayer.initUnit(iExplorer, x, y, UnitAITypes.UNITAI_EXPLORE, DirectionTypes.DIRECTION_SOUTH)
			if iMerchant > -1:
				iTemp = 2 + 2*(iEra + 1)
				for i in xrange(iTemp):
					CyPlayer.initUnit(iMerchant, x, y, UnitAITypes.UNITAI_MERCHANT, DirectionTypes.DIRECTION_SOUTH)

		# Bonus units in capital
		if iWorker != -1:
			CyPlayer.initUnit(iWorker, iX, iY, UnitAITypes.UNITAI_WORKER, DirectionTypes.DIRECTION_SOUTH)
		i1 = iCounter
		i2 = iMobile
		i3 = iAttack
		i4 = iAttackCity
		i5 = iMerchant
		aList = (i1, i1, i2, i3, i3, i3, i3, i4, i4, i4, i5, i5, i5)
		amount = (bNewWorld * iEra * 2 + iEra + iNumBarbDefenders - iCities) * fMilitaryMod
		if amount < 1:
			amount = 1
		iLen = len(aList); iCount = 0
		while iCount < amount:
			iUnit = aList[GAME.getSorenRandNum(iLen, 'Military')]
			iUnitAI = GC.getUnitInfo(iUnit).getDefaultUnitAIType()
			CyUnit = CyPlayer.initUnit(iUnit, iX, iY, UnitAITypes(iUnitAI), DirectionTypes.DIRECTION_SOUTH)
			CyUnit.changeExperience(iEra + GAME.getSorenRandNum(2*(iEra+1), 'Experience'), -1, False, False, False)
			iCount += 1

		# Great persons
		i1 = GC.getInfoTypeForString("UNIT_PROPHET")
		i2 = GC.getInfoTypeForString("UNIT_ARTIST")
		i3 = GC.getInfoTypeForString("UNIT_MERCHANT")
		i4 = GC.getInfoTypeForString("UNIT_GREAT_STATESMAN")
		i5 = GC.getInfoTypeForString("UNIT_GREAT_HUNTER")
		iGeneral = GC.getInfoTypeForString("UNIT_GREAT_GENERAL")
		aList = [
			i1, i1, i2, i2, i2, i2, i3, i3, i4, i4, i5, i5, i5, i5, iGeneral, iGeneral, iGeneral,
			GC.getInfoTypeForString("UNIT_SCIENTIST"), GC.getInfoTypeForString("UNIT_ENGINEER"),
			GC.getInfoTypeForString("UNIT_GREAT_SPY"), GC.getInfoTypeForString("UNIT_GREAT_DOCTOR")
		]
		iMax = int((iEra + 2)**0.8)
		iLen = len(aList); iCount = 0

		while iCount < iMax and iLen:
			if iLen > 1:
				iTemp = GAME.getSorenRandNum(iLen, 'Great Person')
			else: iTemp = 0
			iUnit = aList[iTemp]
			del aList[iTemp]
			iLen -= 1
			iUnitAI = GC.getUnitInfo(iUnit).getDefaultUnitAIType()
			CyPlayer.initUnit(iUnit, iX, iY, UnitAITypes(iUnitAI), DirectionTypes.DIRECTION_SOUTH)
			iCount += 1

		# Gold
		CyPlayer.changeGold(2 * iFactorGS * (iEra + 1))
		CyPlayer.changeGoldenAgeTurns(GAME.goldenAgeLength())
		CyTeam.setIsMinorCiv(False, False)

		# Add replay message
		szMsg = TRNSLTR.getText("TXT_KEY_BARBCIV_MINOR_SETTLE", ()) %(CyPlayer.getName(), CyPlayer.getCivilizationAdjective(1), civName)
		iColor = GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")
		GAME.addReplayMessage(ReplayMessageTypes.REPLAY_MESSAGE_MAJOR_EVENT, iPlayer, szMsg, iX, iY, iColor)

		# Announce the barb civ settling
		iTemp = GC.getDefineINT("EVENT_MESSAGE_TIME")
		eMsg = InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT
		if CyPlayer.isHuman():
			CyInterface().addMessage(iPlayer, False, iTemp, TRNSLTR.getText("TXT_KEY_BARBCIV_FULL_CIV", ()), None, eMsg, None, iColor, -1, -1, False, False)

		szTxt = TRNSLTR.getText("TXT_KEY_BARBCIV_WORD_SPREADS", ()) + " " + szMsg
		for iPlayerX, CyPlayerX in hasMet:
			if not CyPlayerX.isHuman(): continue
			CyInterface().addMessage(iPlayerX, False, iTemp, szTxt, None, eMsg, None, iColor, -1, -1, False, False)