#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertySolver.h
//
//  PURPOSE: Singleton class for solving the system of property manipulators
//
//------------------------------------------------------------------------------------------------
#ifndef CV_PROPERTY_SOLVER_H
#define CV_PROPERTY_SOLVER_H

#include <vector>
#include <map>
#include "CvProperties.h"
#include "CvPropertySource.h"
#include "CvPropertyInteraction.h"
#include "CvPropertyPropagator.h"

#define BOOST_THREAD_NO_LIB
#define BOOST_THREAD_USE_LIB
#include <boost/thread/thread.hpp>

typedef std::map<const CvGameObject*, CvProperties> PropertySolverMapType;

class PropertySolverMap
{
public:
	int getPredictValue(const CvGameObject* pObject, PropertyTypes eProperty) const;
	void addChange(const CvGameObject* pObject, PropertyTypes eProperty, int iChange, int iThread);

	void computePredictValues();
	void applyChanges();

protected:
	PropertySolverMapType m_mapProperties;
	PropertySolverMapType m_mapPropertyChanges[NUM_THREADS];
};

class CvPropertySolver;

class PropertySourceContext
{
public:
	PropertySourceContext(const CvPropertySource *const pSource, const CvGameObject* pObject);
	const CvPropertySource *const getSource() const;
	const CvGameObject* getObject() const;
	int getData1();
	int getData2();
	void setData1(int iData);
	void setData2(int iData);

	void doPredict(CvPropertySolver* pSolver);
	void doCorrect(CvPropertySolver* pSolver);

protected:
	const CvPropertySource *const m_pSource;
	const CvGameObject* m_pObject;
	int m_iCurrentAmount;
	int m_iData1;
	int m_iData2;
};

class PropertyInteractionContext
{
public:
	PropertyInteractionContext(const CvPropertyInteraction *const pInteraction, const CvGameObject* pObject);
	const CvPropertyInteraction *const getInteraction() const;
	const CvGameObject* getObject() const;

	void doPredict(CvPropertySolver* pSolver);
	void doCorrect(CvPropertySolver* pSolver);

protected:
	const CvPropertyInteraction *const m_pInteraction;
	const CvGameObject* m_pObject;
	int m_iCurrentAmountSource;
	int m_iCurrentAmountTarget;
};

class PropertyPropagatorContext
{
public:
	PropertyPropagatorContext(const CvPropertyPropagator *const pPropagator, const CvGameObject* pObject);
	const CvPropertyPropagator *const getPropagator() const;
	const CvGameObject* getObject() const;
	std::vector<const CvGameObject*>* getTargetObjects();

	void doPredict(CvPropertySolver* pSolver);
	void doCorrect(CvPropertySolver* pSolver);

protected:
	const CvPropertyPropagator *const m_pPropagator;
	const CvGameObject* m_pObject;
	std::vector<const CvGameObject*> m_apTargetObjects;
	std::vector<int> m_aiCurrentAmount;
};

class CvMainPropertySolver;

class CvPropertySolver
{
public:
	void instantiateSource(const CvGameObject* pObject, const CvPropertySource *const pSource);
	void instantiateInteraction(const CvGameObject* pObject, const CvPropertyInteraction *const pInteraction);
	void instantiatePropagator(const CvGameObject* pObject, const CvPropertyPropagator *const pPropagator);
	void instantiateManipulators(const CvGameObject* pObject, CvPropertyManipulators* pMani);
	void instantiateGlobalManipulators(const CvGameObject* pObject);
	void gatherActiveManipulators();
	void gatherActiveManipulatorsThreaded();
	
	void predictSources();
	void correctSources();
	void clearSources();
	
	void predictInteractions();
	void correctInteractions();
	void clearInteractions();

	void predictPropagators();
	void correctPropagators();
	void clearPropagators();

	void setMainSolver(CvMainPropertySolver* pMainSolver, int iThread);

	void waitAndJoin();

	void resetPropertyChangesStart();
	void resetPropertyChanges();

	void gatherAndPredictStart();
	void gatherAndPredictThreaded();

	void correctStart();
	void correctThreaded();

	std::vector<int>& getCache1();
	std::vector<int>& getCache2();

	// Passed on to the solver map
	void addChange(const CvGameObject* pObject, PropertyTypes eProperty, int iChange);
	int getPredictValue(const CvGameObject* pObject, PropertyTypes eProperty) const;

protected:
	std::vector<PropertySourceContext*> m_aSourceContexts;
	std::vector<PropertyInteractionContext*> m_aInteractionContexts;
	std::vector<PropertyPropagatorContext*> m_aPropagatorContexts;
	CvMainPropertySolver* m_pMainSolver;
	int m_iThread;
	boost::thread* m_pThread;
	std::vector<int> m_aiCache1;
	std::vector<int> m_aiCache2;
};

class CvMainPropertySolver
{
public:
	CvMainPropertySolver();

	PropertySolverMap* getSolverMap();

	void addGlobalManipulators(CvPropertyManipulators* pMani);
	void gatherGlobalManipulators();
	int getNumGlobalManipulators();
	CvPropertyManipulators* getGlobalManipulator(int index);

	void resetPropertyChanges();
	void resetPropertyChangesThreaded();

	void gatherAndSolve();
	void gatherAndSolveThreaded();

	void doTurn();

protected:
	std::vector<CvPropertyManipulators*> m_apGlobalManipulators;
	CvPropertySolver m_Solvers[NUM_THREADS];
	PropertySolverMap m_mapProperties;
};


#endif