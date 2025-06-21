#ifndef CMD_OXSIXX_HEAD_FILE
#define CMD_OXSIXX_HEAD_FILE

#pragma pack(push)  
#pragma pack(1)

#define		CARD_CONFIG

//////////////////////////////////////////////////////////////////////////
//�����궨��
#define KIND_ID							57									//��Ϸ I D
#define GAME_PLAYER						8									//��Ϸ����
#define GAME_NAME						TEXT("����ţţ")					//��Ϸ����
#define MAX_CARDCOUNT					5									//�����Ŀ
#define MAX_CONFIG						5									//������ø���
#define MAX_RECORD_COUNT				30									//��������ͳ��������
#define MAX_CARD_TYPE					19									//�������
#define MAX_SPECIAL_CARD_TYPE			9									//����������� �������廨ţ��˳��ͬ����«ը��ͬ��˳��Сţ ���޴�С��
#define MAX_BANKERMODE					6									//���ׯ��ģʽ ���ΰ���ׯ��������ׯ��ţţ��ׯ����ţ��ׯ��������ׯ��ͨ���淨
#define MAX_GAMEMODE					2									//�����Ϸģʽ ���ξ���ģʽ�����ģʽ
#define MAX_BEBANKERCON					4									//�����ׯ����	Ĭ���ޣ�100��150��200
#define MAX_USERBETTIMES				4									//����м���ע����	Ĭ���ޣ�5��10��20
#define MAX_ADVANCECONFIG				5									//���߼�ѡ��  ���η��ĵ��壬 ��Ҫ���룬 ��ʼ��ֹ���룬 ��ֹ���ƣ��������ߴ���

#define VERSION_SERVER					PROCESS_VERSION(7,0,1)				//����汾
#define VERSION_CLIENT					PROCESS_VERSION(7,0,1)				//����汾

//��Ϸ״̬
#define GS_TK_FREE						GAME_STATUS_FREE					//�ȴ���ʼ
#define GS_TK_CALL						GAME_STATUS_PLAY					//��ׯ״̬
#define GS_TK_SCORE						GAME_STATUS_PLAY+1					//��ע״̬
#define GS_TK_PLAYING					GAME_STATUS_PLAY+2					//��Ϸ����

//������Ϣ
#define IDM_ADMIN_UPDATE_STORAGE		WM_USER+1001
#define IDM_ADMIN_MODIFY_STORAGE		WM_USER+1011
#define IDM_REQUEST_QUERY_USER			WM_USER+1012
#define IDM_USER_DEBUG				WM_USER+1013
#define IDM_REQUEST_UPDATE_ROOMINFO		WM_USER+1014
#define IDM_CLEAR_CURRENT_QUERYUSER		WM_USER+1015

//������¼
#define MAX_OPERATION_RECORD			20									//������¼����
#define RECORD_LENGTH					128									//ÿ����¼�ֳ�

#define INVALID_LONGLONG				((LONGLONG)(0xFFFFFFFF))			//��Ч��ֵ

//��Ϸ��¼
struct CMD_S_RECORD
{
	int									nCount;
	LONGLONG							lUserWinCount[GAME_PLAYER];						//���ʤ������
	LONGLONG							lUserLostCount[GAME_PLAYER];					//���ʧ�ܴ���
};

//-------------------------------------------
//��Ϸ����
typedef enum
{
	CT_CLASSIC_ = 22,						//����ģʽ
	CT_ADDTIMES_ = 23,						//���ӱ�
	CT_INVALID_ = 255,						//��Ч
}CARDTYPE_CONFIG;

//����ģʽ
typedef enum
{
	ST_SENDFOUR_ = 32,						//���ĵ���
	ST_BETFIRST_ = 33,						//��ע����
	ST_INVALID_ = 255,						//��Ч
}SENDCARDTYPE_CONFIG;

//�˿��淨
typedef enum
{
	GT_HAVEKING_ = 42,						//�д�С��
	GT_NOKING_ = 43,						//�޴�С��
	GT_INVALID_ = 255,						//��Ч
}KING_CONFIG;

//ׯ���淨
typedef enum
{
	BGT_DESPOT_ = 52,						//����ׯ
	BGT_ROB_ = 53,							//������ׯ
	BGT_NIUNIU_ = 54,						//ţţ��ׯ
	BGT_NONIUNIU_ = 55,						//��ţ��ׯ

	BGT_FREEBANKER_ = 56,					//������ׯ
	BGT_TONGBI_ = 57,						//ͨ���淨

	BGT_INVALID_ = 255,						//��Ч
}BANERGAMETYPE_CONFIG;

