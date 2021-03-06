#include "_S6H4D.h"

namespace kai
{

_S6H4D::_S6H4D()
{
	m_pIO = NULL;
	m_state.init();

	m_vPosRangeX.init(0.0, 100.0);
	m_vPosRangeY.init(0.0, 100.0);
	m_vPosRangeZ.init(0.0, 100.0);
	m_vRotRangeX.init(0.0, 180.0);
	m_vRotRangeY.init(0.0, 180.0);
	m_vRotRangeZ.init(0.0, 180.0);
	m_vSpeedRange.init(0.0, 1e5);

	m_dMove = 128;
	m_bOrder = true;

	m_cState.init();
	m_tState.init();
	m_tState.m_vOrigin.init(344.1, 0.0, 417.4);
}

_S6H4D::~_S6H4D()
{
}

bool _S6H4D::init(void *pKiss)
{
	IF_F(!this->_ActuatorBase::init(pKiss));
	Kiss *pK = (Kiss*) pKiss;

	pK->v("vPosRangeX", &m_vPosRangeX);
	pK->v("vPosRangeY", &m_vPosRangeY);
	pK->v("vPosRangeZ", &m_vPosRangeZ);
	pK->v("vRotRangeX", &m_vRotRangeX);
	pK->v("vRotRangeY", &m_vRotRangeY);
	pK->v("vRotRangeZ", &m_vRotRangeZ);
	pK->v("vSpeedRange", &m_vSpeedRange);

	pK->v("bOrder", &m_bOrder);
	pK->v("vOrigin", &m_tState.m_vOrigin);
	pK->v("mode", &m_tState.m_mode);

	string iName = "";
	F_ERROR_F(pK->v("_IOBase", &iName));
	m_pIO = (_IOBase*) (pK->root()->getChildInst(iName));
	IF_Fl(!m_pIO, iName + " not found");

	return true;
}

bool _S6H4D::start(void)
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

int _S6H4D::check(void)
{
	NULL__(m_pIO, -1);
	IF__(!m_pIO->isOpen(), -1);

	return 0;
}

void _S6H4D::update(void)
{
	while (m_bThreadON)
	{
		this->autoFPSfrom();

		setMode(m_tState.m_mode);
		if(m_tState.m_vOrigin != m_cState.m_vOrigin)
			setOrigin(m_tState.m_vOrigin);

		if(m_vNormTargetPos.sum() > -3.0)
		{
			updatePos();
		}
		else if(m_vNormTargetSpeed.sum() > -3.0)
		{
			updateMove();
		}

		readState();

		this->autoFPSto();
	}
}

void _S6H4D::updatePos(void)
{
	IF_(check() < 0);

	vFloat3 vP;
	vP.x = m_vNormTargetPos.x * m_vPosRangeX.d() + m_vPosRangeX.x;
	vP.y = m_vNormTargetPos.y * m_vPosRangeY.d() + m_vPosRangeY.x;
	vP.z = m_vNormTargetPos.z * m_vPosRangeZ.d() + m_vPosRangeZ.x;

	vFloat3 vR;
	vR.x = m_vNormTargetRot.x * m_vRotRangeX.d() + m_vRotRangeX.x;
	vR.y = m_vNormTargetRot.y * m_vRotRangeY.d() + m_vRotRangeY.x;
	vR.z = m_vNormTargetRot.z * m_vRotRangeZ.d() + m_vRotRangeZ.x;

	float spd = m_vNormTargetSpeed.x * m_vSpeedRange.d() + m_vSpeedRange.x;

	gotoPos(vP, vR, spd);
}

void _S6H4D::updateMove(void)
{
	IF_(check() < 0);

	vFloat3 vM;
	vM.x = m_vNormTargetSpeed.x;
	vM.y = m_vNormTargetSpeed.y;
	vM.z = m_vNormTargetSpeed.z;

	move(vM);
}

void _S6H4D::armReset(void)
{
	IF_(check() < 0);

	S6H4D_CMD_CTRL cmd;
	cmd.init();
	cmd.m_b[1] = 12;
	cmd.m_b[2] = 3;
	m_pIO->write(cmd.m_b, S6H4D_CMD_N);
}

void _S6H4D::setOrigin(vFloat3& vP)
{
	IF_(check() < 0);

	S6H4D_CMD_CTRL cmd;
	cmd.init();
	cmd.m_b[1] = 20;
	cmd.m_b[2] = 1;
	f2b(&cmd.m_b[3], vP.x);
	f2b(&cmd.m_b[7], vP.y);
	f2b(&cmd.m_b[11], vP.z);
	m_pIO->write(cmd.m_b, S6H4D_CMD_N);
}

void _S6H4D::setMode(int mode)
{
	IF_(check() < 0);

	S6H4D_CMD_CTRL cmd;
	cmd.init();
	cmd.m_b[1] = 30;
	cmd.m_b[2] = 9;
	cmd.m_b[3] = (mode == 0)?0:9;
	m_pIO->write(cmd.m_b, S6H4D_CMD_N);
}

void _S6H4D::gotoPos(vFloat3 &vP, vFloat3& vR, float speed)
{
	IF_(check() < 0);

	S6H4D_CMD_MOV cmd;
	cmd.init('1', 0);
	cmd.set(vP, vR, speed);
	m_pIO->write(cmd.m_b, S6H4D_CMD_N);
}

void _S6H4D::move(vFloat3& vM)
{
	IF_(check() < 0);

	S6H4D_CMD_CTRL cmd;
	cmd.init();
	cmd.m_b[1] = 30;
	cmd.m_b[2] = 7;
	cmd.m_b[3] = 100;
	cmd.m_b[4] = (vM.x >= 0.0) ? constrain<float>(128 + (vM.x - 0.5)*2.0*m_dMove, 0, 255) : 128;
	cmd.m_b[5] = (vM.y >= 0.0) ? constrain<float>(128 + (vM.y - 0.5)*2.0*m_dMove, 0, 255) : 128;
	cmd.m_b[6] = (vM.z >= 0.0) ? constrain<float>(128 + (vM.z - 0.5)*2.0*m_dMove, 0, 255) : 128;

	//TODO: add border detect and protection


	m_pIO->write(cmd.m_b, S6H4D_CMD_N);
}

void _S6H4D::pause(void)
{
	IF_(check() < 0);

	S6H4D_CMD_CTRL cmd;
	cmd.init();
	cmd.m_b[1] = 30;
	cmd.m_b[2] = 3;
	cmd.m_b[3] = 2;

	m_pIO->write(cmd.m_b, S6H4D_CMD_N);
}

void _S6H4D::readState(void)
{
	IF_(check() < 0);

	uint8_t b;
	int n;
	while ((n = m_pIO->read(&b, 1)) > 0)
	{
		if (m_state.m_iB != 0)
		{
			m_state.m_pB[m_state.m_iB++] = b;

			if(m_state.m_iB == 9)
			{
				decodeState();
				m_state.init();
			}
		}
		else if (b == S6H4D_STAT_BEGIN)
		{
			m_state.m_pB[0] = b;
			m_state.m_iB = 1;
		}
	}
}

void _S6H4D::decodeState(void)
{
	IF_(m_state.m_pB[8] != S6H4D_STAT_END);

	switch (m_state.m_pB[7])
	{
	case 0:
		m_cState.m_vP.x = 0.1 * (float)unpack_int16(&m_state.m_pB[1], m_bOrder) - m_cState.m_vOrigin.x;
		m_cState.m_vP.y = 0.1 * (float)unpack_int16(&m_state.m_pB[3], m_bOrder) - m_cState.m_vOrigin.y;
		m_cState.m_vP.z = 0.1 * (float)unpack_int16(&m_state.m_pB[5], m_bOrder) - m_cState.m_vOrigin.z;

		m_vNormPos.x = (float) (m_cState.m_vP.x - m_vPosRangeX.x) / (float) m_vPosRangeX.len();
		m_vNormPos.y = (float) (m_cState.m_vP.y - m_vPosRangeY.x) / (float) m_vPosRangeY.len();
		m_vNormPos.z = (float) (m_cState.m_vP.z - m_vPosRangeZ.x) / (float) m_vPosRangeZ.len();
		break;
	case 1:
		m_cState.m_vA.x = 0.01 * (float)unpack_int16(&m_state.m_pB[1], m_bOrder);
		m_cState.m_vA.y = 0.01 * (float)unpack_int16(&m_state.m_pB[3], m_bOrder);
		m_cState.m_vA.z = 0.01 * (float)unpack_int16(&m_state.m_pB[5], m_bOrder);

		m_vNormRot.x = (float) (m_cState.m_vA.x - m_vRotRangeX.x) / (float) m_vRotRangeX.len();
		m_vNormRot.y = (float) (m_cState.m_vA.y - m_vRotRangeY.x) / (float) m_vRotRangeY.len();
		m_vNormRot.z = (float) (m_cState.m_vA.z - m_vRotRangeZ.x) / (float) m_vRotRangeZ.len();
		break;
	case 2:
		m_cState.m_vB.x = 0.01 * (float)unpack_int16(&m_state.m_pB[1], m_bOrder);
		m_cState.m_vB.y = 0.01 * (float)unpack_int16(&m_state.m_pB[3], m_bOrder);
		m_cState.m_aw = 0.01 * (float)unpack_int16(&m_state.m_pB[5], m_bOrder);
		break;
	case 3:
		m_cState.m_vOrigin.x = (float)unpack_int16(&m_state.m_pB[1], m_bOrder);
		m_cState.m_vOrigin.y = (float)unpack_int16(&m_state.m_pB[3], m_bOrder);
		m_cState.m_vOrigin.z = (float)unpack_int16(&m_state.m_pB[5], m_bOrder);
		break;
	case 4:
		break;
	case 5:
		m_cState.m_vW.x = (float)unpack_int16(&m_state.m_pB[1], m_bOrder);
		m_cState.m_vW.y = (float)unpack_int16(&m_state.m_pB[3], m_bOrder);
		break;
	default:
		break;
	}

}

void _S6H4D::draw(void)
{
	this->_ActuatorBase::draw();

	addMsg("-- Current state --",1);
	addMsg("vP = (" + f2str(m_cState.m_vP.x) + ", " + f2str(m_cState.m_vP.y) + ", " + f2str(m_cState.m_vP.z) + ")" ,1);
	addMsg("vA = (" + f2str(m_cState.m_vA.x) + ", " + f2str(m_cState.m_vA.y) + ", " + f2str(m_cState.m_vA.z) + ")" ,1);
	addMsg("vB = (" + f2str(m_cState.m_vB.x) + ", " + f2str(m_cState.m_vB.y) + ")" ,1);
	addMsg("aw = " + f2str(m_cState.m_aw) ,1);
	addMsg("vOrigin = (" + f2str(m_cState.m_vOrigin.x) + ", " + f2str(m_cState.m_vOrigin.y) + ", " + f2str(m_cState.m_vOrigin.z) + ")" ,1);
	addMsg("vW = (" + f2str(m_cState.m_vW.x) + ", " + f2str(m_cState.m_vW.y) + ")" ,1);

	addMsg("-- Target state --",1);
	addMsg("vP = (" + f2str(m_tState.m_vP.x) + ", " + f2str(m_tState.m_vP.y) + ", " + f2str(m_tState.m_vP.z) + ")" ,1);
	addMsg("vA = (" + f2str(m_tState.m_vA.x) + ", " + f2str(m_tState.m_vA.y) + ", " + f2str(m_tState.m_vA.z) + ")" ,1);
	addMsg("vB = (" + f2str(m_tState.m_vB.x) + ", " + f2str(m_tState.m_vB.y) + ")" ,1);
	addMsg("aw = " + f2str(m_tState.m_aw) ,1);
	addMsg("vOrigin = (" + f2str(m_tState.m_vOrigin.x) + ", " + f2str(m_tState.m_vOrigin.y) + ", " + f2str(m_tState.m_vOrigin.z) + ")" ,1);
	addMsg("vW = (" + f2str(m_tState.m_vW.x) + ", " + f2str(m_tState.m_vW.y) + ")" ,1);

}

}
