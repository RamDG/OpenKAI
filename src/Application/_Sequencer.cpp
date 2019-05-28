/*
 * _Sequencer.cpp
 *
 *  Created on: May 26, 2019
 *      Author: yankai
 */

#include "_Sequencer.h"

namespace kai
{

_Sequencer::_Sequencer()
{
	m_nActuator = 0;
	m_iAction = 0;
}

_Sequencer::~_Sequencer()
{
}

bool _Sequencer::init(void* pKiss)
{
	IF_F(!this->_ThreadBase::init(pKiss));
	Kiss* pK = (Kiss*) pKiss;

	Kiss* pA;
	Kiss** pItr;
	string iName;

	pA = pK->o("actuator");
	if(pA)
	{
		pItr = pA->getChildItr();
		m_nActuator = 0;
		while (pItr[m_nActuator])
		{
			Kiss* pC = pItr[m_nActuator];

			iName = "";
			F_ERROR_F(pC->v("_ActuatorBase", &iName));
			m_ppA[m_nActuator] = (_ActuatorBase*) (pK->root()->getChildInst(iName));
			IF_Fl(!m_ppA[m_nActuator], iName + " not found");

			m_nActuator++;
		}

	}

	pA = pK->o("action");
	if(pA)
	{
		pItr = pA->getChildItr();
		SequencerAction a;
		int i=0;
		while (pItr[i])
		{
			Kiss* pC = pItr[i++];
			a.init();
			a.m_nA = pC->array("nPos", a.m_pNpos, SQ_N_ACTUATOR);
			pC->v("dT", &a.m_dT);
			m_vAction.push_back(a);
		}
	}

	return true;
}

bool _Sequencer::start(void)
{
	m_bThreadON = true;
	int retCode = pthread_create(&m_threadID, 0, getUpdateThread, this);
	if (retCode != 0)
	{
		m_bThreadON = false;
		return false;
	}

	return true;
}

void _Sequencer::update(void)
{
	while (m_bThreadON)
	{
		this->autoFPSfrom();

		updateAction();

		this->autoFPSto();
	}
}

int _Sequencer::check(void)
{
	return 0;
}

void _Sequencer::updateAction(void)
{
	IF_(m_nActuator<=0);
	IF_(m_vAction.size()<=0);

	int i;

	SequencerAction* pA = &m_vAction[m_iAction];
	for(i=0; i<pA->m_nA; i++)
	{
		m_ppA[i]->moveTo(pA->m_pNpos[i], 1.0);
	}

	for(i=0; i<pA->m_nA; i++)
	{
		IF_(!EAQ(m_ppA[i]->m_nCurrentPos, pA->m_pNpos[i]));
	}

	if(pA->m_dT > 0)
	{
//		usleep(pA->m_dT);
		this->sleepTime(pA->m_dT);
	}
	else if(pA->m_dT < 0)
	{
		this->sleepTime(0);
	}

	m_iAction++;
	if(m_iAction >= m_vAction.size())
		m_iAction = 0;
}

void _Sequencer::resume(void)
{
	this->wakeUp();
}

bool _Sequencer::draw(void)
{
	IF_F(!this->_ThreadBase::draw());
	Window* pWin = (Window*) this->m_pWindow;
	Mat* pMat = pWin->getFrame()->m();

	return true;
}

bool _Sequencer::console(int& iY)
{
	IF_F(!this->_ThreadBase::console(iY));

	string msg;

	return true;
}

}