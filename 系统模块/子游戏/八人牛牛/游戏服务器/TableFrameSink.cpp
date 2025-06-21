#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <conio.h>
#include <locale>

//////////////////////////////////////////////////////////////////////////

//���������Ϣ
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo;	//���USERIDӳ�������Ϣ
//�����û�����
CList<ROOMUSERDEBUG, ROOMUSERDEBUG &> g_ListRoomUserDebug;		//�����û���������
//�������Լ�¼
CList<CString, CString &> g_ListOperationRecord;						//�������Լ�¼

ROOMUSERINFO	g_CurrentQueryUserInfo;								//��ǰ��ѯ�û���Ϣ

//ȫ�ֱ���
LONGLONG						g_lRoomStorageStart = 0LL;								//������ʼ���
LONGLONG						g_lRoomStorageCurrent = 0LL;							//����Ӯ��
LONGLONG						g_lStorageDeductRoom = 0LL;								//�ؿ۱���
LONGLONG						g_lStorageMax1Room = 0LL;								//���ⶥ
LONGLONG						g_lStorageMul1Room = 0LL;								//ϵͳ��Ǯ����
LONGLONG						g_lStorageMax2Room = 0LL;								//���ⶥ
LONGLONG						g_lStorageMul2Room = 0LL;								//ϵͳ��Ǯ����
//////////////////////////////////////////////////////////////////////////

#define	IDI_SO_OPERATE							2							//����ʱ��
#define	IDI_TIME_ELAPSE							12							//���Ŷ�ʱ��
#define	IDI_DELAY_GAMEFREE						13							//�ӳٿ��ж�ʱ��

//ǰ�����еĲ���ʱ�䶼����Ϊ10��
#define	TIME_SO_OPERATE							12000						//����ʱ��
#define	TIME_DELAY_GAMEFREE						6000						//�ӳٿ��ж�ʱ��

#define	IDI_OFFLINE_TRUSTEE_0					3
#define	IDI_OFFLINE_TRUSTEE_1					4
#define	IDI_OFFLINE_TRUSTEE_2					5
#define	IDI_OFFLINE_TRUSTEE_3					6
#define	IDI_OFFLINE_TRUSTEE_4					7
#define	IDI_OFFLINE_TRUSTEE_5					8
#define	IDI_OFFLINE_TRUSTEE_6					9
#define	IDI_OFFLINE_TRUSTEE_7					10

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
    //��Ϸ����
    m_wPlayerCount = GAME_PLAYER;

    m_lExitScore = 0;

    m_cbTimeRemain = TIME_SO_OPERATE / 1000;
    m_wBankerUser = INVALID_CHAIR;
    m_wFirstEnterUser = INVALID_CHAIR;
    m_listEnterUser.RemoveAll();
    m_listCardTypeOrder.RemoveAll();
    m_bReNewTurn = true;

    //�û�״̬
    ZeroMemory(m_cbDynamicJoin, sizeof(m_cbDynamicJoin));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
    ZeroMemory(m_cbCallBankerStatus, sizeof(m_cbCallBankerStatus));
    ZeroMemory(m_cbCallBankerTimes, sizeof(m_cbCallBankerTimes));
    ZeroMemory(m_cbPrevCallBankerTimes, sizeof(m_cbPrevCallBankerTimes));

    ZeroMemory(m_bOpenCard, sizeof(m_bOpenCard));

    //�˿˱���
    ZeroMemory(m_cbOriginalCardData, sizeof(m_cbOriginalCardData));
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
    ZeroMemory(m_bSpecialCard, sizeof(m_bSpecialCard));
    ZeroMemory(m_cbOriginalCardType, sizeof(m_cbOriginalCardType));
    ZeroMemory(m_cbCombineCardType, sizeof(m_cbCombineCardType));

    //��ע��Ϣ
    ZeroMemory(m_lTurnMaxScore, sizeof(m_lTurnMaxScore));

    ZeroMemory(m_bBuckleServiceCharge, sizeof(m_bBuckleServiceCharge));

    //�������
    m_pITableFrame = NULL;
    m_pGameServiceOption = NULL;

    m_ctConfig = CT_ADDTIMES_;
    m_stConfig = ST_SENDFOUR_;
    m_gtConfig = GT_HAVEKING_;
    m_bgtConfig = BGT_ROB_;
    m_btConfig = BT_FREE_;
    m_tyConfig = BT_TUI_INVALID_;

    m_lFreeConfig[0] = 200;
    m_lFreeConfig[1] = 500;
    m_lFreeConfig[2] = 800;
    m_lFreeConfig[3] = 1100;
    m_lFreeConfig[4] = 1500;

    ZeroMemory(m_lPercentConfig, sizeof(m_lPercentConfig));

    m_lMaxCardTimes = 0;

    ZeroMemory(&m_stRecord, sizeof(m_stRecord));
    for(WORD i = 0; i < GAME_PLAYER; i++)
    {
        m_listWinScoreRecord[i].RemoveAll();
    }

    m_cbTrusteeDelayTime = 3;

#ifdef CARD_CONFIG
    ZeroMemory(m_cbconfigCard, sizeof(m_cbconfigCard));
#endif

    //�������
    g_ListRoomUserDebug.RemoveAll();
    g_ListOperationRecord.RemoveAll();
    ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

    //�������
    m_hInst = NULL;
    m_pServerDebug = NULL;
    m_hInst = LoadLibrary(TEXT("OxEightServerDebug.dll"));
    if(m_hInst)
    {
        typedef void *(*CREATE)();
        CREATE ServerDebug = (CREATE)GetProcAddress(m_hInst, "CreateServerDebug");
        if(ServerDebug)
        {
            m_pServerDebug = static_cast<IServerDebug *>(ServerDebug());
        }
    }

    //��Ϸ��Ƶ
    m_hVideoInst = NULL;
    m_pGameVideo = NULL;
    m_hVideoInst = LoadLibrary(TEXT("OxEightGameVideo.dll"));
    if(m_hVideoInst)
    {
        typedef void *(*CREATE)();
        CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
        if(GameVideo)
        {
            m_pGameVideo = static_cast<IGameVideo *>(GameVideo());
        }
    }

    ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));

    //////////////////////////�Ż�����
    m_lBeBankerCondition = INVALID_DWORD;
    m_lPlayerBetTimes = INVALID_DWORD;
    m_cbAdmitRevCard = TRUE;
    m_cbMaxCallBankerTimes = 5;
    for(WORD i = 0; i < MAX_SPECIAL_CARD_TYPE; i++)
    {
        m_cbEnableCardType[i] = TRUE;
    }
    m_cbClassicTypeConfig = 0;

    m_lBgtDespotWinScore = 0L;
    m_wBgtRobNewTurnChairID = INVALID_CHAIR;
	m_cbRCOfflineTrustee = TRUE;

    ZeroMemory(m_bLastTurnBeBanker, sizeof(m_bLastTurnBeBanker));
    ZeroMemory(m_lLastTurnWinScore, sizeof(m_lLastTurnWinScore));
    ZeroMemory(m_bLastTurnBetBtEx, sizeof(m_bLastTurnBetBtEx));
    ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));

	ZeroMemory(&m_GameEndEx, sizeof(m_GameEndEx));

    srand(time(NULL));

    return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
    if(m_pServerDebug)
    {
        delete m_pServerDebug;
        m_pServerDebug = NULL;
    }

    if(m_hInst)
    {
        FreeLibrary(m_hInst);
        m_hInst = NULL;
    }

    if(m_pGameVideo)
    {
        delete m_pGameVideo;
        m_pGameVideo = NULL;
    }

    if(m_hVideoInst)
    {
        FreeLibrary(m_hVideoInst);
        m_hVideoInst = NULL;
    }
}

//�ӿڲ�ѯ--��������Ϣ�汾
void *CTableFrameSink::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
    QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
    QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
    return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx *pIUnknownEx)
{
    //��ѯ�ӿ�
    ASSERT(pIUnknownEx != NULL);
    m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
    if(m_pITableFrame == NULL) { return false; }

    m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

    //��Ϸ����
    m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
    m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

    //��ȡ����
    ReadConfigInformation();



    return true;
}

//��λ����
void CTableFrameSink::RepositionSink()
{
    //��Ϸ����
    m_lExitScore = 0;
    m_cbTimeRemain = TIME_SO_OPERATE / 1000;

    //�û�״̬
    ZeroMemory(m_cbDynamicJoin, sizeof(m_cbDynamicJoin));
    ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
    ZeroMemory(m_bBuckleServiceCharge, sizeof(m_bBuckleServiceCharge));
    ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
    ZeroMemory(m_cbCallBankerStatus, sizeof(m_cbCallBankerStatus));
    ZeroMemory(m_cbCallBankerTimes, sizeof(m_cbCallBankerTimes));

    ZeroMemory(m_bOpenCard, sizeof(m_bOpenCard));

    //�˿˱���
    ZeroMemory(m_cbOriginalCardData, sizeof(m_cbOriginalCardData));
    ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
    ZeroMemory(m_bSpecialCard, sizeof(m_bSpecialCard));
    ZeroMemory(m_cbOriginalCardType, sizeof(m_cbOriginalCardType));
    ZeroMemory(m_cbCombineCardType, sizeof(m_cbCombineCardType));

    //��ע��Ϣ
    ZeroMemory(m_lTurnMaxScore, sizeof(m_lTurnMaxScore));
    m_wBgtRobNewTurnChairID = INVALID_CHAIR;

    //������ע����
    ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));

	ZeroMemory(&m_GameEndEx, sizeof(m_GameEndEx));

    return;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem *pIServerUserItem)
{
    //���·����û���Ϣ
    UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);

    //�����йܶ�ʱ��
    //��ҳ��ͽ�ҷ���Ĭ���йܣ����ַ�����������������Թ�ѡ�й�
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_cbRCOfflineTrustee == TRUE)
      || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
    {
        pIServerUserItem->SetTrusteeUser(true);
        switch(m_pITableFrame->GetGameStatus())
        {
        case GS_TK_CALL:
        {
            if(m_cbCallBankerStatus[wChairID] == FALSE)
            {
                if(wChairID == m_wBgtRobNewTurnChairID && m_wBgtRobNewTurnChairID != INVALID_CHAIR)
                {
                    OnUserCallBanker(wChairID, true, 1);
                }
                else
                {
                    OnUserCallBanker(wChairID, false, m_cbPrevCallBankerTimes[wChairID]);
                }
            }
            break;
        }
        case GS_TK_SCORE:
        {
            if(m_lTableScore[wChairID] > 0 || wChairID == m_wBankerUser)
            {
                break;
            }
            if(m_lTurnMaxScore[wChairID] > 0)
            {
                if(m_btConfig == BT_FREE_)
                {
                    OnUserAddScore(wChairID, m_lFreeConfig[0] * m_pITableFrame->GetCellScore());
                }
                else if(m_btConfig == BT_PENCENT_)
                {
                    OnUserAddScore(wChairID, m_lTurnMaxScore[wChairID] * m_lPercentConfig[0] / 100);
                }
            }
            else
            {
                OnUserAddScore(wChairID, 1);
            }
            break;
        }
        case GS_TK_PLAYING:
        {
            if(m_bOpenCard[wChairID] == false)
            {
                //��ȡţţ����
                BYTE cbTempHandCardData[MAX_CARDCOUNT];
                ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
                CopyMemory(cbTempHandCardData, m_cbHandCardData[wChairID], sizeof(m_cbHandCardData[wChairID]));

                m_GameLogic.GetOxCard(cbTempHandCardData, MAX_CARDCOUNT);
                OnUserOpenCard(wChairID, cbTempHandCardData);
            }
            break;
        }
        default:
            break;
        }
    }

    return true;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //��ʷ����
    if(bLookonUser == false) { m_HistoryScore.OnEventUserEnter(pIServerUserItem->GetChairID()); }

    if(m_pITableFrame->GetGameStatus() != GS_TK_FREE)
    {
        m_cbDynamicJoin[pIServerUserItem->GetChairID()] = TRUE;
    }

    //���·����û���Ϣ
    UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

    //����ͬ���û�����
    UpdateUserDebug(pIServerUserItem);

    //����ׯ �׽����
    if(m_wFirstEnterUser == INVALID_CHAIR && m_bgtConfig == BGT_DESPOT_)
    {
        m_wFirstEnterUser = wChairID;
    }

    m_listEnterUser.AddTail(wChairID);

    return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem *pIServerUserItem, bool bLookonUser)
{
    //��ʷ����
    if(bLookonUser == false)
    {
        m_HistoryScore.OnEventUserLeave(pIServerUserItem->GetChairID());
        m_cbDynamicJoin[pIServerUserItem->GetChairID()] = FALSE;
    }

    //���·����û���Ϣ
    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

    //�Ƿ�������
    if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
    {
        if(m_bgtConfig == BGT_NIUNIU_ || m_bgtConfig == BGT_NONIUNIU_)
        {
            if(wChairID == m_wBankerUser)
            {
                m_wBankerUser = INVALID_CHAIR;
            }
        }
    }

    POSITION ptListHead = m_listEnterUser.GetHeadPosition();
    POSITION ptTemp;

    //ɾ���뿪���
    while(ptListHead)
    {
        ptTemp = ptListHead;
        if(m_listEnterUser.GetNext(ptListHead) == wChairID)
        {
            m_listEnterUser.RemoveAt(ptTemp);
            break;
        }
    }

    ptListHead = m_listCardTypeOrder.GetHeadPosition();

    //ɾ���뿪���
    while(ptListHead)
    {
        ptTemp = ptListHead;
        if(m_listCardTypeOrder.GetNext(ptListHead) == wChairID)
        {
            m_listCardTypeOrder.RemoveAt(ptTemp);
            break;
        }
    }

    //����ģʽ
    if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
    {
        //��ǰԼս��ɢ�����¼
        if(m_pITableFrame->IsPersonalRoomDisumme())
        {
            ZeroMemory(&m_stRecord, sizeof(m_stRecord));
            ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_wFirstEnterUser = INVALID_CHAIR;

            if(!m_listEnterUser.IsEmpty())
            {
                m_listEnterUser.RemoveAll();
            }

            m_wBankerUser = INVALID_CHAIR;
            m_lBgtDespotWinScore = 0L;

            //������ע����
            ZeroMemory(m_bLastTurnBeBanker, sizeof(m_bLastTurnBeBanker));
            ZeroMemory(m_lLastTurnWinScore, sizeof(m_lLastTurnWinScore));
            ZeroMemory(m_bLastTurnBetBtEx, sizeof(m_bLastTurnBetBtEx));
            ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));
        }
    }
    //�Ƿ�������
    else
    {
        BYTE bUserCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == wChairID)
            {
                continue;
            }

            if(m_pITableFrame->GetTableUserItem(i))
            {
                bUserCount++;
            }
        }

        if(bUserCount == 0 && !m_listEnterUser.IsEmpty())
        {
            m_listEnterUser.RemoveAll();
        }
    }

    if(m_bgtConfig == BGT_NIUNIU_)
    {
        m_bReNewTurn = m_listCardTypeOrder.IsEmpty();

        if(!m_listCardTypeOrder.IsEmpty())
        {
            m_wBankerUser = m_listCardTypeOrder.GetHead();
        }
    }
    else
    {
        m_bReNewTurn = true;
    }

    //��ҷ���
    if((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
    {
        m_listWinScoreRecord[wChairID].RemoveAll();
    }

    m_bLastTurnBeBanker[wChairID] = false;
    m_lLastTurnWinScore[wChairID] = 0;
    m_bLastTurnBetBtEx[wChairID] = false;
    m_lPlayerBetBtEx[wChairID] = 0;

    return true;
}

//�û�ͬ��
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem *pIServerUserItem, VOID *pData, WORD wDataSize)
{
    //˽�˷�������Ϸģʽ
    if(((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0)
    {
        //cbGameRule[1] Ϊ  2345678�ֱ��Ӧ�����������
		if (m_pITableFrame->GetStartMode() != START_MODE_ALL_READY)
		{
			m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
		}
    }

	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		if (!pIServerUserItem->IsClientReady() || !pIServerUserItem->IsAndroidUser())
		{
			continue;
		}

		m_pITableFrame->SendTableData(i, SUB_S_ANDROID_READY);
	}

    return true;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
    //������ע����
    ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));

    //���
    if(g_lRoomStorageCurrent > 0 && NeedDeductStorage())
    {
        g_lRoomStorageCurrent = g_lRoomStorageCurrent - g_lRoomStorageCurrent * g_lStorageDeductRoom / 1000;
    }

    //д��־
    CString strInfo;
    strInfo.Format(TEXT("TABLEID = %d, ��ǰ��棺%I64d"), m_pITableFrame->GetTableID(), g_lRoomStorageCurrent);
    CString strFileName = TEXT("�����־");

    tagLogUserInfo LogUserInfo;
    ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
	CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
    CopyMemory(LogUserInfo.szLogContent, strInfo, sizeof(LogUserInfo.szLogContent));
    //m_pITableFrame->SendGameLogData(LogUserInfo);

    if(m_pGameVideo)
    {
        m_pGameVideo->StartVideo(m_pITableFrame, m_wPlayerCount);
    }

    //���������
    tagCustomRule *pCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();
    m_lMaxCardTimes = INVALID_BYTE;

    if(pCustomRule->ctConfig == CT_CLASSIC_)
    {
        m_lMaxCardTimes = pCustomRule->cbCardTypeTimesClassic[0];
        for(WORD i = 0; i < MAX_CARD_TYPE; i++)
        {
            if(pCustomRule->cbCardTypeTimesClassic[i] > m_lMaxCardTimes)
            {
                m_lMaxCardTimes = pCustomRule->cbCardTypeTimesClassic[i];
            }
        }
    }
    else if(pCustomRule->ctConfig == CT_ADDTIMES_)
    {
        m_lMaxCardTimes = pCustomRule->cbCardTypeTimesAddTimes[0];
        for(WORD i = 0; i < MAX_CARD_TYPE; i++)
        {
            if(pCustomRule->cbCardTypeTimesAddTimes[i] > m_lMaxCardTimes)
            {
                m_lMaxCardTimes = pCustomRule->cbCardTypeTimesAddTimes[i];
            }
        }
    }
    ASSERT(m_lMaxCardTimes != INVALID_BYTE);

    //�������ͱ���

    if(m_cbClassicTypeConfig == INVALID_BYTE)
    {
        m_GameLogic.SetCardTypeTimes(pCustomRule->cbCardTypeTimesAddTimes);
    }
    else if(m_cbClassicTypeConfig == 0)
    {
        //���ͱ���
        m_GameLogic.SetCardTypeTimes(pCustomRule->cbCardTypeTimesClassic);
    }
    else if(m_cbClassicTypeConfig == 1)
    {
        //���ͱ���
        m_GameLogic.SetCardTypeTimes(pCustomRule->cbCardTypeTimesClassic);
    }

    //�������ͼ���
    m_GameLogic.SetEnableCardType(m_cbEnableCardType);

    //�û�״̬
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
        {
            m_cbPlayStatus[i] = FALSE;
        }
        else
        {
            m_cbPlayStatus[i] = TRUE;

            //���·����û���Ϣ
            UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
        }
    }

    CopyMemory(m_RoomCardRecord.cbPlayStatus, m_cbPlayStatus, sizeof(m_RoomCardRecord.cbPlayStatus));

    //����˿�
    BYTE bTempArray[GAME_PLAYER * MAX_CARDCOUNT];
    m_GameLogic.RandCardList(bTempArray, sizeof(bTempArray), (m_gtConfig == GT_HAVEKING_ ? true : false));

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUser == NULL)
        {
            continue;
        }

        //�ɷ��˿�
        CopyMemory(m_cbHandCardData[i], &bTempArray[i * MAX_CARDCOUNT], MAX_CARDCOUNT);
    }

#ifdef CARD_CONFIG
    //if (m_cbconfigCard[0][0] != 0 && m_cbconfigCard[0][1] != 0 && m_cbconfigCard[0][2] != 0)
    //{
    //	CopyMemory(m_cbHandCardData, m_cbconfigCard, sizeof(m_cbHandCardData));
    //}

    //m_cbHandCardData[0][0] = 0x32;
    //m_cbHandCardData[0][1] = 0x35;
    //m_cbHandCardData[0][2] = 0x34;
    //m_cbHandCardData[0][3] = 0x37;
    //m_cbHandCardData[0][4] = 0x4E;

    //m_cbHandCardData[1][0] = 0x4E;
    //m_cbHandCardData[1][1] = 0x3B;
    //m_cbHandCardData[1][2] = 0x4F;
    //m_cbHandCardData[1][3] = 0x0C;
    //m_cbHandCardData[1][4] = 0x1B;
