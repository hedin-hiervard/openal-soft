#ifndef PHEROES_GAME_DLG_BATTLERESULT_H_
#define PHEROES_GAME_DLG_BATTLERESULT_H_

class iDlg_BattleResult : public iBaseGameDlg
{
public:
	iDlg_BattleResult(iViewMgr* pViewMgr, iBattleEngine& engine, iBattleResult br);

private:
	void OnCreateDlg();
	void DoCompose(const iRect& rect);
	iSize ClientSize() const;
	void iCMDH_ControlCommand(iView* pView, CTRL_CMD_ID cmd, sint32 param);
	void OnMouseUp(const iPoint &pos, MouseId mID, MouseButtonId mbID);

#if defined( PC_VERSION )
	void OnClickNo() {EndDialog(0);}
	void OnClickYes() { EndDialog(0); }
	void OnDown() { EndDialog(0); }
#endif	
	
	class iCasualtList 
	{
	public:
		void Add(CREATURE_TYPE ct, sint32 cnt)
		{
			for (uint32 xx=0; xx<casualties.GetSize(); ++xx) {
				if (casualties[xx].Type() == ct) {
					casualties[xx].Count() += cnt;
					return;
				}
			}
			casualties.Add(iCreatGroup(ct,cnt));
		}

		inline const iCreatGroup& operator[](uint32 idx) const { return casualties[idx]; }
		inline uint32 Count() const { return casualties.GetSize(); }

	private:
		iSimpleArray<iCreatGroup>	casualties;
	};
#ifdef PC_VERSION    
    void UpdateSize();
#endif
private:
	iCasualtList			m_aCas;
	iCasualtList			m_dCas;
	iStringT				m_Title;
	iStringT				m_Text;
	const iBattleEngine&	m_engine;
	iBattleResult			m_br;
#ifdef PC_VERSION
    iStringT				m_TwitterTitle;
	iStringT				m_TwitterText;
	iImgBtn*				m_pTweetBtn;
#endif
};


#endif //PHEROES_GAME_DLG_BATTLERESULT_H_
