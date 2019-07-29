//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvPropertySolver.cpp
//
//  PURPOSE: Singleton class for solving the system of property manipulators
//
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include <boost/bind.hpp>

PropertySourceContext::PropertySourceContext(const CvPropertySource *const pSource, const CvGameObject *pObject) : m_pSource(pSource), m_pObject(pObject), m_iData1(0), m_iData2(0)
{
	m_iCurrentAmount = pObject->getPropertiesConst()->getValueByProperty(pSource->getProperty());
}

const CvPropertySource *const PropertySourceContext::getSource() const
{
	return m_pSource;
}

const CvGameObject* PropertySourceContext::getObject() const
{
	return m_pObject;
}

int PropertySourceContext::getData1()
{
	return m_iData1;
}

int PropertySourceContext::getData2()
{
	return m_iData2;
}

void PropertySourceContext::setData1(int iData)
{
	m_iData1 = iData;
}

void PropertySourceContext::setData2(int iData)
{
	m_iData2 = iData;
}

void PropertySourceContext::doPredict(CvPropertySolver* pSolver)
{
	int iChange = m_pSource->getSourcePredict(m_pObject, m_iCurrentAmount, this);
	pSolver->addChange(m_pObject, m_pSource->getProperty(), iChange);
}

void PropertySourceContext::doCorrect(CvPropertySolver* pSolver)
{
	PropertyTypes eProperty = m_pSource->getProperty();
	int iPredicted = pSolver->getPredictValue(m_pObject, eProperty);
	int iChange = m_pSource->getSourceCorrect(m_pObject, m_iCurrentAmount, iPredicted, this);
	pSolver->addChange(m_pObject, eProperty, iChange);
}


PropertyInteractionContext::PropertyInteractionContext(const CvPropertyInteraction *const pInteraction, const CvGameObject *pObject) : m_pInteraction(pInteraction), m_pObject(pObject)
{
	m_iCurrentAmountSource = pObject->getPropertiesConst()->getValueByProperty(pInteraction->getSourceProperty());
	m_iCurrentAmountTarget = pObject->getPropertiesConst()->getValueByProperty(pInteraction->getTargetProperty());
}

const CvPropertyInteraction *const PropertyInteractionContext::getInteraction() const
{
	return m_pInteraction;
}

const CvGameObject* PropertyInteractionContext::getObject() const
{
	return m_pObject;
}

void PropertyInteractionContext::doPredict(CvPropertySolver* pSolver)
{
	std::pair<int,int> iChange = m_pInteraction->getPredict(m_iCurrentAmountSource, m_iCurrentAmountTarget);
	pSolver->addChange(m_pObject, m_pInteraction->getSourceProperty(), iChange.first);
	pSolver->addChange(m_pObject, m_pInteraction->getTargetProperty(), iChange.second);
}

void PropertyInteractionContext::doCorrect(CvPropertySolver* pSolver)
{
	PropertyTypes ePropertySource = m_pInteraction->getSourceProperty();
	PropertyTypes ePropertyTarget = m_pInteraction->getTargetProperty();
	int iPredictedAmountSource = pSolver->getPredictValue(m_pObject, ePropertySource);
	int iPredictedAmountTarget = pSolver->getPredictValue(m_pObject, ePropertyTarget);
	std::pair<int,int> iChange = m_pInteraction->getCorrect(m_iCurrentAmountSource, m_iCurrentAmountTarget, iPredictedAmountSource, iPredictedAmountTarget);
	pSolver->addChange(m_pObject, ePropertySource, iChange.first);
	pSolver->addChange(m_pObject, ePropertyTarget, iChange.second);
}


PropertyPropagatorContext::PropertyPropagatorContext(const CvPropertyPropagator *const pPropagator, const CvGameObject *pObject) : m_pPropagator(pPropagator), m_pObject(pObject)
{
	pPropagator->getTargetObjects(pObject, m_apTargetObjects);
	PropertyTypes eProperty = pPropagator->getProperty();

	for(int i=0; i<(int)m_apTargetObjects.size(); i++)
	{
		m_aiCurrentAmount.push_back(m_apTargetObjects[i]->getPropertiesConst()->getValueByProperty(eProperty));
	}
}

