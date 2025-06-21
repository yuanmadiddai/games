
----------------------------------------------------------------------------------------------------

USE RYPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadTaskList]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadTaskList]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_QueryTaskInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_QueryTaskInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskTake]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskTake]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskForward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskForward]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskReward]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_TaskGiveUp]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_TaskGiveUp]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_LoadTaskList
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM TaskInfo	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GR_QueryTaskInfo
	@wKindID INT,								-- ���� I D
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- ��ѯ�û�
	IF not exists(SELECT * FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END

	-- ��ʱ����
	UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask SET TaskStatus=2 
	WHERE UserID=@dwUserID AND TaskStatus=0 AND TimeLimit<DateDiff(s,InputDate,GetDate())

	-- ��ѯ����
	SELECT TaskID,TaskStatus,Progress,(TimeLimit-DateDiff(ss,InputDate,GetDate())) AS ResidueTime FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND DateDiff(d,InputDate,GetDate())=0 AND (((@wKindID=KindID) AND TaskStatus=0) OR @wKindID=0) 		
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------

-- ��ȡ����
CREATE PROC GSP_GR_TaskTake
	@dwUserID INT,								-- �û� I D
	@wTaskID  INT,								-- ���� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @MemberOrder INT
	SELECT @MemberOrder=MemberOrder FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID AND LogonPass=@strPassword
	IF @MemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END

	-- �ظ���ȡ
	IF exists(SELECT * FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND TaskID=@wTaskID AND DateDiff(d,InputDate,GetDate())=0) 
	BEGIN
		SET @strErrorDescribe = N'��Ǹ��ͬһ������ÿ��ֻ����ȡһ�Σ�'
		RETURN 3		
	END

	-- �ж���Ŀ	
	DECLARE @TaskTakeMaxCount AS INT
	SELECT @TaskTakeMaxCount=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'TaskTakeCount'	

	-- ������Ŀ
	IF @TaskTakeMaxCount IS NULL SET @TaskTakeMaxCount=5

	-- ͳ������
	DECLARE @TaskTakeCount AS INT
	SELECT @TaskTakeCount=Count(*) FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND DateDiff(d,InputDate,GetDate())=0 

	-- ��������
	IF @TaskTakeCount>=@TaskTakeMaxCount	
	BEGIN
		SET @strErrorDescribe = N'��Ǹ��������ȡ��������Ŀ�Ѵﵽϵͳ���õ�ÿ����ȡ���ޣ�������������ȡ��'				
		RETURN 3
	END

	-- �������
	DECLARE @KindID INT
	DECLARE @UserType INT
	DECLARE @TimeLimit INT
	DECLARE @TaskType INT
	DECLARE @TaskObject INT	
	SELECT @KindID=KindID,@UserType=UserType,@TimeLimit=TimeLimit,@TaskType=TaskType,@TaskObject=Innings
	FROM TaskInfo WHERE TaskID=@wTaskID
	IF @KindID IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ��ϵͳδ�ҵ�����ȡ��������Ϣ��'
		RETURN 4			
	END

	-- ��ͨ���
	IF @MemberOrder=0 AND (@UserType&0x01)=0
	BEGIN
		SET @strErrorDescribe = N'��Ǹ����������ʱ������ͨ��ҿ��ţ�'
		RETURN 5			
	END

	-- ��Ա���
	IF @MemberOrder>0 AND (@UserType&0x02)=0
	BEGIN
		SET @strErrorDescribe = N'��Ǹ����������ʱ���Ի�Ա��ҿ��ţ�'
		RETURN 6			
	END	

	-- ��������
	INSERT INTO RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask(UserID,TaskID,TaskType,TaskObject,KindID,TimeLimit) 
	VALUES(@dwUserID,@wTaskID,@TaskType,@TaskObject,@KindID,@TimeLimit)	

	-- �ɹ���ʾ
	SET @strErrorDescribe = N'��ϲ����������ȡ�ɹ���'	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �����ƽ�
CREATE PROC GSP_GR_TaskForward
	@dwUserID INT,								-- �û� I D
	@wKindID INT,								-- ��Ϸ��ʶ
	@wMatchID INT,								-- ������ʶ
	@lWinCount INT,								-- Ӯ�־���
	@lLostCount INT,							-- ��־���
	@lDrawCount	INT								-- �;־���
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����	
	SELECT ID,TaskType,TaskObject,TimeLimit,TaskStatus,InputDate,(case 
									  when TaskType=0x01 then Progress+@lWinCount
									  when TaskType=0x02 then Progress+@lWinCount+@lLostCount+@lDrawCount
									  when TaskType=0x04 then @lWinCount
									  when TaskType=0x08 then Progress+@lWinCount
									  else Progress end) AS NewProgress
									  ,(case 
									  when TaskType=0x08 then @lLostCount+@lDrawCount
									  else 0 end) AS OtherProgress
	INTO #TempTaskInfo FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND KindID=@wKindID AND TaskStatus=0 AND DateDiff(d,InputDate,GetDate())=0

	-- ����״̬�����������ʤδʤ����ʱ��
	UPDATE #TempTaskInfo SET TaskStatus=(case										 
										 when TaskType=0x04 AND NewProgress=0 then 2
										 
										 when TimeLimit<DateDiff(s,InputDate,GetDate()) then 2
										 when NewProgress>=TaskObject then 1
										 else 0 end)
	-- ���������
	UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask SET a.Progress=b.NewProgress,a.TaskStatus=b.TaskStatus 
	FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask AS a,#TempTaskInfo AS b WHERE a.ID=b.ID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ȡ����
CREATE PROC GSP_GR_TaskReward
	@dwUserID INT,								-- �û� I D
	@wTaskID  INT,								-- ���� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @MemberOrder INT
	SELECT @MemberOrder=MemberOrder FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID AND LogonPass=@strPassword
	IF @MemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END

	-- ��ѯ����
	DECLARE @TaskInputDate DATETIME
	SELECT @TaskInputDate=InputDate FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND TaskID=@wTaskID AND TaskStatus=1 AND DateDiff(d,InputDate,GetDate())=0
	IF @TaskInputDate IS NULL
	BEGIN
		SET @strErrorDescribe = N'����ɵ�ǰ������������ȡ������'
		return 2		
	END		

	-- ��ѯ����
	DECLARE @StandardAwardGold INT
	DECLARE @StandardAwardIngot INT
	DECLARE @MemberAwardGold INT
	DECLARE @MemberAwardIngot INT
	SELECT @StandardAwardGold=StandardAwardGold,@StandardAwardIngot=StandardAwardMedal,
	@MemberAwardGold=MemberAwardGold,@MemberAwardIngot=MemberAwardMedal	
	FROM TaskInfo WHERE TaskID=@wTaskID
	
    -- ��Ա�ȼ�
    DECLARE @CurrMemberOrder SMALLINT
    SET   @CurrMemberOrder = 0
    SELECT @CurrMemberOrder=MemberOrder FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
    
    --��Ա����
    DECLARE @MemberRate INT
    SET @MemberRate = 0

    SELECT @MemberRate=TaskRate FROM RYAccountsDBLink.RYAccountsDB.dbo.MemberProperty where @CurrMemberOrder =MemberOrder
    SET @MemberAwardGold = (@StandardAwardGold * (100+ @MemberRate)/100)
    SET @MemberAwardIngot = (@StandardAwardIngot * (100+ @MemberRate)/100)

	-- ��������
	IF @StandardAwardGold IS NULL SET @StandardAwardGold=0	
	IF @StandardAwardIngot IS NULL SET @StandardAwardIngot=0
	IF @MemberAwardGold IS NULL SET @MemberAwardGold=0
	IF @MemberAwardIngot IS NULL SET @MemberAwardIngot=0
	
	-- ��ѯ���
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	DECLARE @CurrMedal INT
	SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo  WHERE UserID=@dwUserID
	SELECT @CurrMedal=UserMedal FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	-- ִ�н���
	DECLARE @lRewardGold INT
	DECLARE @lRewardIngot INT
	IF @MemberOrder=0
	BEGIN
		SELECT @lRewardGold=@StandardAwardGold,@lRewardIngot=@StandardAwardIngot
	END ELSE
	BEGIN
		SELECT @lRewardGold=@MemberAwardGold,@lRewardIngot=@MemberAwardIngot	
	END

	-- ����Ԫ��
	UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo SET UserMedal=UserMedal+@lRewardIngot 
	WHERE UserID=@dwUserID	
		
	-- ���½��
	UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET Score=Score+@lRewardGold 
	WHERE UserID=@dwUserID

	-- ��ѯ����
	DECLARE @UserScore BIGINT
	DECLARE @UserIngot BIGINT
	
	-- ��ѯ���
	SELECT @UserScore=Score FROM  RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo 
	WHERE UserID=@dwUserID				

	-- ��ѯԪ��
	SELECT @UserIngot=UserMedal FROM  RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID
	
	-- ��������
	IF @UserScore IS NULL SET @UserScore=0		
	IF @UserIngot IS NULL SET @UserIngot=0

	-- ɾ������	
	UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask SET TaskStatus=3
	WHERE UserID=@dwUserID AND TaskID=@wTaskID AND DateDiff(d,InputDate,GetDate())=0
	
	-- �����¼
	INSERT RYRecordDBLink.RYRecordDB.dbo.RecordTask(DateID,UserID,TaskID,AwardGold,AwardMedal,InputDate)
	VALUES (CAST(CAST(@TaskInputDate AS FLOAT) AS INT),@dwUserID,@wTaskID,@lRewardGold,@lRewardIngot,GetDate())

	-- ����ʱ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	
	IF @lRewardIngot > 0
	BEGIN
		-- Ԫ����¼
		INSERT INTO RYAccountsDBLink.RYAccountsDB.dbo.RecordMedalChange(UserID,SrcMedal,TradeMedal,TypeID,ClientIP,CollectDate)	
		VALUES (@dwUserID,@CurrMedal,@lRewardIngot,1,@strClientIP,GETDATE())
	END
	
	-- ��ˮ��
	INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
	VALUES (@dwUserID,@CurrScore,@CurrInsure,@lRewardGold,7,@strClientIP,GETDATE())
	
	-- ��ͳ��
	UPDATE RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@lRewardGold WHERE UserID=@dwUserID AND TypeID=7
	IF @@ROWCOUNT=0
	BEGIN
		INSERT RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,7,1,@lRewardGold)
	END	
		
	-- �ɹ���ʾ
	SET @strErrorDescribe = N'��ϲ����������ȡ�ɹ���'

	-- �׳�����
	SELECT @UserScore AS Score,@UserIngot AS Ingot
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_TaskGiveUp
	@dwUserID INT,								-- �û� I D
	@wTaskID  INT,								-- ���� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	DECLARE @MemberOrder INT
	SELECT @MemberOrder=MemberOrder FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID AND LogonPass=@strPassword
	IF @MemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END

	-- �ظ���ȡ
	IF not exists(SELECT * FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask 
	WHERE UserID=@dwUserID AND TaskID=@wTaskID AND DateDiff(d,InputDate,GetDate())=0) 
	BEGIN
		SET @strErrorDescribe = N'��Ǹ�����ܷ���û�е�����'
		RETURN 3		
	END

	-- ��������
	DECLARE @TaskTakeCount AS INT
	DELETE FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsTask 	WHERE UserID=@dwUserID AND TaskID=@wTaskID 

	-- �ɹ���ʾ
	SET @strErrorDescribe = N'�����Ѿ�������'	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------