#endif

    //��ʱ�˿�
    BYTE cbTempHandCardData[GAME_PLAYER][MAX_CARDCOUNT];
    ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
    CopyMemory(cbTempHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

    //ԭʼ�˿�
    //CopyMemory(m_cbOriginalCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUser == NULL)
        {
            continue;
        }

        m_bSpecialCard[i] = (m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig) > CT_CLASSIC_OX_VALUENIUNIU ? true : false);

        //��������
        if(m_bSpecialCard[i])
        {
            m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }
        else
        {
            //��ȡţţ����
            m_GameLogic.GetOxCard(cbTempHandCardData[i], MAX_CARDCOUNT);

            m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }
    }

    //��ȡ��ׯģʽ
    ASSERT(m_bgtConfig != BGT_INVALID_);

    bool bRoomServerType = ((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0;

    //ׯ������
    switch(m_bgtConfig)
    {
    //����ׯ
    case BGT_DESPOT_:
    {
        //��ʼĬ��ׯ��
        InitialBanker();

        //����ׯģʽ�� m_lBeBankerConditionΪINVALID_DWORD���������ޣ�����ģʽ��û�и�ѡ��ҲΪINVALID_DWORD

        break;
    }
    //������ׯ
    case BGT_ROB_:
    {
        //����ģʽ�¿�һ��
        //if(bRoomServerType && m_bReNewTurn == true)
        //{
        //    //����ǿ��Ϊׯ����������������Ϸ�����һ����ȥ����Ϸ�����ǿ��Ϊׯ
        //    //��ȡ����
        //    WORD wRoomOwenrChairID = INVALID_CHAIR;
        //    DWORD dwRoomOwenrUserID = INVALID_DWORD;
        //    for(WORD i = 0; i < m_wPlayerCount; i++)
        //    {
        //        //��ȡ�û�
        //        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        //        if(!pIServerUserItem)
        //        {
        //            continue;
        //        }

        //        m_cbCallBankerStatus[i] = TRUE;
        //        m_cbCallBankerTimes[i] = 0;

        //        if(pIServerUserItem->GetUserID() == m_pITableFrame->GetTableOwner() && IsRoomCardType())
        //        {
        //            dwRoomOwenrUserID = pIServerUserItem->GetUserID();
        //            wRoomOwenrChairID = pIServerUserItem->GetChairID();
        //            //break;
        //        }
        //    }

        //    //����������Ϸ
        //    if(dwRoomOwenrUserID != INVALID_DWORD && wRoomOwenrChairID != INVALID_CHAIR)
        //    {
        //        m_wBankerUser = wRoomOwenrChairID;
        //    }
        //    //������������Ϸ�ͷǷ�������
        //    else
        //    {
        //        ASSERT(m_listEnterUser.IsEmpty() == false);
        //        m_wBankerUser = m_listEnterUser.GetHead();
        //    }

        //    ASSERT(m_wBankerUser != INVALID_CHAIR);

        //    m_wBgtRobNewTurnChairID = m_wBankerUser;
        //    m_cbCallBankerStatus[m_wBankerUser] = FALSE;

        //    //��ʼĬ��ׯ��
        //    //InitialBanker();

        //    //�Ƿ��������ö�ʱ��
        //    if(!IsRoomCardType())
        //    {
        //        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
        //        m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
        //    }

        //    //�������ߴ���ʱ��
        //    for(WORD i = 0; i < m_wPlayerCount; i++)
        //    {
        //        if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
        //        {
        //            m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
        //        }
        //    }

        //    //����״̬
        //    m_pITableFrame->SetGameStatus(GS_TK_CALL);
        //    EnableTimeElapse(true);

        //    CMD_S_CallBanker CallBanker;
        //    ZeroMemory(&CallBanker, sizeof(CallBanker));
        //    CallBanker.ctConfig = m_ctConfig;
        //    CallBanker.stConfig = m_stConfig;
        //    CallBanker.bgtConfig = m_bgtConfig;

        //    BYTE *pGameRule = m_pITableFrame->GetGameRule();
        //    CallBanker.wGamePlayerCountRule = pGameRule[1];
        //    CallBanker.cbMaxCallBankerTimes = m_cbMaxCallBankerTimes;
        //    CallBanker.wBgtRobNewTurnChairID = m_wBgtRobNewTurnChairID;

        //    //���·����û���Ϣ
        //    for(WORD i = 0; i < m_wPlayerCount; i++)
        //    {
        //        //��ȡ�û�
        //        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        //        if(pIServerUserItem != NULL)
        //        {
        //            UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
        //        }
        //    }

        //    //��������
        //    for(WORD i = 0; i < m_wPlayerCount; i++)
        //    {
        //        if(m_cbPlayStatus[i] != TRUE)
        //        {
        //            continue;
        //        }
        //        m_pITableFrame->SendTableData(i, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));
        //    }
        //    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));

        //    if(m_pGameVideo)
        //    {
        //        m_pGameVideo->AddVideoData(SUB_S_CALL_BANKER, &CallBanker);
        //    }
        //}
        ////����ģʽ�����¿�һ�� ���߷Ƿ���ģʽ
        //else if((bRoomServerType && !m_bReNewTurn) || !bRoomServerType)
        {
            m_wBgtRobNewTurnChairID = INVALID_CHAIR;

            //�Ƿ��������ö�ʱ��
            if(!IsRoomCardType())
            {
                m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
                m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
            }

            //�������ߴ���ʱ��
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
                {
                    m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
                }
            }

            //����״̬
            m_pITableFrame->SetGameStatus(GS_TK_CALL);
            EnableTimeElapse(true);

            CMD_S_CallBanker CallBanker;
            ZeroMemory(&CallBanker, sizeof(CallBanker));
            CallBanker.ctConfig = m_ctConfig;
            CallBanker.stConfig = m_stConfig;
            CallBanker.bgtConfig = m_bgtConfig;

            BYTE *pGameRule = m_pITableFrame->GetGameRule();
            CallBanker.wGamePlayerCountRule = pGameRule[1];
            CallBanker.cbMaxCallBankerTimes = m_cbMaxCallBankerTimes;
            CallBanker.wBgtRobNewTurnChairID = m_wBgtRobNewTurnChairID;

            //���·����û���Ϣ
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //��ȡ�û�
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItem != NULL)
                {
                    UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
                }
            }

            //��������
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] != TRUE)
                {
                    continue;
                }
                m_pITableFrame->SendTableData(i, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));
            }
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));

            if(m_pGameVideo)
            {
                m_pGameVideo->AddVideoData(SUB_S_CALL_BANKER, &CallBanker);
            }

            //���ĵ���
            if(m_stConfig == ST_SENDFOUR_)
            {
                //���ñ���
                CMD_S_SendFourCard SendFourCard;
                ZeroMemory(&SendFourCard, sizeof(SendFourCard));

                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                    {
                        continue;
                    }

                    //�ɷ��˿�(��ʼֻ��������)
                    CopyMemory(SendFourCard.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
                }

                m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SEND_FOUR_CARD, &SendFourCard, sizeof(SendFourCard));
            }
        }

        break;
    }
    //ţţ��ׯ
    //��ţ��ׯ
    case BGT_NIUNIU_:
    case BGT_NONIUNIU_:
    {
        //�¿�һ��
        if(m_bReNewTurn == true)
        {
            //��ʼĬ��ׯ��
            InitialBanker();
        }
        else
        {
            ASSERT(m_wBankerUser != INVALID_CHAIR);

            m_bBuckleServiceCharge[m_wBankerUser] = true;

            //�Ƿ��������ö�ʱ��
            if(!IsRoomCardType())
            {
                m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
                m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
            }

            //�������ߴ���ʱ��
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
                {
                    m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
                }
            }

            //����״̬
            m_pITableFrame->SetGameStatus(GS_TK_SCORE);
            EnableTimeElapse(true);

            //���·����û���Ϣ
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //��ȡ�û�
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItem != NULL)
                {
                    UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
                }
            }

            //��ȡ�����ע
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] != TRUE || i == m_wBankerUser)
                {
                    continue;
                }

                //��ע����
                m_lTurnMaxScore[i] = GetUserMaxTurnScore(i);
            }

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                if(m_bLastTurnBetBtEx[i] == true)
                {
                    m_bLastTurnBetBtEx[i] = false;
                }
            }

            m_lPlayerBetBtEx[m_wBankerUser] = 0;

            //���ñ���
            CMD_S_GameStart GameStart;
            ZeroMemory(&GameStart, sizeof(GameStart));
            GameStart.wBankerUser = m_wBankerUser;
            CopyMemory(GameStart.cbPlayerStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));

            //���ĵ���
            if(m_stConfig == ST_SENDFOUR_)
            {
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                    {
                        continue;
                    }

                    //�ɷ��˿�(��ʼֻ��������)
                    CopyMemory(GameStart.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
                }
            }

            GameStart.stConfig = m_stConfig;
            GameStart.bgtConfig = m_bgtConfig;
            GameStart.btConfig = m_btConfig;
            GameStart.gtConfig = m_gtConfig;

            CopyMemory(GameStart.lFreeConfig, m_lFreeConfig, sizeof(GameStart.lFreeConfig));
            CopyMemory(GameStart.lPercentConfig, m_lPercentConfig, sizeof(GameStart.lPercentConfig));
            CopyMemory(GameStart.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(GameStart.lPlayerBetBtEx));

            bool bFirstRecord = true;

            WORD wRealPlayerCount = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }

                if(!pServerUserItem)
                {
                    continue;
                }

                wRealPlayerCount++;
            }

            BYTE *pGameRule = m_pITableFrame->GetGameRule();

            //�����ע
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }
                GameStart.lTurnMaxScore = m_lTurnMaxScore[i];
                m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

                if(m_pGameVideo)
                {
                    Video_GameStart video;
                    ZeroMemory(&video, sizeof(video));
                    video.lCellScore = m_pITableFrame->GetCellScore();
                    video.wPlayerCount = wRealPlayerCount;
                    video.wGamePlayerCountRule = pGameRule[1];
                    video.wBankerUser = GameStart.wBankerUser;
                    CopyMemory(video.cbPlayerStatus, GameStart.cbPlayerStatus, sizeof(video.cbPlayerStatus));
                    video.lTurnMaxScore = GameStart.lTurnMaxScore;
                    CopyMemory(video.cbCardData, GameStart.cbCardData, sizeof(video.cbCardData));
                    video.ctConfig = m_ctConfig;
                    video.stConfig = GameStart.stConfig;
                    video.bgtConfig = GameStart.bgtConfig;
                    video.btConfig = GameStart.btConfig;
                    video.gtConfig = GameStart.gtConfig;

                    CopyMemory(video.lFreeConfig, GameStart.lFreeConfig, sizeof(video.lFreeConfig));
                    CopyMemory(video.lPercentConfig, GameStart.lPercentConfig, sizeof(video.lPercentConfig));
                    CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
                    video.wChairID = i;
                    video.lScore = pServerUserItem->GetUserScore();

                    m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, bFirstRecord);

                    if(bFirstRecord == true)
                    {
                        bFirstRecord = false;
                    }
                }
            }
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
        }

        break;
    }
    //������ׯ
    case BGT_FREEBANKER_:
    {
        //�Ƿ��������ö�ʱ��
        if(!IsRoomCardType())
        {
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
            m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
        }

        //�������ߴ���ʱ��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
            }
        }

        //����״̬
        m_pITableFrame->SetGameStatus(GS_TK_CALL);
        EnableTimeElapse(true);

        CMD_S_CallBanker CallBanker;
        ZeroMemory(&CallBanker, sizeof(CallBanker));
        CallBanker.ctConfig = m_ctConfig;
        CallBanker.stConfig = m_stConfig;
        CallBanker.bgtConfig = m_bgtConfig;

        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        CallBanker.wGamePlayerCountRule = pGameRule[1];
        CallBanker.cbMaxCallBankerTimes = m_cbMaxCallBankerTimes;
        CallBanker.wBgtRobNewTurnChairID = m_wBgtRobNewTurnChairID;

        //���·����û���Ϣ
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserItem != NULL)
            {
                UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
            }
        }

        //��������
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] != TRUE)
            {
                continue;
            }
            m_pITableFrame->SendTableData(i, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BANKER, &CallBanker, sizeof(CallBanker));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_CALL_BANKER, &CallBanker);
        }

        break;
    }
    //ͨ���淨
    case BGT_TONGBI_:
    {
        bool bFirstRecord = true;

        WORD wRealPlayerCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            if(!pServerUserItem)
            {
                continue;
            }

            wRealPlayerCount++;
        }

        BYTE *pGameRule = m_pITableFrame->GetGameRule();

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            if(m_pGameVideo)
            {
                Video_GameStart video;
                ZeroMemory(&video, sizeof(video));
                video.lCellScore = m_pITableFrame->GetCellScore();
                video.wPlayerCount = wRealPlayerCount;
                video.wGamePlayerCountRule = pGameRule[1];
                video.wBankerUser = INVALID_CHAIR;// ͨ�����û��ׯ��
                CopyMemory(video.cbPlayerStatus, m_cbPlayStatus, sizeof(video.cbPlayerStatus));
                video.lTurnMaxScore = 0;
                CopyMemory(video.cbCardData, m_cbHandCardData[i], sizeof(video.cbCardData));
                video.ctConfig = m_ctConfig;
                video.stConfig = m_stConfig;
                video.bgtConfig = m_bgtConfig;
                video.btConfig = m_btConfig;
                video.gtConfig = m_gtConfig;

                CopyMemory(video.lFreeConfig, m_lFreeConfig, sizeof(video.lFreeConfig));
                CopyMemory(video.lPercentConfig, m_lPercentConfig, sizeof(video.lPercentConfig));
                CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
                video.wChairID = i;
                video.lScore = pServerUserItem->GetUserScore();

                m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, bFirstRecord);

                if(bFirstRecord == true)
                {
                    bFirstRecord = false;
                }
            }
        }

        m_wBankerUser = INVALID_CHAIR;

        //�Ƿ��������ö�ʱ��
        if(!IsRoomCardType())
        {
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
            m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
        }

        //�������ߴ���ʱ��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
            }
        }

        //����״̬
        m_pITableFrame->SetGameStatus(GS_TK_PLAYING);
        EnableTimeElapse(true);

        //���·����û���Ϣ
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserItem != NULL)
            {
                UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
            }

            if(m_cbPlayStatus[i] == TRUE)
            {
                //��ע���(ͨ������µ�ע)
                m_lTableScore[i] = m_pITableFrame->GetCellScore();
                m_bBuckleServiceCharge[i] = true;
            }
        }

        //��������
        CMD_S_SendCard SendCard;
        ZeroMemory(SendCard.cbCardData, sizeof(SendCard.cbCardData));

        //ͨ���淨������
        //���������ݣ���ע���ƺͷ��ĵ������ִ���
        //AnalyseCard(m_stConfig);

        //��������
        ROOMUSERDEBUG roomuserdebug;
        ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
        POSITION posKeyList;

        //���� (��ע���ƺͷ��ĵ������ֵ���)
        if(m_pServerDebug != NULL && AnalyseRoomUserDebug(roomuserdebug, posKeyList))
        {
            //У������
            ASSERT(roomuserdebug.roomUserInfo.wChairID != INVALID_CHAIR && roomuserdebug.userDebug.cbDebugCount != 0
                   && roomuserdebug.userDebug.debug_type != CONTINUE_CANCEL);

            if(m_pServerDebug->DebugResult(m_cbHandCardData, roomuserdebug, m_stConfig, m_ctConfig, m_gtConfig))
            {
                //��ȡԪ��
                ROOMUSERDEBUG &tmproomuserdebug = g_ListRoomUserDebug.GetAt(posKeyList);

                //У������
                ASSERT(roomuserdebug.userDebug.cbDebugCount == tmproomuserdebug.userDebug.cbDebugCount);

                //���Ծ���
                tmproomuserdebug.userDebug.cbDebugCount--;

                CMD_S_UserDebugComplete UserDebugComplete;
                ZeroMemory(&UserDebugComplete, sizeof(UserDebugComplete));
                UserDebugComplete.dwGameID = roomuserdebug.roomUserInfo.dwGameID;
                CopyMemory(UserDebugComplete.szNickName, roomuserdebug.roomUserInfo.szNickName, sizeof(UserDebugComplete.szNickName));
                UserDebugComplete.debugType = roomuserdebug.userDebug.debug_type;
                UserDebugComplete.cbRemainDebugCount = tmproomuserdebug.userDebug.cbDebugCount;

                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                    if(!pIServerUserItem)
                    {
                        continue;
                    }
                    if(pIServerUserItem->IsAndroidUser() == true || CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
                    {
                        continue;
                    }

                    //��������
                    m_pITableFrame->SendTableData(i, SUB_S_USER_DEBUG_COMPLETE, &UserDebugComplete, sizeof(UserDebugComplete));
                    m_pITableFrame->SendLookonData(i, SUB_S_USER_DEBUG_COMPLETE, &UserDebugComplete, sizeof(UserDebugComplete));

                }
            }
        }

        //��ʱ�˿�,��Ϊ�����͵����˿ˣ�����ԭʼ����
        BYTE cbTempHandCardData[GAME_PLAYER][MAX_CARDCOUNT];
        ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
        CopyMemory(cbTempHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUser == NULL)
            {
                continue;
            }

            m_bSpecialCard[i] = (m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig) > CT_CLASSIC_OX_VALUENIUNIU ? true : false);

            //��������
            if(m_bSpecialCard[i])
            {
                m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
            }
            else
            {
                //��ȡţţ����
                m_GameLogic.GetOxCard(cbTempHandCardData[i], MAX_CARDCOUNT);

                m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
            }
        }

        CopyMemory(SendCard.cbCardData, m_cbHandCardData, sizeof(SendCard.cbCardData));
        CopyMemory(SendCard.bSpecialCard, m_bSpecialCard, sizeof(SendCard.bSpecialCard));
        CopyMemory(SendCard.cbOriginalCardType, m_cbOriginalCardType, sizeof(SendCard.cbOriginalCardType));

        //��������
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard);
        }

        break;
    }
    default:
        break;
    }

    //�û�״̬
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem != NULL)
        {
            //д��־
            CString strOperationRecord;
            strOperationRecord.Format(TEXT("TABLEID = %d �û�USERID = %d, userstatus = %d����Ϸ��ʽ��ʼ"), m_pITableFrame->GetTableID(), pIServerUserItem->GetUserID(), pIServerUserItem->GetUserStatus());
			CString strFileName = TEXT("������־");

            tagLogUserInfo LogUserInfo;
            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
            CopyMemory(LogUserInfo.szLogContent, strOperationRecord, sizeof(LogUserInfo.szLogContent));
            //m_pITableFrame->SendGameLogData(LogUserInfo);
        }
    }

    return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbReason)
{
    switch(cbReason)
    {
    case GER_DISMISS:		//��Ϸ��ɢ
    {
        //��������
        CMD_S_GameEnd GameEnd = { 0 };

        //������Ϣ
        m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

        //������Ϸ
        EnableTimeElapse(false);
        m_pITableFrame->ConcludeGame(GS_TK_FREE);

        if(!IsRoomCardType())
        {
            //ɾ��ʱ��
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

            //ɾ�����ߴ���ʱ��
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
            }
        }

        m_wBankerUser = INVALID_CHAIR;
        m_wFirstEnterUser = INVALID_CHAIR;

        //����ģʽ
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            //��ɢ�����¼
            if(m_pITableFrame->IsPersonalRoomDisumme())
            {
                ZeroMemory(&m_stRecord, sizeof(m_stRecord));
                m_lBgtDespotWinScore = 0L;
            }
        }

        //���·����û���Ϣ
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            if(!pIServerUserItem)
            {
                continue;
            }

            UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
        }

        m_bReNewTurn = true;

        return true;
    }
    case GER_NORMAL:		//�������
    {
        //�������
        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd, sizeof(GameEnd));

        //�����˿�
        BYTE cbUserCardData[GAME_PLAYER][MAX_CARDCOUNT];
        CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

        //��ֵ���һ����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];
        }

        WORD wWinTimes[GAME_PLAYER];

        //��ͨ���淨
        if(m_bgtConfig != BGT_TONGBI_)
        {
            WORD wWinCount[GAME_PLAYER];
            ZeroMemory(wWinCount, sizeof(wWinCount));
            ZeroMemory(wWinTimes, sizeof(wWinTimes));

            //������ׯ ������Ҫ����cbMaxCallBankerTimes
            BYTE cbMaxCallBankerTimes = 1;
            if(m_bgtConfig == BGT_ROB_)
            {
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
                    {
                        cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
                    }
                }
            }

            //ׯ�ұ���
            wWinTimes[m_wBankerUser] = m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[m_wBankerUser]);

            //�Ա����
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE) { continue; }

                //�Ա��˿�
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[i], m_cbCombineCardType[m_wBankerUser]))
                {
                    wWinCount[i]++;
                    //��ȡ����
                    wWinTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[i]);
                }
                else
                {
                    wWinCount[m_wBankerUser]++;
                }
            }

            //��ʱ��ע��Ŀ
            LONGLONG lTempTableScore[GAME_PLAYER];
            ZeroMemory(lTempTableScore, sizeof(lTempTableScore));
            CopyMemory(lTempTableScore, m_lTableScore, sizeof(lTempTableScore));

            //ͳ�Ƶ÷�
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{
				if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)continue;

				if (wWinCount[i]>0)	//�м�ʤ��
				{
					LONGLONG lXianWinScore = m_lTableScore[i] * wWinTimes[i] * cbMaxCallBankerTimes;
					LONGLONG lCarryScore = m_pITableFrame->GetTableUserItem(i)->GetUserScore();

					//��ҵ������Ӯ��������Я����ֵΪ����
					lXianWinScore = min(lXianWinScore, lCarryScore);

					GameEnd.lGameScore[i] = lXianWinScore;
					GameEnd.lGameScore[m_wBankerUser] -= lXianWinScore;
					m_lTableScore[i] = 0;
				}
				else					//ׯ��ʤ��
				{
					//��������ж��ٷ־��⸶���ٷ֡�
					LONGLONG lXianLostScore = m_lTableScore[i] * wWinTimes[m_wBankerUser] * cbMaxCallBankerTimes;
					LONGLONG lCarryScore = m_pITableFrame->GetTableUserItem(i)->GetUserScore();

					lXianLostScore = -min(lXianLostScore, lCarryScore);

					GameEnd.lGameScore[i] = lXianLostScore;
					GameEnd.lGameScore[m_wBankerUser] += (-1)*GameEnd.lGameScore[i];
					m_lTableScore[i] = 0;
				}
			}

            //�м�ǿ�˷���
            GameEnd.lGameScore[m_wBankerUser] += m_lExitScore;

            //�뿪�û�
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_lTableScore[i] > 0) { GameEnd.lGameScore[i] = -m_lTableScore[i]; }
            }

            //��������÷�(ׯ��Я���ֲ���������)
            ASSERT(m_wBankerUser != INVALID_CHAIR);

            LONGLONG lAllPlayerWinScore = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                lAllPlayerWinScore += GameEnd.lGameScore[i];
            }

			//ׯ�Ҳ�����
			ASSERT(lAllPlayerWinScore == -GameEnd.lGameScore[m_wBankerUser]);
			if (lAllPlayerWinScore > 0 && (m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore() + GameEnd.lGameScore[m_wBankerUser]) < 0)
			{
				LONGLONG lBankerRemainScore = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore();
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] >= 0)
					{
						continue;
					}

					lBankerRemainScore += (-GameEnd.lGameScore[i]);
				}

				LONGLONG lUserNeedScore = 0;
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] <= 0)
					{
						continue;
					}

					lUserNeedScore += GameEnd.lGameScore[i];
				}

				LONGLONG lTotalMidVal = 0;
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] <= 0)
					{
						continue;
					}

					GameEnd.lGameScore[i] = (double)(((double)lBankerRemainScore) / ((double)(lUserNeedScore))) * GameEnd.lGameScore[i];

					//���Ӯ�ָĳɣ���ҵ������Ӯ��������Я����ֵΪ���ޡ�
					LONGLONG lUserCarryScore = m_pITableFrame->GetTableUserItem(i)->GetUserScore();
					if (GameEnd.lGameScore[i] > lUserCarryScore)
					{
						GameEnd.lGameScore[i] = lUserCarryScore;
						LONGLONG lMidVal = GameEnd.lGameScore[i] - lUserCarryScore;
						lTotalMidVal += lMidVal;
					}
				}

				GameEnd.lGameScore[m_wBankerUser] = -m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore() + lTotalMidVal;
			}

			//ׯ��Ӯ�ָĳɣ�ׯ�ҵ������Ӯ��������Я����ֵΪ���ޡ�
			if (lAllPlayerWinScore < 0 && (-lAllPlayerWinScore) > m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore())
			{
				LONGLONG lUserNeedScore = 0;
				LONGLONG lBankerCarryScore = m_pITableFrame->GetTableUserItem(m_wBankerUser)->GetUserScore();
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] >= 0)
					{
						continue;
					}

					lUserNeedScore += GameEnd.lGameScore[i];
				}

				//�����м�Ӯ�� 
				LONGLONG lUserWinScore = 0;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] <= 0)
					{
						continue;
					}

					//�м�Ӯ���ܷ�
					lUserWinScore += GameEnd.lGameScore[i];
				}

				//���м�Ӯ��ת�޵�ׯ��Я���Ľ��
				lBankerCarryScore += lUserWinScore;

				//�����мҰ��ձ����⸶
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE || GameEnd.lGameScore[i] >= 0)
					{
						continue;
					}

					GameEnd.lGameScore[i] = -((double)((double)GameEnd.lGameScore[i] / (double)lUserNeedScore)) * lBankerCarryScore;
				}

				GameEnd.lGameScore[m_wBankerUser] = 0;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
					{
						continue;
					}

					GameEnd.lGameScore[m_wBankerUser] += (-GameEnd.lGameScore[i]);
				}
			}

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                m_lLastTurnWinScore[i] = GameEnd.lGameScore[i];
                m_bLastTurnBeBanker[i] = (i == m_wBankerUser ? true : false);

                //��ȡ�û�
                IServerUserItem *pIKeyServerUserItem = m_pITableFrame->GetTableUserItem(i);

                //д��־
                CString strUser;
                strUser.Format(TEXT("TABLEID = %d, CHAIRID = %d USERID = %d�� ׯ�ұ�־ %d, �÷� %I64d ,����USERID = %d, m_bgtConfig = %d"), m_pITableFrame->GetTableID(), i, pIKeyServerUserItem->GetUserID(), m_bLastTurnBeBanker[i], m_lLastTurnWinScore[i], m_pITableFrame->GetTableOwner(), m_bgtConfig);
				CString strFileName = TEXT("������־");

                tagLogUserInfo LogUserInfo;
                ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
                CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
                CopyMemory(LogUserInfo.szLogContent, strUser, sizeof(LogUserInfo.szLogContent));
                //m_pITableFrame->SendGameLogData(LogUserInfo);

				strUser += TEXT("\n");
				WriteInfo(TEXT("����ţ������־.log"), strUser);
            }
        }
        else
        {
            ASSERT(m_wBankerUser == INVALID_CHAIR);

            //���һ����ƣ���������ͨ�ԡ�
            //ʤ�����
            WORD wWinner = INVALID_CHAIR;

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE) { continue; }

                if(wWinner == INVALID_CHAIR)
                {
                    wWinner = i;
                }

                //�Ա��˿�
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinner], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[i], m_cbCombineCardType[wWinner]))
                {
                    wWinner = i;
                }
            }

            WORD wWinTimes = m_GameLogic.GetTimes(cbUserCardData[wWinner], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[wWinner]);

            //ͳ�Ƶ÷�
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(i == wWinner || m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                GameEnd.lGameScore[i] = -min(m_pITableFrame->GetTableUserItem(i)->GetUserScore(), m_lTableScore[i] * wWinTimes);
                m_lTableScore[i] = 0;

                GameEnd.lGameScore[wWinner] += (-GameEnd.lGameScore[i]);
            }

            GameEnd.lGameScore[wWinner] += m_lExitScore;
            m_lTableScore[wWinner] = 0;

            //�뿪�û�
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_lTableScore[i] > 0) { GameEnd.lGameScore[i] = -m_lTableScore[i]; }
            }
        }

        //�޸Ļ���
        tagScoreInfo ScoreInfoArray[GAME_PLAYER];
        ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));

        //����˰��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE) { continue; }

            if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                    ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
            {
                if(GameEnd.lGameScore[i] > 0L)
                {
                    GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                    if(GameEnd.lGameTax[i] > 0)
                    {
                        GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
                    }
                }
            }

            //��ʷ����
            m_HistoryScore.OnEventUserScore(i, GameEnd.lGameScore[i]);

            //�������
            ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
            ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
            ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;

            //Լս��¼
            if(m_stRecord.nCount < MAX_RECORD_COUNT)
            {
                if(GameEnd.lGameScore[i] > 0)
                {
                    m_stRecord.lUserWinCount[i]++;
                }
                else
                {
                    m_stRecord.lUserLostCount[i]++;
                }
            }

            //����ģʽ
            //���ַ���
            if((m_pITableFrame->GetDataBaseMode() == 0) && !m_pITableFrame->IsPersonalRoomDisumme() && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && (m_RoomCardRecord.nCount < MAX_RECORD_COUNT))
            {
                m_RoomCardRecord.lDetailScore[i][m_RoomCardRecord.nCount] = ScoreInfoArray[i].lScore;
            }
            //��ҷ���
            else if((m_pITableFrame->GetDataBaseMode() == 1) && ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
            {
                m_listWinScoreRecord[i].AddHead(ScoreInfoArray[i].lScore);
            }
        }

        m_stRecord.nCount++;

        //����ģʽ
        if((m_pITableFrame->GetDataBaseMode() == 0) && (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && (m_RoomCardRecord.nCount < MAX_RECORD_COUNT))
        {
            m_RoomCardRecord.nCount++;
        }

        //����ģʽ
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE || m_bSpecialCard[i] == false)
            {
                continue;
            }

            m_GameLogic.GetSpecialSortCard(m_cbCombineCardType[i], m_cbHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }

        CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

        //��ȡ�������
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            GameEnd.cbCardType[i] = m_cbCombineCardType[i];
        }

        //�������ͱ���
        CopyMemory(GameEnd.wCardTypeTimes, wWinTimes, sizeof(wWinTimes));

        //������Ϣ
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
            m_pITableFrame->SendTableData(i, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
        }

        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		CopyMemory(&m_GameEndEx, &GameEnd, sizeof(GameEnd));
		m_GameEndEx.dwTickCountGameEnd = (DWORD)time(NULL);

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
        }

        if(m_pGameVideo)
        {
            m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
        }

        TryWriteTableScore(ScoreInfoArray);

		//������Ϣ
		for (WORD i = 0; i<m_wPlayerCount; i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL)
			{
				continue;
			}

			if (m_cbPlayStatus[i] == FALSE&&m_cbDynamicJoin[i] == FALSE)
			{
				continue;
			}

			if (!pIServerUserItem->IsAndroidUser())
			{
				continue;
			}

			if (!pIServerUserItem->IsClientReady())
			{
				continue;
			}

			m_pITableFrame->SendTableData(i, SUB_S_ANDROID_BANKOPER);
		}

        //���ͳ��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserIte == NULL) { continue; }

            //����ۼ�
            if(!pIServerUserIte->IsAndroidUser())
            {
                g_lRoomStorageCurrent -= GameEnd.lGameScore[i];
            }

        }

        //ţţ��ׯ ������ţţ�Լ�ţţ���ϵ����ͣ�������������ҵ�ׯ, ���û�л���ԭ���ĵ�ׯ
        if(m_bgtConfig == BGT_NIUNIU_)
        {
            //����������
            WORD wMaxPlayerUser = INVALID_CHAIR;

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //��ȡ�û�
                IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItem == NULL)
                {
                    continue;
                }

                if(wMaxPlayerUser == INVALID_CHAIR)
                {
                    wMaxPlayerUser = i;
                }

                //��ȡ�ϴ���
                if(m_GameLogic.CompareCard(cbUserCardData[i], m_cbHandCardData[wMaxPlayerUser], MAX_CARDCOUNT, m_ctConfig, m_cbCombineCardType[i], m_cbCombineCardType[wMaxPlayerUser]) == true)
                {
                    wMaxPlayerUser = i;
                }
            }

            BYTE cbMaxCardType = ((m_cbCombineCardType[wMaxPlayerUser] == INVALID_BYTE) ? (m_GameLogic.GetCardType(m_cbHandCardData[wMaxPlayerUser], MAX_CARDCOUNT, m_ctConfig)) : m_cbCombineCardType[wMaxPlayerUser]);

            //������ͱ�ţţ��ԭ������ҵ�ׯ
            if(cbMaxCardType >= CT_ADDTIMES_OX_VALUENIUNIU)
            {
                //���������ҵ�ׯ
                m_wBankerUser = wMaxPlayerUser;
            }

            //�������������С
            //�����˿�
            m_listCardTypeOrder.RemoveAll();
            BYTE cbTempCardData[GAME_PLAYER][MAX_CARDCOUNT];
            CopyMemory(cbTempCardData, m_cbHandCardData, sizeof(cbTempCardData));

            BYTE cbTempCombineCardType[GAME_PLAYER];
            CopyMemory(cbTempCombineCardType, m_cbCombineCardType, sizeof(cbTempCombineCardType));

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                //��ȡ�û�
                IServerUserItem *pIServerUserItemI = m_pITableFrame->GetTableUserItem(i);
                if(pIServerUserItemI == NULL)
                {
                    continue;
                }

                for(WORD j = 0; j < m_wPlayerCount - i - 1; j++)
                {
                    //��ȡ�û�
                    IServerUserItem *pIServerUserItemJ = m_pITableFrame->GetTableUserItem(j);
                    if(pIServerUserItemJ == NULL)
                    {
                        continue;
                    }

                    //��ȡ�ϴ���
                    if(m_GameLogic.CompareCard(cbTempCardData[i], cbTempCardData[j], MAX_CARDCOUNT, m_ctConfig, cbTempCombineCardType[i], cbTempCombineCardType[j]) == false)
                    {
                        BYTE cbTempData[MAX_CARDCOUNT];
                        CopyMemory(cbTempData, cbTempCardData[j], sizeof(cbTempData));
                        CopyMemory(cbTempCardData[j], cbTempCardData[i], sizeof(cbTempCardData[j]));
                        CopyMemory(cbTempCardData[i], cbTempData, sizeof(cbTempCardData[i]));

                        BYTE cbTempType = cbTempCombineCardType[j];
                        cbTempCombineCardType[j] = cbTempCombineCardType[i];
                        cbTempCombineCardType[i] = cbTempType;
                    }
                }
            }

            for(WORD i = 0; i < GAME_PLAYER; i++)
            {
                if(cbTempCardData[i][0] == 0)
                {
                    continue;
                }

                WORD wKeyChairID = SearchKeyCardChairID(cbTempCardData[i]);
                ASSERT(wKeyChairID != INVALID_CHAIR);

                m_listCardTypeOrder.AddTail(wKeyChairID);
            }
        }
        //��ţ��ׯ
        else if(m_bgtConfig == BGT_NONIUNIU_)
        {
            //ׯ����ţ�ֵ���һλ��ׯ
            ASSERT(m_wBankerUser != INVALID_CHAIR);
            BYTE cbBankerCardType = ((m_cbCombineCardType[m_wBankerUser] == INVALID_BYTE) ? (m_GameLogic.GetCardType(m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig)) : m_cbCombineCardType[m_wBankerUser]);
            if(cbBankerCardType == CT_CLASSIC_OX_VALUE0
                    || cbBankerCardType == CT_ADDTIMES_OX_VALUE0)
            {
                //ʼ���û�
                while(true)
                {
                    m_wBankerUser = (m_wBankerUser + 1) % m_wPlayerCount;
                    if(m_pITableFrame->GetTableUserItem(m_wBankerUser) != NULL && m_cbPlayStatus[m_wBankerUser] == TRUE)
                    {
                        break;
                    }
                }
            }
        }

        //���Ϳ��
		CMD_S_ADMIN_STORAGE_INFO StorageInfo;
		ZeroMemory(&StorageInfo, sizeof(StorageInfo));
		StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
		StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
		StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
		StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
		StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
		StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
		StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
		m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

        //����ģʽ
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            //��ɢ�����¼
            if(m_pITableFrame->IsPersonalRoomDisumme())
            {
                ZeroMemory(&m_stRecord, sizeof(m_stRecord));
                ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
                m_lBgtDespotWinScore = 0L;
            }
        }

        //˽�˷�����ׯׯ��ͳ����Ӯ
        BOOL bEndLoop = FALSE;
        if(m_bgtConfig == BGT_DESPOT_ && m_lBeBankerCondition != INVALID_DWORD)
        {
            m_lBgtDespotWinScore += GameEnd.lGameScore[m_wBankerUser];

            if((-m_lBgtDespotWinScore) > m_lBeBankerCondition)
            {
                bEndLoop = TRUE;
            }
        }

        EnableTimeElapse(false);
		m_pITableFrame->SetGameTimer(IDI_DELAY_GAMEFREE, TIME_DELAY_GAMEFREE, 1, (WPARAM)(&bEndLoop));
		//m_pITableFrame->ConcludeGame(GS_TK_FREE, bEndLoop);

        if(!IsRoomCardType())
        {
            //ɾ��ʱ��
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

            //ɾ�����ߴ���ʱ��
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
            }
        }

        //���·����û���Ϣ
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            if(!pIServerUserItem)
            {
                continue;
            }

            UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
        }

        m_bReNewTurn = false;

        //�޸�©��
        //���� 1�����Ӵ������䣬����ͨ���淨��Ȼ������֮���ɢ�����ʱ���ٴ���һ��ţţ��ׯ������ţ��ׯ������պ�������1�����Ӵ�����m_wBankerUser��Ϊ��Ч����˱���
        if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0) && m_pITableFrame->IsPersonalRoomDisumme())
        {
            m_bReNewTurn = true;
            m_wBankerUser = INVALID_CHAIR;
        }

        return true;
    }
    case GER_USER_LEAVE:		//�û�ǿ��
    case GER_NETWORK_ERROR:
    {
        //�콢ƽ̨û��ǿ��
        //Ч�����
        ASSERT(pIServerUserItem != NULL);
        ASSERT(wChairID < m_wPlayerCount && (m_cbPlayStatus[wChairID] == TRUE || m_cbDynamicJoin[wChairID] == FALSE));

        if(m_cbPlayStatus[wChairID] == FALSE) { return true; }
        //����״̬
        m_cbPlayStatus[wChairID] = FALSE;
        m_cbDynamicJoin[wChairID] = FALSE;

        //�������
        CMD_S_PlayerExit PlayerExit;
        PlayerExit.wPlayerID = wChairID;

        //������Ϣ
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == wChairID || (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)) { continue; }
            m_pITableFrame->SendTableData(i, SUB_S_PLAYER_EXIT, &PlayerExit, sizeof(PlayerExit));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PLAYER_EXIT, &PlayerExit, sizeof(PlayerExit));

        ////////////////////////////////////
        if(m_bgtConfig == BGT_TONGBI_)
        {
            //�����Ǹ����ֱ�ӿ۵�ס��Ȼ��ֱ��д�֣����ʣ�³���2�����ϵ���Ҽ������ƣ�  ��������ҵĵ�ס�ӵ��Ǹ�Ӯ��������ϣ�  ������ܺ�ֻʣһ����ң��ͽ���
            m_lExitScore += m_lTableScore[wChairID];

            tagScoreInfo ScoreInfo;
            ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
            ScoreInfo.lScore = -m_lTableScore[wChairID];
            ScoreInfo.cbType = SCORE_TYPE_FLEE;

            m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

            m_lTableScore[wChairID] = 0;

            //��ȡ�û�
            IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(wChairID);

            //����ۼ�
            if((pIServerUserIte != NULL) && (!pIServerUserIte->IsAndroidUser()))
            {
                g_lRoomStorageCurrent -= ScoreInfo.lScore;
            }

            //�������
            WORD wUserCount = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE)
                {
                    wUserCount++;
                }
            }

            //������Ϸ
            if(wUserCount == 1)
            {
                //�������
                CMD_S_GameEnd GameEnd;
                ZeroMemory(&GameEnd, sizeof(GameEnd));
                CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

                //��ֵ���һ����
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];
                }

                //��ȡ�������
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && i != wChairID)
                    {
                        continue;
                    }

                    if(i == wChairID)
                    {
                        GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                        continue;
                    }

                    GameEnd.lGameScore[i] = m_pITableFrame->GetCellScore();
                    if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                            ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                    {
                        GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                    }
                    GameEnd.lGameScore[i] = GameEnd.lGameScore[i] - GameEnd.lGameTax[i];
                    GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                }

                //������Ϣ
                m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                if(m_pGameVideo)
                {
                    m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                    m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                }

                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE)
                    {
                        continue;
                    }

                    //�޸Ļ���
                    tagScoreInfo ScoreInfo;
                    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
                    ScoreInfo.lScore = GameEnd.lGameScore[i];
                    ScoreInfo.lRevenue = GameEnd.lGameTax[i];
                    ScoreInfo.cbType = (GameEnd.lGameScore[i] > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE);

                    m_pITableFrame->WriteUserScore(i, ScoreInfo);

                    //��ȡ�û�
                    IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(i);

                    //����ۼ�
                    if((pIServerUserIte != NULL) && (!pIServerUserIte->IsAndroidUser()))
                    {
                        g_lRoomStorageCurrent -= GameEnd.lGameScore[i];
                    }
                }

                //������Ϸ
                m_pITableFrame->ConcludeGame(GS_TK_FREE);

                if(!IsRoomCardType())
                {
                    //ɾ��ʱ��
                    m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                    //ɾ�����ߴ���ʱ��
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                    }
                }

                UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                //���·����û���Ϣ
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == wChairID)
                    {
                        continue;
                    }

                    //��ȡ�û�
                    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                    if(!pIServerUserItem)
                    {
                        continue;
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
                }

                m_bReNewTurn = false;

                return true;
            }

            OnUserOpenCard(wChairID, m_cbHandCardData[wChairID]);
        }

        ////////////////////////////////////
        WORD wWinTimes[GAME_PLAYER];
        ZeroMemory(wWinTimes, sizeof(wWinTimes));

        //������ׯ ������Ҫ����cbMaxCallBankerTimes
        BYTE cbMaxCallBankerTimes = 1;
        if(m_bgtConfig == BGT_ROB_)
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
                {
                    cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
                }
            }
        }

        //��Ϸ���� ����ע��
        if(m_pITableFrame->GetGameStatus() == GS_TK_PLAYING)
        {
            if(wChairID == m_wBankerUser)	//ׯ��ǿ��
            {
                //�������
                CMD_S_GameEnd GameEnd;
                ZeroMemory(&GameEnd, sizeof(GameEnd));
                ZeroMemory(wWinTimes, sizeof(wWinTimes));
                CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

                BYTE cbUserCardData[GAME_PLAYER][MAX_CARDCOUNT];
                CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(cbUserCardData));

                //��ֵ���һ����
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];
                }

                //�÷ֱ���
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE) { continue; }
                    wWinTimes[i] = (m_pITableFrame->GetGameStatus() != GS_TK_PLAYING) ? (1) : (m_GameLogic.GetTimes(cbUserCardData[i], MAX_CARDCOUNT, m_ctConfig, INVALID_BYTE));
                }

                //ͳ�Ƶ÷� ���»�û��
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE) { continue; }
                    GameEnd.lGameScore[i] = m_lTableScore[i] * wWinTimes[i] * cbMaxCallBankerTimes;
                    GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameScore[i];
                    m_lTableScore[i] = 0;
                }

                //�޸Ļ���
                tagScoreInfo ScoreInfoArray[GAME_PLAYER];
                ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

                //����˰��
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && i != m_wBankerUser) { continue; }

                    if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                            ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                    {
                        if(GameEnd.lGameScore[i] > 0L)
                        {
                            GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                            if(GameEnd.lGameTax[i] > 0)
                            {
                                GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
                            }
                        }
                    }

                    //�������
                    ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
                    ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];

                    if(i == m_wBankerUser)
                    {
                        ScoreInfoArray[i].cbType = SCORE_TYPE_FLEE;
                    }
                    else if(m_cbPlayStatus[i] == TRUE)
                    {
                        ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
                    }

                    m_pITableFrame->WriteUserScore(i, ScoreInfoArray[i]);
                }

                //��ȡ�������
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE)
                    {
                        continue;
                    }

                    GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                }

                //�������ͱ���
                CopyMemory(GameEnd.wCardTypeTimes, wWinTimes, sizeof(wWinTimes));

                //������Ϣ
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == m_wBankerUser || (m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)) { continue; }
                    m_pITableFrame->SendTableData(i, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                }
                m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                if(m_pGameVideo)
                {
                    m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                }

                if(m_pGameVideo)
                {
                    m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                }

                //TryWriteTableScore(ScoreInfoArray);

                //д����
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && i != m_wBankerUser) { continue; }

                    //��ȡ�û�
                    IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(i);

                    //����ۼ�
                    if((pIServerUserIte != NULL) && (!pIServerUserIte->IsAndroidUser()))
                    {
                        g_lRoomStorageCurrent -= GameEnd.lGameScore[i];
                    }

                }
                //������Ϸ
                m_pITableFrame->ConcludeGame(GS_TK_FREE);

                if(!IsRoomCardType())
                {
                    //ɾ��ʱ��
                    m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                    //ɾ�����ߴ���ʱ��
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                    }
                }

                UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                //���·����û���Ϣ
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == wChairID)
                    {
                        continue;
                    }

                    //��ȡ�û�
                    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                    if(!pIServerUserItem)
                    {
                        continue;
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
                }

                m_bReNewTurn = false;

                return true;
            }
            else						//�м�ǿ��
            {
                //�Ѿ���ע
                if(m_lTableScore[wChairID] > 0L)
                {
                    ZeroMemory(wWinTimes, sizeof(wWinTimes));

                    //�û��˿�
                    BYTE cbUserCardData[MAX_CARDCOUNT];
                    CopyMemory(cbUserCardData, m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);

                    //�û�����
                    wWinTimes[m_wBankerUser] = (m_pITableFrame->GetGameStatus() == GS_TK_SCORE) ? (1) : (m_GameLogic.GetTimes(cbUserCardData, MAX_CARDCOUNT, m_ctConfig, INVALID_BYTE));

                    //�޸Ļ���
                    LONGLONG lScore = -m_lTableScore[wChairID] * wWinTimes[m_wBankerUser] * cbMaxCallBankerTimes;
                    m_lExitScore += (-1 * lScore);
                    m_lTableScore[wChairID] = (-1 * lScore);

                    tagScoreInfo ScoreInfo;
                    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
                    ScoreInfo.lScore = lScore;
                    ScoreInfo.cbType = SCORE_TYPE_FLEE;

                    m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

                    //��ȡ�û�
                    IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(wChairID);

                    //����ۼ�
                    if((pIServerUserIte != NULL) && (!pIServerUserIte->IsAndroidUser()))
                    {
                        g_lRoomStorageCurrent -= lScore;
                    }
                }

                //�������
                WORD wUserCount = 0;
                for(WORD i = 0; i < m_wPlayerCount; i++)if(m_cbPlayStatus[i] == TRUE) { wUserCount++; }

                //������Ϸ
                if(wUserCount == 1)
                {
                    //�������
                    CMD_S_GameEnd GameEnd;
                    ZeroMemory(&GameEnd, sizeof(GameEnd));
                    CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
                    ASSERT(m_lExitScore >= 0L);

                    //��ֵ���һ����
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];

                    }
                    //ͳ�Ƶ÷�
                    GameEnd.lGameScore[m_wBankerUser] += m_lExitScore;
                    if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                            ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                    {
                        GameEnd.lGameTax[m_wBankerUser] = m_pITableFrame->CalculateRevenue(m_wBankerUser, GameEnd.lGameScore[m_wBankerUser]);
                    }
                    GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameTax[m_wBankerUser];

                    //��ȡ�������
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(m_cbPlayStatus[i] == FALSE)
                        {
                            continue;
                        }

                        GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                    }

                    //�������ͱ���
                    CopyMemory(GameEnd.wCardTypeTimes, wWinTimes, sizeof(wWinTimes));

                    //������Ϣ
                    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                    if(m_pGameVideo)
                    {
                        m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                        m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                    }

                    WORD Zero = 0;
                    for(; Zero < m_wPlayerCount; Zero++)if(m_lTableScore[Zero] != 0) { break; }
                    if(Zero != m_wPlayerCount)
                    {
                        //�޸Ļ���
                        tagScoreInfo ScoreInfo;
                        ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
                        ScoreInfo.lScore = GameEnd.lGameScore[m_wBankerUser];
                        ScoreInfo.lRevenue = GameEnd.lGameTax[m_wBankerUser];
                        ScoreInfo.cbType = SCORE_TYPE_WIN;

                        m_pITableFrame->WriteUserScore(m_wBankerUser, ScoreInfo);

                        //��ȡ�û�
                        IServerUserItem *pIServerUserIte = m_pITableFrame->GetTableUserItem(wChairID);

                        //����ۼ�
                        if((pIServerUserIte != NULL) && (!pIServerUserIte->IsAndroidUser()))
                        {
                            g_lRoomStorageCurrent -= GameEnd.lGameScore[m_wBankerUser];
                        }

                    }

                    //������Ϸ
                    m_pITableFrame->ConcludeGame(GS_TK_FREE);

                    if(!IsRoomCardType())
                    {
                        //ɾ��ʱ��
                        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                        //ɾ�����ߴ���ʱ��
                        for(WORD i = 0; i < m_wPlayerCount; i++)
                        {
                            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                        }
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                    //���·����û���Ϣ
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(i == wChairID)
                        {
                            continue;
                        }

                        //��ȡ�û�
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                        if(!pIServerUserItem)
                        {
                            continue;
                        }

                        UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
                    }

                    m_bReNewTurn = false;

                    return true;
                }

                OnUserOpenCard(wChairID, m_cbHandCardData[wChairID]);
            }
        }
        //��ע����
        else if(m_pITableFrame->GetGameStatus() == GS_TK_SCORE)
        {
            //ʣ���������
            WORD wUserCount = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE)
                {
                    wUserCount++;
                }
            }

            //���ĵ��� (�ѷ�4����) ������С��������⸶
            if(m_stConfig == ST_SENDFOUR_)
            {
                //��ȡ��С����
                LONGLONG lMinJetton = 0L;

                //�������ö��
                if(m_btConfig == BT_FREE_)
                {
                    lMinJetton = m_lFreeConfig[0];
                }
                //�ٷֱ����ö��
                else if(m_btConfig == BT_PENCENT_)
                {
                    //��С��ע���
                    WORD wMinJettonChairID = INVALID_CHAIR;
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        //��ȡ�û�
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                        if(pIServerUserItem == NULL || m_cbPlayStatus[i] == FALSE)
                        {
                            continue;
                        }

                        if(wMinJettonChairID == INVALID_CHAIR) { wMinJettonChairID = i; }

                        //��ȡ�ϴ���
                        if(m_lTurnMaxScore[i] < m_lTurnMaxScore[wMinJettonChairID])
                        {
                            wMinJettonChairID = i;
                        }
                    }

                    ASSERT(wMinJettonChairID != INVALID_CHAIR);

                    lMinJetton = m_lTurnMaxScore[wMinJettonChairID] * m_lPercentConfig[0] / 100;
                }

                CMD_S_GameEnd GameEnd;
                ZeroMemory(&GameEnd, sizeof(GameEnd));
                CopyMemory(GameEnd.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

                //��ֵ���һ����
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    GameEnd.cbLastSingleCardData[i] = m_cbOriginalCardData[i][4];
                }

                tagScoreInfo ScoreInfoArray[GAME_PLAYER];
                ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

                //ׯ�����ܽ�����Ϸ
                if(wChairID == m_wBankerUser)
                {
                    //���ĵ���	ׯ�����ܣ�����ʾ����
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        GameEnd.cbCardType[i] = INVALID_BYTE;
                        //if(m_cbPlayStatus[i]==FALSE)
                        //{
                        //	continue;
                        //}

                        //GameEnd.cbCardType[i] = (m_cbCombineCardType[i] == 0 ? m_cbOriginalCardType[i] : m_cbCombineCardType[i]);
                    }

                    LONGLONG lBankerScore = 0L;
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        //��ȡ�û�
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                        if(pIServerUserItem == NULL || m_cbPlayStatus[i] == FALSE || i == m_wBankerUser)
                        {
                            continue;
                        }

                        //ͳ�Ƶ÷�
                        GameEnd.lGameScore[i] += lMinJetton;
                        if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                                ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                        {
                            GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
                        }
                        GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];

                        //д��
                        ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
                        ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
                        ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;

                        lBankerScore -= lMinJetton;
                    }

                    GameEnd.lGameScore[m_wBankerUser] = lBankerScore;
                    ScoreInfoArray[m_wBankerUser].lScore = GameEnd.lGameScore[m_wBankerUser];
                    ScoreInfoArray[m_wBankerUser].cbType = SCORE_TYPE_FLEE;

                    //������Ϣ
                    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                    if(m_pGameVideo)
                    {
                        m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                        m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                    }

                    //д��
                    //TryWriteTableScore(ScoreInfoArray);

                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        //��ȡ�û�
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                        if(pIServerUserItem == NULL)
                        {
                            continue;
                        }

                        m_pITableFrame->WriteUserScore(i, ScoreInfoArray[i]);
                    }

                    //������Ϸ
                    m_pITableFrame->ConcludeGame(GS_TK_FREE);

                    if(!IsRoomCardType())
                    {
                        //ɾ��ʱ��
                        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                        //ɾ�����ߴ���ʱ��
                        for(WORD i = 0; i < m_wPlayerCount; i++)
                        {
                            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                        }
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                    //���·����û���Ϣ
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(i == wChairID)
                        {
                            continue;
                        }

                        //��ȡ�û�
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                        if(!pIServerUserItem)
                        {
                            continue;
                        }

                        UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
                    }

                    m_bReNewTurn = false;

                    return true;
                }
                //�м����ܣ�ֻʣ��һ�����
                else if(wChairID != m_wBankerUser && wUserCount == 1)
                {
                    //ͳ�Ƶ÷�
                    GameEnd.lGameScore[wChairID] -= lMinJetton;
                    GameEnd.lGameScore[m_wBankerUser] += lMinJetton;
                    if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0) ||
                            ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1))
                    {
                        GameEnd.lGameTax[m_wBankerUser] = m_pITableFrame->CalculateRevenue(m_wBankerUser, GameEnd.lGameScore[m_wBankerUser]);
                    }
                    GameEnd.lGameScore[m_wBankerUser] -= GameEnd.lGameTax[m_wBankerUser];

                    //д��
                    ScoreInfoArray[wChairID].lScore = GameEnd.lGameScore[wChairID];
                    ScoreInfoArray[wChairID].cbType = SCORE_TYPE_FLEE;
                    ScoreInfoArray[m_wBankerUser].lScore = GameEnd.lGameScore[m_wBankerUser];
                    ScoreInfoArray[m_wBankerUser].cbType = SCORE_TYPE_WIN;
                    ScoreInfoArray[m_wBankerUser].lRevenue = GameEnd.lGameTax[m_wBankerUser];

                    //������Ϣ
                    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                    if(m_pGameVideo)
                    {
                        m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                        m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                    }

                    //д��
                    //TryWriteTableScore(ScoreInfoArray);

                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        //��ȡ�û�
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
                        if(pIServerUserItem == NULL)
                        {
                            continue;
                        }

                        m_pITableFrame->WriteUserScore(i, ScoreInfoArray[i]);
                    }

                    //������Ϸ
                    m_pITableFrame->ConcludeGame(GS_TK_FREE);

                    if(!IsRoomCardType())
                    {
                        //ɾ��ʱ��
                        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                        //ɾ�����ߴ���ʱ��
                        for(WORD i = 0; i < m_wPlayerCount; i++)
                        {
                            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                        }
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                    //���·����û���Ϣ
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(i == wChairID)
                        {
                            continue;
                        }

                        //��ȡ�û�
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                        if(!pIServerUserItem)
                        {
                            continue;
                        }

                        UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
                    }

                    m_bReNewTurn = false;

                    return true;
                }
                //�м����ܣ�ʣ��2��������� ������Ϸ
                else
                {
                    m_lExitScore += lMinJetton;
                    m_lTableScore[wChairID] = 0;

                    tagScoreInfo ScoreInfoArray[GAME_PLAYER];
                    ZeroMemory(ScoreInfoArray, sizeof(ScoreInfoArray));
                    ScoreInfoArray[wChairID].lScore -= lMinJetton;
                    ScoreInfoArray[wChairID].cbType = SCORE_TYPE_FLEE;

                    //TryWriteTableScore(ScoreInfoArray);
                    m_pITableFrame->WriteUserScore(wChairID, ScoreInfoArray[wChairID]);

                    OnUserAddScore(wChairID, 0);
                }
            }
            //��ע���� (δ���Ʋ���Ҫ�⸶)
            else if(m_stConfig == ST_BETFIRST_)
            {
                //ׯ�����ܽ�����Ϸ ��ֻʣ��һ�����
                if(wChairID == m_wBankerUser || wUserCount == 1)
                {
                    CMD_S_GameEnd GameEnd;
                    ZeroMemory(&GameEnd, sizeof(GameEnd));

                    //������Ϣ
                    m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                    if(m_pGameVideo)
                    {
                        m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                        m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                    }

                    //������Ϸ
                    m_pITableFrame->ConcludeGame(GS_TK_FREE);

                    if(!IsRoomCardType())
                    {
                        //ɾ��ʱ��
                        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                        //ɾ�����ߴ���ʱ��
                        for(WORD i = 0; i < m_wPlayerCount; i++)
                        {
                            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                        }
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                    //���·����û���Ϣ
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        if(i == wChairID)
                        {
                            continue;
                        }

                        //��ȡ�û�
                        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                        if(!pIServerUserItem)
                        {
                            continue;
                        }

                        UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
                    }

                    m_bReNewTurn = false;

                    return true;
                }
                //�м����� ����Ϸ��������1  ������Ϸ
                else
                {
                    ASSERT(wUserCount >= 2);

                    OnUserAddScore(wChairID, 0);
                    m_lTableScore[wChairID] = 0L;
                }
            }
        }
        //��ׯ״̬����
        else
        {
            //�������
            WORD wUserCount = 0;
            for(WORD i = 0; i < m_wPlayerCount; i++)if(m_cbPlayStatus[i] == TRUE) { wUserCount++; }

            //������Ϸ
            if(wUserCount == 1)
            {
                //�������
                CMD_S_GameEnd GameEnd;
                ZeroMemory(&GameEnd, sizeof(GameEnd));

                //������Ϣ
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
                    m_pITableFrame->SendTableData(i, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
                }
                m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

                if(m_pGameVideo)
                {
                    m_pGameVideo->AddVideoData(SUB_S_GAME_END, &GameEnd);
                }

                if(m_pGameVideo)
                {
                    m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
                }

                //������Ϸ
                m_pITableFrame->ConcludeGame(GS_TK_FREE);

                if(!IsRoomCardType())
                {
                    //ɾ��ʱ��
                    m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

                    //ɾ�����ߴ���ʱ��
                    for(WORD i = 0; i < m_wPlayerCount; i++)
                    {
                        m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + i);
                    }
                }

                UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

                //���·����û���Ϣ
                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(i == wChairID)
                    {
                        continue;
                    }

                    //��ȡ�û�
                    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

                    if(!pIServerUserItem)
                    {
                        continue;
                    }

                    UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
                }

                m_bReNewTurn = false;

                return true;
            }
            else
            {
                OnUserCallBanker(wChairID, false, 0);
            }
        }

        return true;
    }
    }

    return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
    switch(cbGameStatus)
    {
    case GAME_STATUS_FREE:		//����״̬
    {
        //˽�˷���
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            //cbGameRule[1] Ϊ  2 ��3 ��4, 5, 6 0�ֱ��Ӧ 2�� �� 3�� �� 4�� ��5, 6 2-6�� �⼸������
            BYTE *pGameRule = m_pITableFrame->GetGameRule();
            if(pGameRule[1] != 0)
            {
                m_wPlayerCount = pGameRule[1];

                //��������
                m_pITableFrame->SetTableChairCount(m_wPlayerCount);
            }
            else
            {
                m_wPlayerCount = GAME_PLAYER;

                //��������
                m_pITableFrame->SetTableChairCount(GAME_PLAYER);
            }

            ASSERT(pGameRule[3] == 22 || pGameRule[3] == 23);
            if(pGameRule[3] == 22 || pGameRule[3] == 23)
            {
                //Ĭ�Ͼ���ģʽ
                m_ctConfig = (CARDTYPE_CONFIG)(pGameRule[3]);
            }

            ASSERT(pGameRule[4] == 32 || pGameRule[4] == 33);
            if(pGameRule[4] == 32 || pGameRule[4] == 33)
            {
                //Ĭ�Ϸ��ĵ���
                m_stConfig = (SENDCARDTYPE_CONFIG)(pGameRule[4]);
            }

            //ASSERT (pGameRule[5] == 42 || pGameRule[5] == 43);
            //m_gtConfig = (KING_CONFIG)(pGameRule[5]);

            ASSERT(pGameRule[6] == 52 || pGameRule[6] == 53 || pGameRule[6] == 54 || pGameRule[6] == 55 || pGameRule[6] == 56 || pGameRule[6] == 57);
            if(pGameRule[6] == 52 || pGameRule[6] == 53 || pGameRule[6] == 54 || pGameRule[6] == 55 || pGameRule[6] == 56 || pGameRule[6] == 57)
            {
                //Ĭ�ϰ���ׯ
                m_bgtConfig = (BANERGAMETYPE_CONFIG)(pGameRule[6]);
            }

            //��ע����ֻ���ں�̨����
            //�Ż���������
            LONG *plConfig = new LONG;
            ZeroMemory(plConfig, sizeof(LONG));

            CopyMemory(plConfig, &(pGameRule[7]), sizeof(LONG));
            m_lBeBankerCondition = *plConfig;

            CopyMemory(plConfig, &(pGameRule[7 + sizeof(LONG)]), sizeof(LONG));
            m_lPlayerBetTimes = *plConfig;

            m_cbAdmitRevCard = pGameRule[15];
            m_cbMaxCallBankerTimes = pGameRule[16];

            for(WORD i = 0; i < MAX_SPECIAL_CARD_TYPE - 1; i++)
            {
                m_cbEnableCardType[i] = pGameRule[17 + i];
            }

            //Ĭ���д�С��
            m_gtConfig = (pGameRule[25] == TRUE ? GT_HAVEKING_ : GT_NOKING_);

            m_cbClassicTypeConfig = pGameRule[26];
            if(m_ctConfig == CT_ADDTIMES_)
            {
                m_cbClassicTypeConfig = INVALID_BYTE;
            }

            //���ַ���
            if(IsRoomCardType())
            {
				//��ע����
                m_tyConfig = (pGameRule[27] == TRUE ? BT_TUI_DOUBLE_ : BT_TUI_NONE_);

				//�������ߴ����ʶ����ҷ���Ĭ��m_cbRCOfflineTrusteeΪTRUE
				m_cbRCOfflineTrustee = pGameRule[28];
            }
			else
			{
				m_cbRCOfflineTrustee = TRUE;
			}
        }

        //��������
        CMD_S_StatusFree StatusFree;
        ZeroMemory(&StatusFree, sizeof(StatusFree));

        //CString cs;
        //cs.Format(TEXT("CellScore = %d"), m_pITableFrame->GetCellScore());
        //CTraceService::TraceString(cs,TraceLevel_Exception);

        //���ñ���
        StatusFree.lCellScore = m_pITableFrame->GetCellScore();
        StatusFree.lRoomStorageStart = g_lRoomStorageStart;
        StatusFree.lRoomStorageCurrent = g_lRoomStorageCurrent;

        StatusFree.ctConfig = m_ctConfig;
        StatusFree.stConfig = m_stConfig;
        StatusFree.bgtConfig = m_bgtConfig;
        StatusFree.btConfig = m_btConfig;
        StatusFree.gtConfig = m_gtConfig;

        //��ʷ����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusFree.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusFree.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //��ȡ�Զ�������
        tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
        ASSERT(pCustomRule);
        tagCustomAndroid CustomAndroid;
        ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
        CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
        CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
        CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
        CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
        CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
        CopyMemory(&StatusFree.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        StatusFree.wGamePlayerCountRule = pGameRule[1];
        StatusFree.cbAdmitRevCard = m_cbAdmitRevCard;
        StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();

        //������
        if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
        {
            StatusFree.bIsAllowAvertDebug = true;
        }

        //Ȩ���ж�
        if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
        {
            CMD_S_ADMIN_STORAGE_INFO StorageInfo;
            ZeroMemory(&StorageInfo, sizeof(StorageInfo));
            StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
            StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
            StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
            StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
            StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
            StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
            StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;

            m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
        }

        //����ģʽ
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

		//��ҳ��ͽ�ҷ��������йܣ����ַ���Ҳ�й�
		//���߻���ȡ������
		//if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
		//        || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
		{
			//����ȡ���йܱ�־
			pIServerUserItem->SetTrusteeUser(false);
			m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
		}

        //���ͳ���
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
    }
    case GS_TK_CALL:	//��ׯ״̬
    {
        //��������
        CMD_S_StatusCall StatusCall;
        ZeroMemory(&StatusCall, sizeof(StatusCall));

        //��ʷ����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusCall.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusCall.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //���ñ���
        StatusCall.lCellScore = m_pITableFrame->GetCellScore();
        StatusCall.cbDynamicJoin = m_cbDynamicJoin[wChairID];
        CopyMemory(StatusCall.cbPlayStatus, m_cbPlayStatus, sizeof(StatusCall.cbPlayStatus));
        StatusCall.lRoomStorageStart = g_lRoomStorageStart;
        StatusCall.lRoomStorageCurrent = g_lRoomStorageCurrent;

        StatusCall.ctConfig = m_ctConfig;
        StatusCall.stConfig = m_stConfig;
        StatusCall.bgtConfig = m_bgtConfig;
        StatusCall.btConfig = m_btConfig;
        StatusCall.gtConfig = m_gtConfig;

        //��ȡ�Զ�������
        tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
        ASSERT(pCustomRule);
        tagCustomAndroid CustomAndroid;
        ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
        CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
        CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
        CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
        CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
        CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
        CopyMemory(&StatusCall.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        StatusCall.wGamePlayerCountRule = pGameRule[1];
        StatusCall.cbAdmitRevCard = m_cbAdmitRevCard;
        StatusCall.cbMaxCallBankerTimes = m_cbMaxCallBankerTimes;
        StatusCall.wBgtRobNewTurnChairID = m_wBgtRobNewTurnChairID;
        StatusCall.cbPlayMode = m_pITableFrame->GetDataBaseMode();

        //������
        if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
        {
            StatusCall.bIsAllowAvertDebug = true;
        }

        CopyMemory(StatusCall.cbCallBankerStatus, m_cbCallBankerStatus, sizeof(StatusCall.cbCallBankerStatus));
        CopyMemory(StatusCall.cbCallBankerTimes, m_cbCallBankerTimes, sizeof(StatusCall.cbCallBankerTimes));

        //���·����û���Ϣ
        UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

        //Ȩ���ж�
        if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
        {
            CMD_S_ADMIN_STORAGE_INFO StorageInfo;
            ZeroMemory(&StorageInfo, sizeof(StorageInfo));
            StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
            StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
            StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
            StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
            StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
            StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
            StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
            m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
        }

        //���ͳ���
        bool bResult = m_pITableFrame->SendGameScene(pIServerUserItem, &StatusCall, sizeof(StatusCall));

        //����ģʽ
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

        //���ĵ���
        if(m_stConfig == ST_SENDFOUR_ && m_bgtConfig == BGT_ROB_)
        {
            //���ñ���
            CMD_S_SendFourCard SendFourCard;
            ZeroMemory(&SendFourCard, sizeof(SendFourCard));

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }

                //�ɷ��˿�(��ʼֻ��������)
                CopyMemory(SendFourCard.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
            }

            m_pITableFrame->SendTableData(wChairID, SUB_S_SEND_FOUR_CARD, &SendFourCard, sizeof(SendFourCard));
        }

        //��ҳ��ͽ�ҷ��������йܣ����ַ���Ҳ�й�
		//���߻���ȡ������
        //if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
        //        || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
        {
            //����ȡ���йܱ�־
            pIServerUserItem->SetTrusteeUser(false);
            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
        }

        return bResult;
    }
    case GS_TK_SCORE:	//��ע״̬
    {
        //��������
        CMD_S_StatusScore StatusScore;
        memset(&StatusScore, 0, sizeof(StatusScore));

        //��ʷ����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusScore.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusScore.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //��ע��Ϣ
        StatusScore.lCellScore = m_pITableFrame->GetCellScore();
        StatusScore.lTurnMaxScore = m_lTurnMaxScore[wChairID];
        StatusScore.wBankerUser = m_wBankerUser;
        StatusScore.cbDynamicJoin = m_cbDynamicJoin[wChairID];
        CopyMemory(StatusScore.cbPlayStatus, m_cbPlayStatus, sizeof(StatusScore.cbPlayStatus));
        StatusScore.lRoomStorageStart = g_lRoomStorageStart;
        StatusScore.lRoomStorageCurrent = g_lRoomStorageCurrent;

        //���ĵ���
        if(m_stConfig == ST_SENDFOUR_)
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }

                //�ɷ��˿�(��ʼֻ��������)
                CopyMemory(StatusScore.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
            }
        }

        StatusScore.ctConfig = m_ctConfig;
        StatusScore.stConfig = m_stConfig;
        StatusScore.bgtConfig = m_bgtConfig;
        StatusScore.btConfig = m_btConfig;
        StatusScore.gtConfig = m_gtConfig;

        CopyMemory(StatusScore.lFreeConfig, m_lFreeConfig, sizeof(StatusScore.lFreeConfig));
        CopyMemory(StatusScore.lPercentConfig, m_lPercentConfig, sizeof(StatusScore.lPercentConfig));
        CopyMemory(StatusScore.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(StatusScore.lPlayerBetBtEx));

        //���û���
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE) { continue; }
            StatusScore.lTableScore[i] = m_lTableScore[i];
        }

        //��ȡ�Զ�������
        tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
        ASSERT(pCustomRule);
        tagCustomAndroid CustomAndroid;
        ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
        CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
        CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
        CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
        CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
        CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
        CopyMemory(&StatusScore.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        StatusScore.wGamePlayerCountRule = pGameRule[1];
        StatusScore.cbAdmitRevCard = m_cbAdmitRevCard;
        StatusScore.cbPlayMode = m_pITableFrame->GetDataBaseMode();

        //������
        if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
        {
            StatusScore.bIsAllowAvertDebug = true;
        }

		CopyMemory(StatusScore.cbCallBankerTimes, m_cbCallBankerTimes, sizeof(StatusScore.cbCallBankerTimes));

        //���·����û���Ϣ
        UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

        //Ȩ���ж�
        if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
        {
            CMD_S_ADMIN_STORAGE_INFO StorageInfo;
            ZeroMemory(&StorageInfo, sizeof(StorageInfo));
            StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
            StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
            StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
            StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
            StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
            StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
            StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
            m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
        }

        //����ģʽ
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

		//��ҳ��ͽ�ҷ��������йܣ����ַ���Ҳ�й�
		//���߻���ȡ������
		//if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
		//        || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
        {
            //����ȡ���йܱ�־
            pIServerUserItem->SetTrusteeUser(false);
            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
        }

        //���ͳ���
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusScore, sizeof(StatusScore));
    }
    case GS_TK_PLAYING:	//��Ϸ״̬
    {
        //��������
        CMD_S_StatusPlay StatusPlay;
        memset(&StatusPlay, 0, sizeof(StatusPlay));

        //��ʷ����
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            tagHistoryScore *pHistoryScore = m_HistoryScore.GetHistoryScore(i);
            StatusPlay.lTurnScore[i] = pHistoryScore->lTurnScore;
            StatusPlay.lCollectScore[i] = pHistoryScore->lCollectScore;
        }

        //������Ϣ
        StatusPlay.lCellScore = m_pITableFrame->GetCellScore();
        StatusPlay.lTurnMaxScore = m_lTurnMaxScore[wChairID];
        StatusPlay.wBankerUser = m_wBankerUser;
        StatusPlay.cbDynamicJoin = m_cbDynamicJoin[wChairID];
        CopyMemory(StatusPlay.bOpenCard, m_bOpenCard, sizeof(StatusPlay.bOpenCard));
        CopyMemory(StatusPlay.bSpecialCard, m_bSpecialCard, sizeof(StatusPlay.bSpecialCard));
        CopyMemory(StatusPlay.cbOriginalCardType, m_cbOriginalCardType, sizeof(StatusPlay.cbOriginalCardType));
        CopyMemory(StatusPlay.cbCombineCardType, m_cbCombineCardType, sizeof(StatusPlay.cbCombineCardType));

        CopyMemory(StatusPlay.cbPlayStatus, m_cbPlayStatus, sizeof(StatusPlay.cbPlayStatus));
        StatusPlay.lRoomStorageStart = g_lRoomStorageStart;
        StatusPlay.lRoomStorageCurrent = g_lRoomStorageCurrent;

        StatusPlay.ctConfig = m_ctConfig;
        StatusPlay.stConfig = m_stConfig;
        StatusPlay.bgtConfig = m_bgtConfig;
        StatusPlay.btConfig = m_btConfig;
        StatusPlay.gtConfig = m_gtConfig;

        CopyMemory(StatusPlay.lFreeConfig, m_lFreeConfig, sizeof(StatusPlay.lFreeConfig));
        CopyMemory(StatusPlay.lPercentConfig, m_lPercentConfig, sizeof(StatusPlay.lPercentConfig));
        CopyMemory(StatusPlay.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(StatusPlay.lPlayerBetBtEx));

        //��ȡ�Զ�������
        tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
        ASSERT(pCustomRule);
        tagCustomAndroid CustomAndroid;
        ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
        CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
        CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
        CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
        CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
        CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
        CopyMemory(&StatusPlay.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));
        BYTE *pGameRule = m_pITableFrame->GetGameRule();
        StatusPlay.wGamePlayerCountRule = pGameRule[1];
        StatusPlay.cbAdmitRevCard = m_cbAdmitRevCard;
        StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();

        //������
        if(CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule))
        {
            StatusPlay.bIsAllowAvertDebug = true;
        }

        //�����˿�
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE) { continue; }
            WORD j = i;
            StatusPlay.lTableScore[j] = m_lTableScore[j];
            CopyMemory(StatusPlay.cbHandCardData[j], m_cbHandCardData[j], MAX_CARDCOUNT);
        }

		CopyMemory(StatusPlay.cbCallBankerTimes, m_cbCallBankerTimes, sizeof(StatusPlay.cbCallBankerTimes));

        //���·����û���Ϣ
        UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

        //Ȩ���ж�
        if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
        {
            CMD_S_ADMIN_STORAGE_INFO StorageInfo;
            ZeroMemory(&StorageInfo, sizeof(StorageInfo));
            StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
            StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
            StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
            StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
            StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
            StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
            StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
            m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
        }

        //����ģʽ
        if((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
        {
            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));

            CMD_S_RoomCardRecord RoomCardRecord;
            ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));

            m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
            m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        }

		//��ҳ��ͽ�ҷ��������йܣ����ַ���Ҳ�й�
		//���߻���ȡ������
		//if(((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 1)
		//        || (m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) == 0)
        {
            //����ȡ���йܱ�־
            pIServerUserItem->SetTrusteeUser(false);
            m_pITableFrame->KillGameTimer(IDI_OFFLINE_TRUSTEE_0 + wChairID);
        }

		//��ǰtm
		DWORD dwCurTickCount = (DWORD)time(NULL);
		DWORD dwDTmVal = dwCurTickCount - m_GameEndEx.dwTickCountGameEnd;

		if (dwDTmVal >= 1 && dwDTmVal <= TIME_DELAY_GAMEFREE / 1000 && !pIServerUserItem->IsAndroidUser())
		{
			StatusPlay.bDelayFreeDynamicJoin = true;
		}

		CopyMemory(StatusPlay.lTurnScore, m_GameEndEx.GameEnd.lGameScore, sizeof(StatusPlay.lTurnScore));

        //���ͳ���
        return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
    }
    }
    //Ч�����
    ASSERT(FALSE);

    return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{
    switch(dwTimerID)
    {
	case IDI_DELAY_GAMEFREE:
	{
		//ɾ��ʱ��
		m_pITableFrame->KillGameTimer(IDI_DELAY_GAMEFREE);

		BOOL bEndLoop = *((BOOL*)wBindParam);
		m_pITableFrame->ConcludeGame(GS_TK_FREE, bEndLoop);

		return true;
	}
    case IDI_SO_OPERATE:
    {
        //ɾ��ʱ��
        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

        //��Ϸ״̬
        switch(m_pITableFrame->GetGameStatus())
        {
        case GS_TK_CALL:			//�û���ׯ
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] != TRUE)
                {
                    continue;
                }
                if(m_cbCallBankerStatus[i] == TRUE)
                {
                    continue;
                }

                if(i == m_wBgtRobNewTurnChairID && m_wBgtRobNewTurnChairID != INVALID_CHAIR)
                {
                    OnUserCallBanker(i, true, 1);
                }
                else
                {
                    OnUserCallBanker(i, false, m_cbPrevCallBankerTimes[i]);
                }
            }

            break;
        }
        case GS_TK_SCORE:			//��ע����
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_lTableScore[i] > 0L || m_cbPlayStatus[i] == FALSE || i == m_wBankerUser)
                {
                    continue;
                }

                if(m_lTurnMaxScore[i] > 0)
                {
                    if(m_btConfig == BT_FREE_)
                    {
                        OnUserAddScore(i, m_lFreeConfig[0] * m_pITableFrame->GetCellScore());
                    }
                    else if(m_btConfig == BT_PENCENT_)
                    {
                        OnUserAddScore(i, m_lTurnMaxScore[i] * m_lPercentConfig[0] / 100);
                    }
                }
                else
                {
                    OnUserAddScore(i, 1);
                }
            }

            break;
        }
        case GS_TK_PLAYING:			//�û�����
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_bOpenCard[i] == true || m_cbPlayStatus[i] == FALSE)
                {
                    continue;
                }

                //��ȡţţ����
                BYTE cbTempHandCardData[MAX_CARDCOUNT];
                ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
                CopyMemory(cbTempHandCardData, m_cbHandCardData[i], sizeof(m_cbHandCardData[i]));

                m_GameLogic.GetOxCard(cbTempHandCardData, MAX_CARDCOUNT);

                OnUserOpenCard(i, cbTempHandCardData);
            }

            break;
        }
        default:
        {
            break;
        }
        }

        if(m_pITableFrame->GetGameStatus() != GS_TK_FREE)
        {
            m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
        }
        return true;
    }
    case IDI_OFFLINE_TRUSTEE_0:
    case IDI_OFFLINE_TRUSTEE_1:
    case IDI_OFFLINE_TRUSTEE_2:
    case IDI_OFFLINE_TRUSTEE_3:
    case IDI_OFFLINE_TRUSTEE_4:
    case IDI_OFFLINE_TRUSTEE_5:
    case IDI_OFFLINE_TRUSTEE_6:
    case IDI_OFFLINE_TRUSTEE_7:
    {
        m_pITableFrame->KillGameTimer(dwTimerID);
        WORD wOfflineTrustee = dwTimerID - IDI_OFFLINE_TRUSTEE_0;

        if(m_pITableFrame->GetTableUserItem(wOfflineTrustee) && m_pITableFrame->GetTableUserItem(wOfflineTrustee)->IsTrusteeUser())
        {
            //��Ϸ״̬
            switch(m_pITableFrame->GetGameStatus())
            {
            case GS_TK_CALL:			//�û���ׯ
            {
                if(m_cbPlayStatus[wOfflineTrustee] != TRUE)
                {
                    break;
                }
                if(m_cbCallBankerStatus[wOfflineTrustee] == TRUE)
                {
                    break;
                }

                if(wOfflineTrustee == m_wBgtRobNewTurnChairID && m_wBgtRobNewTurnChairID != INVALID_CHAIR)
                {
                    OnUserCallBanker(wOfflineTrustee, true, 1);
                }
                else
                {
                    OnUserCallBanker(wOfflineTrustee, false, 0);
                }

                break;
            }
            case GS_TK_SCORE:			//��ע����
            {
                if(m_lTableScore[wOfflineTrustee] > 0L || m_cbPlayStatus[wOfflineTrustee] == FALSE || wOfflineTrustee == m_wBankerUser)
                {
                    break;
                }

                if(m_lTurnMaxScore[wOfflineTrustee] > 0)
                {
                    if(m_btConfig == BT_FREE_)
                    {
                        OnUserAddScore(wOfflineTrustee, m_lFreeConfig[0] * m_pITableFrame->GetCellScore());
                    }
                    else if(m_btConfig == BT_PENCENT_)
                    {
                        OnUserAddScore(wOfflineTrustee, m_lTurnMaxScore[wOfflineTrustee] * m_lPercentConfig[0] / 100);
                    }
                }
                else
                {
                    OnUserAddScore(wOfflineTrustee, 1);
                }

                break;
            }
            case GS_TK_PLAYING:			//�û�����
            {
                if(m_bOpenCard[wOfflineTrustee] == true || m_cbPlayStatus[wOfflineTrustee] == FALSE)
                {
                    break;
                }

                //��ȡţţ����
                BYTE cbTempHandCardData[MAX_CARDCOUNT];
                ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
                CopyMemory(cbTempHandCardData, m_cbHandCardData[wOfflineTrustee], sizeof(m_cbHandCardData[wOfflineTrustee]));

                m_GameLogic.GetOxCard(cbTempHandCardData, MAX_CARDCOUNT);

                OnUserOpenCard(wOfflineTrustee, cbTempHandCardData);

                break;
            }
            default:
            {
                break;
            }
            }
        }

        return true;
    }
    case IDI_TIME_ELAPSE:
    {
        if(m_cbTimeRemain > 0)
        {
            m_cbTimeRemain--;
        }

        return true;
    }
    }
    return false;
}