const CvPropertyPropagator *const PropertyPropagatorContext::getPropagator() const
{
	return m_pPropagator;
}

const CvGameObject* PropertyPropagatorContext::getObject() const
{
	return m_pObject;
}

std::vector<const CvGameObject*>* PropertyPropagatorContext::getTargetObjects()
{
	return &m_apTargetObjects;
}

void PropertyPropagatorContext::doPredict(CvPropertySolver* pSolver)
{
	std::vector<int>& aiPredict = pSolver->getCache1();
	aiPredict.resize(m_aiCurrentAmount.size());
	m_pPropagator->getPredict(m_aiCurrentAmount, aiPredict);

	PropertyTypes eProperty = m_pPropagator->getProperty();
	for(int i=0; i<(int)aiPredict.size(); i++)
	{
		pSolver->addChange(m_apTargetObjects[i], eProperty, aiPredict[i]);
	}
}

void PropertyPropagatorContext::doCorrect(CvPropertySolver* pSolver)
{
	PropertyTypes eProperty = m_pPropagator->getProperty();
	std::vector<int>& aiPredict = pSolver->getCache1();
	for(int i=0; i<(int)m_apTargetObjects.size(); i++)
	{
		aiPredict.push_back(pSolver->getPredictValue(m_apTargetObjects[i], eProperty));
	}
	std::vector<int> aiCorrect = pSolver->getCache2();
	aiCorrect.resize(m_aiCurrentAmount.size());
	m_pPropagator->getCorrect(m_aiCurrentAmount, aiPredict, aiCorrect);
	for(int i=0; i<(int)aiCorrect.size(); i++)
	{
		pSolver->addChange(m_apTargetObjects[i], eProperty, aiCorrect[i]);
	}
}


void PropertySolverMap::addChange(const CvGameObject *pObject, PropertyTypes eProperty, int iChange, int iThread)
{
	m_mapPropertyChanges[iThread][pObject].changeValueByProperty(eProperty, iChange);
}

int PropertySolverMap::getPredictValue(const CvGameObject *pObject, PropertyTypes eProperty) const
{
	return m_mapProperties.find(pObject)->second.getValueByProperty(eProperty);
}

void PropertySolverMap::computePredictValues()
{
	for (int i=0; i<NUM_THREADS; i++)
	{
		for(PropertySolverMapType::iterator it = m_mapPropertyChanges[i].begin(); it != m_mapPropertyChanges[i].end(); ++it)
		{
			const CvGameObject* pObject = it->first;
			PropertySolverMapType::iterator it2 = m_mapProperties.find(pObject);
			if (it2 == m_mapProperties.end())
			{
				CvProperties* pObjProp = pObject->getProperties();
				it2 = m_mapProperties.insert(std::make_pair(pObject, CvProperties())).first;
				it2->second.addProperties(pObjProp);
			}
			it2->second.addProperties(&(it->second));
		}
		m_mapPropertyChanges[i].clear();
	}
}

void PropertySolverMap::applyChanges()
{
	for (int i=0; i<NUM_THREADS; i++)
	{
		for(PropertySolverMapType::iterator it = m_mapPropertyChanges[i].begin(); it != m_mapPropertyChanges[i].end(); ++it)
		{
			CvProperties* pProp = it->first->getProperties();
			pProp->addProperties(&(it->second));
		}
		m_mapPropertyChanges[i].clear();
	}
	// Changes are applied, clear the intermediate values
	m_mapProperties.clear();
}



void CvPropertySolver::instantiateSource(const CvGameObject* pObject, const CvPropertySource *const pSource)
{
	if (pSource->isActive(pObject))
	{
		m_aSourceContexts.push_back(new PropertySourceContext(pSource, pObject));
	}
}

void callInstantiateSource(const CvGameObject* pObject, const CvPropertySource *const pSource, CvPropertySolver* pSolver)
{
	pSolver->instantiateSource(pObject, pSource);
}

void CvPropertySolver::instantiateInteraction(const CvGameObject* pObject, const CvPropertyInteraction *const pInteraction)
{
	if (pInteraction->isActive(pObject))
	{
		m_aInteractionContexts.push_back(new PropertyInteractionContext(pInteraction, pObject));
	}
}

