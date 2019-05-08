#include "stdafx.h"

iAnimationHost::iAnimationHost() {}

iAnimationHost::~iAnimationHost() {}

fix32 iAnimationHost::TimeLeft(AniQID qid)
{
	fix32 res = fix32::zero;
	for(uint32 xx=0; xx<m_qList[qid].queue.GetSize(); xx++)
		res += m_qList[qid].queue[xx]->GetDuration();
	return res;
}

AniQID iAnimationHost::AddAniObj(iGAniObj* obj, bool newq, AniQID waitForQueue)
{	
	if(m_qList.GetSize() == 0) newq = true;

	AniQID qid = 0;
	if(newq) {
		bool found = false;
/*		for(qid=0; qid<m_qList.GetSize(); qid++)
			if(m_qList[qid].queue.GetSize() == 0 && !m_qList[qid].fixId) { // not allowing to auto-select a fixed queue
				found = true;
				m_qList[qid].loop = false;
				m_qList[qid].pos = 0;
				m_qList[qid].enabled = true;
				break;
			}*/
		if(!found) {
			iAniQ q;
			q.loop = false;
			q.pos = 0;		
			q.enabled = true;
			q.fixId = false;
			m_qList.Add(q);		
			qid = m_qList.GetSize() - 1;
		}
	} 	
	return AddAniObjToQ(obj, qid, waitForQueue);	
}

AniQID iAnimationHost::AddAniObjToQ(iGAniObj* obj, AniQID &qid, AniQID waitForQueue)
{
	if(qid == INVALID_QUEUE) {
		qid = AddAniObj(obj, true);
		return qid;
	}

	if(waitForQueue != INVALID_QUEUE) {
		AddAniObjToQ(new iGAniObj_Delay(TimeLeft(waitForQueue) - TimeLeft(qid)), qid);
	}

	m_qList[qid].queue.Add(obj); 	
	obj->SetHost(this); 
	return qid;
}

bool iAnimationHost::IsPlaying(AniQID qid)
{
	if(qid == INVALID_QUEUE) {
		for(AniQID qid = 0; qid < m_qList.GetSize(); qid++)
			if(m_qList[qid].queue.GetSize() > 0) return true;
		return false;
	}
	return m_qList[qid].queue.GetSize() > 0;
}

void iAnimationHost::StopAllAnimation(bool endAniObjects)
{
	bool thereAreFixedQueues = false;
	
	for(AniQID qid = 0; qid < m_qList.GetSize(); qid++) {
		if(m_qList[qid].fixId) thereAreFixedQueues = true; 
		StopAnimation(qid, endAniObjects);
	}

	if(!thereAreFixedQueues) { // massive-killing of queues with fixed IDs will certainly cause bugs
		m_qList.RemoveAll();
	}
}

void iAnimationHost::StopAnimation(AniQID qid, bool endAniObjects) 
{
	if(qid == INVALID_QUEUE) return;
	while(m_qList[qid].queue.GetSize() > 0) {
		if(endAniObjects) {
			if(m_qList[qid].queue[0]->OnEnd()) {
				gApp.ViewMgr().CurView()->Invalidate();
			}
		}
		delete m_qList[qid].queue[0];
		m_qList[qid].queue.RemoveAt(0);
	}
}

void iAnimationHost::Process(fix32 t) {	
	for(AniQID qid = 0; qid < m_qList.GetSize(); qid++) 
		if(m_qList[qid].enabled) {		
			while(m_qList[qid].queue.GetSize() > 0) {
				iGAniObj* curobj = m_qList[qid].queue[m_qList[qid].pos];
				if(curobj->Process(t)) {
					if(!m_qList[qid].loop) {
						delete curobj;
						m_qList[qid].queue.RemoveAt(0);
						if(m_qList[qid].queue.GetSize() > 0 && m_qList[qid].queue[0]->OnStart())							
							gApp.ViewMgr().CurView()->Invalidate();

					} else {
						curobj->ResetTimer();
						m_qList[qid].pos = (m_qList[qid].pos + 1) % m_qList[qid].queue.GetSize();				
						m_qList[qid].queue[m_qList[qid].pos]->OnStart();
						if(m_qList[qid].pos == 0)
							break;
					}
				} else
					break;
			}	
		}
}

void iAnimationHost::Finish(AniQID qid) { 
	while(IsPlaying(qid)) gApp.Cycle(); 
}


/*
iAniDriver::iAniDriver()
{
	gAniHost = new iAnimationHost(&gApp.ViewMgr(), iRect(0, 0, 0, 0), 0);
}

void iAniDriver::Process(fix32 t) {
	for(uint32 xx=0; xx<m_hostList.GetSize(); xx++) 
		if(m_hostList[xx]->IsVisible() && m_hostList[xx]->IsEnabled())
			m_hostList[xx]->Process(t); 
		else
			m_hostList[xx]->StopAllAnimation();
	//gAniHost->Process(t);
}


iAnimationHost::iAnimationHost(iViewMgr* pViewMgr, const iRect& rect, uint32 uid): iView(pViewMgr, rect, GENERIC_VIEWPORT, uid, Visible | Enabled)
{
	gAniDriver.RegisterHost(this);
}

iAnimationHost::~iAnimationHost() {
	gAniDriver.UnregisterHost(this);
}
*/
iGAniObj::iGAniObj(fix32 t): m_Timer(0), m_Duration(t) { 	
}

bool iGAniObj::Process(fix32 t) { 
	m_Timer +=t; 
	if(OnProcess() && gApp.ViewMgr().CurView())
		gApp.ViewMgr().CurView()->Invalidate();
	if(m_Timer >= m_Duration) {
		if(OnEnd() && gApp.ViewMgr().CurView())
			gApp.ViewMgr().CurView()->Invalidate();
		return true; 
	} else 
		return false; 
}

bool iGAniObj_EnableQueue::OnEnd() 
{
	m_host->SetQEnabled(m_qid, true);
	return false;
}

bool iGAniObj_LoopMarker::OnEnd() 
{
	m_host->SetLoop(true, m_qid);
	return false;
}