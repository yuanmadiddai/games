
----------------------------------------------------------------------------------------------------

USE RYPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadBaseEnsure]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadBaseEnsure]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_TakeBaseEnsure]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_TakeBaseEnsure]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- ���صͱ�
CREATE PROC GSP_GP_LoadBaseEnsure
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ȡ����
	DECLARE @ScoreCondition AS BIGINT
	SELECT @ScoreCondition=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'SubsistenceCondition'
	IF @ScoreCondition IS NULL SET @ScoreCondition=0

	-- ��ȡ����
	DECLARE @TakeTimes AS SMALLINT
	SELECT @TakeTimes=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'SubsistenceNumber'
	IF @TakeTimes IS NULL SET @TakeTimes=0

	-- ��ȡ����
	DECLARE @ScoreAmount AS BIGINT
	SELECT @ScoreAmount=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'SubsistenceGold'
	IF @ScoreAmount IS NULL SET @ScoreAmount=0

	-- �׳�����
	SELECT @ScoreCondition AS ScoreCondition,@TakeTimes AS TakeTimes,@ScoreAmount AS ScoreAmount
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��ȡ�ͱ�
CREATE PROC GSP_GP_TakeBaseEnsure
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strNotifyContent NVARCHAR(127) OUTPUT		-- ��ʾ����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ�û�
	IF NOT EXISTS(SELECT * FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword)
	BEGIN
		SET @strNotifyContent = N'�û���Ϣ�����ڻ������벻��ȷ��'
		RETURN 1
	END

	-- ��ȡ����
	DECLARE @ScoreCondition AS BIGINT
	SELECT @ScoreCondition=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'SubsistenceCondition'
	IF @ScoreCondition IS NULL SET @ScoreCondition=0

	-- ��ȡ����
	DECLARE @TakeTimes AS SMALLINT
	SELECT @TakeTimes=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'SubsistenceNumber'
	IF @TakeTimes IS NULL SET @TakeTimes=0

	-- ��ȡ����
	DECLARE @ScoreAmount AS BIGINT
	SELECT @ScoreAmount=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'SubsistenceGold'
	IF @ScoreAmount IS NULL SET @ScoreAmount=0
	
	-- ��ȡ����
	DECLARE @LimitMachineCount AS  SMALLINT
	SELECT @LimitMachineCount=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'SubsistenceLimitMachine'
	IF @LimitMachineCount IS NULL SET @LimitMachineCount=0

	-- �����ж�
	IF @LimitMachineCount <> 0
	BEGIN
		DECLARE @TodayMachineCount INT
		SELECT @TodayMachineCount = COUNT(ClinetMachine) FROM RYRecordDB.dbo.RecordBaseEnsure WHERE DateDiff(d,CollectDate,GetDate())=0 AND @strMachineID = ClinetMachine
		IF @TodayMachineCount >= @LimitMachineCount
		BEGIN
			SET @strNotifyContent = N'��ǰ�����Ѿ��������ƣ�'	
		RETURN 4
		END
	END	
		
	-- ��ȡ���
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	IF @@rowcount = 0
	BEGIN
		-- ��������
		INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo (UserID,Score,LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, 0, @strClientIP, @strMachineID, @strClientIP, @strMachineID)

		-- ���ý��
		SELECT @CurrScore=0,@CurrInsure=0
	END

	-- �������	
	SET @CurrScore = @CurrScore + @CurrInsure

	-- ��ȡ�ж�
	IF @CurrScore >= @ScoreCondition
	BEGIN
		SET @strNotifyContent = N'������Ϸ�Ҳ����� '+CAST(@ScoreCondition AS NVARCHAR)+N'��������ȡ��'	
		RETURN 1	
	END	

	-- ��ȡ��¼
	DECLARE @TodayDateID INT
	DECLARE @TodayTakeTimes INT	
	SET @TodayDateID=CAST(CAST(GetDate() AS FLOAT) AS INT)	
	SELECT @TodayTakeTimes=TakeTimes FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsBaseEnsure WHERE UserID=@dwUserID AND TakeDateID=@TodayDateID
	IF @TodayTakeTimes IS NULL SET @TodayTakeTimes=0	

	-- �����ж�
	IF @TodayTakeTimes >= @TakeTimes
	BEGIN
		SET @strNotifyContent = N'����������ȡ '+CAST(@TodayTakeTimes AS NVARCHAR)+N' �Σ���ȡʧ�ܣ�'
		return 3		
	END
	


	-- ���¼�¼
	IF @TodayTakeTimes=0
	BEGIN
		SET @TodayTakeTimes = 1
		INSERT INTO RYAccountsDBLink.RYAccountsDB.dbo.AccountsBaseEnsure(UserID,TakeDateID,TakeTimes,TakeGold) VALUES(@dwUserID,@TodayDateID,@TodayTakeTimes,@ScoreAmount)		
	END ELSE
	BEGIN
		SET @TodayTakeTimes = @TodayTakeTimes+1
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsBaseEnsure SET TakeTimes=@TodayTakeTimes,TakeGold=TakeGold+@ScoreAmount WHERE UserID = @dwUserID AND TakeDateID=@TodayDateID		
	END	

	-- ��ȡ���	
	UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET Score = Score + @ScoreAmount WHERE UserID = @dwUserID
	
	-- ��ˮ��
	INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
	VALUES (@dwUserID,@CurrScore,@CurrInsure,@ScoreAmount,2,@strClientIP,GETDATE())	
	
	-- ��ͳ��
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@ScoreAmount WHERE UserID=@dwUserID AND TypeID=2
	IF @@ROWCOUNT=0
	BEGIN
		INSERT RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,2,1,@ScoreAmount)
	END		
	
	-- �����¼
	INSERT INTO RYRecordDB.dbo.RecordBaseEnsure(UserID,Gold,ClinetIP,ClinetMachine,CollectDate) VALUES (@dwUserID,@ScoreAmount,@strClientIP,@strMachineID,GETDATE())

	-- ��ѯ���
	DECLARE @Score BIGINT
	SELECT @Score=Score FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID = @dwUserID 	

	-- �����ʾ
	SET @strNotifyContent = N'��ϲ�����ͱ���ȡ�ɹ��������ջ�����ȡ '+CAST(@TakeTimes-@TodayTakeTimes AS NVARCHAR)+N' �Σ�'
	
	-- �׳�����
	SELECT @Score AS Score	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------