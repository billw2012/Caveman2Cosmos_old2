#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvGameObject.h
//
//  PURPOSE: Wrapper classes for Civ4 game objects, to avoid memory leaks stored in the objects themselves
//
//------------------------------------------------------------------------------------------------
#ifndef CV_GAME_OBJECT_H
#define CV_GAME_OBJECT_H

#include <boost/function.hpp>
#include "CvProperties.h"
#include "CvPropertyManipulators.h"
#include "BoolExpr.h"
#include "CyArgsList.h"

class CvGameObjectGame;
class CvGameObjectTeam;
class CvGameObjectPlayer;
class CvGameObjectCity;
class CvGameObjectUnit;
class CvGameObjectPlot;
class CvGame;
class CvTeam;
class CvPlayer;
class CvCity;
class CvUnit;
class CvPlot;
class CvProperties;

class CvGameObject
{
public:
	virtual GameObjectTypes getGameObjectType() const = 0;
	virtual CvProperties* getProperties() const = 0;
	virtual const CvProperties* getPropertiesConst() const = 0;
	virtual void foreach(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func) const = 0;
	virtual void foreachOn(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func) const;
	virtual void foreachNear(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func, int iDistance) const;
	virtual void foreachRelated(GameObjectTypes eType, RelationTypes eRelation, boost::function<void (const CvGameObject*)> func, int iData = 0) const;
	virtual void foreachRelatedCond(GameObjectTypes eType, RelationTypes eRelation, boost::function<void (const CvGameObject*)> func, BoolExpr* pExpr = NULL, int iData = 0) const;
	virtual void enumerate(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType) const;
	virtual void enumerateOn(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType) const;
	virtual void enumerateNear(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType, int iDistance) const;
	virtual void enumerateRelated(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType, RelationTypes eRelation, int iData = 0) const;
	virtual void enumerateRelatedCond(std::vector<const CvGameObject*>& kEnum, GameObjectTypes eType, RelationTypes eRelation, BoolExpr* pExpr = NULL, int iData = 0) const;

	virtual void foreachManipulator(boost::function<void (const CvGameObject*, CvPropertyManipulators*)> func) const = 0;

	virtual void eventPropertyChanged(PropertyTypes eProperty, int iNewValue);

	virtual bool isTag(TagTypes eTag) const;
	virtual int getAttribute(AttributeTypes eAttribute) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const = 0;

	virtual const CvGameObjectPlayer *const getOwner() const = 0;
	virtual const CvGameObjectPlot *const getPlot() const = 0;
	virtual const CvGameObjectTeam *const getTeam() const = 0;

	virtual int getResponsibleThread() const = 0;
	virtual void foreachThreaded(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func, int iThread) const = 0;

	virtual void* addPythonArgument(CyArgsList* argsList) const = 0;
	virtual void disposePythonArgument(void* pArgument) const = 0;

	virtual int adaptValueToGame(int iID, int iValue) const;
};

class CvGameObjectGame : public CvGameObject
{
public:
	CvGameObjectGame();
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func) const;

	virtual void foreachManipulator(boost::function<void (const CvGameObject*, CvPropertyManipulators*)> func) const;

	virtual int getAttribute(AttributeTypes eAttribute) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const;

	virtual const CvGameObjectPlayer *const getOwner() const;
	virtual const CvGameObjectPlot *const getPlot() const;
	virtual const CvGameObjectTeam *const getTeam() const;

	virtual int getResponsibleThread() const;
	virtual void foreachThreaded(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func, int iThread) const;

	virtual void* addPythonArgument(CyArgsList* argsList) const;
	virtual void disposePythonArgument(void* pArgument) const;
};

class CvGameObjectTeam : public CvGameObject
{
public:
	CvGameObjectTeam(CvTeam* pTeam);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func) const;

	virtual void foreachManipulator(boost::function<void (const CvGameObject*, CvPropertyManipulators*)> func) const;

	virtual bool hasGOM(GOMTypes eType, int iID) const;
	
	virtual const CvGameObjectPlayer *const getOwner() const;
	virtual const CvGameObjectPlot *const getPlot() const;
	virtual const CvGameObjectTeam *const getTeam() const;

	virtual int getResponsibleThread() const;
	virtual void foreachThreaded(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func, int iThread) const;

	virtual void* addPythonArgument(CyArgsList* argsList) const;
	virtual void disposePythonArgument(void* pArgument) const;

protected:
	CvTeam* m_pTeam;
};