//��Ϸ��Ϣ����
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    bool bResult = false;
    switch(wSubCmdID)
    {
    case SUB_C_CALL_BANKER:			//�û���ׯ
    {
        //Ч������
        ASSERT(wDataSize == sizeof(CMD_C_CallBanker));
        if(wDataSize != sizeof(CMD_C_CallBanker)) { return false; }

        //��������
        CMD_C_CallBanker *pCallBanker = (CMD_C_CallBanker *)pDataBuffer;

        //�û�Ч��
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        //if (pUserData->cbUserStatus!=US_PLAYING) return true;

        //״̬�ж�
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] != TRUE)
        {
            //д��־
			CString strFileName = TEXT("������־");

            tagLogUserInfo LogUserInfo;
            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
            CopyMemory(LogUserInfo.szLogContent, TEXT("m_cbPlayStatus = FALSE"), sizeof(LogUserInfo.szLogContent));
            //m_pITableFrame->SendGameLogData(LogUserInfo);

            return false;
        }

        //��Ϣ����
        bResult = OnUserCallBanker(pUserData->wChairID, pCallBanker->bBanker, pCallBanker->cbBankerTimes);
        break;
    }
    case SUB_C_ADD_SCORE:			//�û���ע
    {
        //Ч������
        ASSERT(wDataSize == sizeof(CMD_C_AddScore));
        if(wDataSize != sizeof(CMD_C_AddScore))
        {
            return false;
        }

        //��������
        CMD_C_AddScore *pAddScore = (CMD_C_AddScore *)pDataBuffer;

        //�û�Ч��
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        //if (pUserData->cbUserStatus!=US_PLAYING) return true;

        //״̬�ж�
        ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
        if(m_cbPlayStatus[pUserData->wChairID] != TRUE)
        {
            //д��־
			CString strFileName = TEXT("������־");

            tagLogUserInfo LogUserInfo;
            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
            CopyMemory(LogUserInfo.szLogContent, TEXT("m_cbPlayStatus = FALSE"), sizeof(LogUserInfo.szLogContent));
            //m_pITableFrame->SendGameLogData(LogUserInfo);

            return false;
        }

        //��Ϣ����
        bResult = OnUserAddScore(pUserData->wChairID, pAddScore->lScore);
        break;
    }
    case SUB_C_OPEN_CARD:			//�û�̯��
    {
        ASSERT(wDataSize == sizeof(CMD_C_OpenCard));
        if(wDataSize != sizeof(CMD_C_OpenCard)) { return false; }

        CMD_C_OpenCard *pOpenCard = (CMD_C_OpenCard *)pDataBuffer;

        //�û�Ч��
        tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
        //if (pUserData->cbUserStatus!=US_PLAYING) return true;

        //״̬�ж�
        ASSERT(m_cbPlayStatus[pUserData->wChairID] != FALSE);
        if(m_cbPlayStatus[pUserData->wChairID] == FALSE)
        {
            //д��־
			CString strFileName = TEXT("������־");

            tagLogUserInfo LogUserInfo;
            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
            CopyMemory(LogUserInfo.szLogContent, TEXT("m_cbPlayStatus = FALSE"), sizeof(LogUserInfo.szLogContent));
            //m_pITableFrame->SendGameLogData(LogUserInfo);

            return false;
        }

        //��Ϣ����
        bResult = OnUserOpenCard(pUserData->wChairID, pOpenCard->cbCombineCardData);
        break;
    }
    case SUB_C_REQUEST_RCRecord:
    {
        //ASSERT((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0);
        if(!((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0))
        {
            return true;
        }

        if(m_pITableFrame->IsPersonalRoomDisumme())
        {
            return true;
        }

        ASSERT(pIServerUserItem->IsMobileUser());
        if(!pIServerUserItem->IsMobileUser())
        {
            return false;
        }

        CMD_S_RoomCardRecord RoomCardRecord;
        ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

        //���ַ���
        if((m_pITableFrame->GetDataBaseMode() == 0) && (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0))
        {
            CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
        }
        else
        {
            WORD wChairID = pIServerUserItem->GetChairID();
            POSITION pos = m_listWinScoreRecord[wChairID].GetHeadPosition();

            WORD wIndex = 0;
            while(pos)
            {
                RoomCardRecord.lDetailScore[wChairID][wIndex++] = m_listWinScoreRecord[wChairID].GetNext(pos);

                if(wIndex >= MAX_RECORD_COUNT)
                {
                    break;
                }
            }

            RoomCardRecord.nCount = wIndex;
        }

        m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
        m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));

        return true;
    }