//��ע����
typedef enum
{
	BT_FREE_ = 62,							//�������ö��
	BT_PENCENT_ = 63,						//�ٷֱ����ö��
	BT_INVALID_ = 255,						//��Ч
}BETTYPE_CONFIG;

//��ע����(ֻ�ڻ��ַ�����Ч)
typedef enum
{
	BT_TUI_NONE_ = 72,						//����ע
	BT_TUI_DOUBLE_ = 73,					//������ע(�Ͼ�Ӯ���ܶ�ķ���Ϊ������ע�Ķ��)
	BT_TUI_INVALID_ = 255,					//��Ч
}TUITYPE_CONFIG;

//-------------------------------------------

//////////////////////////////////////////////////////////////////////////
//����������ṹ
#define SUB_S_GAME_START				100									//��Ϸ��ʼ
#define SUB_S_ADD_SCORE					101									//��ע���
#define SUB_S_PLAYER_EXIT				102									//�û�ǿ��
#define SUB_S_SEND_CARD					103									//������Ϣ
#define SUB_S_GAME_END					104									//��Ϸ����
#define SUB_S_OPEN_CARD					105									//�û�̯��
#define SUB_S_CALL_BANKER				106									//�û���ׯ
#define SUB_S_CALL_BANKERINFO			107									//�û���ׯ��Ϣ
#define SUB_S_ADMIN_STORAGE_INFO		112									//ˢ�µ��Է����
#define SUB_S_RECORD					113									//��Ϸ��¼
#define SUB_S_ROOMCARD_RECORD			114									//������¼

#define SUB_S_REQUEST_QUERY_RESULT		115									//��ѯ�û����
#define SUB_S_USER_DEBUG				116									//�û�����
#define SUB_S_USER_DEBUG_COMPLETE		117									//�û��������
#define SUB_S_OPERATION_RECORD		    118									//������¼
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 119
#define SUB_S_SEND_FOUR_CARD			120
#define SUB_S_ANDROID_BANKOPER			122									//AI�洢�����
#define SUB_S_ANDROID_READY				124									//AI׼��

//////////////////////////////////////////////////////////////////////////////////////

//Ԥ��AI���ȡ��
struct tagCustomAndroid
{
	SCORE								lRobotScoreMin;	
	SCORE								lRobotScoreMax;
	SCORE	                            lRobotBankGet; 
	SCORE								lRobotBankGetBanker; 
	SCORE								lRobotBankStoMul; 
};

//��������
typedef enum{CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL}DEBUG_TYPE;

//���Խ��      ���Գɹ� ������ʧ�� ������ȡ���ɹ� ������ȡ����Ч
typedef enum{DEBUG_SUCCEED, DEBUG_FAIL, DEBUG_CANCEL_SUCCEED, DEBUG_CANCEL_INVALID}DEBUG_RESULT;

//�û���Ϊ
typedef enum{USER_SITDOWN = 11, USER_STANDUP, USER_OFFLINE, USER_RECONNECT}USERACTION;

//������Ϣ
typedef struct
{
	DEBUG_TYPE						debug_type;					  //��������
	BYTE								cbDebugCount;					  //���Ծ���
	bool							    bCancelDebug;					  //ȡ����ʶ
}USERDEBUG;

//�����û���Ϣ
typedef struct
{
	WORD								wChairID;							//����ID
	WORD								wTableID;							//����ID
	DWORD								dwGameID;							//GAMEID
	bool								bAndroid;							//AI��ʶ
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	BYTE								cbUserStatus;						//�û�״̬
	BYTE								cbGameStatus;						//��Ϸ״̬
}ROOMUSERINFO;

//�����û�����
typedef struct
{
	ROOMUSERINFO						roomUserInfo;						//�����û���Ϣ
	USERDEBUG							userDebug;						//�û�����
}ROOMUSERDEBUG;

//////////////////////////////////////////////////////////////////////////////////////

//��Ϸ״̬
struct CMD_S_StatusFree
{
	LONGLONG							lCellScore;							//��������
	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//AI����

	bool								bIsAllowAvertDebug;					//�����Ա�־
	
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	BYTE								cbAdmitRevCard;						//������� TRUEΪ����

	BYTE								cbPlayMode;							//Լս��Ϸģʽ
};

//��ׯ״̬
struct CMD_S_StatusCall
{
	LONGLONG							lCellScore;							//��������
	BYTE                                cbDynamicJoin;                      //��̬���� 
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//AI����

	bool								bIsAllowAvertDebug;					//�����Ա�־
	
	BYTE								cbCallBankerStatus[GAME_PLAYER];	//��ׯ״̬
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//��ׯ����
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨 (������ׯĬ�϶���1��)
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	BYTE								cbAdmitRevCard;						//������� TRUEΪ����
	BYTE								cbMaxCallBankerTimes;				//�����ׯ����
	WORD								wBgtRobNewTurnChairID;				//������ׯ�¿�һ����ׯ����ң���ЧΪINVALID_CHAIR�� ����Чʱ��ֻ��������ѡ���������Ҹ������ׯ�ң�

	BYTE								cbPlayMode;							//Լս��Ϸģʽ
	BYTE								cbTimeRemain;						//����ʣ������
};

//��ע״̬
struct CMD_S_StatusScore
{
	LONGLONG							lCellScore;							//��������
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
	BYTE                                cbDynamicJoin;                      //��̬����
	LONGLONG							lTurnMaxScore;						//�����ע
	LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	WORD								wBankerUser;						//ׯ���û�

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//AI����

	bool								bIsAllowAvertDebug;					//�����Ա�־

	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//��ׯ����
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	LONG								lFreeConfig[MAX_CONFIG];			//�������ö��(��Чֵ0)
	LONG								lPercentConfig[MAX_CONFIG];			//�ٷֱ����ö��(��Чֵ0)
	LONG								lPlayerBetBtEx[GAME_PLAYER];		//�мҶ������ע����

	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	BYTE								cbAdmitRevCard;						//������� TRUEΪ����

	BYTE								cbPlayMode;							//Լս��Ϸģʽ
	BYTE								cbTimeRemain;						//����ʣ������
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	LONGLONG							lCellScore;							//��������
	BYTE                                cbPlayStatus[GAME_PLAYER];          //�û�״̬
	BYTE                                cbDynamicJoin;                      //��̬����
	LONGLONG							lTurnMaxScore;						//�����ע
	LONGLONG							lTableScore[GAME_PLAYER];			//��ע��Ŀ
	WORD								wBankerUser;						//ׯ���û�

	LONGLONG							lRoomStorageStart;					//������ʼ���
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���

	//�˿���Ϣ
	BYTE								cbHandCardData[GAME_PLAYER][MAX_CARDCOUNT];//�����˿�
	bool								bOpenCard[GAME_PLAYER];				//���Ʊ�ʶ
	bool								bSpecialCard[GAME_PLAYER];			//�������ͱ�־ ���Ļ�ţ���廨ţ��˳�ӣ�ͬ������«��ը����ͬ��˳����Сţ��
	BYTE								cbOriginalCardType[GAME_PLAYER];	//���ԭʼ���ͣ�û�о��������ϵ����ͣ�
	BYTE								cbCombineCardType[GAME_PLAYER];     //���������ͣ����������ϵ����ͣ�

	//��ʷ����
	LONGLONG							lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG							lCollectScore[GAME_PLAYER];			//������Ϣ
	tagCustomAndroid					CustomAndroid;						//AI����
	bool								bIsAllowAvertDebug;					//�����Ա�־
	
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//��ׯ����
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	LONG								lFreeConfig[MAX_CONFIG];			//�������ö��(��Чֵ0)
	LONG								lPercentConfig[MAX_CONFIG];			//�ٷֱ����ö��(��Чֵ0)
	LONG								lPlayerBetBtEx[GAME_PLAYER];		//�мҶ������ע����

	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	BYTE								cbAdmitRevCard;						//������� TRUEΪ����

	BYTE								cbPlayMode;							//Լս��Ϸģʽ
	BYTE								cbTimeRemain;						//����ʣ������

	bool								bDelayFreeDynamicJoin;
};

//�û���ׯ��Ϣ
struct CMD_S_CallBankerInfo
{
	BYTE								cbCallBankerStatus[GAME_PLAYER];	//��ׯ״̬
	BYTE								cbCallBankerTimes[GAME_PLAYER];		//��ׯ����(���û�����ׯ����ֵ0)������ׯĬ�϶���1��
};