void callInstantiateInteraction(const CvGameObject* pObject, const CvPropertyInteraction *const pInteraction, CvPropertySolver* pSolver)
{
	pSolver->instantiateInteraction(pObject, pInteraction);
}

void CvPropertySolver::instantiatePropagator(const CvGameObject* pObject, const CvPropertyPropagator *const pPropagator)
{
	if (pPropagator->isActive(pObject))
	{
		m_aPropagatorContexts.push_back(new PropertyPropagatorContext(pPropagator, pObject));
	}
}

void callInstantiatePropagator(const CvGameObject* pObject, const CvPropertyPropagator *const pPropagator, CvPropertySolver* pSolver)
{
	pSolver->instantiatePropagator(pObject, pPropagator);
}

void CvPropertySolver::instantiateManipulators(const CvGameObject* pObject, CvPropertyManipulators* pMani)
{
	// Sources
	for (int j=0; j<pMani->getNumSources(); j++)
	{
		const CvPropertySource *const pSource = pMani->getSource(j);
		RelationTypes eRelation = pSource->getRelation();
		if (eRelation == NO_RELATION)
		{
			instantiateSource(pObject, pSource);
		}
		else
		{
			pObject->foreachRelated(pSource->getObjectType(), eRelation, boost::bind(callInstantiateSource, _1, pSource, this), pSource->getRelationData());
		}
	}
	// Interactions
	for (int j=0; j<pMani->getNumInteractions(); j++)
	{
		const CvPropertyInteraction *const pInteraction = pMani->getInteraction(j);
		RelationTypes eRelation = pInteraction->getRelation();
		if (eRelation == NO_RELATION)
		{
			instantiateInteraction(pObject, pInteraction);
		}
		else
		{
			pObject->foreachRelated(pInteraction->getObjectType(), eRelation, boost::bind(callInstantiateInteraction, _1, pInteraction, this), pInteraction->getRelationData());
		}
	}
	// Propagators
	for (int j=0; j<pMani->getNumPropagators(); j++)
	{
		const CvPropertyPropagator *const pPropagator = pMani->getPropagator(j);
		RelationTypes eRelation = pPropagator->getRelation();
		if (eRelation == NO_RELATION)
		{
			instantiatePropagator(pObject, pPropagator);
		}
		else
		{
			pObject->foreachRelated(pPropagator->getObjectType(), eRelation, boost::bind(callInstantiatePropagator, _1, pPropagator, this), pPropagator->getRelationData());
		}
	}
}

void CvPropertySolver::instantiateGlobalManipulators(const CvGameObject *pObject)
{
	for (int i=0; i < m_pMainSolver->getNumGlobalManipulators(); i++)
	{
		instantiateManipulators(pObject, m_pMainSolver->getGlobalManipulator(i));
	}
}

// helper functions
void callInstantiateManipulators(const CvGameObject* pObject, CvPropertyManipulators* pMani, CvPropertySolver* pSolver)
{
	pSolver->instantiateManipulators(pObject, pMani);
}

void callInstantiateGlobalManipulators(const CvGameObject* pObject, CvPropertySolver* pSolver)
{
	pSolver->instantiateGlobalManipulators(pObject);
	pObject->foreachManipulator(boost::bind(callInstantiateManipulators, _1, _2, pSolver));
}

void CvPropertySolver::gatherActiveManipulators()
{
	for (int i=0; i<NUM_GAMEOBJECTS; i++)
	{
		GC.getGameINLINE().getGameObject()->foreach((GameObjectTypes)i, boost::bind(callInstantiateGlobalManipulators, _1, this));
	}
}

void CvPropertySolver::gatherActiveManipulatorsThreaded()
{
	for (int i=0; i<NUM_GAMEOBJECTS; i++)
	{
		GC.getGameINLINE().getGameObject()->foreachThreaded((GameObjectTypes)i, boost::bind(callInstantiateGlobalManipulators, _1, this), m_iThread);
	}
}

void CvPropertySolver::predictSources()
{
	for (std::vector<PropertySourceContext*>::iterator it = m_aSourceContexts.begin(); it != m_aSourceContexts.end(); it++)
	{
		(*it)->doPredict(this);
	}
}