#ifdef CARD_CONFIG
    case SUB_C_CARD_CONFIG:
    {
        //Ч������
        ASSERT(wDataSize == sizeof(CMD_C_CardConfig));
        if(wDataSize != sizeof(CMD_C_CardConfig))
        {
            return true;
        }

        //��Ϣ����
        CMD_C_CardConfig *pCardConfig = (CMD_C_CardConfig *)pDataBuffer;

        CopyMemory(m_cbconfigCard, pCardConfig->cbconfigCard, sizeof(m_cbconfigCard));

        return true;
    }
#endif
    }

    BYTE cbGameStatus = m_pITableFrame->GetGameStatus();

    return true;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, void *pDataBuffer, WORD wDataSize, IServerUserItem *pIServerUserItem)
{
    // ��Ϣ����
    if(wSubCmdID >= SUB_GF_FRAME_MESSAG_GAME_MIN && wSubCmdID <= SUB_GF_FRAME_MESSAG_GAME_MAX)
    {
        switch(wSubCmdID - SUB_GF_FRAME_MESSAG_GAME_MIN)
        {
        case SUB_C_STORAGE:
        {
            ASSERT(wDataSize == sizeof(CMD_C_UpdateStorage));
            if(wDataSize != sizeof(CMD_C_UpdateStorage)) { return false; }

            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
            {
                return false;
            }

            CMD_C_UpdateStorage *pUpdateStorage = (CMD_C_UpdateStorage *)pDataBuffer;
            g_lRoomStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
            g_lStorageDeductRoom = pUpdateStorage->lRoomStorageDeduct;

            //20��������¼
            if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
            {
                g_ListOperationRecord.RemoveHead();
            }

            //д��־
            CString strOperationRecord;
            strOperationRecord.Format(TEXT("�����˻�[%s],�޸ĵ�ǰ���Ϊ %I64d,˥��ֵΪ %I64d"), pIServerUserItem->GetNickName(),
                                      g_lRoomStorageCurrent, g_lStorageDeductRoom);

            g_ListOperationRecord.AddTail(strOperationRecord);

			CString strFileName = TEXT("������־");

            tagLogUserInfo LogUserInfo;
            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
            CopyMemory(LogUserInfo.szLogContent, strOperationRecord, sizeof(LogUserInfo.szLogContent));
            //m_pITableFrame->SendGameLogData(LogUserInfo);

            //��������
            CMD_S_Operation_Record OperationRecord;
            ZeroMemory(&OperationRecord, sizeof(OperationRecord));
            POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
            WORD wIndex = 0;//�����±�
            while(posListRecord)
            {
                CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                wIndex++;
            }

            ASSERT(wIndex <= MAX_OPERATION_RECORD);

            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

            return true;
        }
        case SUB_C_STORAGEMAXMUL:
        {
            ASSERT(wDataSize == sizeof(CMD_C_ModifyStorage));
            if(wDataSize != sizeof(CMD_C_ModifyStorage)) { return false; }

            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false)
            {
                return false;
            }

            CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pDataBuffer;
            g_lStorageMax1Room = pModifyStorage->lMaxRoomStorage[0];
            g_lStorageMax2Room = pModifyStorage->lMaxRoomStorage[1];
            g_lStorageMul1Room = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
            g_lStorageMul2Room = (SCORE)(pModifyStorage->wRoomStorageMul[1]);

            //20��������¼
            if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
            {
                g_ListOperationRecord.RemoveHead();
            }

            //д����־
            CString strOperationRecord;
            strOperationRecord.Format(TEXT("�����˻�[%s], �޸Ŀ������ֵ1Ϊ %I64d,Ӯ�ָ���1Ϊ %I64d,����ֵ2Ϊ %I64d,Ӯ�ָ���2Ϊ %I64d"), pIServerUserItem->GetNickName(), g_lStorageMax1Room, g_lStorageMul1Room, g_lStorageMax2Room, g_lStorageMul2Room);

            g_ListOperationRecord.AddTail(strOperationRecord);

			CString strFileName = TEXT("������־");

            tagLogUserInfo LogUserInfo;
            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
            CopyMemory(LogUserInfo.szLogContent, strOperationRecord, sizeof(LogUserInfo.szLogContent));
            //m_pITableFrame->SendGameLogData(LogUserInfo);

            //��������
            CMD_S_Operation_Record OperationRecord;
            ZeroMemory(&OperationRecord, sizeof(OperationRecord));
            POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
            WORD wIndex = 0;//�����±�
            while(posListRecord)
            {
                CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                wIndex++;
            }

            ASSERT(wIndex <= MAX_OPERATION_RECORD);

            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

            return true;
        }
        case SUB_C_REQUEST_QUERY_USER:
        {
            ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
            if(wDataSize != sizeof(CMD_C_RequestQuery_User))
            {
                return false;
            }

            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
            {
                return false;
            }

            CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pDataBuffer;

            //����ӳ��
            POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
            DWORD dwUserID = 0;
            ROOMUSERINFO userinfo;
            ZeroMemory(&userinfo, sizeof(userinfo));

            CMD_S_RequestQueryResult QueryResult;
            ZeroMemory(&QueryResult, sizeof(QueryResult));

            while(ptHead)
            {
                g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
                if(pQuery_User->dwGameID == userinfo.dwGameID || _tcscmp(pQuery_User->szNickName, userinfo.szNickName) == 0)
                {
                    //�����û���Ϣ����
                    QueryResult.bFind = true;
                    CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

                    ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
                    CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
                }
            }

            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

            return true;
        }
        case SUB_C_USER_DEBUG:
        {
            ASSERT(wDataSize == sizeof(CMD_C_UserDebug));
            if(wDataSize != sizeof(CMD_C_UserDebug))
            {
                return false;
            }

            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
            {
                return false;
            }

            CMD_C_UserDebug *pUserDebug = (CMD_C_UserDebug *)pDataBuffer;

            //����ӳ��
            POSITION ptMapHead = g_MapRoomUserInfo.GetStartPosition();
            DWORD dwUserID = 0;
            ROOMUSERINFO userinfo;
            ZeroMemory(&userinfo, sizeof(userinfo));

            //20��������¼
            if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
            {
                g_ListOperationRecord.RemoveHead();
            }

            //��������
            CMD_S_UserDebug serverUserDebug;
            ZeroMemory(&serverUserDebug, sizeof(serverUserDebug));

            TCHAR szNickName[LEN_NICKNAME];
            ZeroMemory(szNickName, sizeof(szNickName));

            //�������
            if(pUserDebug->userDebugInfo.bCancelDebug == false)
            {
                ASSERT(pUserDebug->userDebugInfo.debug_type == CONTINUE_WIN || pUserDebug->userDebugInfo.debug_type == CONTINUE_LOST);

                while(ptMapHead)
                {
                    g_MapRoomUserInfo.GetNextAssoc(ptMapHead, dwUserID, userinfo);

                    if(_tcscmp(pUserDebug->szNickName, szNickName) == 0 && _tcscmp(userinfo.szNickName, szNickName) == 0)
                    {
                        continue;
                    }

                    if(pUserDebug->dwGameID == userinfo.dwGameID || _tcscmp(pUserDebug->szNickName, userinfo.szNickName) == 0)
                    {
                        //������Ա�ʶ
                        bool bEnableDebug = false;
                        IsSatisfyDebug(userinfo, bEnableDebug);

                        //�������
                        if(bEnableDebug)
                        {
                            ROOMUSERDEBUG roomuserdebug;
                            ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
                            CopyMemory(&(roomuserdebug.roomUserInfo), &userinfo, sizeof(userinfo));
                            CopyMemory(&(roomuserdebug.userDebug), &(pUserDebug->userDebugInfo), sizeof(roomuserdebug.userDebug));


                            //������������
                            TravelDebugList(roomuserdebug);

                            //ѹ��������ѹ��ͬGAMEID��NICKNAME)
                            g_ListRoomUserDebug.AddHead(roomuserdebug);

                            //��������
                            serverUserDebug.dwGameID = userinfo.dwGameID;
                            CopyMemory(serverUserDebug.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
                            serverUserDebug.debugResult = DEBUG_SUCCEED;
                            serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
                            serverUserDebug.cbDebugCount = pUserDebug->userDebugInfo.cbDebugCount;

                            //������¼
                            //д����־
                            CString strOperationRecord;
                            CString strDebugType;
                            GetDebugTypeString(serverUserDebug.debugType, strDebugType);
                            strOperationRecord.Format(TEXT("�����˻�[%s], �������%s,%s,���Ծ���%d"),
                                                      pIServerUserItem->GetNickName(), serverUserDebug.szNickName, strDebugType, serverUserDebug.cbDebugCount);

                            g_ListOperationRecord.AddTail(strOperationRecord);

							CString strFileName = TEXT("������־");

                            tagLogUserInfo LogUserInfo;
                            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
                            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
                            CopyMemory(LogUserInfo.szLogContent, strOperationRecord, sizeof(LogUserInfo.szLogContent));
                            //m_pITableFrame->SendGameLogData(LogUserInfo);
                        }
                        else	//������
                        {
                            //��������
                            serverUserDebug.dwGameID = userinfo.dwGameID;
                            CopyMemory(serverUserDebug.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
                            serverUserDebug.debugResult = DEBUG_FAIL;
                            serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
                            serverUserDebug.cbDebugCount = 0;

                            //������¼
                            //д����־
                            CString strOperationRecord;
                            CString strDebugType;
                            GetDebugTypeString(serverUserDebug.debugType, strDebugType);
                            strOperationRecord.Format(TEXT("�����˻�[%s], �������%s,%s,ʧ�ܣ�"),
                                                      pIServerUserItem->GetNickName(), serverUserDebug.szNickName, strDebugType);

                            g_ListOperationRecord.AddTail(strOperationRecord);

							CString strFileName = TEXT("������־");

                            tagLogUserInfo LogUserInfo;
                            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
                            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
                            CopyMemory(LogUserInfo.szLogContent, strOperationRecord, sizeof(LogUserInfo.szLogContent));
                            //m_pITableFrame->SendGameLogData(LogUserInfo);
                        }

                        //��������
                        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

                        CMD_S_Operation_Record OperationRecord;
                        ZeroMemory(&OperationRecord, sizeof(OperationRecord));
                        POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
                        WORD wIndex = 0;//�����±�
                        while(posListRecord)
                        {
                            CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                            CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                            wIndex++;
                        }

                        ASSERT(wIndex <= MAX_OPERATION_RECORD);

                        //��������
                        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
                        return true;
                    }
                }

                ASSERT(FALSE);
                return false;
            }
            else	//ȡ������
            {
                ASSERT(pUserDebug->userDebugInfo.debug_type == CONTINUE_CANCEL);

                POSITION ptListHead = g_ListRoomUserDebug.GetHeadPosition();
                POSITION ptTemp;
                ROOMUSERDEBUG roomuserdebug;
                ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

                //��������
                while(ptListHead)
                {
                    ptTemp = ptListHead;
                    roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);
                    if(pUserDebug->dwGameID == roomuserdebug.roomUserInfo.dwGameID || _tcscmp(pUserDebug->szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
                    {
                        //��������
                        serverUserDebug.dwGameID = roomuserdebug.roomUserInfo.dwGameID;
                        CopyMemory(serverUserDebug.szNickName, roomuserdebug.roomUserInfo.szNickName, sizeof(roomuserdebug.roomUserInfo.szNickName));
                        serverUserDebug.debugResult = DEBUG_CANCEL_SUCCEED;
                        serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
                        serverUserDebug.cbDebugCount = 0;

                        //�Ƴ�Ԫ��
                        g_ListRoomUserDebug.RemoveAt(ptTemp);

                        //��������
                        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

                        //������¼
                        //д����־
                        CString strOperationRecord;
                        CTime time = CTime::GetCurrentTime();
                        strOperationRecord.Format(TEXT("�����˻�[%s], ȡ�������%s�ĵ��ԣ�"),
                                                  pIServerUserItem->GetNickName(), serverUserDebug.szNickName);

                        g_ListOperationRecord.AddTail(strOperationRecord);

						CString strFileName = TEXT("������־");

                        tagLogUserInfo LogUserInfo;
                        ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
                        CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
                        CopyMemory(LogUserInfo.szLogContent, strOperationRecord, sizeof(LogUserInfo.szLogContent));
                        //m_pITableFrame->SendGameLogData(LogUserInfo);

                        CMD_S_Operation_Record OperationRecord;
                        ZeroMemory(&OperationRecord, sizeof(OperationRecord));
                        POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
                        WORD wIndex = 0;//�����±�
                        while(posListRecord)
                        {
                            CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                            CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                            wIndex++;
                        }

                        ASSERT(wIndex <= MAX_OPERATION_RECORD);

                        //��������
                        m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

                        return true;
                    }
                }

                //��������
                serverUserDebug.dwGameID = pUserDebug->dwGameID;
                CopyMemory(serverUserDebug.szNickName, pUserDebug->szNickName, sizeof(serverUserDebug.szNickName));
                serverUserDebug.debugResult = DEBUG_CANCEL_INVALID;
                serverUserDebug.debugType = pUserDebug->userDebugInfo.debug_type;
                serverUserDebug.cbDebugCount = 0;

                //��������
                m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_USER_DEBUG, &serverUserDebug, sizeof(serverUserDebug));

                //������¼
                //д����־
                CString strOperationRecord;
                strOperationRecord.Format(TEXT("�����˻�[%s], ȡ�������%s�ĵ��ԣ�������Ч��"),
                                          pIServerUserItem->GetNickName(), serverUserDebug.szNickName);

                g_ListOperationRecord.AddTail(strOperationRecord);

				CString strFileName = TEXT("������־");

                tagLogUserInfo LogUserInfo;
                ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
                CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
                CopyMemory(LogUserInfo.szLogContent, strOperationRecord, sizeof(LogUserInfo.szLogContent));
                //m_pITableFrame->SendGameLogData(LogUserInfo);

                CMD_S_Operation_Record OperationRecord;
                ZeroMemory(&OperationRecord, sizeof(OperationRecord));
                POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
                WORD wIndex = 0;//�����±�
                while(posListRecord)
                {
                    CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

                    CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
                    wIndex++;
                }

                ASSERT(wIndex <= MAX_OPERATION_RECORD);

                //��������
                m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

            }

            return true;
        }
        case SUB_C_REQUEST_UDPATE_ROOMINFO:
        {
            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
            {
                return false;
            }

            CMD_S_RequestUpdateRoomInfo_Result RoomInfo_Result;
            ZeroMemory(&RoomInfo_Result, sizeof(RoomInfo_Result));

            RoomInfo_Result.lRoomStorageCurrent = g_lRoomStorageCurrent;


            DWORD dwKeyGameID = g_CurrentQueryUserInfo.dwGameID;
            TCHAR szKeyNickName[LEN_NICKNAME];
            ZeroMemory(szKeyNickName, sizeof(szKeyNickName));
            CopyMemory(szKeyNickName, g_CurrentQueryUserInfo.szNickName, sizeof(szKeyNickName));

            //����ӳ��
            POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
            DWORD dwUserID = 0;
            ROOMUSERINFO userinfo;
            ZeroMemory(&userinfo, sizeof(userinfo));

            while(ptHead)
            {
                g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
                if(dwKeyGameID == userinfo.dwGameID && _tcscmp(szKeyNickName, userinfo.szNickName) == 0)
                {
                    //�����û���Ϣ����
                    CopyMemory(&(RoomInfo_Result.currentqueryuserinfo), &userinfo, sizeof(userinfo));

                    ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
                    CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
                }
            }


            //
            //��������
            POSITION ptListHead = g_ListRoomUserDebug.GetHeadPosition();
            POSITION ptTemp;
            ROOMUSERDEBUG roomuserdebug;
            ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

            //��������
            while(ptListHead)
            {
                ptTemp = ptListHead;
                roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

                //Ѱ�����
                if((dwKeyGameID == roomuserdebug.roomUserInfo.dwGameID) &&
                        _tcscmp(szKeyNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
                {
                    RoomInfo_Result.bExistDebug = true;
                    CopyMemory(&(RoomInfo_Result.currentuserdebug), &(roomuserdebug.userDebug), sizeof(roomuserdebug.userDebug));
                    break;
                }
            }

            //��������
            m_pITableFrame->SendRoomData(pIServerUserItem, SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));

            CMD_S_ADMIN_STORAGE_INFO StorageInfo;
            ZeroMemory(&StorageInfo, sizeof(StorageInfo));
            StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
            StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
            StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
            StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
            StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
            StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
            StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
            m_pITableFrame->SendRoomData(NULL, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));

            return true;
        }
        case SUB_C_CLEAR_CURRENT_QUERYUSER:
        {
            //Ȩ���ж�
            if(CUserRight::IsGameDebugUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
            {
                return false;
            }

            ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

            return true;
        }
        }
    }
    return false;
}

//��ׯ�¼�
bool CTableFrameSink::OnUserCallBanker(WORD wChairID, bool bBanker, BYTE cbBankerTimes)
{
    //״̬Ч��
    BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
    ASSERT(cbGameStatus == GS_TK_CALL);
    if(cbGameStatus != GS_TK_CALL) { return true; }

    //���ñ���
    m_cbCallBankerStatus[wChairID] = TRUE;
    m_cbCallBankerTimes[wChairID] = cbBankerTimes;

    //���ñ���
    CMD_S_CallBankerInfo CallBanker;
    ZeroMemory(&CallBanker, sizeof(CallBanker));

    CopyMemory(CallBanker.cbCallBankerStatus, m_cbCallBankerStatus, sizeof(m_cbCallBankerStatus));
    CopyMemory(CallBanker.cbCallBankerTimes, m_cbCallBankerTimes, sizeof(m_cbCallBankerTimes));

    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
        m_pITableFrame->SendTableData(i, SUB_S_CALL_BANKERINFO, &CallBanker, sizeof(CallBanker));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_BANKERINFO, &CallBanker, sizeof(CallBanker));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_CALL_BANKERINFO, &CallBanker);
    }

    //��ׯ����
    WORD wCallUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE) { wCallUserCount++; }
        else if(m_cbPlayStatus[i] != TRUE) { wCallUserCount++; }
    }

    //ȫ���˽���ׯ����ע��ʼ
    if(wCallUserCount == m_wPlayerCount)
    {
        CopyMemory(m_cbPrevCallBankerTimes, m_cbCallBankerTimes, sizeof(m_cbPrevCallBankerTimes));

        //
        if(m_wBgtRobNewTurnChairID == INVALID_CHAIR)
        {
            //��ׯ�����
            BYTE cbMaxBankerTimes = cbBankerTimes;
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxBankerTimes)
                {
                    cbMaxBankerTimes = m_cbCallBankerTimes[i];
                }
            }

            //��ׯ�������������CHAIRID
            BYTE cbMaxBankerCount = 0;
            WORD *pwMaxBankerTimesChairID = new WORD[m_wPlayerCount];
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] == cbMaxBankerTimes)
                {
                    pwMaxBankerTimesChairID[cbMaxBankerCount++] = i;
                }
            }

            ASSERT(cbMaxBankerCount <= m_wPlayerCount);
            m_wBankerUser = pwMaxBankerTimesChairID[rand() % cbMaxBankerCount];
            delete[] pwMaxBankerTimesChairID;
        }
        else
        {
            m_wBankerUser = m_wBgtRobNewTurnChairID;
        }

        m_bBuckleServiceCharge[m_wBankerUser] = true;

        bool bTrusteeUser = false;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                bTrusteeUser = true;
            }
        }

        //�Ƿ��������ö�ʱ��
        if(!IsRoomCardType()/* || bTrusteeUser*/)
        {
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
            m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
        }

        //�������ߴ���ʱ��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
            }
        }

        //����״̬
        m_pITableFrame->SetGameStatus(GS_TK_SCORE);
        EnableTimeElapse(true);

        //���·����û���Ϣ
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserItem != NULL)
            {
                UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
            }
        }

        //��ȡ�����ע
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] != TRUE || i == m_wBankerUser)
            {
                continue;
            }

            //��ע����
            m_lTurnMaxScore[i] = GetUserMaxTurnScore(i);
        }

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(m_bLastTurnBetBtEx[i] == true)
            {
                m_bLastTurnBetBtEx[i] = false;
            }
        }

        m_lPlayerBetBtEx[m_wBankerUser] = 0;

        //���ñ���
        CMD_S_GameStart GameStart;
        ZeroMemory(&GameStart, sizeof(GameStart));
        GameStart.wBankerUser = m_wBankerUser;
        CopyMemory(GameStart.cbPlayerStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));

        //���ĵ���
        if(m_stConfig == ST_SENDFOUR_)
        {
            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
                {
                    continue;
                }

                //�ɷ��˿�(��ʼֻ��������)
                CopyMemory(GameStart.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
            }
        }

        GameStart.stConfig = m_stConfig;
        GameStart.bgtConfig = m_bgtConfig;
        GameStart.btConfig = m_btConfig;
        GameStart.gtConfig = m_gtConfig;

        CopyMemory(GameStart.lFreeConfig, m_lFreeConfig, sizeof(GameStart.lFreeConfig));
        CopyMemory(GameStart.lPercentConfig, m_lPercentConfig, sizeof(GameStart.lPercentConfig));
        CopyMemory(GameStart.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(GameStart.lPlayerBetBtEx));

        bool bFirstRecord = true;

        WORD wRealPlayerCount = 0;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            if(!pServerUserItem)
            {
                continue;
            }

            wRealPlayerCount++;
        }

        BYTE *pGameRule = m_pITableFrame->GetGameRule();

        //�����ע
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }
            GameStart.lTurnMaxScore = m_lTurnMaxScore[i];
            m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

            if(m_pGameVideo)
            {
                Video_GameStart video;
                ZeroMemory(&video, sizeof(video));
                video.lCellScore = m_pITableFrame->GetCellScore();
                video.wPlayerCount = wRealPlayerCount;
                video.wGamePlayerCountRule = pGameRule[1];
                video.wBankerUser = GameStart.wBankerUser;
                CopyMemory(video.cbPlayerStatus, GameStart.cbPlayerStatus, sizeof(video.cbPlayerStatus));
                video.lTurnMaxScore = GameStart.lTurnMaxScore;
                CopyMemory(video.cbCardData, GameStart.cbCardData, sizeof(video.cbCardData));
                video.ctConfig = m_ctConfig;
                video.stConfig = GameStart.stConfig;
                video.bgtConfig = GameStart.bgtConfig;
                video.btConfig = GameStart.btConfig;
                video.gtConfig = GameStart.gtConfig;

                CopyMemory(video.lFreeConfig, GameStart.lFreeConfig, sizeof(video.lFreeConfig));
                CopyMemory(video.lPercentConfig, GameStart.lPercentConfig, sizeof(video.lPercentConfig));
                CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
                video.wChairID = i;
                video.lScore = pServerUserItem->GetUserScore();

                m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, bFirstRecord);

                if(bFirstRecord == true)
                {
                    bFirstRecord = false;
                }
            }
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
    }

    return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
    //״̬Ч��
    BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
    ASSERT(cbGameStatus == GS_TK_SCORE);
    if(cbGameStatus != GS_TK_SCORE)
    {
        //д��־
		CString strFileName = TEXT("������־");

        tagLogUserInfo LogUserInfo;
        ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
        CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
        CopyMemory(LogUserInfo.szLogContent, TEXT("cbGameStatus = FALSE"), sizeof(LogUserInfo.szLogContent));
        //m_pITableFrame->SendGameLogData(LogUserInfo);

        return true;
    }

    //ׯ��У��
    if(wChairID == m_wBankerUser)
    {
        //д��־
		CString strFileName = TEXT("������־");

        tagLogUserInfo LogUserInfo;
        ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
        CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
        CopyMemory(LogUserInfo.szLogContent, TEXT("wChairID == m_wBankerUser = FALSE"), sizeof(LogUserInfo.szLogContent));
        //m_pITableFrame->SendGameLogData(LogUserInfo);

        return false;
    }

    //���Ч��
    if(m_cbPlayStatus[wChairID] == TRUE)
    {
		//��ע����У��
		bool bValidBet = false;
		for (WORD i = 0; i<MAX_CONFIG - 1; i++)
		{
			if (lScore == m_lFreeConfig[i] * m_pITableFrame->GetCellScore())
			{
				bValidBet = true;
				break;
			}
		}

		if (lScore == m_lPlayerBetBtEx[wChairID])
		{
			bValidBet = true;
		}

		//��Ч��ע����
		if (!bValidBet)
		{
			//д��־
			CString strFileName = TEXT("������־");

			tagLogUserInfo LogUserInfo;
			ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
			CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
			CopyMemory(LogUserInfo.szLogContent, TEXT("INValidBet"), sizeof(LogUserInfo.szLogContent));
			//m_pITableFrame->SendGameLogData(LogUserInfo);

			WriteInfo(TEXT("����ţ������־.log"), TEXT("invalid_bet\n"));

			return false;
		}

        ASSERT(lScore > 0);
        if(lScore <= 0)
        {
            //д��־
			CString strFileName = TEXT("������־");

            tagLogUserInfo LogUserInfo;
            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
            CopyMemory(LogUserInfo.szLogContent, TEXT("lScore < 0"), sizeof(LogUserInfo.szLogContent));
            //m_pITableFrame->SendGameLogData(LogUserInfo);

            return false;
        }

        if(lScore > m_lTurnMaxScore[wChairID])
        {
            lScore = m_lTurnMaxScore[wChairID];
        }
    }
    else //û��ע���ǿ��
    {
        ASSERT(lScore == 0);
        if(lScore != 0)
        {
            //д��־
			CString strFileName = TEXT("������־");

            tagLogUserInfo LogUserInfo;
            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
            CopyMemory(LogUserInfo.szLogContent, TEXT("lScore!=0"), sizeof(LogUserInfo.szLogContent));
            //m_pITableFrame->SendGameLogData(LogUserInfo);

            return false;
        }
    }

    if(lScore > 0L)
    {
        //��ע���
        m_lTableScore[wChairID] = lScore;
        m_bBuckleServiceCharge[wChairID] = true;
        //��������
        CMD_S_AddScore AddScore;
        AddScore.wAddScoreUser = wChairID;
        AddScore.lAddScoreCount = m_lTableScore[wChairID];

        //��������
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
            m_pITableFrame->SendTableData(i, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_ADD_SCORE, &AddScore);
        }
    }

    if(lScore == m_lPlayerBetBtEx[wChairID])
    {
        m_bLastTurnBetBtEx[wChairID] = true;
    }

    //��ע����
    BYTE bUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_lTableScore[i] > 0L && m_cbPlayStatus[i] == TRUE)
        {
            bUserCount++;
        }
        else if(m_cbPlayStatus[i] == FALSE || i == m_wBankerUser)
        {
            bUserCount++;
        }
    }

    //�м�ȫ��
    if(bUserCount == m_wPlayerCount)
    {
        bool bTrusteeUser = false;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                bTrusteeUser = true;
            }
        }

        //�Ƿ��������ö�ʱ��
        if(!IsRoomCardType()/* || bTrusteeUser*/)
        {
            m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
            m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
        }

        //�������ߴ���ʱ��
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
            {
                m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
            }
        }

        //����״̬
        m_pITableFrame->SetGameStatus(GS_TK_PLAYING);
        EnableTimeElapse(true);

        //���·����û���Ϣ
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserItem != NULL)
            {
                UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
            }
        }

        //��������
        CMD_S_SendCard SendCard;
        ZeroMemory(SendCard.cbCardData, sizeof(SendCard.cbCardData));

        //���������ݣ���ע���ƺͷ��ĵ������ִ���
        UINT nCirCount = 0;
        while(true)
        {
            if(nCirCount > 50000)
            {
                break;
            }

            nCirCount++;
            bool AnalyseCardValid = true;

            AnalyseCard(m_stConfig);

            for(WORD i = 0; i < m_wPlayerCount; i++)
            {
                if(m_cbPlayStatus[i] == TRUE && m_cbHandCardData[i][0] == 0 && m_cbHandCardData[i][1] == 0 && m_cbHandCardData[i][2] == 0)
                {
                    AnalyseCardValid = false;
                    break;
                }
            }

            //����������Ч�����ܶ��ǡ�0����0����0����0����0��
            if(AnalyseCardValid == false)
            {
                //����˿�
                BYTE bTempArray[GAME_PLAYER * MAX_CARDCOUNT];
                m_GameLogic.RandCardList(bTempArray, sizeof(bTempArray), (m_gtConfig == GT_HAVEKING_ ? true : false));

                for(WORD i = 0; i < m_wPlayerCount; i++)
                {
                    if(m_cbPlayStatus[i] == TRUE)
                    {
                        //�ɷ��˿�
                        CopyMemory(m_cbHandCardData[i], &bTempArray[i * MAX_CARDCOUNT], MAX_CARDCOUNT);
                    }
                }
            }
            else
            {
                break;
            }
        }

        //��������
        ROOMUSERDEBUG roomuserdebug;
        ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));
        POSITION posKeyList;

        //���� (��ע���ƺͷ��ĵ������ֵ���)
        if(m_pServerDebug != NULL && AnalyseRoomUserDebug(roomuserdebug, posKeyList))
        {
            //У������
            ASSERT(roomuserdebug.roomUserInfo.wChairID != INVALID_CHAIR && roomuserdebug.userDebug.cbDebugCount != 0
                   && roomuserdebug.userDebug.debug_type != CONTINUE_CANCEL);

            if(m_pServerDebug->DebugResult(m_cbHandCardData, roomuserdebug, m_stConfig, m_ctConfig, m_gtConfig))
            {
                //��ȡԪ��
                ROOMUSERDEBUG &tmproomuserdebug = g_ListRoomUserDebug.GetAt(posKeyList);

                //У������
                ASSERT(roomuserdebug.userDebug.cbDebugCount == tmproomuserdebug.userDebug.cbDebugCount);

                //���Ծ���
                tmproomuserdebug.userDebug.cbDebugCount--;

                CMD_S_UserDebugComplete UserDebugComplete;
                ZeroMemory(&UserDebugComplete, sizeof(UserDebugComplete));
                UserDebugComplete.dwGameID = roomuserdebug.roomUserInfo.dwGameID;
                CopyMemory(UserDebugComplete.szNickName, roomuserdebug.roomUserInfo.szNickName, sizeof(UserDebugComplete.szNickName));
                UserDebugComplete.debugType = roomuserdebug.userDebug.debug_type;
                UserDebugComplete.cbRemainDebugCount = tmproomuserdebug.userDebug.cbDebugCount;

				//��������
				m_pITableFrame->SendRoomData(NULL, SUB_S_USER_DEBUG_COMPLETE, &UserDebugComplete, sizeof(UserDebugComplete));
            }
        }

        //��ʱ�˿�,��Ϊ�����͵����˿ˣ�����ԭʼ����
        BYTE cbTempHandCardData[GAME_PLAYER][MAX_CARDCOUNT];
        ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
        CopyMemory(cbTempHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

		//ԭʼ�˿�
		CopyMemory(m_cbOriginalCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUser == NULL)
            {
                continue;
            }

            m_bSpecialCard[i] = (m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig) > CT_CLASSIC_OX_VALUENIUNIU ? true : false);

            //��������
            if(m_bSpecialCard[i])
            {
                m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
            }
            else
            {
                //��ȡţţ����
                m_GameLogic.GetOxCard(cbTempHandCardData[i], MAX_CARDCOUNT);

                m_cbOriginalCardType[i] = m_GameLogic.GetCardType(cbTempHandCardData[i], MAX_CARDCOUNT, m_ctConfig);
            }
        }

        CopyMemory(SendCard.cbCardData, m_cbHandCardData, sizeof(SendCard.cbCardData));
        CopyMemory(SendCard.bSpecialCard, m_bSpecialCard, sizeof(SendCard.bSpecialCard));
        CopyMemory(SendCard.cbOriginalCardType, m_cbOriginalCardType, sizeof(SendCard.cbOriginalCardType));

        //��������
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
        }
        m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

        if(m_pGameVideo)
        {
            m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard);
        }

        CString strUserCard;
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            IServerUserItem *pIServer = m_pITableFrame->GetTableUserItem(i);
            if(!pIServer)
            {
                continue;
            }

            //д��־
            CString strUserCard;
            strUserCard.Format(TEXT("-USERID[%d],�����ݷֱ�Ϊ[%d][%d][%d][%d][%d],����Ϊ[%d]"), pIServer->GetUserID(),
                               m_cbHandCardData[i][0], m_cbHandCardData[i][1], m_cbHandCardData[i][2], m_cbHandCardData[i][3], m_cbHandCardData[i][4], m_cbOriginalCardType[i]);
			CString strFileName = TEXT("������־");

            tagLogUserInfo LogUserInfo;
            ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
            CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
            CopyMemory(LogUserInfo.szLogContent, strUserCard, sizeof(LogUserInfo.szLogContent));
            //m_pITableFrame->SendGameLogData(LogUserInfo);
        }
    }

    return true;
}