//��4����
struct CMD_S_SendFourCard
{
	//��ǰ��4��
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD								wBankerUser;						//ׯ���û�
	BYTE								cbPlayerStatus[GAME_PLAYER];		//���״̬
	LONGLONG							lTurnMaxScore;						//�����ע

	//(����ģʽ���Ϊ���ĵ��壬�������ƣ� ����ȫΪ0)
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	LONG								lFreeConfig[MAX_CONFIG];			//�������ö��(��Чֵ0)
	LONG								lPercentConfig[MAX_CONFIG];			//�ٷֱ����ö��(��Чֵ0)
	LONG								lPlayerBetBtEx[GAME_PLAYER];		//�мҶ������ע����
};

//�û���ע
struct CMD_S_AddScore
{
	WORD								wAddScoreUser;						//��ע�û�
	LONGLONG							lAddScoreCount;						//��ע��Ŀ
};

//��Ϸ����
struct CMD_S_GameEnd
{
	LONGLONG							lGameTax[GAME_PLAYER];				//��Ϸ˰��
	LONGLONG							lGameScore[GAME_PLAYER];			//��Ϸ�÷�
	BYTE								cbHandCardData[GAME_PLAYER][MAX_CARDCOUNT];//�����˿�
	BYTE								cbCardType[GAME_PLAYER];			//�������
	WORD								wCardTypeTimes[GAME_PLAYER];		//���ͱ���
	BYTE								cbDelayOverGame;
	BYTE								cbLastSingleCardData[GAME_PLAYER];	//ԭʼ���һ����
};

//�������ݰ�
struct CMD_S_SendCard
{
	//(��ȫ��5���ƣ��������ģʽ�Ƿ��ĵ��壬��ǰ�����ź�CMD_S_GameStart��Ϣһ��) ͨ���淨�յ�����ϢҪĬ���µ�ע
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
	bool								bSpecialCard[GAME_PLAYER];				//�������ͱ�־ ���Ļ�ţ���廨ţ��˳�ӣ�ͬ������«��ը����ͬ��˳����Сţ��
	BYTE								cbOriginalCardType[GAME_PLAYER];		//���ԭʼ���ͣ�û�о��������ϵ����ͣ�
};

//�û��˳�
struct CMD_S_PlayerExit
{
	WORD								wPlayerID;							//�˳��û�
};

//�û�̯��
struct CMD_S_Open_Card
{
	WORD								wOpenChairID;						//̯���û�
	BYTE								bOpenCard;							//̯�Ʊ�־
};

struct CMD_S_RequestQueryResult
{
	ROOMUSERINFO						userinfo;							//�û���Ϣ
	bool								bFind;								//�ҵ���ʶ
};

//�û�����
struct CMD_S_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	DEBUG_RESULT						debugResult;						//���Խ��
	DEBUG_TYPE						debugType;						//��������
	BYTE								cbDebugCount;						//���Ծ���
};

//�û�����
struct CMD_S_UserDebugComplete
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	DEBUG_TYPE						debugType;						//��������
	BYTE								cbRemainDebugCount;				//ʣ����Ծ���
};

//���Է���˿����Ϣ
struct CMD_S_ADMIN_STORAGE_INFO
{
	LONGLONG							lRoomStorageStart;						//������ʼ���
	LONGLONG							lRoomStorageCurrent;
	LONGLONG							lRoomStorageDeduct;
	LONGLONG							lMaxRoomStorage[2];
	WORD								wRoomStorageMul[2];
};

//������¼
struct CMD_S_Operation_Record
{
	TCHAR								szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];					//��¼���²�����20����¼
};

//������½��
struct CMD_S_RequestUpdateRoomInfo_Result
{
	LONGLONG							lRoomStorageCurrent;				//���䵱ǰ���
	ROOMUSERINFO						currentqueryuserinfo;				//��ǰ��ѯ�û���Ϣ
	bool								bExistDebug;						//��ѯ�û����ڵ��Ա�ʶ
	USERDEBUG							currentuserdebug;
};

//¼������
struct Video_GameStart
{
	LONGLONG							lCellScore;							//��������
	WORD								wPlayerCount;						//��ʵ��������
	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	WORD								wBankerUser;						//ׯ���û�
	BYTE								cbPlayerStatus[GAME_PLAYER];		//���״̬
	LONGLONG							lTurnMaxScore;						//�����ע