class CvGameObjectPlayer : public CvGameObject
{
public:
	CvGameObjectPlayer(CvPlayer* pPlayer);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func) const;

	virtual void foreachManipulator(boost::function<void (const CvGameObject*, CvPropertyManipulators*)> func) const;

	virtual bool hasGOM(GOMTypes eType, int iID) const;
	virtual bool isTag(TagTypes eTag) const;
	
	virtual const CvGameObjectPlayer *const getOwner() const;
	virtual const CvGameObjectPlot *const getPlot() const;
	virtual const CvGameObjectTeam *const getTeam() const;

	virtual int getResponsibleThread() const;
	virtual void foreachThreaded(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func, int iThread) const;

	virtual void* addPythonArgument(CyArgsList* argsList) const;
	virtual void disposePythonArgument(void* pArgument) const;

	virtual int adaptValueToGame(int iID, int iValue) const;

protected:
	CvPlayer* m_pPlayer;
};

class CvGameObjectCity : public CvGameObject
{
public:
	CvGameObjectCity(CvCity* pCity);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func) const;
	virtual void foreachRelated(GameObjectTypes eType, RelationTypes eRelation, boost::function<void (const CvGameObject*)> func, int iData = 0) const;

	virtual void foreachManipulator(boost::function<void (const CvGameObject*, CvPropertyManipulators*)> func) const;

	virtual void eventPropertyChanged(PropertyTypes eProperty, int iNewValue);

	virtual int getAttribute(AttributeTypes eAttribute) const;
	virtual bool isTag(TagTypes eTag) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const;
	
	virtual const CvGameObjectPlayer *const getOwner() const;
	virtual const CvGameObjectPlot *const getPlot() const;
	virtual const CvGameObjectTeam *const getTeam() const;

	virtual int getResponsibleThread() const;
	virtual void foreachThreaded(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func, int iThread) const;

	virtual void* addPythonArgument(CyArgsList* argsList) const;
	virtual void disposePythonArgument(void* pArgument) const;

	virtual int adaptValueToGame(int iID, int iValue) const;

protected:
	CvCity* m_pCity;
};

class CvGameObjectUnit : public CvGameObject
{
public:
	CvGameObjectUnit(CvUnit* pUnit);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func) const;

	virtual void foreachManipulator(boost::function<void (const CvGameObject*, CvPropertyManipulators*)> func) const;

	virtual void eventPropertyChanged(PropertyTypes eProperty, int iNewValue);

	virtual bool isTag(TagTypes eTag) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const;
	
	virtual const CvGameObjectPlayer *const getOwner() const;
	virtual const CvGameObjectPlot *const getPlot() const;
	virtual const CvGameObjectTeam *const getTeam() const;

	virtual int getResponsibleThread() const;
	virtual void foreachThreaded(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func, int iThread) const;

	virtual void* addPythonArgument(CyArgsList* argsList) const;
	virtual void disposePythonArgument(void* pArgument) const;

	virtual int adaptValueToGame(int iID, int iValue) const;

protected:
	CvUnit* m_pUnit;
};

class CvGameObjectPlot : public CvGameObject
{
public:
	CvGameObjectPlot(CvPlot* pPlot);
	virtual GameObjectTypes getGameObjectType() const;
	virtual CvProperties* getProperties() const;
	virtual const CvProperties* getPropertiesConst() const;
	virtual void foreach(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func) const;
	virtual void foreachOn(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func) const;
	virtual void foreachNear(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func, int iDistance) const;
	virtual void foreachRelated(GameObjectTypes eType, RelationTypes eRelation, boost::function<void (const CvGameObject*)> func, int iData = 0) const;

	virtual void foreachManipulator(boost::function<void (const CvGameObject*, CvPropertyManipulators*)> func) const;

	virtual bool isTag(TagTypes eTag) const;
	virtual bool hasGOM(GOMTypes eType, int iID) const;
	
	virtual const CvGameObjectPlayer *const getOwner() const;
	virtual const CvGameObjectPlot *const getPlot() const;
	virtual const CvGameObjectTeam *const getTeam() const;

	virtual int getResponsibleThread() const;
	virtual void foreachThreaded(GameObjectTypes eType, boost::function<void (const CvGameObject*)> func, int iThread) const;

	virtual void* addPythonArgument(CyArgsList* argsList) const;
	virtual void disposePythonArgument(void* pArgument) const;

protected:
	CvPlot* m_pPlot;
};

#endif