//̯���¼�
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE cbCombineCardData[MAX_CARDCOUNT])
{
    //״̬Ч��
    BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
    ASSERT(cbGameStatus == GS_TK_PLAYING);
    if(cbGameStatus != GS_TK_PLAYING) { return true; }
    if(m_bOpenCard[wChairID] != false) { return true; }

    //̯�Ʊ�־
    m_bOpenCard[wChairID] = true;

    //����ţţ�ͻ��˲�����ƣ�ֱ��̯��
    BYTE cbTempHandCardData[MAX_CARDCOUNT];
    ZeroMemory(cbTempHandCardData, sizeof(cbTempHandCardData));
    CopyMemory(cbTempHandCardData, m_cbHandCardData[wChairID], sizeof(cbTempHandCardData));

    bool bSpecialCard = (m_GameLogic.GetCardType(cbTempHandCardData, MAX_CARDCOUNT, m_ctConfig) > CT_CLASSIC_OX_VALUENIUNIU ? true : false);
    if(!bSpecialCard)
    {
        //��ȡţţ����
        m_GameLogic.GetOxCard(cbTempHandCardData, MAX_CARDCOUNT);

        CopyMemory(cbCombineCardData, cbTempHandCardData, sizeof(cbTempHandCardData));
    }

    //�����������Ʊ�־
    //bool bUserCombine = false;
    //for (WORD i=0; i<MAX_CARDCOUNT; i++)
    //{
    //	if (m_cbHandCardData[wChairID][i] != cbCombineCardData[i])
    //	{
    //		bUserCombine = true;
    //		break;
    //	}
    //}
    //
    ////û���������
    //if (!bUserCombine)
    //{
    //	m_cbCombineCardType[wChairID] = m_cbOriginalCardType[wChairID];
    //}
    //else
    {
        //�������� ��ֵ��ʼ����
        if(m_bSpecialCard[wChairID])
        {
            m_cbCombineCardType[wChairID] = m_cbOriginalCardType[wChairID];
        }
        else
        {
            //ǰ��������������
            BYTE cbFirstKingCount = m_GameLogic.GetKingCount(cbCombineCardData, 3);
            BYTE cbSecondKingCount = m_GameLogic.GetKingCount(&cbCombineCardData[3], 2);

            if(cbFirstKingCount == 0)
            {
                //ǰ�������߼�ֵ
                BYTE cbFirstNNLogicValue = 0;
                for(WORD i = 0; i < 3; i++)
                {
                    cbFirstNNLogicValue += m_GameLogic.GetNNCardLogicValue(cbCombineCardData[i]);
                }

                if(cbFirstNNLogicValue % 10 != 0)
                {
                    m_cbCombineCardType[wChairID] = CT_CLASSIC_OX_VALUE0;
                }
                else
                {
                    if(cbSecondKingCount != 0)
                    {
                        m_cbCombineCardType[wChairID] = CT_CLASSIC_OX_VALUENIUNIU;
                    }
                    else
                    {
                        BYTE cbSecondNNLogicValue = 0;
                        for(WORD i = 3; i < 5; i++)
                        {
                            cbSecondNNLogicValue += m_GameLogic.GetNNCardLogicValue(cbCombineCardData[i]);
                        }

                        m_cbCombineCardType[wChairID] = ((cbSecondNNLogicValue % 10 == 0) ? CT_CLASSIC_OX_VALUENIUNIU : (cbSecondNNLogicValue % 10));
                    }
                }
            }
            else
            {
                if(cbSecondKingCount != 0)
                {
                    m_cbCombineCardType[wChairID] = CT_CLASSIC_OX_VALUENIUNIU;
                }
                else
                {
                    BYTE cbSecondNNLogicValue = 0;
                    for(WORD i = 3; i < 5; i++)
                    {
                        cbSecondNNLogicValue += m_GameLogic.GetNNCardLogicValue(cbCombineCardData[i]);
                    }

                    m_cbCombineCardType[wChairID] = ((cbSecondNNLogicValue % 10 == 0) ? CT_CLASSIC_OX_VALUENIUNIU : (cbSecondNNLogicValue % 10));
                }
            }

            //������Ϲ����˿�
            CopyMemory(m_cbHandCardData[wChairID], cbCombineCardData, sizeof(m_cbHandCardData[wChairID]));
        }
    }

    //̯������
    BYTE bUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_bOpenCard[i] == true && m_cbPlayStatus[i] == TRUE) { bUserCount++; }
        else if(m_cbPlayStatus[i] == FALSE) { bUserCount++; }
    }

    //�������
    CMD_S_Open_Card OpenCard;
    ZeroMemory(&OpenCard, sizeof(OpenCard));

    //���ñ���
    OpenCard.bOpenCard = true;
    OpenCard.wOpenChairID = wChairID;

    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE) { continue; }
        m_pITableFrame->SendTableData(i, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));

    if(m_pGameVideo)
    {
        m_pGameVideo->AddVideoData(SUB_S_OPEN_CARD, &OpenCard);
    }

    //������Ϸ
    if(bUserCount == m_wPlayerCount)
    {
        return OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
    }

    return true;
}

