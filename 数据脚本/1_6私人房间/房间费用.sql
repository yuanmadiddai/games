USE RYTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CreateTableFee]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CreateTableFee]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CreateTableQuit]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CreateTableQuit]

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_InsertCreateRecord]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_InsertCreateRecord]



---����˽�˷�����Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_QueryPersonalRoomInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_QueryPersonalRoomInfo]
GO





SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �۳�����
CREATE PROC GSP_GR_CreateTableFee
	@dwUserID INT,								-- �û� I D
	@dwServerID INT,							-- ���� I D
	@dwDrawCountLimit INT,						-- ʱ������
	@dwDrawTimeLimit INT,						-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	DECLARE @ReturnValue INT
	DECLARE @dUserBeans DECIMAL(18,2)
	DECLARE @dCurBeans DECIMAL(18,2)
	DECLARE @Fee INT
	DECLARE @lRoomCard bigint
	DECLARE @cbIsJoinGame TINYINT
	DECLARE @MemberOrder TINYINT
	DECLARE @CreateRight TINYINT

	DECLARE @wKindID INT
	SELECT  @wKindID = KindID FROM RYPlatformDBLink.RYPlatformDB.dbo.GameRoomInfo WHERE ServerID = @dwServerID 
	-- ��ѯ���� �� �����Ƿ������Ϸ
	SELECT  @CreateRight = CreateRight FROM RYPlatformDBLink.RYPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 

	--�����һ�Ա�ȼ�������vip��Ա���ܴ���˽�˷���
	SELECT @MemberOrder=MemberOrder FROM RYAccountsDB..AccountsInfo WHERE UserID=@dwUserID
	IF @MemberOrder < @CreateRight
	BEGIN
		SET @strErrorDescribe=N'�������д���˽�˷����Ȩ��!'
		RETURN 1
	END

	--�û���Ϸ��
	SELECT @dUserBeans=Currency FROM RYTreasureDB..UserCurrencyInfo WHERE UserID=@dwUserID


	--�û�����
	SELECT @lRoomCard=RoomCard FROM RYTreasureDB..UserRoomCard WHERE UserID=@dwUserID


		-- ��ѯ����
	DECLARE @LockServerID INT
	SELECT @LockServerID=ServerID FROM GameScoreLocker WHERE UserID=@dwUserID

		-- �����ж�
	IF  @LockServerID IS NOT NULL AND @LockServerID<>@dwServerID
	BEGIN
		-- ��ѯ��Ϣ
		DECLARE @ServerName NVARCHAR(31)
		SELECT @ServerName=ServerName FROM RYPlatformDBLink.RYPlatformDB.dbo.GameRoomInfo WHERE ServerID=@LockServerID

		-- ������Ϣ
		IF @ServerName IS NULL SET @ServerName=N'δ֪����'
		SET @strErrorDescribe=N'������ [ '+@ServerName+N' ] ��Ϸ�����У�����ͬʱ�ٽ������Ϸ���䣡'
		RETURN 2
	END


	DECLARE @cbBeanOrRoomCard TINYINT

	SELECT  @cbBeanOrRoomCard = CardOrBean, @cbIsJoinGame = IsJoinGame FROM RYPlatformDBLink.RYPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
		--SET @strErrorDescribe=N'���ݿ�� 1' + '����' +  CONVERT(nvarchar(32), @dwDrawCountLimit)+ 'ʱ��' +  CONVERT(nvarchar(32), @dwDrawTimeLimit) + '����' +  CONVERT(nvarchar(32), @wKindID)
		--RETURN 1
	-- ��ѯ����
	SELECT @Fee=TableFee FROM RYPlatformDBLink.RYPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID
	IF @Fee IS NULL OR @Fee=0
	BEGIN
		SET @strErrorDescribe=N'���ݿ��ѯ����ʧ�ܣ������³��ԣ�'
		RETURN 3
	END


	IF @Fee IS NULL OR @Fee=0
	BEGIN
		SET @strErrorDescribe=N'���ݿ��ѯ����ʧ�ܣ������³��ԣ�'
		RETURN 3
	END
	
	IF @cbBeanOrRoomCard = 0
	BEGIN

		IF @dUserBeans IS NULL
		BEGIN
			SET @strErrorDescribe=N'�û���Ϸ�����㣬�빺��'
			RETURN 1
		END
				-- д�����
		IF @dUserBeans < @Fee
		BEGIN
			SET @strErrorDescribe=N'�������㣬���ȳ�ֵ��'
			RETURN 4
		END
		ELSE
		BEGIN
		
			-- �仯��־
			INSERT INTO RecordCurrencyChange(UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
			VALUES(@dwUserID,@Fee,1,@dUserBeans,@dUserBeans-@Fee,@strClientIP,GETDATE(),'����˽�˷���')	

			UPDATE RYTreasureDB..UserCurrencyInfo SET Currency=@dUserBeans-@Fee WHERE UserID=@dwUserID		
		END
	END
	ELSE
	BEGIN

		IF @lRoomCard IS NULL
		BEGIN
			SET @strErrorDescribe=N'�û��������㣬�빺��'
			RETURN 1
		END

		-- д�����
		IF @lRoomCard < @Fee
		BEGIN
			SET @strErrorDescribe=N'���ķ������㣬���ȳ�ֵ��'
			RETURN 4
		END
		ELSE
		BEGIN
		

			UPDATE RYTreasureDB..UserRoomCard SET RoomCard=@lRoomCard-@Fee WHERE UserID=@dwUserID	
			
			--��ȡ��Һ���Ϸ��
			DECLARE @lGold  BIGINT
			DECLARE @Currency  DECIMAL
			SELECT  @lGold = Score FROM GameScoreInfo WHERE UserID = @dwUserID
			SELECT  @Currency = Currency FROM UserCurrencyInfo WHERE UserID = @dwUserID
			IF @Currency IS NULL SET @Currency=0

			INSERT INTO RYRecordDB..RecordRoomCard(SourceUserID, SBeforeCard, RoomCard, TargetUserID, TBeforeCard, TypeID, Currency, Gold, Remarks, ClientIP, CollectDate)
			VALUES (@dwUserID, @lRoomCard, @Fee, 0, 0, 3, @Currency, @lGold, '��ɢ�����˻�����', @strClientIP, GETDATE())
	

		END		
	END


	SELECT @dCurBeans=Currency FROM RYTreasureDB..UserCurrencyInfo WHERE UserID=@dwUserID
	IF @dCurBeans IS NULL SET @dCurBeans=0

		--�û�����
	SELECT @lRoomCard=RoomCard FROM RYTreasureDB..UserRoomCard WHERE UserID=@dwUserID
	IF @lRoomCard IS NULL SET @lRoomCard=0

	SELECT @dCurBeans AS dCurBeans, @lRoomCard AS RoomCard, @cbIsJoinGame AS IsJoinGame
END

RETURN 0
GO

----------------------------------------------------------------------------------------------------

-- �˻�����
CREATE PROC GSP_GR_CreateTableQuit
	@dwUserID INT,								-- �û� I D
	@strRoomID NVARCHAR(6),							-- �����ʶ
	@dwServerID INT,							-- �����ʶ
	@dwDrawCountLimit INT,						-- ʱ������
	@dwDrawTimeLimit INT,						-- ��������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	

	DECLARE @ReturnValue INT
	SET @ReturnValue=0

	DECLARE @Fee INT
	DECLARE @cbBeanOrRoomCard TINYINT
	DECLARE @cbJoin TINYINT

	DECLARE @wKindID INT
	SELECT  @wKindID = KindID FROM RYPlatformDBLink.RYPlatformDB.dbo.GameRoomInfo WHERE ServerID = @dwServerID 
	-- ��ѯ���� �� �����Ƿ������Ϸ
	SELECT  @cbBeanOrRoomCard = CardOrBean, @cbJoin = IsJoinGame FROM RYPlatformDBLink.RYPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 

	-- ��ѯ����
	SELECT @Fee=TableFee FROM RYPlatformDBLink.RYPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID 

	IF @Fee IS NULL OR @Fee=0
	BEGIN
		SET @strErrorDescribe=N'���ݿ��ѯ����ʧ�ܣ������³��ԣ�'
		RETURN 3
	END

	IF @cbBeanOrRoomCard = 0
	BEGIN
		IF  EXISTS(SELECT * FROM RYTreasureDB..UserCurrencyInfo WHERE UserID=@dwUserID)
		BEGIN
			DECLARE @dUserBeans DECIMAL(18,2)
			SELECT @dUserBeans=Currency FROM RYTreasureDB..UserCurrencyInfo WHERE UserID=@dwUserID

			-- �仯��־
			INSERT INTO RecordCurrencyChange(UserID,ChangeCurrency,ChangeType,BeforeCurrency,AfterCurrency,ClinetIP,InputDate,Remark)
			VALUES(@dwUserID,@Fee,1,@dUserBeans,@dUserBeans+@Fee,@strClientIP,GETDATE(),'˽�˷����˻�����')
			
			UPDATE UserCurrencyInfo SET Currency=Currency+@Fee WHERE UserID=@dwUserID
			SET @ReturnValue=0
		END	
		ELSE
		BEGIN
			SET @ReturnValue=2
			SET @strErrorDescribe=N'�Ҳ����û���Ϣ'
		END
	END
	ELSE
	BEGIN
		IF  EXISTS(SELECT * FROM RYTreasureDB..UserRoomCard WHERE UserID=@dwUserID)
		BEGIN
			DECLARE @lRoomCard bigint
			SELECT @lRoomCard=RoomCard FROM RYTreasureDB..UserRoomCard WHERE UserID=@dwUserID

			
			UPDATE UserRoomCard SET RoomCard=@lRoomCard+@Fee WHERE UserID=@dwUserID
			SET @ReturnValue=0

			--��ȡ��Һ���Ϸ��
			DECLARE @lGold  BIGINT
			DECLARE @Currency  DECIMAL
			SELECT  @lGold = Score FROM GameScoreInfo WHERE UserID = @dwUserID
			SELECT  @Currency = Currency FROM UserCurrencyInfo WHERE UserID = @dwUserID
			IF @Currency IS NULL SET @Currency=0

			INSERT INTO RYRecordDB..RecordRoomCard(SourceUserID, SBeforeCard, RoomCard, TargetUserID, TBeforeCard, TypeID, Currency, Gold, Remarks, ClientIP, CollectDate)
			VALUES (@dwUserID, @lRoomCard + @Fee, @Fee, 0, 0, 3, @Currency, @lGold, '�����������ķ���', @strClientIP, GETDATE())

		END	
		ELSE
		BEGIN
			SET @ReturnValue=2
			SET @strErrorDescribe=N'�Ҳ����û���Ϣ'
		END
	END

	--���������Ϸ��������
	IF @cbJoin = 1
	BEGIN
		DELETE FROM GameScoreLocker WHERE UserID = @dwUserID
	END

	DECLARE @dCurBeans DECIMAL(18,2)
	SELECT @dCurBeans=Currency FROM RYTreasureDB..UserCurrencyInfo WHERE UserID=@dwUserID
	IF @dCurBeans IS NULL SET @dCurBeans=0

		--�û�����
	SELECT @lRoomCard=RoomCard FROM RYTreasureDB..UserRoomCard WHERE UserID=@dwUserID
	IF @lRoomCard IS NULL SET @lRoomCard=0

	SELECT @dCurBeans AS dCurBeans, @lRoomCard AS RoomCard

END

RETURN @ReturnValue
GO


-----------------------------------------------------------------------
-- ���������¼
CREATE PROC GSP_GR_InsertCreateRecord
	@dwUserID INT,								-- �û� I D
	@dwServerID INT,							-- ���� ��ʶ
	@RoomID nvarchar(6),						-- ���� ID
	@lCellScore INT,							-- ���� �׷�
	@dwDrawCountLimit INT,						-- ʱ������
	@dwDrawTimeLimit INT,						-- ��������
	@szPassWord NVARCHAR(15),					-- ���ӵ�ַ
	@wJoinGamePeopleCount SMALLINT,				-- ʱ������
	@dwRoomTax BIGINT,							-- ˽�˷���˰��
	@strClientAddr NVARCHAR(15),
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	

	DECLARE @Fee INT
	DECLARE @Nicname NVARCHAR(31)
	DECLARE @lPersonalRoomTax BIGINT

		-- ��ѯ���� �� �����Ƿ������Ϸ
	DECLARE @cbBeanOrRoomCard TINYINT
	DECLARE @wKindID INT
	SELECT  @wKindID = KindID FROM RYPlatformDBLink.RYPlatformDB.dbo.GameRoomInfo  WHERE ServerID = @dwServerID 
	-- ��ѯ����
	SELECT @Fee=TableFee FROM RYPlatformDBLink.RYPlatformDB.dbo.PersonalTableFee WHERE DrawCountLimit=@dwDrawCountLimit AND DrawTimeLimit=@dwDrawTimeLimit AND KindID = @wKindID 
	IF @Fee IS NULL OR @Fee=0
	BEGIN
		SET @strErrorDescribe=N'���ݿ��ѯ����ʧ�ܣ������³��ԣ�'
		RETURN 3
	END

	-- ��ȡ����������ҵ��ǳ�
	SELECT @Nicname =NickName FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
	IF @Nicname IS NULL
	SET @Nicname =''
		
	SELECT   @cbBeanOrRoomCard = CardOrBean, @lPersonalRoomTax = PersonalRoomTax FROM RYPlatformDBLink.RYPlatformDB.dbo.PersonalRoomInfo WHERE KindID = @wKindID 
	
	--��������ķ�������ѯ����˰��
	DECLARE @lTaxAgency BIGINT
	SELECT  @lTaxAgency = AgentScale FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsAgent WHERE UserID = @dwUserID 
	IF @lTaxAgency IS NOT NULL
	BEGIN
		SET @lPersonalRoomTax = @lTaxAgency
	END

	--��ȡ��Һ���Ϸ��
	DECLARE @lGold  BIGINT
	DECLARE @Currency  DECIMAL
	SELECT  @lGold = Score FROM GameScoreInfo WHERE UserID = @dwUserID
	SELECT  @Currency = Currency FROM UserCurrencyInfo WHERE UserID = @dwUserID

	IF @cbBeanOrRoomCard = 1
	BEGIN
		--�û�����
		DECLARE @lRoomCard BIGINT
		SELECT @lRoomCard=RoomCard FROM RYTreasureDB..UserRoomCard WHERE UserID=@dwUserID
		IF @lRoomCard IS NULL SET @lRoomCard=0
		IF @Currency IS NULL SET @Currency=0

		INSERT INTO RYRecordDB..RecordRoomCard(SourceUserID, SBeforeCard, RoomCard, TargetUserID, TBeforeCard, TypeID, Currency, Gold, Remarks, ClientIP, CollectDate)
		VALUES (@dwUserID, @lRoomCard + @Fee, @Fee, 0, 0, 3, @Currency, @lGold, '�����������ķ���', @strClientAddr, GETDATE())
	END

	-- д�����ķſ���¼
	INSERT INTO RYPlatformDB..StreamCreateTableFeeInfo(UserID,NickName, ServerID, RoomID, CellScore, CardOrBean, CountLimit,TimeLimit,CreateTableFee,CreateDate, TaxAgency, JoinGamePeopleCount)
												VALUES(@dwUserID,@Nicname, @dwServerID, @RoomID, @lCellScore,@cbBeanOrRoomCard, @dwDrawCountLimit, @dwDrawTimeLimit, @Fee,GETDATE(), @dwRoomTax, @wJoinGamePeopleCount)	


END

RETURN 0
GO

-----------------------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------------------------
-- ����˽�˷�����Ϣ
CREATE  PROCEDURE dbo.GSP_GS_QueryPersonalRoomInfo
	@dwRoomID NVARCHAR(7),								-- �����ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	DECLARE @dwUserID	INT
	DECLARE @Nicname NVARCHAR(31)
	DECLARE @dwPlayTurnCount INT
	DECLARE @dwPlayTimeLimit INT
	DECLARE @cbIsDisssumRoom TINYINT
	DECLARE @sysCreateTime DATETIME
	DECLARE @sysDissumeTime DATETIME
	DECLARE @lTaxCount BIGINT
	DECLARE @cbRoomCardOrBean TINYINT
	DECLARE @lCreateFee TINYINT
	DECLARE @bnryRoomScoreInfo varbinary(MAX)
	-- ���ط���
	SELECT @dwUserID = UserID, @dwPlayTurnCount=CountLimit, @dwPlayTimeLimit = TimeLimit, @sysCreateTime = CreateDate, @sysDissumeTime = DissumeDate, @lTaxCount = TaxRevenue, @lCreateFee = CreateTableFee,
	 @cbRoomCardOrBean = CardOrBean, @bnryRoomScoreInfo = RoomScoreInfo
	FROM RYPlatformDB..StreamCreateTableFeeInfo WHERE RoomID = @dwRoomID
	IF @sysDissumeTime IS NULL
	BEGIN
		SET @cbIsDisssumRoom = 0
		SET @sysDissumeTime = @sysCreateTime
	END
	ELSE
	BEGIN
		SET @cbIsDisssumRoom = 1
	END

	declare @strRoomScoreInfo varchar(8000),@i int
	select @strRoomScoreInfo='',@i=datalength(@bnryRoomScoreInfo)
	while @i>0
		select @strRoomScoreInfo=substring('0123456789ABCDEF',substring(@bnryRoomScoreInfo,@i,1)/16+1,1)
				+substring('0123456789ABCDEF',substring(@bnryRoomScoreInfo,@i,1)%16+1,1)
				+@strRoomScoreInfo
			,@i=@i-1

	-- ��ȡ����ǳ�
	SELECT @Nicname =NickName FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
	IF @Nicname IS NULL
	SET @Nicname =''


	if @dwPlayTurnCount is null
	set @dwPlayTurnCount=0

	if @dwPlayTimeLimit is null
	set @dwPlayTimeLimit=0

	if @cbIsDisssumRoom is null
	set @cbIsDisssumRoom=0

	if @sysDissumeTime is null
	set @sysDissumeTime=GETDATE()

	if @sysCreateTime is null
	set @sysCreateTime=GETDATE()

	if @lTaxCount is null
	set @lTaxCount=0

	if @cbRoomCardOrBean is null
	set @cbRoomCardOrBean=0

	if @lCreateFee is null
	set @lCreateFee=0

	SELECT @Nicname AS UserNicname, @dwPlayTurnCount AS dwPlayTurnCount, @dwPlayTimeLimit AS dwPlayTimeLimit, @cbIsDisssumRoom AS cbIsDisssumRoom, @sysCreateTime AS sysCreateTime, 
	@sysDissumeTime AS sysDissumeTime, @lTaxCount AS lTaxCount, @lCreateFee AS CreateTableFee,@cbRoomCardOrBean AS CardOrBean, @bnryRoomScoreInfo AS RoomScoreInfo, @strRoomScoreInfo AS strRoomScoreInfo

END

RETURN 0

GO


-------------------------------------
