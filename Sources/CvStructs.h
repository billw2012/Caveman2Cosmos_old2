#pragma once

#ifndef CVSTRUCTS_H
#define CVSTRUCTS_H

// structs.h

//#include "CvEnums.h"
#include "CvString.h"
/************************************************************************************************/
/* Afforess	                  Start		 02/02/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
#include "CvGlobals.h"
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

// XXX these should not be in the DLL per se (if the user changes them, we are screwed...)

typedef std::vector<std::pair<int,int> > ModifierArray;
//TB Combat Mod
//typedef std::vector< std::pair<UnitCombatTypes, int> > UnitCombatModifierArray;
//typedef std::vector< std::pair<TechTypes, int> > TechModifierArray;
//typedef std::vector< std::pair<TerrainTypes, int> > TerrainModifierArray;
//typedef std::vector< std::pair<FeatureTypes, int> > FeatureModifierArray;
//typedef std::vector< std::pair<BuildTypes, int> > BuildModifierArray;
//typedef std::vector< std::pair<BonusTypes, int> > BonusModifierArray;
////typedef std::vector< std::pair<PromotionTypes, int> > PromotionModifierArray;
//typedef std::vector< std::pair<PromotionLineTypes, int> > PromotionLineModifierArray;
////typedef std::vector< std::pair<ImprovementTypes, int> > ImprovementModifierArray;
//typedef std::vector< std::pair<InvisibleTypes, int> > InvisibilityArray;
//typedef std::vector< std::pair<EraTypes, int> > EraArray;
//typedef std::vector< std::pair<PropertyTypes, int> > AidArray;
struct AidStruct
{
	PropertyTypes eProperty;
	int iChange;
};
//struct PromotionModifier
//{
//	PromotionTypes ePromotion;
//	int iModifier;
//};
//struct PromotionLineModifier
//{
//	PromotionLineTypes ePromotionLine;
//	int iModifier;
//};
struct AfflictOnAttackChange
{
	int iProbabilityChange;
	int iMelee;
	int iDistance;
	int iImmediate;
};
struct PlotTeamVisibilityIntensity
{
	TeamTypes eTeam;
	InvisibleTypes eInvisibility;
	int iUnitID;
	int iIntensity;
	int iCount;
};
//struct FreePromoTypes
//{
//	PromotionTypes ePromotion;
//	BoolExpr* m_pExprFreePromotionCondition;
//};
//struct FreeTraitTypes
//{
//	TraitTypes eTrait;
//};
struct AfflictOnAttack
{
	int iProbability;
	int iMelee;
	int iDistance;
	int iImmediate;
};
//struct TerrainModifier
//{
//	TerrainTypes eTerrain;
//	int iModifier;
//};
//struct FeatureModifier
//{
//	FeatureTypes eFeature;
//	int iModifier;
//};
//struct BuildModifier
//{
//	BuildTypes eBuild;
//	int iModifier;
//};
//struct UnitCombatModifier
//{
//	UnitCombatTypes eUnitCombat;
//	int iModifier;
//};
//struct BonusModifier
//{
//	BonusTypes eBonus;
//	int iModifier;
//};
//struct ImprovementModifier
//{
//	ImprovementTypes eImprovement;
//	int iModifier;
//};
//struct DisallowedTraitType
//{
//	TraitTypes eTrait;
//};
//struct DomainModifier
//{
//	DomainTypes eDomain;
//	int iModifier;
//};
//struct TechModifier
//{
//	TechTypes eTech;
//	int iModifier;
//};
//struct BuildingModifier
//{
//	BuildingTypes eBuilding;
//	int iModifier;
//};
//struct SpecialBuildingModifier
//{
//	SpecialBuildingTypes eSpecialBuilding;
//	int iModifier;
//};
//struct UnitModifier
//{
//	UnitTypes eUnit;
//	int iModifier;
//};
//struct SpecialUnitModifier
//{
//	SpecialUnitTypes eSpecialUnit;
//	int iModifier;
//};
//struct ImprovementYieldModifier
//{
//	ImprovementTypes eImprovement;
//	YieldTypes eYield;
//	int iModifier;
//};
//struct CivicOptionTypeBool
//{
//	CivicOptionTypes eCivicOption;
//	bool bBool;
//};
//struct GameOptionTypeBool
//{
//	GameOptionTypes eGameOption;
//	bool bBool;
//};
struct HealUnitCombat
{
	int iHeal;
	int iAdjacentHeal;
};
struct GroupSpawnUnitCombat
{
	UnitCombatTypes eUnitCombat;
	int iChance;
	CvWString m_szTitle;
};
struct InvisibleTerrainChanges
{
	InvisibleTypes eInvisible;
	TerrainTypes eTerrain;
	int iIntensity;
};
struct InvisibleFeatureChanges
{
	InvisibleTypes eInvisible;
	FeatureTypes eFeature;
	int iIntensity;
};
struct InvisibleImprovementChanges
{
	InvisibleTypes eInvisible;
	ImprovementTypes eImprovement;
	int iIntensity;
};
//struct EnabledCivilizations
//{
//	CivilizationTypes eCivilization;
//};
//struct AfflictionLineChanges
//{
//	PromotionLineTypes eAfflictionLine;
//	int iChange;
//};
struct BonusAidModifiers
{
	BonusTypes eBonusType;
	PropertyTypes ePropertyType;
	int iModifier;
};
//struct AidRateChanges
//{
//	PropertyTypes ePropertyType;
//	int iChange;
//};
//struct PrereqBuildingClass
//{
//	BuildingClassTypes eBuildingClass;
//	int iMinimumRequired;
//};
struct TerrainStructs
{
	TerrainTypes eTerrain;
	TechTypes ePrereqTech;
	int iTime;
};
struct PlaceBonusTypes
{
	BonusTypes eBonus;
	int iProbability;
	bool bRequiresAccess;
	TechTypes ePrereqTech;
	TerrainTypes ePrereqTerrain;
	FeatureTypes ePrereqFeature;
	MapCategoryTypes ePrereqMapCategory;
};
struct PromotionLineAfflictionModifier
{
	int iModifier;
	bool bWorkedTile;
	bool bVicinity;
	bool bAccessVolume;
};
//TB Combat Mod end

struct XYCoords
{
	XYCoords(int x=0, int y=0) : iX(x), iY(y) {}
	int iX;
	int iY;

	bool operator<  (const XYCoords xy) const { return ((iY < xy.iY) || (iY == xy.iY && iX < xy.iX)); }
	bool operator<= (const XYCoords xy) const { return ((iY < xy.iY) || (iY == xy.iY && iX <= xy.iX)); }
	bool operator!= (const XYCoords xy) const { return (iY != xy.iY || iX != xy.iX); }
	bool operator== (const XYCoords xy) const { return (!(iY != xy.iY || iX != xy.iX)); }
	bool operator>= (const XYCoords xy) const { return ((iY > xy.iY) || (iY == xy.iY && iX >= xy.iX)); }
	bool operator>  (const XYCoords xy) const { return ((iY > xy.iY) || (iY == xy.iY && iX > xy.iX)); }
};

struct IDInfo
{

	IDInfo(PlayerTypes eOwner=NO_PLAYER, int iID=FFreeList::INVALID_INDEX) : eOwner(eOwner), iID(iID) {}
	PlayerTypes eOwner;
	int iID;

	bool operator== (const IDInfo& info) const
	{
		return (eOwner == info.eOwner && iID == info.iID);
	}

	void reset()
	{
		eOwner = NO_PLAYER;
		iID = FFreeList::INVALID_INDEX;
	}
};

struct GameTurnInfo				// Exposed to Python
{
	int iMonthIncrement;
	int iNumGameTurnsPerIncrement;
};

struct OrderData					// Exposed to Python
{
	OrderTypes eOrderType;
	int iData1;
	int iData2;
	bool bSave;
};

//	Koshling - need to cram some extra info into some orders.  Existing usage
//	means iData1 and iData2 can actually be shorts, so we divide them into two 16 bit fields
//	iData1 gets the unitType in the external data (previous usage), and the location the unit
//	is to head once built (if any)
//	iData2 gets the UnitAI being built against in the external (previous usage) and the contract-requested
//	UnitAI in the auxiliary (which can be NO_UNITAI)
#define	EXTERNAL_ORDER_IDATA(iData)				(short)((iData) & 0xFFFF)
#define	INTERNAL_AUXILIARY_ORDER_IDATA(iData)	(short)(((iData) & 0xffff0000) >> 16)
#define	PACK_INTERNAL_ORDER_IDATA(iBase, iAux)	(((unsigned int)(iBase) & 0xFFFF) | (((unsigned int)(iAux)) << 16))

//	Contract auxiliary flags
#define	AUX_CONTRACT_FLAG_IS_UNIT_CONTRACT	0x01

struct MissionData				// Exposed to Python
{
	MissionTypes eMissionType;
	int iData1;
	int iData2;
	int iFlags;
	int iPushTurn;
};

struct TradeData					// Exposed to Python
{
	TradeableItems m_eItemType;				//	What type of item is this
	int m_iData;											//	Any additional data?
	bool m_bOffering;									//	Is this item up for grabs?
	bool m_bHidden;										//	Are we hidden?
};

struct EventTriggeredData
{
	int m_iId;
	EventTriggerTypes m_eTrigger;
	int m_iTurn;
	PlayerTypes m_ePlayer;
	int m_iCityId;
	int m_iPlotX;
	int m_iPlotY;
	int m_iUnitId;
	PlayerTypes m_eOtherPlayer;
	int m_iOtherPlayerCityId;
	ReligionTypes m_eReligion;
	CorporationTypes m_eCorporation;
	BuildingTypes m_eBuilding;
	CvWString m_szText;
	CvWString m_szGlobalText;
	bool m_bExpired;

	int getID() const;
	void setID(int iID);
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct VoteSelectionSubData
{
	VoteTypes eVote;
	PlayerTypes ePlayer;
	int iCityId;
	PlayerTypes eOtherPlayer;
	CvWString szText;
};

struct VoteSelectionData
{
	int iId;
	VoteSourceTypes eVoteSource;
	std::vector<VoteSelectionSubData> aVoteOptions;

	int getID() const;
	void setID(int iID);
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct VoteTriggeredData
{
	int iId;
	VoteSourceTypes eVoteSource;
	VoteSelectionSubData kVoteOption;

	int getID() const;
	void setID(int iID);
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct EventMessage
{
	CvWString szDescription;
	int iExpirationTurn;
	int iX;
	int iY;

	// python friendly accessors
	std::wstring getDescription() const { return szDescription;	}
};

struct PlotExtraYield
{
	int m_iX;
	int m_iY;
	std::vector<int> m_aeExtraYield;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct PlotExtraCost
{
	int m_iX;
	int m_iY;
	int m_iCost;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

typedef std::vector< std::pair<BuildingClassTypes, int> > BuildingChangeArray;

struct BuildingYieldChange
{
	BuildingClassTypes eBuildingClass;
	YieldTypes eYield;
	int iChange;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct BuildingCommerceChange
{
	BuildingClassTypes eBuildingClass;
	CommerceTypes eCommerce;
	int iChange;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

/************************************************************************************************/
/* Afforess	                  Start		 01/25/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
struct PropertySpawns
{
	PropertyTypes eProperty;
	UnitClassTypes eUnitClass;
	
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};
struct BuildingYieldModifier
{
	BuildingClassTypes eBuildingClass;
	YieldTypes eYield;
	int iChange;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct BuildingCommerceModifier
{
	BuildingClassTypes eBuildingClass;
	CommerceTypes eCommerce;
	int iChange;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/



struct FOWVis
{
	uint uiCount;
	POINT* pOffsets;  // array of "Offset" points

	// python friendly accessors
	POINT getOffsets(int i) const { return pOffsets[i]; }
};

struct DllExport PBGameSetupData
{
	PBGameSetupData();

	int iSize;
	int iClimate;
	int iSeaLevel;
	int iSpeed;
	int iEra;

	int iMaxTurns;
	int iCityElimination;
	int iAdvancedStartPoints;
	int iTurnTime;

	int iNumCustomMapOptions;
	int * aiCustomMapOptions;
	int getCustomMapOption(int iOption) {return aiCustomMapOptions[iOption];}

	int iNumVictories;
	bool * abVictories;
	bool getVictory(int iVictory) {return abVictories[iVictory];}

	std::wstring szMapName;
	std::wstring getMapName() {return szMapName;}

	std::vector<bool> abOptions;
	bool getOptionAt(int iOption) {return abOptions[iOption];}

	std::vector<bool> abMPOptions;
	bool getMPOptionAt(int iOption) {return abMPOptions[iOption];}
};

struct DllExport PBPlayerSetupData
{
	int iWho;
	int iCiv;
	int iLeader;
	int iTeam;
	int iDifficulty;

	std::wstring szStatusText;
	std::wstring getStatusText() {return szStatusText;}
};

struct DllExport PBPlayerAdminData
{
	std::wstring szName;
	std::wstring getName() {return szName;}
	std::wstring szPing;
	std::wstring getPing() {return szPing;}
	std::wstring szScore;
	std::wstring getScore() {return szScore;}
	bool bHuman;
	bool bClaimed;
	bool bTurnActive;
};

class CvUnit;
class CvPlot;

//! An enumeration for indexing units within a CvBattleDefinition
enum BattleUnitTypes
{
	BATTLE_UNIT_ATTACKER,	//!< Index referencing the attacking unit
	BATTLE_UNIT_DEFENDER,	//!< Index referencing the defending unit
	BATTLE_UNIT_COUNT		//!< The number of unit index references
};

void checkBattleUnitType(BattleUnitTypes unitType);

//!< An enumeration for indexing times within the CvBattleDefinition
enum BattleTimeTypes
{
	BATTLE_TIME_BEGIN,
	BATTLE_TIME_RANGED,
	BATTLE_TIME_END,
	BATTLE_TIME_COUNT
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT:      CvBattleRound
//!  \brief		Represents a single round within a battle.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBattleRound
{
public:
	CvBattleRound();
	bool isValid() const;

	DllExport bool isRangedRound() const;
	void setRangedRound(bool value);

	DllExport int getWaveSize() const;
	DllExport void setWaveSize(int size);

	DllExport int getNumKilled(BattleUnitTypes unitType) const;
	DllExport void setNumKilled(BattleUnitTypes unitType, int value);
	void addNumKilled(BattleUnitTypes unitType, int increment);

	DllExport int getNumAlive(BattleUnitTypes unitType) const;
	DllExport void setNumAlive(BattleUnitTypes unitType, int value);

private:
	int		m_aNumKilled[BATTLE_UNIT_COUNT];		//!< The number of units killed during this round for both sides
	int		m_aNumAlive[BATTLE_UNIT_COUNT];		//!< The number of units alive at the end of this round for both sides
	int		m_iWaveSize;				//!< The number of units that can perform exchanges
	bool	m_bRangedRound;				//!< true if this round is ranged, false otherwise
};

//------------------------------------------------------------------------------------------------

typedef std::vector<CvBattleRound> CvBattleRoundVector;		//!< Type declaration for a collection of battle round definitions

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMissionDefinition
//!  \brief		Base mission definition struct
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMissionDefinition
{
public:
	CvMissionDefinition();

	DllExport MissionTypes getMissionType() const;
	void setMissionType(MissionTypes missionType);

	DllExport float getMissionTime() const;
	void setMissionTime(float time);

	DllExport CvUnit *getUnit(BattleUnitTypes unitType) const;
	void setUnit(BattleUnitTypes unitType, CvUnit *unit);

	DllExport const CvPlot *getPlot() const;
	void setPlot(const CvPlot *plot);

protected:
	MissionTypes		m_eMissionType;			//!< The type of event
	CvUnit *			m_aUnits[BATTLE_UNIT_COUNT];		//!< The units involved
	float				m_fMissionTime;			//!< The amount of time that the event will take
	const CvPlot *		m_pPlot;					//!< The plot associated with the event
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT:     CvBattleDefinition
//!  \brief		A definition passed to CvBattleManager to start a battle between units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBattleDefinition : public CvMissionDefinition
{
public:
	CvBattleDefinition();
	DllExport CvBattleDefinition( const CvBattleDefinition & kCopy );
        DllExport ~CvBattleDefinition();

	DllExport int getDamage(BattleUnitTypes unitType, BattleTimeTypes timeType) const;
	void setDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int damage);
	void addDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int increment);
	
	DllExport int getFirstStrikes(BattleUnitTypes unitType) const;
	void setFirstStrikes(BattleUnitTypes unitType, int firstStrikes);
	void addFirstStrikes(BattleUnitTypes unitType, int increment);
	
	DllExport bool isAdvanceSquare() const;
	void setAdvanceSquare(bool advanceSquare);

	int getNumRangedRounds() const;
	void setNumRangedRounds(int count);
	void addNumRangedRounds(int increment);
	
	int getNumMeleeRounds() const;
	void setNumMeleeRounds(int count);
	void addNumMeleeRounds(int increment);

	DllExport int getNumBattleRounds() const;
	DllExport void clearBattleRounds();
	DllExport CvBattleRound &getBattleRound(int index);
	DllExport const CvBattleRound &getBattleRound(int index) const;
	DllExport void addBattleRound(const CvBattleRound &round);
	void setBattleRound(int index, const CvBattleRound &round);

private:
	void checkBattleTimeType(BattleTimeTypes timeType) const;
	void checkBattleRound(int index) const;

	int					m_aDamage[BATTLE_UNIT_COUNT][BATTLE_TIME_COUNT];	//!< The beginning damage of the units
	int					m_aFirstStrikes[BATTLE_UNIT_COUNT];		//!< The number of ranged first strikes the units made
	int					m_iNumRangedRounds;				//!< The number of ranged rounds
	int					m_iNumMeleeRounds;				//!< The number of melee rounds
	bool				m_bAdvanceSquare;					//!< true if the attacking unit should move into the new square
	CvBattleRoundVector	m_aBattleRounds;					//!< The rounds that define the battle plan
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAirMissionDefinition
//!  \brief		A definition passed to CvAirMissionManager to start an air mission
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAirMissionDefinition : public CvMissionDefinition
{
public:
	CvAirMissionDefinition();
	DllExport CvAirMissionDefinition( const CvAirMissionDefinition & kCopy );

	DllExport int getDamage(BattleUnitTypes unitType) const;
	void setDamage(BattleUnitTypes unitType, int damage);
	DllExport bool isDead(BattleUnitTypes unitType) const;

private:
	int					m_aDamage[BATTLE_UNIT_COUNT];		//!< The ending damage of the units
};

struct CvWidgetDataStruct
{
	int m_iData1;										//	The first bit of data
	int m_iData2;										//	The second piece of data

	bool m_bOption;									//	A boolean piece of data

	WidgetTypes m_eWidgetType;			//	What the 'type' of this widget is (for parsing help and executing actions)
};

struct DllExport CvPlotIndicatorData
{
	CvPlotIndicatorData() : m_eVisibility(PLOT_INDICATOR_VISIBLE_ALWAYS), m_bFlashing(false), m_pUnit(NULL), m_bTestEnemyVisibility(false), m_bVisibleOnlyIfSelected(false), m_bPersistentRotation(false)
	{
	}
	CvString m_strIcon;
	CvString m_strLabel;
	NiColor m_kColor;
	CvWString m_strHelpText;
	PlotIndicatorVisibilityFlags m_eVisibility;
	bool m_bFlashing;
	NiPoint2 m_Target;
	const CvUnit* m_pUnit;
	bool m_bTestEnemyVisibility;
	bool m_bVisibleOnlyIfSelected;
	bool m_bPersistentRotation;
};

struct DllExport CvGlobeLayerData
{
	CvGlobeLayerData(GlobeLayerTypes eType) : m_eType(eType), m_bGlobeViewRequired(true), m_bShouldCitiesZoom(false), m_iNumOptions(0) { }
	GlobeLayerTypes m_eType;
	CvString m_strName;
	CvString m_strButtonHelpTag;
	CvString m_strButtonStyle;
	bool m_bGlobeViewRequired;
	bool m_bShouldCitiesZoom;
	int m_iNumOptions;
};

struct DllExport CvFlyoutMenuData
{
	CvFlyoutMenuData(FlyoutTypes eType, int iId, int iX, int iY, const wchar* strTitle) : m_eFlyout(eType), m_iID(iId), m_iX(iX), m_iY(iY), m_strTitle(strTitle) { }
	FlyoutTypes m_eFlyout;
	int m_iID;
	int m_iX;
	int m_iY;
	CvWString m_strTitle;
};

struct CvStatBase
{
	CvStatBase(const char* strKey) : m_strKey(strKey) { }
	virtual ~CvStatBase() { }
	CvString m_strKey;
};

struct CvStatInt : public CvStatBase
{
	CvStatInt(const char* strKey, int iValue) : CvStatBase(strKey), m_iValue(iValue) { }
	int m_iValue;
};

struct CvStatString : public CvStatBase
{
	CvStatString(const char* strKey, const char* strValue) : CvStatBase(strKey), m_strValue(strValue) { }
	CvString m_strValue;
};

struct CvStatFloat : public CvStatBase
{
	CvStatFloat(const char* strKey, float fValue) : CvStatBase(strKey), m_fValue(fValue) { }
	float m_fValue;
};

struct DllExport CvWBData
{
	CvWBData(int iId, const wchar* strHelp, const char* strButton) : m_iId(iId), m_strHelp(strHelp), m_strButton(strButton) { }
	int m_iId;
	CvWString m_strHelp;
	CvString m_strButton;
};


#endif	// CVSTRUCTS_H