//�˿˷���
void CTableFrameSink::AnalyseCard(SENDCARDTYPE_CONFIG stConfig)
{
    //AI��
    bool bIsAiBanker = false;
    WORD wAiCount = 0;
    WORD wPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE)
        {
            if(pIServerUserItem->IsAndroidUser())
            {
                wAiCount++;
                if(i == m_wBankerUser)
                {
                    bIsAiBanker = true;
                }
            }

            wPlayerCount++;
        }
    }

    //ȫ������
    if(wPlayerCount == wAiCount || wAiCount == 0)
    {
        return;
    }

    //�˿˱���
    BYTE cbUserCardData[GAME_PLAYER][MAX_CARDCOUNT];
    CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

    //��ȡ�������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            m_GameLogic.GetOxCard(cbUserCardData[i], MAX_CARDCOUNT);
        }
    }

    //��������
    LONGLONG lAndroidScore = 0;

    //��������
    BYTE cbCardTimes[GAME_PLAYER];
    ZeroMemory(cbCardTimes, sizeof(cbCardTimes));

    //���ұ���
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            cbCardTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }
    }

    //������ׯ ������Ҫ����cbMaxCallBankerTimes
    BYTE cbMaxCallBankerTimes = 1;
    if(m_bgtConfig == BGT_ROB_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
            {
                cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
            }
        }
    }

    //����ׯ��
    if(bIsAiBanker)
    {
        //�Ա��˿�
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //�û�����
            if((i == m_wBankerUser) || (m_cbPlayStatus[i] == FALSE)) { continue; }

            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            //��������
            if((pIServerUserItem != NULL) && (pIServerUserItem->IsAndroidUser())) { continue; }

            //�Ա��˿�
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig) == true)
            {
                lAndroidScore -= cbCardTimes[i] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
            else
            {
                lAndroidScore += cbCardTimes[m_wBankerUser] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
        }
    }
    else//�û�ׯ��
    {
        //�Ա��˿�
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            //�û�����
            if((i == m_wBankerUser) || (pIServerUserItem == NULL) || !(pIServerUserItem->IsAndroidUser())) { continue; }

            //�Ա��˿�
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig) == true)
            {
                lAndroidScore += cbCardTimes[i] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
            else
            {
                lAndroidScore -= cbCardTimes[m_wBankerUser] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
        }
    }

    LONGLONG lGameEndStorage = g_lRoomStorageCurrent + lAndroidScore;

	BYTE cbRandVal = rand() % 100;

    //��ע����
    if(stConfig == ST_BETFIRST_)
    {
        //��������
        WORD wMaxUser = INVALID_CHAIR;
        WORD wMinAndroid = INVALID_CHAIR;
        WORD wMaxAndroid = INVALID_CHAIR;

        //�����������
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
            if(pIServerUserItem == NULL) { continue; }

            //�������
            if(pIServerUserItem->IsAndroidUser() == false)
            {
                //��ʼ����
                if(wMaxUser == INVALID_CHAIR) { wMaxUser = i; }

                //��ȡ�ϴ���
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wMaxUser], MAX_CARDCOUNT, m_ctConfig) == true)
                {
                    wMaxUser = i;
                }
            }

            //�������
            if(pIServerUserItem->IsAndroidUser() == true)
            {
                //��ʼ����
                if(wMinAndroid == INVALID_CHAIR) { wMinAndroid = i; }
                if(wMaxAndroid == INVALID_CHAIR) { wMaxAndroid = i; }

                //��ȡ��С��
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wMinAndroid], MAX_CARDCOUNT, m_ctConfig) == false)
                {
                    wMinAndroid = i;
                }

                //��ȡ�ϴ���
                if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wMaxAndroid], MAX_CARDCOUNT, m_ctConfig) == true)
                {
                    wMaxAndroid = i;
                }
            }
        }

		if (g_lRoomStorageCurrent + lAndroidScore<0 || (lGameEndStorage < (g_lRoomStorageCurrent * (double)(1 - (double)10 / (double)100))))
		{
			//�����ң��������˺�AI��
			WORD wWinUser = wMaxUser;
			//��С��ң��������˺�AI��
			WORD wLostUser = wMaxUser;

			//����������
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{

				//�û�����
				if (m_cbPlayStatus[i] == FALSE) continue;

				//��ȡ�ϴ���
				if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_CARDCOUNT, m_ctConfig) == true)
				{
					wWinUser = i;
				}
			}

			//������С���
			for (WORD i = 0; i<m_wPlayerCount; i++)
			{

				//�û�����
				if (m_cbPlayStatus[i] == FALSE) continue;

				//��ȡ�ϴ���
				if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wLostUser], MAX_CARDCOUNT, m_ctConfig) == false)
				{
					wLostUser = i;
				}
			}

			//AI��ׯ
			if (bIsAiBanker)
			{
				//��������
				BYTE cbTempData[MAX_CARDCOUNT];
				CopyMemory(cbTempData, m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);
				CopyMemory(m_cbHandCardData[m_wBankerUser], m_cbHandCardData[wWinUser], MAX_CARDCOUNT);
				CopyMemory(m_cbHandCardData[wWinUser], cbTempData, MAX_CARDCOUNT);
			}
			else
			{
				//��С���Ƹ��������
				BYTE cbTempData[MAX_CARDCOUNT];
				CopyMemory(cbTempData, m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);
				CopyMemory(m_cbHandCardData[m_wBankerUser], m_cbHandCardData[wLostUser], MAX_CARDCOUNT);
				CopyMemory(m_cbHandCardData[wLostUser], cbTempData, MAX_CARDCOUNT);
			}
		}
		else
		{
			//��������2
			if (g_lRoomStorageCurrent>0 && g_lRoomStorageCurrent > g_lStorageMax2Room && g_lRoomStorageCurrent + lAndroidScore > 0 && cbRandVal < g_lStorageMul2Room)
			{
				if (m_GameLogic.CompareCard(cbUserCardData[wMaxAndroid], cbUserCardData[wMaxUser], MAX_CARDCOUNT, m_ctConfig) == true)
				{
					//��������
					BYTE cbTempData[MAX_CARDCOUNT];
					CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);

					//��治��������
					if (JudgeStock() == false)
					{
						CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);
					}
				}
				else
				{
					if (JudgeStock() == false)
					{
						BYTE cbTempData[MAX_CARDCOUNT];
						ZeroMemory(cbTempData, sizeof(cbTempData));
						CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);
					}
				}
			}
			//��������1
			else if (g_lRoomStorageCurrent>0 && g_lRoomStorageCurrent > g_lStorageMax1Room && g_lRoomStorageCurrent + lAndroidScore > 0 && cbRandVal < g_lStorageMul1Room)
			{
				if (m_GameLogic.CompareCard(cbUserCardData[wMaxAndroid], cbUserCardData[wMaxUser], MAX_CARDCOUNT, m_ctConfig) == true)
				{
					//��������
					BYTE cbTempData[MAX_CARDCOUNT];
					CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);

					//��治��������
					if (JudgeStock() == false)
					{
						CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);
					}
				}
				else
				{
					if (JudgeStock() == false)
					{
						BYTE cbTempData[MAX_CARDCOUNT];
						ZeroMemory(cbTempData, sizeof(cbTempData));
						CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);
					}
				}
			}
			//����������1.2,���Ӯ�ָ��ʰٷ�֮50
			else if (cbRandVal < 50)
			{
				if (m_GameLogic.CompareCard(cbUserCardData[wMaxAndroid], cbUserCardData[wMaxUser], MAX_CARDCOUNT, m_ctConfig) == true)
				{
					//��������
					BYTE cbTempData[MAX_CARDCOUNT];
					CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);

					//��治��������
					if (JudgeStock() == false)
					{
						CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);
					}
				}
				else
				{
					if (JudgeStock() == false)
					{
						BYTE cbTempData[MAX_CARDCOUNT];
						ZeroMemory(cbTempData, sizeof(cbTempData));
						CopyMemory(cbTempData, m_cbHandCardData[wMaxUser], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxUser], m_cbHandCardData[wMaxAndroid], MAX_CARDCOUNT);
						CopyMemory(m_cbHandCardData[wMaxAndroid], cbTempData, MAX_CARDCOUNT);
					}
				}
			}
			else
			{
				//�����ң��������˺�AI��
				WORD wWinUser = wMaxUser;
				//��С��ң��������˺�AI��
				WORD wLostUser = wMaxUser;

				//����������
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{

					//�û�����
					if (m_cbPlayStatus[i] == FALSE) continue;

					//��ȡ�ϴ���
					if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wWinUser], MAX_CARDCOUNT, m_ctConfig) == true)
					{
						wWinUser = i;
					}
				}

				//������С���
				for (WORD i = 0; i<m_wPlayerCount; i++)
				{

					//�û�����
					if (m_cbPlayStatus[i] == FALSE) continue;

					//��ȡ�ϴ���
					if (m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[wLostUser], MAX_CARDCOUNT, m_ctConfig) == false)
					{
						wLostUser = i;
					}
				}

				//AI��ׯ
				if (bIsAiBanker)
				{
					//��������
					BYTE cbTempData[MAX_CARDCOUNT];
					CopyMemory(cbTempData, m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[m_wBankerUser], m_cbHandCardData[wWinUser], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wWinUser], cbTempData, MAX_CARDCOUNT);
				}
				else
				{
					//��С���Ƹ��������
					BYTE cbTempData[MAX_CARDCOUNT];
					CopyMemory(cbTempData, m_cbHandCardData[m_wBankerUser], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[m_wBankerUser], m_cbHandCardData[wLostUser], MAX_CARDCOUNT);
					CopyMemory(m_cbHandCardData[wLostUser], cbTempData, MAX_CARDCOUNT);
				}
			}
		}
    }
    else if(stConfig == ST_SENDFOUR_)
    {
        //�˿�����
        CList<BYTE, BYTE &> cardlist;
        cardlist.RemoveAll();

        //����С��
        if(m_gtConfig == GT_HAVEKING_)
        {
            for(WORD i = 0; i < 54; i++)
            {
                cardlist.AddTail(m_GameLogic.m_cbCardListDataHaveKing[i]);
            }
        }
        else if(m_gtConfig == GT_NOKING_)
        {
            for(WORD i = 0; i < 52; i++)
            {
                cardlist.AddTail(m_GameLogic.m_cbCardListDataNoKing[i]);
            }
        }

        //ɾ���˿� ��ɾ��ǰ��4�ţ��������һ�ţ�
        for(WORD i = 0; i < GAME_PLAYER; i++)
        {
            for(WORD j = 0; j < MAX_CARDCOUNT - 1; j++)
            {
                if(m_cbHandCardData[i][j] != 0)
                {
                    POSITION ptListHead = cardlist.GetHeadPosition();
                    POSITION ptTemp;
                    BYTE cbCardData = INVALID_BYTE;

                    //��������
                    while(ptListHead)
                    {
                        ptTemp = ptListHead;
                        if(cardlist.GetNext(ptListHead) == m_cbHandCardData[i][j])
                        {
                            cardlist.RemoveAt(ptTemp);
                            break;
                        }
                    }
                }
            }
        }

		//����ж�
		if (g_lRoomStorageCurrent + lAndroidScore<0 || (lGameEndStorage < (g_lRoomStorageCurrent * (double)(1 - (double)10 / (double)100))))
		{
			//��ֻ�����Ӯ

			//��������ׯ, ʣ�������У���ȡ������֪��������A �� ������ׯ�ҹ����A��
			if (m_wBankerUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
			{
				BYTE cbMaxCardTypeSendFour = m_GameLogic.GetMaxCardTypeSendFourRealPlayer(cardlist, m_cbHandCardData, m_ctConfig, m_pITableFrame, m_cbPlayStatus, m_cbDynamicJoin, m_wPlayerCount);
				cbMaxCardTypeSendFour = (cbMaxCardTypeSendFour >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSendFour);

				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[m_wBankerUser], cbMaxCardTypeSendFour + 1, m_gtConfig);
				if (cbKeyCardData != INVALID_BYTE)
				{
					m_cbHandCardData[m_wBankerUser][4] = cbKeyCardData;
				}
			}
			//������ׯ ,Ϊ�˱�֤������Ӯ�����л����˹��������ׯ�Ҵ���� (������5���ƶ����Ի� )
			else if (m_wBankerUser != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
			{
				BYTE cbMaxCardTypeSingle = m_GameLogic.GetMaxCardTypeSingle(cardlist, m_cbHandCardData[m_wBankerUser], m_ctConfig);
				cbMaxCardTypeSingle = (cbMaxCardTypeSingle >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSingle);

				for (WORD i = 0; i<m_wPlayerCount; i++)
				{
					//��ȡ�û�
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
					{
						//AI
						if (pIServerUserItem->IsAndroidUser() == true)
						{
							BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], cbMaxCardTypeSingle + 1, m_gtConfig);
							if (cbKeyCardData != INVALID_BYTE)
							{
								m_cbHandCardData[i][4] = cbKeyCardData;
							}
						}
					}
				}
			}

			//ţ��Ϊ��׼�㣬AI��ţ�����Ͽ�ʼ���죬 ��ͨ��Ҵ�ţ�����¿�ʼ����
			//for (WORD i = 0; i<m_wPlayerCount; i++)
			//{
			//	//��ȡ�û�
			//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			//	if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
			//	{
			//		//AI
			//		if (pIServerUserItem->IsAndroidUser() == true)
			//		{
			//			//AI��ţ�����Ͽ�ʼ����
			//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE6; wCardTypeIndex <= CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
			//			{
			//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
			//				if (cbKeyCardData == INVALID_BYTE)
			//				{
			//					continue;
			//				}
			//				else
			//				{
			//					m_cbHandCardData[i][4] = cbKeyCardData;
			//					break;
			//				}
			//			}
			//		}
			//		else if (pIServerUserItem->IsAndroidUser() == false)
			//		{
			//			//��ͨ��Ҵ�ţ�����¿�ʼ����
			//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE5; wCardTypeIndex >= CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
			//			{
			//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
			//				if (cbKeyCardData == INVALID_BYTE)
			//				{
			//					continue;
			//				}
			//				else
			//				{
			//					m_cbHandCardData[i][4] = cbKeyCardData;
			//					break;
			//				}
			//			}
			//		}
			//	}
			//}
		}
		else
		{
			if (g_lRoomStorageCurrent>0 /*&& lAndroidScore>0*/ && g_lRoomStorageCurrent > g_lStorageMax2Room && g_lRoomStorageCurrent + lAndroidScore > 0 && cbRandVal < g_lStorageMul2Room)
			{
				//�������Ӯ

				//��������ׯ, Ϊ�˱�֤����Ӯ���������˹���Ȼ�����ׯ�Ҵ���� 
				if (m_wBankerUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSingle = m_GameLogic.GetMaxCardTypeSingle(cardlist, m_cbHandCardData[m_wBankerUser], m_ctConfig);
					cbMaxCardTypeSingle = (cbMaxCardTypeSingle >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSingle);

					for (WORD i = 0; i<m_wPlayerCount; i++)
					{
						//��ȡ�û�
						IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
						{
							//����
							if (pIServerUserItem->IsAndroidUser() == false)
							{
								BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], cbMaxCardTypeSingle + 1, m_gtConfig);
								if (cbKeyCardData != INVALID_BYTE)
								{
									m_cbHandCardData[i][4] = cbKeyCardData;
								}
							}
						}
					}					
				}
				//������ׯ ,ʣ�»������У���ȡ������֪��������A �� ����ׯ�ҹ����A��
				else if (m_wBankerUser != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSendFour = m_GameLogic.GetMaxCardTypeSendFourRealPlayer(cardlist, m_cbHandCardData, m_ctConfig, m_pITableFrame, m_cbPlayStatus, m_cbDynamicJoin, m_wPlayerCount);
					cbMaxCardTypeSendFour = (cbMaxCardTypeSendFour >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSendFour);

					BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[m_wBankerUser], cbMaxCardTypeSendFour + 1, m_gtConfig);
					if (cbKeyCardData != INVALID_BYTE)
					{
						m_cbHandCardData[m_wBankerUser][4] = cbKeyCardData;
					}
				}

				//ţ��Ϊ��׼�㣬AI��ţ�����¿�ʼ���� ��ͨ��Ҵ�ţ�����Ͽ�ʼ����
				//for (WORD i = 0; i<m_wPlayerCount; i++)
				//{
				//	//��ȡ�û�
				//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				//	if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
				//	{
				//		//AI
				//		if (pIServerUserItem->IsAndroidUser() == true)
				//		{
				//			//AI��ţ�����¿�ʼ����
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE5; wCardTypeIndex >= CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//		else if (pIServerUserItem->IsAndroidUser() == false)
				//		{
				//			//��ͨ��Ҵ�ţ�����Ͽ�ʼ����
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE6; wCardTypeIndex <= CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//	}
				//}
			}
			else if (g_lRoomStorageCurrent>0 /*&& lAndroidScore>0*/ && g_lRoomStorageCurrent > g_lStorageMax1Room && g_lRoomStorageCurrent + lAndroidScore > 0 && cbRandVal < g_lStorageMul1Room)
			{
				//�������Ӯ

				//��������ׯ, Ϊ�˱�֤����Ӯ���������˹���Ȼ�����ׯ�Ҵ���� 
				if (m_wBankerUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSingle = m_GameLogic.GetMaxCardTypeSingle(cardlist, m_cbHandCardData[m_wBankerUser], m_ctConfig);
					cbMaxCardTypeSingle = (cbMaxCardTypeSingle >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSingle);

					for (WORD i = 0; i<m_wPlayerCount; i++)
					{
						//��ȡ�û�
						IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
						{
							//����
							if (pIServerUserItem->IsAndroidUser() == false)
							{
								BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], cbMaxCardTypeSingle + 1, m_gtConfig);
								if (cbKeyCardData != INVALID_BYTE)
								{
									m_cbHandCardData[i][4] = cbKeyCardData;
								}
							}
						}
					}
				}
				//������ׯ ,ʣ�»������У���ȡ������֪��������A �� ����ׯ�ҹ����A��
				else if (m_wBankerUser != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSendFour = m_GameLogic.GetMaxCardTypeSendFourRealPlayer(cardlist, m_cbHandCardData, m_ctConfig, m_pITableFrame, m_cbPlayStatus, m_cbDynamicJoin, m_wPlayerCount);
					cbMaxCardTypeSendFour = (cbMaxCardTypeSendFour >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSendFour);

					BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[m_wBankerUser], cbMaxCardTypeSendFour + 1, m_gtConfig);
					if (cbKeyCardData != INVALID_BYTE)
					{
						m_cbHandCardData[m_wBankerUser][4] = cbKeyCardData;
					}
				}

				//ţ��Ϊ��׼�㣬AI��ţ�����¿�ʼ���� ��ͨ��Ҵ�ţ�����Ͽ�ʼ����
				//for (WORD i = 0; i<m_wPlayerCount; i++)
				//{
				//	//��ȡ�û�
				//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				//	if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
				//	{
				//		//AI
				//		if (pIServerUserItem->IsAndroidUser() == true)
				//		{
				//			//AI��ţ�����¿�ʼ����
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE5; wCardTypeIndex >= CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//		else if (pIServerUserItem->IsAndroidUser() == false)
				//		{
				//			//��ͨ��Ҵ�ţ�����Ͽ�ʼ����
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE6; wCardTypeIndex <= CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//	}
				//}
			}
			//����������1.2,���Ӯ�ָ��ʰٷ�֮50
			else if (cbRandVal < 50)
			{
				//�������Ӯ

				//��������ׯ, Ϊ�˱�֤����Ӯ���������˹���Ȼ�����ׯ�Ҵ���� 
				if (m_wBankerUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSingle = m_GameLogic.GetMaxCardTypeSingle(cardlist, m_cbHandCardData[m_wBankerUser], m_ctConfig);
					cbMaxCardTypeSingle = (cbMaxCardTypeSingle >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSingle);

					for (WORD i = 0; i<m_wPlayerCount; i++)
					{
						//��ȡ�û�
						IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
						{
							//����
							if (pIServerUserItem->IsAndroidUser() == false)
							{
								BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], cbMaxCardTypeSingle + 1, m_gtConfig);
								if (cbKeyCardData != INVALID_BYTE)
								{
									m_cbHandCardData[i][4] = cbKeyCardData;
								}
							}
						}
					}
				}
				//������ׯ ,ʣ�»������У���ȡ������֪��������A �� ����ׯ�ҹ����A��
				else if (m_wBankerUser != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSendFour = m_GameLogic.GetMaxCardTypeSendFourRealPlayer(cardlist, m_cbHandCardData, m_ctConfig, m_pITableFrame, m_cbPlayStatus, m_cbDynamicJoin, m_wPlayerCount);
					cbMaxCardTypeSendFour = (cbMaxCardTypeSendFour >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSendFour);

					BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[m_wBankerUser], cbMaxCardTypeSendFour + 1, m_gtConfig);
					if (cbKeyCardData != INVALID_BYTE)
					{
						m_cbHandCardData[m_wBankerUser][4] = cbKeyCardData;
					}
				}

				//ţ��Ϊ��׼�㣬AI��ţ�����¿�ʼ���� ��ͨ��Ҵ�ţ�����Ͽ�ʼ����
				//for (WORD i = 0; i<m_wPlayerCount; i++)
				//{
				//	//��ȡ�û�
				//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				//	if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
				//	{
				//		//AI
				//		if (pIServerUserItem->IsAndroidUser() == true)
				//		{
				//			//AI��ţ�����¿�ʼ����
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE5; wCardTypeIndex >= CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//		else if (pIServerUserItem->IsAndroidUser() == false)
				//		{
				//			//��ͨ��Ҵ�ţ�����Ͽ�ʼ����
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE6; wCardTypeIndex <= CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//	}
				//}
			}
			else
			{
				//��ֻ�����Ӯ

				//��������ׯ, ʣ�������У���ȡ������֪��������A �� ������ׯ�ҹ����A��
				if (m_wBankerUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSendFour = m_GameLogic.GetMaxCardTypeSendFourRealPlayer(cardlist, m_cbHandCardData, m_ctConfig, m_pITableFrame, m_cbPlayStatus, m_cbDynamicJoin, m_wPlayerCount);
					cbMaxCardTypeSendFour = (cbMaxCardTypeSendFour >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSendFour);

					BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[m_wBankerUser], cbMaxCardTypeSendFour + 1, m_gtConfig);
					if (cbKeyCardData != INVALID_BYTE)
					{
						m_cbHandCardData[m_wBankerUser][4] = cbKeyCardData;
					}
				}
				//������ׯ ,Ϊ�˱�֤������Ӯ�����л����˹��������ׯ�Ҵ���� (������5���ƶ����Ի� )
				else if (m_wBankerUser != INVALID_CHAIR && !m_pITableFrame->GetTableUserItem(m_wBankerUser)->IsAndroidUser())
				{
					BYTE cbMaxCardTypeSingle = m_GameLogic.GetMaxCardTypeSingle(cardlist, m_cbHandCardData[m_wBankerUser], m_ctConfig);
					cbMaxCardTypeSingle = (cbMaxCardTypeSingle >= CT_CLASSIC_OX_VALUE9 ? CT_CLASSIC_OX_VALUE9 : cbMaxCardTypeSingle);

					for (WORD i = 0; i<m_wPlayerCount; i++)
					{
						//��ȡ�û�
						IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
						if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
						{
							//AI
							if (pIServerUserItem->IsAndroidUser() == true)
							{
								BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], cbMaxCardTypeSingle + 1, m_gtConfig);
								if (cbKeyCardData != INVALID_BYTE)
								{
									m_cbHandCardData[i][4] = cbKeyCardData;
								}
							}
						}
					}
				}

				////ţ��Ϊ��׼�㣬AI��ţ�����Ͽ�ʼ���죬 ��ͨ��Ҵ�ţ�����¿�ʼ����
				//for (WORD i = 0; i<m_wPlayerCount; i++)
				//{
				//	//��ȡ�û�
				//	IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
				//	if (pIServerUserItem != NULL && m_cbPlayStatus[i] == TRUE && m_cbDynamicJoin[i] == FALSE)
				//	{
				//		//AI
				//		if (pIServerUserItem->IsAndroidUser() == true)
				//		{
				//			//AI��ţ�����Ͽ�ʼ����
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE6; wCardTypeIndex <= CT_CLASSIC_OX_VALUENIUNIU; wCardTypeIndex++)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//		else if (pIServerUserItem->IsAndroidUser() == false)
				//		{
				//			//��ͨ��Ҵ�ţ�����¿�ʼ����
				//			for (int wCardTypeIndex = CT_CLASSIC_OX_VALUE5; wCardTypeIndex >= CT_CLASSIC_OX_VALUE0; wCardTypeIndex--)
				//			{
				//				BYTE cbKeyCardData = m_GameLogic.ConstructCardType(cardlist, m_cbHandCardData[i], wCardTypeIndex, m_gtConfig);
				//				if (cbKeyCardData == INVALID_BYTE)
				//				{
				//					continue;
				//				}
				//				else
				//				{
				//					m_cbHandCardData[i][4] = cbKeyCardData;
				//					break;
				//				}
				//			}
				//		}
				//	}
				//}
			}
		}
    }

    return;
}

