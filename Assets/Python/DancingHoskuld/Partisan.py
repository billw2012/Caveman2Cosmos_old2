## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## Partisan Mod (by GIR)
##

from CvPythonExtensions import *
import BugUtil

# globals
gc = CyGlobalContext()

####################################################


def onCityAcquired(argsList):
	owner,playerType,city,bConquest,bTrade = argsList


	### number of Partisan units depending on city size (before conquest) and if  ###
	### the loser of the city is stronger or weaker (military power) as the new   ###
	###                          owner of the city.                               ###
	###---------------------------------------------------------------------------###
	###  city size  | Partisan Units if | Partisan Units if   | reduce population ###
	### before conq.| loser weaker conq.| loser stronger conq.|                   ###
	###-------------+-------------------+---------------------+-------------------###
	###    01-06    |       1 - 2       |       0 - 1         |         0         ###
	###    07-13    |       2 - 3       |       1 - 2         |  - num partisans  ###
	###    14-20    |       3 - 5       |       1 - 3         |  - num partisans  ###
	###    21-27    |       4 - 6       |       2 - 4         |  - num partisans  ###
	###    28-34    |       5 - 8       |       2 - 5         |  - num partisans  ###
	###    35-41    |       6 - 9       |       3 - 6         |  - num partisans  ###
	###     ...     |        ...        |        ...          |  - num partisans  ###
	###-------------+-------------------+---------------------+-------------------###
	###             |  +3 units with    |  +3 units with      |                   ###
	###             |  NATIONHOOD civic |  NATIONHOOD civic   |                   ###
	###-------------+-------------------+---------------------+-------------------###
	###             |  +1 units with    |  +1 units with      |                   ###
	###             |  PROTECTIVE trait |  PROTECTIVE trait   |                   ###


	###      new created Partisan units will get more and more promotions with higher       ###
	###      tech level and also get special promotions dependig on the "start" plot        ###
	###-------------------------------------------------------------------------------------###
	###     technology      |     plot types      |      promotions      |      chance      ###
	###---------------------+---------------------+----------------------+------------------###
	###      rifling        |          you need rifling tech to get Partisan units          ###
	###    assembly line    |          -          |       + drill        |       100%       ###
	###    industrialism    |          -          |       + drill        |       100%       ###
	###      rocketry       |          -          |       + drill        |       100%       ###
	###      plastics       |          -          |       + drill        |       100%       ###
	###      railroad       |          -          |     + flanking I     |       100%       ###
	###     combustion      |          -          |     + flanking II    |       100%       ###
	###       radio         |          -          |       + morale       |        50%       ###
	###      fascism        |          -          |       + combat       |        50%       ###
	###      computers      |          -          |       + combat       |       100%       ###
	###      robotics       |          -          |       + combat       |       100%       ###
	###     composites      |          -          |       + combat       |       100%       ###
	###       laser         |          -          |       + combat       |       100%       ###
	###         -           |   forest / jungle   |     + woodsman I     |       100%       ###
	###    assembly line    |   forest / jungle   |     + woodsman II    |        75%       ###
	###    industrialism    |   forest / jungle   |     + woodsman III   |        50%       ###
	###         -           |        hill         |     + guerilla I     |       100%       ###
	###    assembly line    |        hill         |     + guerilla II    |        75%       ###
	###    industrialism    |        hill         |     + guerilla III   |        50%       ###


	if bConquest:
		loserPlayer = gc.getPlayer(owner)
		loserPlayerTeam = loserPlayer.getTeam()

		### partisans only available with rifling tech ###
		if ( gc.getTeam(loserPlayerTeam).isHasTech(gc.getInfoTypeForString("TECH_RIFLING")) == True ):

			### dont place partisan units if you capture your city back ###
			if ( city.isOccupation()==True ):

				### dont show partisan message if loserPlayer is already death ###
				if ( loserPlayer.isAlive()==True ):

					u_partisan = gc.getInfoTypeForString( 'UNIT_PARTISAN' )
					loserTeam = gc.getTeam(owner)
					loserPlayerID = loserPlayer.getID()
					lPnCities = loserPlayer.getNumCities()
					conqTeam = gc.getTeam(city.getOwner())
					conqPlayer = gc.getPlayer(city.getOwner())
					conqPlayerID = conqPlayer.getID()
					iX = city.getX()
					iY = city.getY()
					cityName = city.getName()
					traitt_protective = gc.getInfoTypeForString('TRAIT_PROTECTIVE')
					ct_nationhood = gc.getInfoTypeForString('CIVIC_NATIONHOOD')
					tt_noterrain = gc.getInfoTypeForString( 'NO_TERRAIN' )
					tt_coast = gc.getInfoTypeForString( 'TERRAIN_COAST' )
					tt_ocean = gc.getInfoTypeForString( 'TERRAIN_OCEAN' )
					tt_desert = gc.getInfoTypeForString( 'TERRAIN_DESERT' )
					tt_tundra = gc.getInfoTypeForString( 'TERRAIN_TAIGA' )
					tt_snow = gc.getInfoTypeForString( 'TERRAIN_ICE' )
					ft_ice = gc.getInfoTypeForString('FEATURE_ICE')
					ft_forest = gc.getInfoTypeForString('FEATURE_FOREST')
					ft_jungle = gc.getInfoTypeForString( 'FEATURE_JUNGLE' )
					it_fort = gc.getInfoTypeForString("IMPROVEMENT_FORT")
					t_rocketry = gc.getInfoTypeForString("TECH_ROCKETRY")
					t_plastics = gc.getInfoTypeForString("TECH_PLASTICS")
					t_assemblyline = gc.getInfoTypeForString("TECH_ASSEMBLY_LINE")
					t_industrialism = gc.getInfoTypeForString("TECH_INDUSTRIALISM")
					t_computers = gc.getInfoTypeForString("TECH_COMPUTERS")
					t_railroad = gc.getInfoTypeForString("TECH_RAILROAD")
					t_combustion = gc.getInfoTypeForString("TECH_COMBUSTION")
					t_radio = gc.getInfoTypeForString("TECH_RADIO")
					t_fascism = gc.getInfoTypeForString("TECH_FASCISM")
					t_robotics = gc.getInfoTypeForString("TECH_ROBOTICS")
					t_composites = gc.getInfoTypeForString("TECH_COMPOSITES")
					t_laser = gc.getInfoTypeForString("TECH_LASER")
					p_drill1 = gc.getInfoTypeForString( "PROMOTION_DRILL1" )
					p_drill2 = gc.getInfoTypeForString( "PROMOTION_DRILL2" )
					p_drill3 = gc.getInfoTypeForString( "PROMOTION_DRILL3" )
					p_drill4 = gc.getInfoTypeForString( "PROMOTION_DRILL4" )
					p_flanking1 = gc.getInfoTypeForString( 'PROMOTION_FLANKING1' )
					p_flanking2 = gc.getInfoTypeForString( 'PROMOTION_FLANKING2' )
					p_morale = gc.getInfoTypeForString( 'PROMOTION_MORALE' )
					p_combat1 = gc.getInfoTypeForString("PROMOTION_COMBAT1")
					p_combat2 = gc.getInfoTypeForString("PROMOTION_COMBAT2")
					p_combat3 = gc.getInfoTypeForString("PROMOTION_COMBAT3")
					p_combat4 = gc.getInfoTypeForString("PROMOTION_COMBAT4")
					p_combat5 = gc.getInfoTypeForString("PROMOTION_COMBAT5")
					p_combat6 = gc.getInfoTypeForString("PROMOTION_COMBAT6")
					p_woodsman1 = gc.getInfoTypeForString("PROMOTION_WOODSMAN1")
					p_guerilla1 = gc.getInfoTypeForString("PROMOTION_GUERILLA1")
					p_freedom_fighter = gc.getInfoTypeForString("PROMOTION_FREEDOM_FIGHTER")
					#p_warmth1 = gc.getInfoTypeForString("PROMOTION_WARMTH1")
					#p_cold1 = gc.getInfoTypeForString("PROMOTION_COLD1")
					i_inner_plot = -1
					l_inner_plot = [0,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0]
					lpPlots = []
					promotiontech = 0
					count_Partisan = 0
					damage = 0

	########################################################
	### get number of Partisan Units (nPartisan) [begin] ###
	########################################################

					### get base nPartisan number ###
					citysize = city.getPopulation()
					nPartisan = (citysize / 7) + 1

					### random number (between 1 and 3*) depending on city size (begin) ###
					rel_ativ = 1
					if ( citysize >= 14 ):
						rel_ativ = 2
					if ( citysize >= 28 ):
						rel_ativ = 3
					iRand = CyGame().getSorenRandNum( rel_ativ, "get number between 1 and 3 depending on city size")
					rel_ativ = iRand + 1
					### random number (between 1 and 3*) depending on city size (end) ###

					### add/take away the random number from the nPartisan (depending on power) ###
					if ( loserPlayer.getPower() > conqPlayer.getPower() ):
						iRand = CyGame().getSorenRandNum( 10, "Random for less Partisans")
						### 40% chance for more units / 60% chance for fewer units ###
						if ( iRand < 6 ):
							nPartisan -= rel_ativ
					elif ( loserPlayer.getPower() < conqPlayer.getPower() ):
						iRand = CyGame().getSorenRandNum( 10, "Random for more Partisans")
						### 40% chance for more units / 60% chance for fewer units ###
						if ( iRand >= 6 ):
							nPartisan += rel_ativ

					### change base nPartisan number depending on the culture of the conqPlayer in the city (if the conqPlayer has 40% culture in the city, only create 60% of the number of the Partisans) ######
					conqCulture = city.plot().calculateCulturePercent(conqPlayerID)
					nPartisan = (nPartisan * 0.01) * (100 - conqCulture)
					nPartisan = int(nPartisan)
					if nPartisan  < 0:
						nPartisan = 0

					### +3 partisans with nationhood civic (only loserPlayer) ###
					if ( citysize >= 6 ):
						if ( loserPlayer.getCivics(gc.getInfoTypeForString('CIVICOPTION_LEGAL')) == ct_nationhood ):
							nPartisan += 3

					### +1 partisans with protective trait (only loserPlayer) ###
					if ( loserPlayer.hasTrait(traitt_protective) == True ):
						nPartisan += 1

	######################################################
	### get number of Partisan Units (nPartisan) [end] ###
	######################################################

	######################################################
	### identify tech related promotions once rather than for eah unit  [start]  ###
	######################################################

					### promotion counts and booleans ###
					drillpromotiontech = 0
					combatpromotiontech = 0

					promotionflanking1 = False
					promotionflanking2 = False
					promotionmorale = False
					chance_combatpromotiontech = False

					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_assemblyline) == True ):
						drillpromotiontech += 1
					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_industrialism) == True ):
						drillpromotiontech += 1
					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_rocketry) == True ):
						drillpromotiontech += 1
					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_plastics) == True ):
						drillpromotiontech += 1

					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_railroad) == True ):
						promotionflanking1 = True
					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_combustion) == True ):
						promotionflanking2 = True
					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_radio) == True ):
						promotionmorale = True

					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_fascism) == True ):
						chance_combatpromotiontech = True
					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_computers) == True ):
						combatpromotiontech += 1
					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_robotics) == True ):
						combatpromotiontech += 1
					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_composites) == True ):
						combatpromotiontech += 1
					if ( gc.getTeam(loserPlayerTeam).isHasTech(t_laser) == True ):
						combatpromotiontech += 1

	######################################################
	### identify tech related promotions once rather than for eah unit  [end]   ###
	######################################################

	###################################
	### set Partisan Units  [begin] ###
	###################################

					### check all city radius plots ###
					for iXLoop in range(iX - 2, iX + 3, 1):
						for iYLoop in range(iY - 2, iY + 3, 1):
							i_inner_plot += 1
							pPlot = CyMap().plot(iXLoop, iYLoop)
							if ( pPlot.getTerrainType()!=tt_noterrain and pPlot.getTerrainType()!=tt_coast and pPlot.getTerrainType()!=tt_ocean and pPlot.getFeatureType()!=ft_ice and pPlot.isPeak()==False ):
								if ( pPlot.isCity()==False ):
									if ( pPlot.isVisibleEnemyUnit(loserPlayerID)==False ):
										lpPlots.append(pPlot)
										### doubles the chance to set the partisan in the inner city radius ###
										if ( l_inner_plot[i_inner_plot] == 1 ):
											lpPlots.append(pPlot)
											### increase the chance to set the partisan in the inner city radius with certain extras ###
											if ( pPlot.getFeatureType()==ft_forest or pPlot.getFeatureType()==ft_jungle or pPlot.isHills()==True or pPlot.getImprovementType()==it_fort ):
												for i in range(4):
													lpPlots.append(pPlot)
					### set partisan(s) ###
					if ( len(lpPlots) > 0 ):
						while ( nPartisan > 0 ):
							nPartisan -= 1
							count_Partisan += 1
							iRand = CyGame().getSorenRandNum( len(lpPlots), "Random which pPlot for Partisan")
							pPlot = lpPlots[iRand]
							iiX = pPlot.getX()
							iiY = pPlot.getY()
							pNewUnit = loserPlayer.initUnit( u_partisan, iiX, iiY, UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.NO_DIRECTION )
							### C2C - all partisan units get the Freedom Fighter promotion
							if ( pNewUnit.isHasPromotion(p_freedom_fighter)==False ):
								pNewUnit.setHasPromotion(p_freedom_fighter, True)

	#################################
	### set Partisan Units  [end] ###
	#################################

	########################################
	### set aditional promotions [begin] ###
	########################################

						### tech lvl depending promotions ###

							### DRILL promotions ###
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_assemblyline) == True ):
								#~ promotiontech += 1
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_industrialism) == True ):
								#~ promotiontech += 1
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_rocketry) == True ):
								#~ promotiontech += 1
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_plastics) == True ):
								#~ promotiontech += 1
							while ( drillpromotiontech > 0 ):
								drillpromotiontech -= 1
								if ( pNewUnit.isHasPromotion(p_drill1)==False ):
									pNewUnit.setHasPromotion(p_drill1, True)
								elif ( pNewUnit.isHasPromotion(p_drill2)==False ):
									pNewUnit.setHasPromotion(p_drill2, True)
								elif ( pNewUnit.isHasPromotion(p_drill3)==False ):
									pNewUnit.setHasPromotion(p_drill3, True)
								elif ( pNewUnit.isHasPromotion(p_drill4)==False ):
									pNewUnit.setHasPromotion(p_drill4, True)

							### FLANKING promotions ###
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_railroad) == True ):
								#~ pNewUnit.setHasPromotion(p_flanking1, True)
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_combustion) == True ):
								#~ pNewUnit.setHasPromotion(p_flanking2, True)
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_radio) == True ):
								#~ chance = CyGame().getSorenRandNum( 2, "Random Morale")
								#~ if ( chance == 0 ):
									#~ pNewUnit.setHasPromotion(p_morale, True)
							if ( promotionflanking1 == True ):
								pNewUnit.setHasPromotion(p_flanking1, True)
							if ( promotionflanking2 == True ):
								pNewUnit.setHasPromotion(p_flanking2, True)
							if ( promotionmorale == True ):
								chance = CyGame().getSorenRandNum( 2, "Random Morale")
								if ( chance == 0 ):
									pNewUnit.setHasPromotion(p_morale, True)

							### COMBAT promotion ###
							#~ promotiontech = 0
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_fascism) == True ):
								#~ chance = CyGame().getSorenRandNum( 2, "Random Combat1/2")
								#~ if ( chance == 0 ):
									#~ promotiontech += 1
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_computers) == True ):
								#~ promotiontech += 1
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_robotics) == True ):
								#~ promotiontech += 1
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_composites) == True ):
								#~ promotiontech += 1
							#~ if ( gc.getTeam(loserPlayerTeam).isHasTech(t_laser) == True ):
								#~ promotiontech += 1

							if ( chance_combatpromotiontech == True ):
								chance = CyGame().getSorenRandNum( 2, "Random Combat1/2")
								if ( chance == 0 ):
									combatpromotiontech += 1
							while ( combatpromotiontech > 0 ):
								combatpromotiontech -= 1
								if ( pNewUnit.isHasPromotion(p_combat1)==False ):
									pNewUnit.setHasPromotion(p_combat1, True)
								elif ( pNewUnit.isHasPromotion(p_combat2)==False ):
									pNewUnit.setHasPromotion(p_combat2, True)
								elif ( pNewUnit.isHasPromotion(p_combat3)==False ):
									pNewUnit.setHasPromotion(p_combat3, True)
								elif ( pNewUnit.isHasPromotion(p_combat4)==False ):
									pNewUnit.setHasPromotion(p_combat4, True)
								elif ( pNewUnit.isHasPromotion(p_combat5)==False ):
									pNewUnit.setHasPromotion(p_combat5, True)
								elif ( pNewUnit.isHasPromotion(p_combat6)==False ):
									pNewUnit.setHasPromotion(p_combat6, True)

						### plot type depending promotions ###

							### WOODSMAN promotion ###
							if ( pPlot.getFeatureType()==ft_forest or pPlot.getFeatureType()==ft_jungle ):
								pNewUnit.setHasPromotion(p_woodsman1, True)
								chance = CyGame().getSorenRandNum( 4, "Random Woodsman2")
								if ( chance >= 1 ):
									if ( gc.getTeam(loserPlayerTeam).isHasTech(t_assemblyline) == True ):
										pNewUnit.setHasPromotion(gc.getInfoTypeForString("PROMOTION_WOODSMAN2"), True)
										chance = CyGame().getSorenRandNum( 4, "Random Woodsman3")
										if ( chance >= 1 ):
											if ( gc.getTeam(loserPlayerTeam).isHasTech(t_industrialism) == True ):
												pNewUnit.setHasPromotion(gc.getInfoTypeForString("PROMOTION_WOODSMAN3"), True)

					### DESERT COMBAT promotion ###
					#		if ( pPlot.getTerrainType()==tt_desert ):
					#			pNewUnit.setHasPromotion(p_warmth1, True)
					#			chance = CyGame().getSorenRandNum( 4, "Random DesertCombat2")
					#			if ( chance >= 1 ):
					#				if ( gc.getTeam(loserPlayerTeam).isHasTech(t_assemblyline) == True ):
					#					pNewUnit.setHasPromotion(gc.getInfoTypeForString("PROMOTION_WARMTH2"), True)
					### ARCTIC COMBAT promotion ###
					#		if ( pPlot.getTerrainType()==tt_tundra or pPlot.getTerrainType()==tt_snow ):
					#			pNewUnit.setHasPromotion(p_cold1, True)
					#			chance = CyGame().getSorenRandNum( 4, "Random ArcticCombat2")
					#			if ( chance >= 1 ):
					#				if ( gc.getTeam(loserPlayerTeam).isHasTech(t_assemblyline) == True ):
					#					pNewUnit.setHasPromotion(gc.getInfoTypeForString("PROMOTION_COLD2"), True)

							### GUERILLA promotion ###
							if ( pPlot.isHills()==True ):
								pNewUnit.setHasPromotion(p_guerilla1, True)
								chance = CyGame().getSorenRandNum( 4, "Random Guerilla2")
								if ( chance >= 1 ):
									if ( gc.getTeam(loserPlayerTeam).isHasTech(t_assemblyline) == True ):
										pNewUnit.setHasPromotion(gc.getInfoTypeForString("PROMOTION_GUERILLA2"), True)
										chance = CyGame().getSorenRandNum( 4, "Random Guerilla3")
										if ( chance >= 1 ):
											if ( gc.getTeam(loserPlayerTeam).isHasTech(t_industrialism) == True ):
												pNewUnit.setHasPromotion(gc.getInfoTypeForString("PROMOTION_GUERILLA3"), True)

							### set random damage to random enemy units near the partisan unit ###
							### remember all EnemyUnits near the partisan ###
							lEnemyUnits = []
							for iiXLoop in range(iiX - 1, iiX + 2, 1):
								for iiYLoop in range(iiY - 1, iiY + 2, 1):
									ppPlot = CyMap().plot(iiXLoop, iiYLoop)
									if ( ppPlot.isVisibleEnemyUnit(loserPlayerID)==True ):
										for i in range(ppPlot.getNumUnits()):
											if ( ppPlot.isVisibleEnemyUnit(loserPlayerID)==True ):
												ppUnit = ppPlot.getUnit(i)
												lEnemyUnits.append(ppUnit)

	######################################
	### set aditional promotions [end] ###
	######################################

	############################################
	### random damage to enemy units [begin] ###
	############################################

							if ( len(lEnemyUnits) > 0 ):

								### random number between 1 and 5 (but not more than EnemyUnits near the partisan unit) ###
								n_EnemyUnits = len(lEnemyUnits)
								if n_EnemyUnits >= 5:
									n_EnemyUnits = 5
								n_EnemyUnits = CyGame().getSorenRandNum( n_EnemyUnits, "Random for how many eunits will suffer damage") + 1

								### choose which EnemyUnit will suffer damage from partisan unit ###
								while ( n_EnemyUnits > 0 ):
									n_EnemyUnits -= 1
									iRand = CyGame().getSorenRandNum( len(lEnemyUnits), "Random which eunit will suffer damage")
									ppUnit = lEnemyUnits[iRand]

									### random for how many damage the eunit will suffer from the partisan attack (15 - 30) ###
									iRand = CyGame().getSorenRandNum( 16, "rand damage") + 15

									### check to not kill the eunit ###
									if ( (ppUnit.getDamage() + iRand) >= 99 ):
										if ( ppUnit.getDamage() <= 98 ):
											iRand = (iRand - ((ppUnit.getDamage() + iRand) - 100))
										else:
											iRand = 0
										ppUnit.setDamage(99, 0)
									else:
										ppUnit.changeDamage(iRand, 0)
									damage = damage + (ppUnit.baseCombatStr() * (iRand * 0.01))
									damage = int(damage)
									if damage < 1:
										damage = 1
									CyEngine().triggerEffect(gc.getInfoTypeForString("EFFECT_EXPLOSION_CITY"), ppPlot.getPoint())

									### ausgabe ###
									i_iX = ppUnit.getX()
									i_iY = ppUnit.getY()
									CyInterface().addMessage(loserPlayerID,True,-1,'.','',1,'Art/Interface/Buttons/actions/destroy.dds',ColorTypes(11), i_iX, i_iY, True,True)
									CyInterface().addMessage(conqPlayerID,True,-1,'.','',1,'Art/Interface/Buttons/actions/destroy.dds',ColorTypes(7), i_iX, i_iY, True,True)

	##########################################
	### random damage to enemy units [end] ###
	##########################################

					### ausgabe ###
							if ( lPnCities > 0 ):
								if ( count_Partisan > 0 ):
									CyInterface().addMessage(loserPlayerID,True,-1,'.','',1,'Art/Interface/Buttons/Units/sparth/guerilla.dds',ColorTypes(11), iiX, iiY, True,True)
									CyInterface().addMessage(conqPlayerID,True,-1,'.','',1,'Art/Interface/Buttons/Units/sparth/guerilla.dds',ColorTypes(7), iiX, iiY, True,True)
									CyInterface().addMessage(loserPlayerID,False,-1,'.','AS2D_CITY_REVOLT',1,'Art/Interface/Buttons/Units/sparth/guerilla.dds',ColorTypes(11), iiX, iiY, True,True)
									CyInterface().addMessage(conqPlayerID,False,-1,'.','AS2D_CITY_REVOLT',1,'Art/Interface/Buttons/Units/sparth/guerilla.dds',ColorTypes(7), iiX, iiY, True,True)
						if ( lPnCities > 0 ):
							if ( count_Partisan > 0 ):
								if ( damage < 1 ):
									CyInterface().addMessage(loserPlayerID,True,15,CyTranslator().getText("TXT_KEY_PARTISAN_GAMETXT3",(cityName,damage)),'',0,'Art/Interface/Buttons/civics/despotism.dds',ColorTypes(44), iX, iY, True,True)
									### message: Our Brave People near %s will Fight against the Enemy! ###
									CyInterface().addMessage(conqPlayerID,True,15,CyTranslator().getText("TXT_KEY_PARTISAN_GAMETXT4",(cityName,damage)),'',0,'Art/Interface/Buttons/civics/despotism.dds',ColorTypes(7), iX, iY, True,True)
									### message: Partisans and Terrorists near %s Fight against our Brave Army! ###
								else:
									CyInterface().addMessage(loserPlayerID,True,15,CyTranslator().getText("TXT_KEY_PARTISAN_GAMETXT1",(cityName,damage)),'',0,'Art/Interface/Buttons/civics/despotism.dds',ColorTypes(44), iX, iY, True,True)
									### message: Our Brave People near %s will Fight against the Enemy! They already caused a total of %d Damage to the Evil Enemy Force.  ###
									CyInterface().addMessage(conqPlayerID,True,15,CyTranslator().getText("TXT_KEY_PARTISAN_GAMETXT2",(cityName,damage)),'',0,'Art/Interface/Buttons/civics/despotism.dds',ColorTypes(7), iX, iY, True,True)
									### message: Partisans and Terrorists near %s Fight against our Brave Army! They already caused a total of %d Damage to our Glorious Force.  ###

	#################################
	### reduce population [begin] ###
	#################################

					### reduce population ###
						if ( count_Partisan > 0 ):
							if ( lPnCities > 0 ):
								if ( citysize > 8 ):
									city.changePopulation(-count_Partisan)

	###############################
	### reduce population [end] ###
	###############################

