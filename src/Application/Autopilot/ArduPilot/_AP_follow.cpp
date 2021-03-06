#include "../ArduPilot/_AP_follow.h"

#ifdef USE_OPENCV

namespace kai
{

_AP_follow::_AP_follow()
{
	m_pDet = NULL;
	m_pT = NULL;
	m_iClass = -1;

	m_bTarget = false;
	m_vTargetBB.init();
	m_vP.init();
	m_vP.x = 0.5;
	m_vP.y = 0.5;
	m_vTargetP.init();
	m_vTargetP.x = 0.5;
	m_vTargetP.y = 0.5;

//	m_ieSend.init(100000);
	m_apMount.init();
}

_AP_follow::~_AP_follow()
{
}

bool _AP_follow::init(void* pKiss)
{
	IF_F(!this->_AP_posCtrl::init(pKiss));
	Kiss* pK = (Kiss*) pKiss;

	pK->v("iClass",&m_iClass);
//	pK->v("tIntSend",&m_ieSend.m_tInterval);

	Kiss* pG = pK->child("mount");
	if(!pG->empty())
	{
		pG->v("bEnable", &m_apMount.m_bEnable);

		float p=0, r=0, y=0;
		pG->v("pitch", &p);
		pG->v("roll", &r);
		pG->v("yaw", &y);

		m_apMount.m_control.input_a = p * 100;	//pitch
		m_apMount.m_control.input_b = r * 100;	//roll
		m_apMount.m_control.input_c = y * 100;	//yaw
		m_apMount.m_control.save_position = 0;

		pG->v("stabPitch", &m_apMount.m_config.stab_pitch);
		pG->v("stabRoll", &m_apMount.m_config.stab_roll);
		pG->v("stabYaw", &m_apMount.m_config.stab_yaw);
		pG->v("mountMode", &m_apMount.m_config.mount_mode);
	}

	string iName;

	iName = "";
	pK->v("_TrackerBase", &iName);
	m_pT = (_TrackerBase*) (pK->root()->getChildInst(iName));

	iName = "";
	pK->v("_DetectorBase", &iName);
	m_pDet = (_DetectorBase*) (pK->root()->getChildInst(iName));

	return true;
}

bool _AP_follow::start(void)
{
	m_bThreadON = true;
	int retCode = pthread_create(&m_threadID, 0, getUpdateThread, this);
	if (retCode != 0)
	{
		LOG(ERROR) << "Return code: " << retCode;
		m_bThreadON = false;
		return false;
	}

	return true;
}

int _AP_follow::check(void)
{
	NULL__(m_pDet,-1);

	return this->_AP_posCtrl::check();
}

void _AP_follow::update(void)
{
	while (m_bThreadON)
	{
		this->autoFPSfrom();

		this->_AP_posCtrl::update();
		if(updateTarget())
		{
			setPosLocal();
		}
		else
		{
			releaseCtrl();
		}

		this->autoFPSto();
	}
}

bool _AP_follow::updateTarget(void)
{
	IF_F(check()<0);
	if(!bActive())
	{
		m_bTarget = false;
		if(m_pT)
			m_pT->stopTrack();

		return false;
	}

	if(m_apMount.m_bEnable)
		m_pAP->setMount(m_apMount);

	m_bTarget = findTarget();
	if(m_pT)
	{
		if(m_bTarget)
			m_pT->startTrack(m_vTargetBB);

		if(m_pT->trackState() == track_update)
		{
			m_vTargetBB = *m_pT->getBB();
			m_bTarget = true;
		}
	}

	IF_F(!m_bTarget);

	m_vP.x = m_vTargetBB.midX();
	m_vP.y = m_vTargetBB.midY();
	m_vP.z = m_vTargetP.z;
	m_vP.w = m_vTargetP.w;

	return true;
}

bool _AP_follow::findTarget(void)
{
	IF_F(check()<0);

	_Object* pO;
	_Object* tO = NULL;
	float topProb = 0.0;
	int i=0;
	while((pO = m_pDet->m_pU->get(i++)) != NULL)
	{
		IF_CONT(pO->getTopClass() != m_iClass);
		IF_CONT(pO->getTopClassProb() < topProb);

		tO = pO;
		topProb = pO->getTopClassProb();
	}

	NULL_F(tO);
	m_vTargetBB = tO->getBB2D();

	return true;
}

void _AP_follow::draw(void)
{
	this->_AP_posCtrl::draw();
}

}
#endif