//�жϿ��
bool CTableFrameSink::JudgeStock()
{
    //AI��ׯ��ʶ
    bool bIsAiBanker = false;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser() && m_cbPlayStatus[i] == TRUE && i == m_wBankerUser)
        {
            bIsAiBanker = true;
        }
    }

    //�˿˱���
    BYTE cbUserCardData[GAME_PLAYER][MAX_CARDCOUNT];
    CopyMemory(cbUserCardData, m_cbHandCardData, sizeof(m_cbHandCardData));

    //��ȡ�������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            m_GameLogic.GetOxCard(cbUserCardData[i], MAX_CARDCOUNT);
        }
    }

    //��������
    LONGLONG lAndroidScore = 0;

    //��������
    BYTE cbCardTimes[GAME_PLAYER];
    ZeroMemory(cbCardTimes, sizeof(cbCardTimes));

    //���ұ���
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE)
        {
            cbCardTimes[i] = m_GameLogic.GetTimes(cbUserCardData[i], MAX_CARDCOUNT, m_ctConfig);
        }
    }

    //������ׯ ������Ҫ����cbMaxCallBankerTimes
    BYTE cbMaxCallBankerTimes = 1;
    if(m_bgtConfig == BGT_ROB_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
            {
                cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
            }
        }
    }

    //����ׯ��
    if(bIsAiBanker)
    {
        //�Ա��˿�
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //�û�����
            if((i == m_wBankerUser) || (m_cbPlayStatus[i] == FALSE)) { continue; }

            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            //��������
            if((pIServerUserItem != NULL) && (pIServerUserItem->IsAndroidUser())) { continue; }

            //�Ա��˿�
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig) == true)
            {
                lAndroidScore -= cbCardTimes[i] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
            else
            {
                lAndroidScore += cbCardTimes[m_wBankerUser] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
        }
    }
    else//�û�ׯ��
    {
        //�Ա��˿�
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            //��ȡ�û�
            IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

            //�û�����
            if((i == m_wBankerUser) || (pIServerUserItem == NULL) || !(pIServerUserItem->IsAndroidUser())) { continue; }

            //�Ա��˿�
            if(m_GameLogic.CompareCard(cbUserCardData[i], cbUserCardData[m_wBankerUser], MAX_CARDCOUNT, m_ctConfig) == true)
            {
                lAndroidScore += cbCardTimes[i] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
            else
            {
                lAndroidScore -= cbCardTimes[m_wBankerUser] * m_lTableScore[i] * cbMaxCallBankerTimes;
            }
        }
    }

    LONGLONG lGameEndStorage = g_lRoomStorageCurrent + lAndroidScore;

    return lGameEndStorage > 0 && (lGameEndStorage >= (g_lRoomStorageCurrent * (double)(1 - (double)5 / (double)100)));
}

