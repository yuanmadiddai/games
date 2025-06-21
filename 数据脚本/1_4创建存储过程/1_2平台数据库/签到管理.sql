
----------------------------------------------------------------------------------------------------

USE RYPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadCheckInReward]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadCheckInReward]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CheckInQueryInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CheckInQueryInfo]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_CheckInDone]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_CheckInDone]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ؽ���
CREATE PROC GSP_GP_LoadCheckInReward
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM SigninConfig	

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ѯǩ��
CREATE PROC GSP_GR_CheckInQueryInfo
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF not exists(SELECT * FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword)
	BEGIN
		SET @strErrorDescribe = N'��Ǹ������û���Ϣ�����ڻ������벻��ȷ��'
		return 1
	END

	-- ǩ����¼
	DECLARE @wSeriesDate INT	
	DECLARE @StartDateTime DateTime
	DECLARE @LastDateTime DateTime
	SELECT @StartDateTime=StartDateTime,@LastDateTime=LastDateTime,@wSeriesDate=SeriesDate FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsSignin 	
	WHERE UserID=@dwUserID
	IF @StartDateTime IS NULL OR @LastDateTime IS NULL OR @wSeriesDate IS NULL
	BEGIN
		SELECT @StartDateTime=GetDate(),@LastDateTime=GetDate(),@wSeriesDate=0
		INSERT INTO RYAccountsDBLink.RYAccountsDB.dbo.AccountsSignin VALUES(@dwUserID,@StartDateTime,@LastDateTime,0)		
	END

	-- ��������
	IF @wSeriesDate > 7 SET @wSeriesDate = 7

	-- �����ж�
	DECLARE @TodayCheckIned TINYINT
	SET @TodayCheckIned = 0
	IF DateDiff(dd,@LastDateTime,GetDate()) = 0 	
	BEGIN
		IF @wSeriesDate > 0 SET @TodayCheckIned = 1
	END ELSE
	BEGIN		
		IF DateDiff(dd,@StartDateTime,GetDate()) <> @wSeriesDate OR @wSeriesDate >= 7 
		BEGIN
			SET @wSeriesDate = 0
			UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsSignin SET StartDateTime=GetDate(),LastDateTime=GetDate(),SeriesDate=0 WHERE UserID=@dwUserID									
		END
	END

	-- �׳�����
	SELECT @wSeriesDate AS SeriesDate,@TodayCheckIned AS TodayCheckIned	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GR_CheckInDone
	@dwUserID INT,								-- �û� I D
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
	IF NOT EXISTS(SELECT * FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword)
	BEGIN
		SET @strErrorDescribe = N'�û���Ϣ�����ڻ������벻��ȷ��'
		RETURN 1
	END
	
	-- ��ȡ����
	DECLARE @LimitMachineCount AS  SMALLINT
	SELECT @LimitMachineCount=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'SignInLimitMachine'
	IF @LimitMachineCount IS NULL SET @LimitMachineCount=0

	-- �����ж�
	IF @LimitMachineCount <> 0
	BEGIN
		DECLARE @TodayMachineCount INT
		SELECT @TodayMachineCount = COUNT(ClinetMachine) FROM RYRecordDB.dbo.RecordSignin WHERE DateDiff(d,CollectDate,GetDate())=0 AND @strMachineID = ClinetMachine
		IF @TodayMachineCount >= @LimitMachineCount
		BEGIN
			SET @strErrorDescribe = N'��ǰ�����Ѿ��������ƣ�'	
		RETURN 4		
		END
	END


	-- ǩ����¼
	DECLARE @wSeriesDate INT	
	DECLARE @StartDateTime DateTime
	DECLARE @LastDateTime DateTime
	SELECT @StartDateTime=StartDateTime,@LastDateTime=LastDateTime,@wSeriesDate=SeriesDate FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsSignin 
	WHERE UserID=@dwUserID
	IF @StartDateTime IS NULL OR @LastDateTime IS NULL OR @wSeriesDate IS NULL
	BEGIN
		SELECT @StartDateTime = GetDate(),@LastDateTime = GetDate(),@wSeriesDate = 0
		INSERT INTO RYAccountsDBLink.RYAccountsDB.dbo.AccountsSignin VALUES(@dwUserID,@StartDateTime,@LastDateTime,0)		
	END

	-- ǩ���ж�
	IF DateDiff(dd,@LastDateTime,GetDate()) = 0 AND @wSeriesDate > 0
	BEGIN
		SET @strErrorDescribe = N'��Ǹ���������Ѿ�ǩ���ˣ�'
		return 3		
	END

	-- ����Խ��
	IF @wSeriesDate > 7
	BEGIN
		SET @strErrorDescribe = N'����ǩ����Ϣ�����쳣���������ǵĿͷ���Ա��ϵ��'
		return 2				
	END

	-- ���¼�¼
	SET @wSeriesDate = @wSeriesDate+1
	UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsSignin SET LastDateTime = GetDate(),SeriesDate = @wSeriesDate WHERE UserID = @dwUserID

	-- ��ѯ����
	DECLARE @lRewardGold BIGINT
	SELECT @lRewardGold=RewardGold FROM RYPlatformDBLink.RYPlatformDB.dbo.SigninConfig WHERE DayID=@wSeriesDate
	IF @lRewardGold IS NULL 
	BEGIN
		SET @lRewardGold = 0
	END	

	-- ��ѯ���
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo  WHERE UserID=@dwUserID
	
	IF @CurrScore IS NULL SET @CurrScore=0
	IF @CurrInsure IS NULL SET @CurrInsure=0
	
	-- �������	
	UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET Score = Score + @lRewardGold WHERE UserID = @dwUserID
	IF @@rowcount = 0
	BEGIN
		-- ��������
		INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo (UserID,Score, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @lRewardGold, @strClientIP, @strMachineID, @strClientIP, @strMachineID)
	END
	

	-- �����¼
	INSERT INTO RYRecordDB.dbo.RecordSignin(UserID,Gold,ClinetIP,ClinetMachine,CollectDate) VALUES (@dwUserID,@lRewardGold,@strClientIP,@strMachineID,GETDATE())	
	
	-- ��ˮ��
	INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
	VALUES (@dwUserID,@CurrScore,@CurrInsure,@lRewardGold,3,@strClientIP,GETDATE())	
	
	-- ��ͳ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@lRewardGold WHERE UserID=@dwUserID AND TypeID=3
	IF @@ROWCOUNT=0
	BEGIN
		INSERT RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,3,1,@lRewardGold)
	END	

	-- ��ѯ���
	DECLARE @lScore BIGINT	
	SELECT @lScore=Score FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID = @dwUserID 	
	IF @lScore IS NULL SET @lScore = 0
	
	-- �׳�����
	SELECT @lScore AS Score	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------