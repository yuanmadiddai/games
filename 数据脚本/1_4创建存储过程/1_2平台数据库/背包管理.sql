
----------------------------------------------------------------------------------------------------

USE RYPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID =OBJECT_ID(N'[dbo].[GSP_GP_QueryUserBackpack]')and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserBackpack]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UseProp]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UseProp]
GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

-- ��ѯ����
CREATE PROC GSP_GP_QueryUserBackpack
	@dwUserID INT,								-- �û� I D
	@dwKind	INT									-- ��������
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
-- ��������
	DECLARE @LogonPass AS NCHAR(32)
	-- ��ѯ�û�
	SELECT @LogonPass=LogonPass FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	IF @LogonPass is NULL
	BEGIN
		--N'������Ϣ��ѯʧ�ܣ�'
		RETURN 1
	END
	
	if @dwKind = 0
		select * from RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage as a, GameProperty as b where a.GoodsID=b.ID and a.UserID=@dwUserID and a.GoodsCount > 0
	else
		select * from RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage as a, GameProperty as b where a.GoodsID=b.ID and a.UserID=@dwUserID and a.GoodsCount > 0 and b.Kind=@dwKind
	RETURN 0
	END

RETURN 0

GO

---------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

CREATE PROC GSP_GP_UseProp
	@dwUserID INT,				-- �û� I D
	@dwRecvUserID INT,			-- ���շ��û�ID
	@dwPropID INT,				-- ����ID
	@dwPropCount INT,			-- ��������
	@strClientIP NVARCHAR(15),	-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	--�������
	DECLARE @useArea INT 
	DECLARE @ServiceArea INT 
	DECLARE @UseResultsGold INT 
	DECLARE @SendLoveLiness INT 
	DECLARE @RecvLoveLiness INT 
	DECLARE @UseResultsValidTime BIGINT 
	DECLARE @UseResultsValidTimeScoreMultiple SMALLINT 
	DECLARE @Kind INT 
	DECLARE @szPropName NVARCHAR(31)
	DECLARE @CurrMemberOrder TINYINT
	DECLARE @GoodsCount INT 
	DECLARE @BuffName NVARCHAR(32)
	DECLARE @UseTime DATETIME 
	DECLARE @ValidTime BIGINT
	DECLARE @CurrGetGold BIGINT
	DECLARE @OverTime DATETIME
	DECLARE @CurGoodsCount INT
	
	SET @useArea = 0
	SET @ServiceArea = 0
	SET @UseResultsGold = 0
	SET @SendLoveLiness = 0
	SET @RecvLoveLiness = 0
	SET @UseResultsValidTime = 0
	SET @UseResultsValidTimeScoreMultiple = 0
	SET @Kind = 0
	SET @CurrMemberOrder =0
	SET @GoodsCount = 0
	SET @UseTime = GETDATE()
	SET @ValidTime = 0
	SET @CurrGetGold=0
	
				
	-- ��ѯ����
	SELECT @GoodsCount = GoodsCount FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=@dwPropID
	IF @GoodsCount = 0 or @GoodsCount < @dwPropCount
	BEGIN
		SET @strErrorDescribe=N'������������'
		RETURN 1
	END

	-- ��ѯ����
	SELECT @useArea = UseArea, @ServiceArea = ServiceArea, @UseResultsGold = UseResultsGold, @szPropName = Name,
	@SendLoveLiness = SendLoveLiness, @RecvLoveLiness = RecvLoveLiness, @UseResultsValidTime = UseResultsValidTime,
	@Kind = Kind, @UseResultsValidTimeScoreMultiple=UseResultsValidTimeScoreMultiple
	FROM GameProperty  WHERE ID=@dwPropID
	IF @szPropName is null
	BEGIN
		SET @strErrorDescribe=N'���߲�����'
		RETURN 2
	END

	-- �������
	IF @Kind = 1
	BEGIN
		-- ��ѯ���
		IF NOT EXISTS (SELECT UserID FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE @dwRecvUserID=UserID)
		BEGIN
			SET @strErrorDescribe=N'��������Ҳ�����'
			RETURN 3
		END	
		
		-- ��������
		IF @SendLoveLiness <> 0
			UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo SET LoveLiness = LoveLiness+@SendLoveLiness WHERE UserID=@dwUserID
		
		IF @RecvLoveLiness <> 0
			UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo SET LoveLiness = LoveLiness+@RecvLoveLiness WHERE UserID=@dwRecvUserID
	END
	
	-- ��ʯ����
	IF @Kind = 2
	BEGIN
		SET @CurrGetGold = @UseResultsGold *@dwPropCount
		UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET Score = @CurrGetGold + Score WHERE UserID=@dwUserID
	END
	
	--��Ƭ����
	IF @Kind = 3 or @Kind=4 or  @Kind=5 
	BEGIN
		IF @Kind = 3
			SET @BuffName=N'˫����'
		ELSE IF @Kind = 4
			SET @BuffName=N'����'
		ELSE IF @Kind = 5
			SET @BuffName=N'���߿�'
			
		--����Buff��
		SELECT @UseTime=UseTime, @ValidTime = UseResultsValidTime FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfoPoperty WHERE UserID=@dwUserID and KindID=@Kind
		IF @ValidTime = 0
		BEGIN
			SET @UseTime = GETDATE()
			SET @ValidTime = @UseResultsValidTime*@dwPropCount
			INSERT RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfoPoperty(UserID, KindID, UseTime, UseResultsValidTime, UseResultsValidTimeScoreMultiple, Name) 
			VALUES(@dwUserID, @Kind, GETDATE(), @UseResultsValidTime*@dwPropCount, @UseResultsValidTimeScoreMultiple, @BuffName)
		END
		ELSE
		BEGIN
			SET @OverTime = DATEADD(SECOND,@ValidTime,@UseTime)
			--���ڼ��
			IF @OverTime > GETDATE() --û�й���
			BEGIN
				SET @ValidTime = @ValidTime+@UseResultsValidTime*@dwPropCount
				UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfoPoperty SET UseResultsValidTime=@ValidTime WHERE UserID=@dwUserID and KindID=@Kind
			END
			ELSE	--����
			BEGIN
				SET @UseTime = GETDATE()
				SET @ValidTime = @UseResultsValidTime*@dwPropCount
				UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfoPoperty SET UseTime=@UseTime, UseResultsValidTime=@ValidTime WHERE UserID=@dwUserID and KindID=@Kind
			END
		END
	END
	
	--��Ա����
	IF @Kind = 6 
	BEGIN
		-- ��Ա����
		DECLARE @MemberRight AS INT	
		DECLARE @MemberOverDate DATETIME
		DECLARE @MemberOrder TINYINT	
		DECLARE @MemberValidMonth INT
		DECLARE @MaxUserRight INT
		DECLARE @MaxMemberOrder TINYINT	 
		DECLARE @MemberSwitchDate DATETIME		
		
		-- ɾ������
		DELETE FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsMember WHERE UserID=@dwUserID  AND MemberOverDate<=GETDATE()
		
		-- ��ѯȨ��	
		SELECT @MemberRight=UserRight FROM RYAccountsDBLink.RYAccountsDB.dbo.MemberProperty
		
		-- ��ѯʱ��
		SET @MemberValidMonth = 1 * @dwPropCount 
		
		-- ��ѯ�ȼ�
		SET @MemberOrder= @dwPropID % 100
		
		-- ��Ա��¼
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsMember SET MemberOverDate=DATEADD(mm,@MemberValidMonth, MemberOverDate)
		WHERE UserID=@dwUserID AND MemberOrder=@MemberOrder
		IF @@ROWCOUNT=0
		BEGIN
		INSERT RYAccountsDBLink.RYAccountsDB.dbo.AccountsMember(UserID,MemberOrder,UserRight,MemberOverDate)
			VALUES (@dwUserID,@MemberOrder,@MemberRight,DATEADD(mm,@MemberValidMonth, GETDATE()))
		END
		
		-- �л���Ա
		SELECT @MaxMemberOrder=MAX(MemberOrder),@MemberOverDate=MAX(MemberOverDate),@MemberSwitchDate=MIN(MemberOverDate)
		FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsMember WHERE UserID=@dwUserID
		
		-- �л�Ȩ��
		SELECT @MaxUserRight=UserRight FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsMember
		WHERE UserID=@dwUserID AND MemberOrder=@MaxMemberOrder	
		
		-- ���»�Ա
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo
		SET MemberOrder=@MaxMemberOrder,UserRight=@MaxUserRight,MemberOverDate=@MemberOverDate,MemberSwitchDate=@MemberSwitchDate
		WHERE UserID=@dwUserID	
	
	END
	
	--��������
	IF @Kind=9  
	BEGIN
		DECLARE @UserGameScore INT 
		SET @UserGameScore = 0
		SELECT @UserGameScore=Score FROM RYGameScoreDBLink.RYGameScoreDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
		IF @UserGameScore >= 0
		BEGIN
			SET @strErrorDescribe=N'���Ļ��ֲ�Ϊ���֣��޷����'
			RETURN 3
		END
		ELSE
			UPDATE RYGameScoreDBLink.RYGameScoreDB.dbo.GameScoreInfo SET Score=0 WHERE UserID=@dwUserID
	END
	
	--��������
	IF @Kind=10  
	BEGIN
		DECLARE @FleeCount INT 
		SET @FleeCount = 0
		SELECT @FleeCount=FleeCount FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
		IF @FleeCount=0
		BEGIN
			SET @strErrorDescribe=N'����������δ����0���޷����'
			RETURN 4
		END
		ELSE
			UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET FleeCount=0 WHERE UserID=@dwUserID
	END
	
	
	--���ٵ���
	SET @CurGoodsCount= @GoodsCount-@dwPropCount
	IF @CurGoodsCount > 0
	BEGIN
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage SET GoodsCount = @CurGoodsCount WHERE UserID=@dwUserID and GoodsID=@dwPropID
	END
	ELSE
	BEGIN
		DELETE RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=@dwPropID
	END
		
	--���߼�¼
	insert RYRecordDBLink.RYRecordDB.dbo.RecordUseProperty(SourceUserID, TargetUserID,PropertyID, PropertyName,
	PropertyCount, LovelinessRcv, LovelinessSend,KindID, ServerID, ClientIP, UseDate)
	values(@dwUserID, @dwRecvUserID, @dwPropID, @szPropName, @dwPropCount, 
	@RecvLoveLiness, @SendLoveLiness, 0, 0, @strClientIP, GETDATE())
	
	-- ��ѯ���
	DECLARE @Score BIGINT
	SET @Score = 0
	SELECT @Score=Score FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID = @dwUserID 	
	
	--��ǰ��Ա
	SELECT @CurrMemberOrder=MemberOrder FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID = @dwUserID
	
	SELECT @useArea AS UseArea, @ServiceArea AS ServiceArea, @CurGoodsCount AS CurGoodsCount, @Score AS Score,@CurrGetGold AS UseResultsGold,
	@SendLoveLiness AS SendLoveLiness, @RecvLoveLiness AS RecvLoveLiness,@UseTime AS UseTime, @ValidTime AS UseResultsValidTime, @Kind AS Kind, 
	@szPropName AS Name,@UseResultsValidTimeScoreMultiple AS UseResultsValidTimeScoreMultiple, @CurGoodsCount AS RemainderCount ,@CurrMemberOrder AS MemberOrder
	
	SET @strErrorDescribe=@szPropName + N'ʹ�óɹ�'
	RETURN 0

END

RETURN 0

GO

---------------------------------------------------------------------------------------------