//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
    for(BYTE i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pUserItem == NULL) { continue; }

        if(m_bBuckleServiceCharge[i] && i == wChairID)
        {
            return true;
        }

    }
    return false;
}


bool CTableFrameSink::TryWriteTableScore(tagScoreInfo ScoreInfoArray[])
{
    //�޸Ļ���
    tagScoreInfo ScoreInfo[GAME_PLAYER];
    ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
    memcpy(&ScoreInfo, ScoreInfoArray, sizeof(ScoreInfo));
    //��¼�쳣
    LONGLONG beforeScore[GAME_PLAYER];
    ZeroMemory(beforeScore, sizeof(beforeScore));
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pItem = m_pITableFrame->GetTableUserItem(i);
        if(pItem != NULL && m_cbPlayStatus[i] == TRUE)
        {
            beforeScore[i] = pItem->GetUserScore();
            m_pITableFrame->WriteUserScore(i, ScoreInfo[i]);
        }
    }

    LONGLONG afterScore[GAME_PLAYER];
    ZeroMemory(afterScore, sizeof(afterScore));
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pItem = m_pITableFrame->GetTableUserItem(i);
        if(pItem != NULL)
        {
            afterScore[i] = pItem->GetUserScore();

            if(afterScore[i] < 0)
            {
                //�쳣д����־
                CString strInfo;
                strInfo.Format(TEXT("USERID[%d] ���ָ���, д��ǰ������%I64d, д����� %I64d��˰�� %I64d, д�ֺ������%I64d"), pItem->GetUserID(), beforeScore[i], ScoreInfoArray[i].lScore, ScoreInfoArray[i].lRevenue, afterScore[i]);
				CString strFileName = TEXT("������־");

                tagLogUserInfo LogUserInfo;
                ZeroMemory(&LogUserInfo, sizeof(LogUserInfo));
                CopyMemory(LogUserInfo.szDescName, strFileName, sizeof(LogUserInfo.szDescName));
                CopyMemory(LogUserInfo.szLogContent, strInfo, sizeof(LogUserInfo.szLogContent));
                //m_pITableFrame->SendGameLogData(LogUserInfo);
            }

        }
    }
    return true;
}

//����·�
SCORE CTableFrameSink::GetUserMaxTurnScore(WORD wChairID)
{
    SCORE lMaxTurnScore = 0L;
    if(wChairID == m_wBankerUser) { return 0; }
    //ׯ�һ���
    IServerUserItem *pIBankerItem = m_pITableFrame->GetTableUserItem(m_wBankerUser);
    LONGLONG lBankerScore = 0L;
    if(pIBankerItem != NULL)
    {
        lBankerScore = pIBankerItem->GetUserScore();
    }

    //�������
    WORD wUserCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
        if(m_cbPlayStatus[i] == TRUE) { wUserCount++; }

    //��ȡ�û�
    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);

    BYTE cbMaxCallBankerTimes = 1;
    if(m_bgtConfig == BGT_ROB_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == TRUE && m_cbCallBankerStatus[i] == TRUE && m_cbCallBankerTimes[i] > cbMaxCallBankerTimes)
            {
                cbMaxCallBankerTimes = m_cbCallBankerTimes[i];
            }
        }
    }

    //��ע����ֻ�ڻ��ַ����У���ҷ����ͽ�ҳ���֧��
    //ǰ�˻��ַ��������������ѡ��:����ע�ͷ�����ע
    //��һ�ֿ�����ע�� ���� 1. ������м� 2. ��һ��ӮǮ���м� 3.��һ��û����ע
    //ĳ�мҿ���ע���: �Ͼ�Ӯ���ܶ�ķ���Ϊ������ע�Ķ�ȣ������������������޴󣬲��ÿ��ǲ�����������
    //������������ע
    //��ע������5�ı�����������ע���100

    //���ַ���
	if (IsRoomCardType() && m_tyConfig == BT_TUI_DOUBLE_ && m_bgtConfig != BGT_TONGBI_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
            {
                continue;
            }

            if(m_bLastTurnBeBanker[i] == false && m_lLastTurnWinScore[i] > 0 && !m_bLastTurnBetBtEx[i])
            {
                m_lPlayerBetBtEx[i] = m_lLastTurnWinScore[i] * 2 / m_pITableFrame->GetCellScore();

                //��ע��5�ı���
                int nDiv = m_lPlayerBetBtEx[i] / 5;
                if(nDiv > 0)
                {
                    m_lPlayerBetBtEx[i] = nDiv * 5;
                }
                else
                {
                    m_lPlayerBetBtEx[i] = 0;
                }

                m_lPlayerBetBtEx[i] = (m_lPlayerBetBtEx[i] > 100 ? 100 : m_lPlayerBetBtEx[i]);
            }
        }
    }
    else
    {
        //������ע����
        ZeroMemory(m_bLastTurnBeBanker, sizeof(m_bLastTurnBeBanker));
        ZeroMemory(m_lLastTurnWinScore, sizeof(m_lLastTurnWinScore));
        ZeroMemory(m_bLastTurnBetBtEx, sizeof(m_bLastTurnBetBtEx));
        ZeroMemory(m_lPlayerBetBtEx, sizeof(m_lPlayerBetBtEx));
    }

    //����������עģʽ
    //��������ͨ��ҳ������ע��ǰ���ж�ĳ����ע�����Ƿ����
    if(pIServerUserItem != NULL && m_btConfig == BT_FREE_)
    {
        //��ȡ����
        LONGLONG lScore = pIServerUserItem->GetUserScore();
        lMaxTurnScore = lScore /*/ m_lMaxCardTimes / cbMaxCallBankerTimes / wUserCount*/;

        if(m_lPlayerBetBtEx[wChairID] != 0)
        {
            lMaxTurnScore = max(lMaxTurnScore, m_lPlayerBetBtEx[wChairID]);
        }

		//��С�����µ�һ������
		lMaxTurnScore = max(lMaxTurnScore, m_lFreeConfig[0] * m_pITableFrame->GetCellScore());

        return lMaxTurnScore;
    }

    return lMaxTurnScore;
}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem *pIServerUserItem)
{
    return 0L;
}

//�Ƿ�˥��(һ��ͬʱ�������˺ͻ����˲�˥��)
bool CTableFrameSink::NeedDeductStorage()
{
	BYTE cbRealPlayerCount = 0;
	BYTE cbAICount = 0;

    for(int i = 0; i < m_wPlayerCount; ++i)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL) 
		{ 
			continue; 
		}

        if(pIServerUserItem->IsAndroidUser())
        {
			cbAICount++;
        }
		else
		{
			cbRealPlayerCount++;
		}
    }

	return (cbRealPlayerCount > 0 && cbAICount > 0);
}

//��ȡ����
void CTableFrameSink::ReadConfigInformation()
{
    //��ȡ�Զ�������
    tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
    ASSERT(pCustomRule);

    g_lRoomStorageStart = pCustomRule->lRoomStorageStart;
    g_lRoomStorageCurrent = pCustomRule->lRoomStorageStart;
    g_lStorageDeductRoom = pCustomRule->lRoomStorageDeduct;
    g_lStorageMax1Room = pCustomRule->lRoomStorageMax1;
    g_lStorageMul1Room = pCustomRule->lRoomStorageMul1;
    g_lStorageMax2Room = pCustomRule->lRoomStorageMax2;
    g_lStorageMul2Room = pCustomRule->lRoomStorageMul2;

    if(g_lStorageDeductRoom < 0 || g_lStorageDeductRoom > 1000)
    {
        g_lStorageDeductRoom = 0;
    }
    if(g_lStorageDeductRoom > 1000)
    {
        g_lStorageDeductRoom = 1000;
    }
    if(g_lStorageMul1Room < 0 || g_lStorageMul1Room > 100)
    {
        g_lStorageMul1Room = 50;
    }
    if(g_lStorageMul2Room < 0 || g_lStorageMul2Room > 100)
    {
        g_lStorageMul2Room = 80;
    }

    m_ctConfig = pCustomRule->ctConfig;
    m_stConfig = pCustomRule->stConfig;
    m_gtConfig = pCustomRule->gtConfig;
    m_bgtConfig = pCustomRule->bgtConfig;
    m_btConfig = pCustomRule->btConfig;

    CopyMemory(m_lFreeConfig, pCustomRule->lFreeConfig, sizeof(m_lFreeConfig));
    CopyMemory(m_lPercentConfig, pCustomRule->lPercentConfig, sizeof(m_lPercentConfig));

    if(m_ctConfig == CT_ADDTIMES_)
    {
        //��Сţ���ͼ����ע
        m_cbEnableCardType[7] = pCustomRule->cbCardTypeTimesAddTimes[18] == 0 ? FALSE : TRUE;

        m_cbClassicTypeConfig = INVALID_BYTE;
    }
    else if(m_ctConfig == CT_CLASSIC_)
    {
        //��Сţ���ͼ����ע
        m_cbEnableCardType[7] = pCustomRule->cbCardTypeTimesClassic[18] == 0 ? FALSE : TRUE;

        if(pCustomRule->cbCardTypeTimesClassic[7] == 1 && pCustomRule->cbCardTypeTimesClassic[8] == 2
                && pCustomRule->cbCardTypeTimesClassic[9] == 2 && pCustomRule->cbCardTypeTimesClassic[10] == 3)
        {
            m_cbClassicTypeConfig = 1;
        }
        else if(pCustomRule->cbCardTypeTimesClassic[7] == 2 && pCustomRule->cbCardTypeTimesClassic[8] == 2
                && pCustomRule->cbCardTypeTimesClassic[9] == 3 && pCustomRule->cbCardTypeTimesClassic[10] == 4)
        {
            m_cbClassicTypeConfig = 0;
        }
    }

    m_cbTrusteeDelayTime = pCustomRule->cbTrusteeDelayTime;

    return;
}

//���·����û���Ϣ
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
    //��������
    ROOMUSERINFO roomUserInfo;
    ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

    roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
    CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
    roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
    roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

    roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

    //�û����º�����
    if(userAction == USER_SITDOWN || userAction == USER_RECONNECT)
    {
        roomUserInfo.wChairID = pIServerUserItem->GetChairID();
        roomUserInfo.wTableID = pIServerUserItem->GetTableID() + 1;
    }
    else if(userAction == USER_STANDUP || userAction == USER_OFFLINE)
    {
        roomUserInfo.wChairID = INVALID_CHAIR;
        roomUserInfo.wTableID = INVALID_TABLE;
    }

    g_MapRoomUserInfo.SetAt(pIServerUserItem->GetUserID(), roomUserInfo);

    //����ӳ�䣬ɾ���뿪�������ң�
    POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
    DWORD dwUserID = 0;
    ROOMUSERINFO userinfo;
    ZeroMemory(&userinfo, sizeof(userinfo));
    TCHAR szNickName[LEN_NICKNAME];
    ZeroMemory(szNickName, sizeof(szNickName));
    DWORD *pdwRemoveKey = new DWORD[g_MapRoomUserInfo.GetSize()];
    ZeroMemory(pdwRemoveKey, sizeof(DWORD)* g_MapRoomUserInfo.GetSize());
    WORD wRemoveKeyIndex = 0;

    while(ptHead)
    {
        g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);

        if(userinfo.dwGameID == 0 && (_tcscmp(szNickName, userinfo.szNickName) == 0) && userinfo.cbUserStatus == 0)
        {
            pdwRemoveKey[wRemoveKeyIndex++] = dwUserID;
        }

    }

    for(WORD i = 0; i < wRemoveKeyIndex; i++)
    {
        g_MapRoomUserInfo.RemoveKey(pdwRemoveKey[i]);
    }

    delete[] pdwRemoveKey;
}

//����ͬ���û�����
void CTableFrameSink::UpdateUserDebug(IServerUserItem *pIServerUserItem)
{
    //��������
    POSITION ptListHead;
    POSITION ptTemp;
    ROOMUSERDEBUG roomuserdebug;

    //��ʼ��
    ptListHead = g_ListRoomUserDebug.GetHeadPosition();
    ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

    //��������
    while(ptListHead)
    {
        ptTemp = ptListHead;
        roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

        //Ѱ���Ѵ��ڵ������
        if((pIServerUserItem->GetGameID() == roomuserdebug.roomUserInfo.dwGameID) &&
                _tcscmp(pIServerUserItem->GetNickName(), roomuserdebug.roomUserInfo.szNickName) == 0)
        {
            //��ȡԪ��
            ROOMUSERDEBUG &tmproomuserdebug = g_ListRoomUserDebug.GetAt(ptTemp);

            //�������
            tmproomuserdebug.roomUserInfo.wChairID = pIServerUserItem->GetChairID();
            tmproomuserdebug.roomUserInfo.wTableID = m_pITableFrame->GetTableID() + 1;

            return;
        }
    }
}

//�����û�����
void CTableFrameSink::TravelDebugList(ROOMUSERDEBUG keyroomuserdebug)
{
    //��������
    POSITION ptListHead;
    POSITION ptTemp;
    ROOMUSERDEBUG roomuserdebug;

    //��ʼ��
    ptListHead = g_ListRoomUserDebug.GetHeadPosition();
    ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

    //��������
    while(ptListHead)
    {
        ptTemp = ptListHead;
        roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

        //Ѱ���Ѵ��ڵ����������һ�������л�����
        if((keyroomuserdebug.roomUserInfo.dwGameID == roomuserdebug.roomUserInfo.dwGameID) &&
                _tcscmp(keyroomuserdebug.roomUserInfo.szNickName, roomuserdebug.roomUserInfo.szNickName) == 0)
        {
            g_ListRoomUserDebug.RemoveAt(ptTemp);
        }
    }
}

//�Ƿ������������
void CTableFrameSink::IsSatisfyDebug(ROOMUSERINFO &userInfo, bool &bEnableDebug)
{
    if(userInfo.wChairID == INVALID_CHAIR || userInfo.wTableID == INVALID_TABLE)
    {
        bEnableDebug = FALSE;
        return;
    }

    if(userInfo.cbUserStatus == US_SIT || userInfo.cbUserStatus == US_READY || userInfo.cbUserStatus == US_PLAYING)
    {
        bEnableDebug = TRUE;
        return;
    }
    else
    {
        bEnableDebug = FALSE;
        return;
    }
}

//���������û�����
bool CTableFrameSink::AnalyseRoomUserDebug(ROOMUSERDEBUG &Keyroomuserdebug, POSITION &ptList)
{
    //��������
    POSITION ptListHead;
    POSITION ptTemp;
    ROOMUSERDEBUG roomuserdebug;

    //��������
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(!pIServerUserItem)
        {
            continue;
        }

        //��ʼ��
        ptListHead = g_ListRoomUserDebug.GetHeadPosition();
        ZeroMemory(&roomuserdebug, sizeof(roomuserdebug));

        //��������
        while(ptListHead)
        {
            ptTemp = ptListHead;
            roomuserdebug = g_ListRoomUserDebug.GetNext(ptListHead);

            //Ѱ�����
            if((pIServerUserItem->GetGameID() == roomuserdebug.roomUserInfo.dwGameID) &&
                    _tcscmp(pIServerUserItem->GetNickName(), roomuserdebug.roomUserInfo.szNickName) == 0)
            {
                //��յ��Ծ���Ϊ0��Ԫ��
                if(roomuserdebug.userDebug.cbDebugCount == 0)
                {
                    g_ListRoomUserDebug.RemoveAt(ptTemp);
                    break;
                }

                if(roomuserdebug.userDebug.debug_type == CONTINUE_CANCEL)
                {
                    g_ListRoomUserDebug.RemoveAt(ptTemp);
                    break;
                }

                //��������
                CopyMemory(&Keyroomuserdebug, &roomuserdebug, sizeof(roomuserdebug));
                ptList = ptTemp;

                return true;
            }

        }

    }

    return false;
}

void CTableFrameSink::GetDebugTypeString(DEBUG_TYPE &debugType, CString &debugTypestr)
{
    switch(debugType)
    {
    case CONTINUE_WIN:
    {
        debugTypestr = TEXT("��������Ϊ��Ӯ");
        break;
    }
    case CONTINUE_LOST:
    {
        debugTypestr = TEXT("��������Ϊ����");
        break;
    }
    case CONTINUE_CANCEL:
    {
        debugTypestr = TEXT("��������Ϊȡ������");
        break;
    }
    }
}

//�жϷ�������
bool CTableFrameSink::IsRoomCardType()
{
    //��ҳ��ͽ�ҷ��������йܣ����ַ������й�
    return (((m_pGameServiceOption->wServerType) & GAME_GENRE_PERSONAL) != 0 && m_pITableFrame->GetDataBaseMode() == 0);
}

//��ʼĬ��ׯ��
void CTableFrameSink::InitialBanker()
{
    //����ǿ��Ϊׯ����������������Ϸ�����һ����ȥ����Ϸ�����ǿ��Ϊׯ
    //��ȡ����
    WORD wRoomOwenrChairID = INVALID_CHAIR;
    DWORD dwRoomOwenrUserID = INVALID_DWORD;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(!pIServerUserItem)
        {
            continue;
        }

        m_cbCallBankerStatus[i] = TRUE;

        if(pIServerUserItem->GetUserID() == m_pITableFrame->GetTableOwner() && IsRoomCardType())
        {
            dwRoomOwenrUserID = pIServerUserItem->GetUserID();
            wRoomOwenrChairID = pIServerUserItem->GetChairID();
            //break;
        }
    }

    //����������Ϸ
    if(dwRoomOwenrUserID != INVALID_DWORD && wRoomOwenrChairID != INVALID_CHAIR)
    {
        m_wBankerUser = wRoomOwenrChairID;
    }
    //������������Ϸ�ͷǷ�������
    else
    {
        ASSERT(m_listEnterUser.IsEmpty() == false);
        m_wBankerUser = m_listEnterUser.GetHead();
    }

    ASSERT(m_wBankerUser != INVALID_CHAIR);

    m_bBuckleServiceCharge[m_wBankerUser] = true;

    bool bTrusteeUser = false;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
        {
            bTrusteeUser = true;
        }
    }

    //�Ƿ��������ö�ʱ��
    if(!IsRoomCardType()/* || bTrusteeUser*/)
    {
        m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
        m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, TIME_SO_OPERATE, 1, 0);
    }

    //�������ߴ���ʱ��
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] == TRUE && m_pITableFrame->GetTableUserItem(i)->IsTrusteeUser())
        {
            m_pITableFrame->SetGameTimer(IDI_OFFLINE_TRUSTEE_0 + i, m_cbTrusteeDelayTime * 1000, 1, 0);
        }
    }

    //����״̬
    m_pITableFrame->SetGameStatus(GS_TK_SCORE);
    EnableTimeElapse(true);

    //���·����û���Ϣ
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem != NULL)
        {
            UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
        }
    }

    //��ȡ�����ע
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(m_cbPlayStatus[i] != TRUE || i == m_wBankerUser)
        {
            continue;
        }

        //��ע����
        m_lTurnMaxScore[i] = GetUserMaxTurnScore(i);
    }

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        if(i == m_wBankerUser || m_cbPlayStatus[i] == FALSE)
        {
            continue;
        }

        if(m_bLastTurnBetBtEx[i] == true)
        {
            m_bLastTurnBetBtEx[i] = false;
        }
    }

    m_lPlayerBetBtEx[m_wBankerUser] = 0;

    //���ñ���
    CMD_S_GameStart GameStart;
    ZeroMemory(&GameStart, sizeof(GameStart));
    GameStart.wBankerUser = m_wBankerUser;
    CopyMemory(GameStart.cbPlayerStatus, m_cbPlayStatus, sizeof(m_cbPlayStatus));

    //���ĵ���
    if(m_stConfig == ST_SENDFOUR_)
    {
        for(WORD i = 0; i < m_wPlayerCount; i++)
        {
            if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
            {
                continue;
            }

            //�ɷ��˿�(��ʼֻ��������)
            CopyMemory(GameStart.cbCardData[i], m_cbHandCardData[i], sizeof(BYTE) * 4);
        }
    }

    GameStart.stConfig = m_stConfig;
    GameStart.bgtConfig = m_bgtConfig;
    GameStart.btConfig = m_btConfig;
    GameStart.gtConfig = m_gtConfig;

    CopyMemory(GameStart.lFreeConfig, m_lFreeConfig, sizeof(GameStart.lFreeConfig));
    CopyMemory(GameStart.lPercentConfig, m_lPercentConfig, sizeof(GameStart.lPercentConfig));
    CopyMemory(GameStart.lPlayerBetBtEx, m_lPlayerBetBtEx, sizeof(GameStart.lPlayerBetBtEx));

    bool bFirstRecord = true;

    WORD wRealPlayerCount = 0;
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }

        if(!pServerUserItem)
        {
            continue;
        }

        wRealPlayerCount++;
    }

    BYTE *pGameRule = m_pITableFrame->GetGameRule();

    //�����ע
    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(m_cbPlayStatus[i] == FALSE && m_cbDynamicJoin[i] == FALSE)
        {
            continue;
        }
        GameStart.lTurnMaxScore = m_lTurnMaxScore[i];
        m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

        if(m_pGameVideo)
        {
            Video_GameStart video;
            ZeroMemory(&video, sizeof(video));
            video.lCellScore = m_pITableFrame->GetCellScore();
            video.wPlayerCount = wRealPlayerCount;
            video.wGamePlayerCountRule = pGameRule[1];
            video.wBankerUser = GameStart.wBankerUser;
            CopyMemory(video.cbPlayerStatus, GameStart.cbPlayerStatus, sizeof(video.cbPlayerStatus));
            video.lTurnMaxScore = GameStart.lTurnMaxScore;
            CopyMemory(video.cbCardData, GameStart.cbCardData, sizeof(video.cbCardData));
            video.ctConfig = m_ctConfig;
            video.stConfig = GameStart.stConfig;
            video.bgtConfig = GameStart.bgtConfig;
            video.btConfig = GameStart.btConfig;
            video.gtConfig = GameStart.gtConfig;

            CopyMemory(video.lFreeConfig, GameStart.lFreeConfig, sizeof(video.lFreeConfig));
            CopyMemory(video.lPercentConfig, GameStart.lPercentConfig, sizeof(video.lPercentConfig));
            CopyMemory(video.szNickName, pServerUserItem->GetNickName(), sizeof(video.szNickName));
            video.wChairID = i;
            video.lScore = pServerUserItem->GetUserScore();

            m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, bFirstRecord);

            if(bFirstRecord == true)
            {
                bFirstRecord = false;
            }
        }
    }
    m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
}

//�����ض��˿����
WORD CTableFrameSink::SearchKeyCardChairID(BYTE cbKeyCardData[MAX_CARDCOUNT])
{
    WORD wKeyChairID = INVALID_CHAIR;

    for(WORD i = 0; i < m_wPlayerCount; i++)
    {
        //��ȡ�û�
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(pIServerUserItem == NULL)
        {
            continue;
        }

        if(cbKeyCardData[0] == m_cbHandCardData[i][0] && cbKeyCardData[1] == m_cbHandCardData[i][1]
                && cbKeyCardData[2] == m_cbHandCardData[i][2] && cbKeyCardData[3] == m_cbHandCardData[i][3]
                && cbKeyCardData[4] == m_cbHandCardData[i][4])
        {
            wKeyChairID = i;
            break;
        }
    }

    return wKeyChairID;
}

//����ʱ��
void CTableFrameSink::EnableTimeElapse(bool bEnable)
{
    if(bEnable)
    {
        m_pITableFrame->KillGameTimer(IDI_TIME_ELAPSE);
        m_cbTimeRemain = TIME_SO_OPERATE / 1000;
        m_pITableFrame->SetGameTimer(IDI_TIME_ELAPSE, 1000, -1, 0);
    }
    else
    {
        m_pITableFrame->KillGameTimer(IDI_TIME_ELAPSE);
        m_cbTimeRemain = TIME_SO_OPERATE / 1000;
    }
}

//����д��Ϣ
void CTableFrameSink::WriteInfo(LPCTSTR pszFileName, LPCTSTR pszString)
{
	//������������
	char *old_locale = _strdup(setlocale(LC_CTYPE, NULL));
	setlocale(LC_CTYPE, "chs");

	CStdioFile myFile;
	CString strFileName;
	strFileName.Format(TEXT("%s"), pszFileName);
	BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if (bOpen)
	{
		myFile.SeekToEnd();
		myFile.WriteString(pszString);
		myFile.Flush();
		myFile.Close();
	}

	//��ԭ�����趨
	setlocale(LC_CTYPE, old_locale);
	free(old_locale);
}

//////////////////////////////////////////////////////////////////////////