	//(����ģʽ���Ϊ���ĵ��壬�������ƣ� ����ȫΪ0)
	BYTE								cbCardData[GAME_PLAYER][MAX_CARDCOUNT];	//�û��˿�
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BETTYPE_CONFIG						btConfig;							//��ע����
	KING_CONFIG							gtConfig;							//���޴�С��

	DWORD								lFreeConfig[MAX_CONFIG];			//�������ö��(��Чֵ0)
	DWORD								lPercentConfig[MAX_CONFIG];			//�ٷֱ����ö��(��Чֵ0)
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�		
	WORD								wChairID;							//����ID
	LONGLONG							lScore;								//����
};

//������¼
struct CMD_S_RoomCardRecord
{
	WORD							nCount;											//����
	LONGLONG						lDetailScore[GAME_PLAYER][MAX_RECORD_COUNT];	//���ֽ����
	BYTE                            cbPlayStatus[GAME_PLAYER];						//�û�״̬
};

////////���ڻط�
//�û���ׯ
struct CMD_S_CallBanker
{
	WORD								wGamePlayerCountRule;				//2-6��Ϊ0����������������ֵΪ����
	CARDTYPE_CONFIG						ctConfig;							//��Ϸ����
	SENDCARDTYPE_CONFIG					stConfig;							//����ģʽ
	BANERGAMETYPE_CONFIG				bgtConfig;							//ׯ���淨
	BYTE								cbMaxCallBankerTimes;				//�����ׯ����������ׯĬ�϶���1��
	WORD								wBgtRobNewTurnChairID;				//������ׯ�¿�һ����ׯ����ң���ЧΪINVALID_CHAIR�� ����Чʱ��ֻ��������ѡ���������Ҹ������ׯ�ң�
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ
#define SUB_C_CALL_BANKER				1									//�û���ׯ
#define SUB_C_ADD_SCORE					2									//�û���ע
#define SUB_C_OPEN_CARD					3									//�û�̯��
#define SUB_C_STORAGE					6									//���¿��
#define	SUB_C_STORAGEMAXMUL				7									//��������

#ifdef CARD_CONFIG
#define SUB_C_CARD_CONFIG				10									//����
#endif

#define SUB_C_REQUEST_RCRecord			12									//��ѯ������¼

#define SUB_C_REQUEST_QUERY_USER		13									//�����ѯ�û�
#define SUB_C_USER_DEBUG				14									//�û�����

//�����������
#define SUB_C_REQUEST_UDPATE_ROOMINFO	15									//������·�����Ϣ
#define SUB_C_CLEAR_CURRENT_QUERYUSER	16


//�û���ׯ
struct CMD_C_CallBanker
{
	bool								bBanker;							//��ׯ��־
	BYTE								cbBankerTimes;						//��ׯ����(���û�����ׯ����ֵ0)
};

//�û���ע
struct CMD_C_AddScore
{
	LONGLONG							lScore;								//��ע��Ŀ
};

//�û�̯��
struct CMD_C_OpenCard
{	
	//������ϵ��ƣ���ǰ�����ܷ�ճ�10���
	BYTE								cbCombineCardData[MAX_CARDCOUNT];	//�������˿�
};

struct CMD_C_UpdateStorage
{
	LONGLONG							lRoomStorageCurrent;				//�����ֵ
	LONGLONG							lRoomStorageDeduct;					//�����ֵ
};

struct CMD_C_ModifyStorage
{
	LONGLONG							lMaxRoomStorage[2];					//�������
	WORD								wRoomStorageMul[2];					//Ӯ�ָ���
};

#ifdef CARD_CONFIG
struct CMD_C_CardConfig
{
	BYTE								cbconfigCard[GAME_PLAYER][MAX_CARDCOUNT];	//�����˿�
};
#endif

struct CMD_C_RequestQuery_User
{
	DWORD								dwGameID;								//��ѯ�û�GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			    //��ѯ�û��ǳ�
};

//�û�����
struct CMD_C_UserDebug
{
	DWORD								dwGameID;							//GAMEID
	TCHAR								szNickName[LEN_NICKNAME];			//�û��ǳ�
	USERDEBUG							userDebugInfo;					//
};

#pragma pack(pop)

#endif
