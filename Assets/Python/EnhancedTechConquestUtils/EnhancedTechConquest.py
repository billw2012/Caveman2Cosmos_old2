## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## TechConquest by Bhruic
## Updated by TheLopez
## Updated by Dancing Hoskuld to allow language support
##	see also EnhancedTechConquest game text xml file.
##
from CvPythonExtensions import *

# globals
GC = CyGlobalContext()
TRNSLTR = CyTranslator()

# Change the value to True if the conquering civilization can receive
# technology without the appropriate prerequisites or ignore their civilization technology restrictions.
g_bCheckPrereq = True

# Increase or decrease the value to change the base technology transfer percentage amount.
g_iBasePercent = 0

# Increase or decrease the value to change the percent amount per city
# population that will be used to transfer technology to the new owners of the conquered city.
g_iPopPercent = 2


def loadConfigurationData():
	global g_bCheckPrereq
	global g_iBasePercent
	global g_iPopPercent

	import SystemPaths
	path = SystemPaths.appDir + "\Mods\Caveman2Cosmos\Caveman2Cosmos Config.ini"
	import ConfigParser
	Config = ConfigParser.ConfigParser()
	Config.read(path)

	if Config:
		g_bCheckPrereq = Config.get("Enhanced Tech Conquest", "Check Prereq")
		if g_bCheckPrereq in ("False", "false", "0"):
			g_bCheckPrereq = False
		else:
			g_bCheckPrereq = True
		g_iBasePercent = Config.get("Enhanced Tech Conquest", "Base Percent")
		if g_iBasePercent.isdigit():
			g_iBasePercent = int(g_iBasePercent)
		else:
			g_iBasePercent = 0
		g_iPopPercent = Config.get("Enhanced Tech Conquest", "Population Percent")
		if g_iPopPercent.isdigit():
			g_iPopPercent = int(g_iPopPercent)
		else:
			g_iPopPercent = 2

	sprint  = "Enhanced Tech Conquest:\n"
	sprint += "\tTechnology Transfer Ignore Prereq = %s\n" %str(g_bCheckPrereq)
	sprint += "\tBase Technology Transfer Percent. = %d\n" %g_iBasePercent
	sprint += "\tPercentage Per City Population... = %d" %g_iPopPercent
	print sprint

class EnhancedTechConquest:

	def onCityAcquired(self, argsList):
		iOwnerOld, iOwnerNew, CyCity, bConquest, bTrade = argsList
		if not bConquest: return

		iBasePercent = g_iBasePercent
		iPopPercent = g_iPopPercent
		if iBasePercent < 1 and iPopPercent < 1: return

		CyPlayerN = GC.getPlayer(iOwnerNew)
		if CyPlayerN.isNPC(): return

		if iPopPercent < 0:
			iPopPercent = 0
		elif iPopPercent > 100:
			iPopPercent = 100

		# Get the map random object
		CyRandom = GC.getGame().getMapRand()

		CyTeamN = GC.getTeam(CyPlayerN.getTeam())

		CyPlayerO = GC.getPlayer(iOwnerOld)
		CyTeamO = GC.getTeam(CyPlayerO.getTeam())

		bCheckPrereq = g_bCheckPrereq
		aList = []
		iTechsBehind = 0
		for iTech in range(GC.getNumTechInfos()):
			# Continue if the conquering team does have the tech
			if CyTeamN.isHasTech(iTech):
				continue
			# Continue if the old team doesn't have the tech
			if not CyTeamO.isHasTech(iTech):
				continue
			iTechsBehind += 1
			# Continue if the conquerer cannot research the technology
			if bCheckPrereq and not CyPlayerN.canResearch(iTech, False):
				continue
			# Append the technology to the possible technology list
			iCost = CyTeamN.getResearchCost(iTech)
			iProgress = CyTeamN.getResearchProgress(iTech)
			iRemaining = iCost - iProgress - 1
			if not iRemaining:
				continue
			# Append the technology to the possible technology list
			aList.append((iTech, iCost, iRemaining))

		if not aList:
			return

		from random import shuffle
		shuffle(aList)

		iBasePercent += iTechsBehind
		charBeaker = GC.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar()
		iPopulation = CyCity.getPopulation() + 1
		fForce = (1 + iTechsBehind/10.0) * iPopulation / (CyPlayerO.getTotalPopulation() + iPopulation)

		iMax = (iPopulation * iPopPercent)
		iCount = 0
		szText = ""
		for iTech, iCost, iRemaining in aList:
			# Get the total number of technology points that will be transfered to the new city owner
			fTemp = 0
			if iPopPercent:
				for i in xrange(iPopulation):
					fTemp += 100 * (1.0 + CyRandom.get(iPopPercent, "TechConquest")) / iMax

			fPercent = iBasePercent + fTemp * fForce

			iBeakers = int(iCost * fPercent / (20 * (iCount + 5)))

			if iBeakers < 1: continue
			if iBeakers > iRemaining:
				iBeakers = iRemaining

			# Increase the research progress for the new city owner
			CyTeamN.changeResearchProgress(iTech, iBeakers, iOwnerNew)

			szText += "\n\t" + GC.getTechInfo(iTech).getDescription() + u" <-> %i%c" %(iBeakers, charBeaker)
			iCount += 1

		if CyPlayerN.isHuman():

			if iCount: # Inform the player they got some new technology points
				szText = TRNSLTR.getText("TXT_KEY_ENHANCED_TECH_CONQUEST_SUCESS", ()) %(CyCity.getName(), szText)
			else: # Inform the player they didn't get any new technologies
				szText = TRNSLTR.getText("TXT_KEY_ENHANCED_TECH_CONQUEST_FAIL", ()) + " %s" %(CyCity.getName())

			artPath = GC.getCivilizationInfo(CyPlayerO.getCivilizationType()).getButton()
			CyInterface().addMessage(iOwnerNew, True, 20, szText, "", 0, artPath, ColorTypes(12), CyCity.getX(), CyCity.getY(), True, True)
