/*
 * _Raspivid.h
 *
 *  Created on: May 6, 2018
 *      Author: yankai
 */

#ifndef OpenKAI_src_Vision__Raspivid_H_
#define OpenKAI_src_Vision__Raspivid_H_

#include "../Base/common.h"
#include "_VisionBase.h"

namespace kai
{

enum RASPIVID_MODE
{
	raspivid_rgb,
	raspivid_y,
};

class _Raspivid: public _VisionBase
{
public:
	_Raspivid();
	virtual ~_Raspivid();

	bool init(void* pKiss);
	bool link(void);
	bool start(void);
	void reset(void);

private:
	bool open(void);
	void update(void);
	static void* getUpdateThread(void* This)
	{
		((_Raspivid*) This)->update();
		return NULL;
	}

	void updateTPP(void);
	static void* getTPP(void* This)
	{
		((_Raspivid*) This)->updateTPP();
		return NULL;
	}

public:
	string m_cmdR;
	string m_option;
	RASPIVID_MODE m_cMode;
	FILE* m_pFr;
    int m_iFr;

	string m_cmdW;
    FILE* m_pFw;
    int m_iFw;

    int m_nFB;
    uint8_t* m_pFB;
    std::vector<uint8_t> m_vImg;
};

}
#endif