void CvPropertySolver::correctSources()
{
	for (std::vector<PropertySourceContext*>::iterator it = m_aSourceContexts.begin(); it != m_aSourceContexts.end(); it++)
	{
		(*it)->doCorrect(this);
	}
}

void CvPropertySolver::clearSources()
{
	for (std::vector<PropertySourceContext*>::iterator it = m_aSourceContexts.begin(); it != m_aSourceContexts.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	m_aSourceContexts.clear();
}

void CvPropertySolver::predictInteractions()
{
	for (std::vector<PropertyInteractionContext*>::iterator it = m_aInteractionContexts.begin(); it != m_aInteractionContexts.end(); it++)
	{
		(*it)->doPredict(this);
	}
}

void CvPropertySolver::correctInteractions()
{
	for (std::vector<PropertyInteractionContext*>::iterator it = m_aInteractionContexts.begin(); it != m_aInteractionContexts.end(); it++)
	{
		(*it)->doCorrect(this);
	}
}

void CvPropertySolver::clearInteractions()
{
	for (std::vector<PropertyInteractionContext*>::iterator it = m_aInteractionContexts.begin(); it != m_aInteractionContexts.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	m_aInteractionContexts.clear();
}

void CvPropertySolver::predictPropagators()
{
	for (std::vector<PropertyPropagatorContext*>::iterator it = m_aPropagatorContexts.begin(); it != m_aPropagatorContexts.end(); it++)
	{
		(*it)->doPredict(this);
	}
}

void CvPropertySolver::correctPropagators()
{
	for (std::vector<PropertyPropagatorContext*>::iterator it = m_aPropagatorContexts.begin(); it != m_aPropagatorContexts.end(); it++)
	{
		(*it)->doCorrect(this);
	}
}

void CvPropertySolver::clearPropagators()
{
	for (std::vector<PropertyPropagatorContext*>::iterator it = m_aPropagatorContexts.begin(); it != m_aPropagatorContexts.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	m_aPropagatorContexts.clear();
}

void CvPropertySolver::setMainSolver(CvMainPropertySolver *pMainSolver, int iThread)
{
	m_pMainSolver = pMainSolver;
	m_iThread = iThread;
}

void CvPropertySolver::waitAndJoin()
{
	m_pThread->join();
	delete m_pThread;
}

void callResetPropertyChange(const CvGameObject* pObject)
{
	pObject->getProperties()->clearChange();
}

void callResetPropertyChanges(CvPropertySolver* pSolver)
{
	pSolver->resetPropertyChanges();
}

void CvPropertySolver::resetPropertyChangesStart()
{
	m_pThread = new boost::thread(boost::bind(callResetPropertyChanges, this));
}

void CvPropertySolver::resetPropertyChanges()
{
	for (int i=0; i<NUM_GAMEOBJECTS; i++)
	{
		GC.getGameINLINE().getGameObject()->foreachThreaded((GameObjectTypes)i, callResetPropertyChange, m_iThread);
	}
}

void callGatherAndPredict(CvPropertySolver* pSolver)
{
	pSolver->gatherAndPredictThreaded();
}

void CvPropertySolver::gatherAndPredictStart()
{
	m_pThread = new boost::thread(boost::bind(callGatherAndPredict, this));
}

void CvPropertySolver::gatherAndPredictThreaded()
{
	gatherActiveManipulatorsThreaded();
	predictSources();
	predictInteractions();
	predictPropagators();
}

void callCorrect(CvPropertySolver* pSolver)
{
	pSolver->correctThreaded();
}

void CvPropertySolver::correctStart()
{
	m_pThread = new boost::thread(boost::bind(callCorrect, this));
}

void CvPropertySolver::correctThreaded()
{
	correctSources();
	clearSources();
	correctInteractions();
	clearInteractions();
	correctPropagators();
	clearPropagators();
}

std::vector<int>& CvPropertySolver::getCache1()
{
	return m_aiCache1;
}

std::vector<int>& CvPropertySolver::getCache2()
{
	return m_aiCache2;
}

int CvPropertySolver::getPredictValue(const CvGameObject *pObject, PropertyTypes eProperty) const
{
	return m_pMainSolver->getSolverMap()->getPredictValue(pObject, eProperty);
}

void CvPropertySolver::addChange(const CvGameObject* pObject, PropertyTypes eProperty, int iChange)
{
	m_pMainSolver->getSolverMap()->addChange(pObject, eProperty, iChange, m_iThread);
}



void CvMainPropertySolver::resetPropertyChanges()
{
	for (int i=0; i<NUM_GAMEOBJECTS; i++)
	{
		GC.getGameINLINE().getGameObject()->foreach((GameObjectTypes)i, callResetPropertyChange);
	}
}

void CvMainPropertySolver::resetPropertyChangesThreaded()
{
	for (int i=0; i<NUM_THREADS; i++)
	{
		m_Solvers[i].resetPropertyChangesStart();
	}
	for (int i=0; i<NUM_THREADS; i++)
	{
		m_Solvers[i].waitAndJoin();
	}
}

CvMainPropertySolver::CvMainPropertySolver()
{
	for (int i=0; i<NUM_THREADS; i++)
	{
		m_Solvers[i].setMainSolver(this, i);
	}
}

PropertySolverMap* CvMainPropertySolver::getSolverMap()
{
	return &m_mapProperties;
}

void CvMainPropertySolver::addGlobalManipulators(CvPropertyManipulators *pMani)
{
	m_apGlobalManipulators.push_back(pMani);
}

void CvMainPropertySolver::gatherGlobalManipulators()
{
	// Global manipulators first
	for (int i=0; i<GC.getNumPropertyInfos(); i++)
	{
		addGlobalManipulators(GC.getPropertyInfo((PropertyTypes)i).getPropertyManipulators());
	}
}

int CvMainPropertySolver::getNumGlobalManipulators()
{
	return (int)m_apGlobalManipulators.size();
}

CvPropertyManipulators* CvMainPropertySolver::getGlobalManipulator(int index)
{
	return m_apGlobalManipulators[index];
}

void CvMainPropertySolver::gatherAndSolve()
{
	m_Solvers[0].gatherActiveManipulators();

	// Propagators first
	m_Solvers[0].predictPropagators();
	m_mapProperties.computePredictValues();
	m_Solvers[0].correctPropagators();
	m_mapProperties.applyChanges();
	m_Solvers[0].clearPropagators();

	// Interactions next
	m_Solvers[0].predictInteractions();
	m_mapProperties.computePredictValues();
	m_Solvers[0].correctInteractions();
	m_mapProperties.applyChanges();
	m_Solvers[0].clearInteractions();

	// Sources last
	m_Solvers[0].predictSources();
	m_mapProperties.computePredictValues();
	m_Solvers[0].correctSources();
	m_mapProperties.applyChanges();
	m_Solvers[0].clearSources();
}

void CvMainPropertySolver::gatherAndSolveThreaded()
{
	// Do all predictions first, then all corrections (not the same result as in the single threaded version so needs to be the same for all computers in a MP game)

	for (int i=0; i<NUM_THREADS; i++)
	{
		m_Solvers[i].gatherAndPredictStart();
	}
	for (int i=0; i<NUM_THREADS; i++)
	{
		m_Solvers[i].waitAndJoin();
	}

	m_mapProperties.computePredictValues();

	for (int i=0; i<NUM_THREADS; i++)
	{
		m_Solvers[i].correctStart();
	}
	for (int i=0; i<NUM_THREADS; i++)
	{
		m_Solvers[i].waitAndJoin();
	}

	m_mapProperties.applyChanges();
}


void CvMainPropertySolver::doTurn()
{
	MEMORY_TRACE_FUNCTION();
	PROFILE_FUNC();

	// Add a BUG option here
	bool bThreaded = GC.getDefineBOOL("USE_MULTIPLE_THREADS_PROPERTY_SOLVER");

	if (bThreaded)
	{
		resetPropertyChangesThreaded();
	}
	else
	{
		resetPropertyChanges();
	}
	
	gatherGlobalManipulators();

	if (bThreaded)
	{
		gatherAndSolveThreaded();
	}
	else
	{
		gatherAndSolve();
	}
	
	m_apGlobalManipulators.clear();
}
	