def onCombatResult(argsList):
	'Combat Result'
	##  First we check that the winning unit is a patisan and the loosing a seige or "armour" unit
	##  There is a small chance that th unit will be captured.

	BugUtil.debug("Partisan - onCombatResult called.")
	pWinner,pLoser = argsList

	player = pWinner.getOwner()
	pPlayer = gc.getPlayer(player)
	iLoserType = pLoser.getUnitType()
	loserPlayerID = pLoser.getID()
	iWinnerType = pWinner.getUnitType()
	winnerPlayerID = pWinner.getID()

	if (iWinnerType == gc.getInfoTypeForString( 'UNIT_PARTISAN' ) ):
		captrureChance = -1
		if (pLoser.getUnitCombatType() == gc.getInfoTypeForString('UNITCOMBAT_SIEGE')):
			captrureChance = 10
		elif (pLoser.getUnitCombatType() == gc.getInfoTypeForString('UNITCOMBAT_TRACKED')) or (pLoser.getUnitCombatType() == gc.getInfoTypeForString('UNITCOMBAT_WHEELED')):
			captrureChance = 5
		if captrureChance >= 0 and	CyGame().getSorenRandNum( 99, "Partisan capture unit") <= captrureChance:
			iX = pWinner.plot().getX()
			iY = pWinner.plot().getY()
			newUnit = pPlayer.initUnit(iLoserType, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_NORTH)
			newUnit.setDamage(75, False)

			## promote unit for terrain damage option  ##
			#~ newUnit.setHasPromotion(gc.getInfoTypeForString("PROMOTION_COLD0"), 1)
			#~ newUnit.setHasPromotion(gc.getInfoTypeForString("PROMOTION_WARMTH0"), 1)
			newUnit.finishMoves()

			sMessageL = BugUtil.getText("TXT_KEY_PARTISAN_CAPTURE_UNIT1", gc.getUnitInfo(iLoserType).getDescription())
			sMessageW = BugUtil.getText("TXT_KEY_PARTISAN_CAPTURE_UNIT2", gc.getUnitInfo(iLoserType).getDescription())

			CyInterface().addMessage(loserPlayerID,True,15,sMessageL,'',0,'Art/Interface/Buttons/civics/despotism.dds',ColorTypes(44), iX, iY, True,True)
			CyInterface().addMessage(winnerPlayerID,True,15,sMessageW,'',0,'Art/Interface/Buttons/civics/despotism.dds',ColorTypes(7), iX, iY, True,True)

