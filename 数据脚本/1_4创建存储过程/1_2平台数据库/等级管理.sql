
----------------------------------------------------------------------------------------------------

USE RYPlatformDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_LoadGrowLevelConfig]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_LoadGrowLevelConfig]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryGrowLevel]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryGrowLevel]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------
-- �ȼ�����
CREATE PROC GSP_GR_LoadGrowLevelConfig
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ��ѯ����
	SELECT LevelID,Experience FROM GrowLevelConfig ORDER BY LevelID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
-- ��ѯ�ȼ�
CREATE PROC GSP_GP_QueryGrowLevel
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strUpgradeDescribe NVARCHAR(127) OUTPUT	-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- �Ƹ�����
DECLARE @Score BIGINT
DECLARE @Ingot BIGINT

-- ִ���߼�
BEGIN
	
	-- ��������
	DECLARE @Experience BIGINT
	DECLARE	@GrowlevelID INT	

	-- ��ѯ�û�
	SELECT @Experience=Experience,@GrowlevelID=GrowLevelID FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo 
	WHERE UserID=@dwUserID AND LogonPass=@strPassword

	-- �����ж�
	IF @Experience IS NULL OR @GrowlevelID IS NULL
	BEGIN
		return 1
	END

	-- �����ж�
	DECLARE @NowGrowLevelID INT
	SET @NowGrowLevelID = 1
	SELECT TOP 1 @NowGrowLevelID=LevelID FROM GrowLevelConfig
	WHERE @Experience>=Experience ORDER BY LevelID DESC

	-- ��������
	IF @NowGrowLevelID IS NULL
	BEGIN
		SET @NowGrowLevelID=@GrowlevelID														
	END

	-- ��������
	IF @NowGrowLevelID>@GrowlevelID
	BEGIN
		DECLARE @UpgradeLevelCount INT
		DECLARE	@lRewardGold BIGINT
		DECLARE	@lRewardIngot BIGINT
		
		DECLARE @CurrScore BIGINT
		DECLARE @CurrInsure BIGINT
		DECLARE @CurrMedal INT
		SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
		SELECT @CurrMedal=UserMedal FROM RYAccountsDB..AccountsInfo WHERE UserID=@dwUserID
		
		-- ��������
		SET @UpgradeLevelCount=@NowGrowLevelID-@GrowlevelID
		
		-- ��ѯ����
		SELECT @lRewardGold=SUM(RewardGold),@lRewardIngot=SUM(RewardMedal) FROM GrowLevelConfig
		WHERE LevelID>@GrowlevelID AND LevelID<=@NowGrowLevelID

		-- ��������
		IF @lRewardGold IS NULL SET @lRewardGold=0				
		IF @lRewardIngot IS NULL SET @lRewardIngot=0

		-- ���½��
		UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET Score=Score+@lRewardGold WHERE UserID=@dwUserID
		IF @@rowcount = 0
		BEGIN
			-- ��������
			INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo (UserID,Score,LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
			VALUES (@dwUserID, @lRewardGold, @strClientIP, @strMachineID, @strClientIP, @strMachineID)		
		END		

		-- ���µȼ�
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo SET UserMedal=UserMedal+@lRewardIngot,GrowLevelID=@NowGrowLevelID WHERE UserID=@dwUserID

		IF @lRewardIngot > 0
		BEGIN
			-- Ԫ����¼
			INSERT INTO RYAccountsDBLink.RYAccountsDB.dbo.RecordMedalChange(UserID,SrcMedal,TradeMedal,TypeID,ClientIP,CollectDate)	
			VALUES (@dwUserID,@CurrMedal,@lRewardIngot,1,@strClientIP,GETDATE())
		END
		
		-- ��ˮ��
		INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
		VALUES (@dwUserID,@CurrScore,@CurrInsure,@lRewardGold,11,@strClientIP,GETDATE())
		
		-- ����ʱ��
		DECLARE @DateID INT
		SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
		-- ��ͳ��
		UPDATE RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@lRewardGold WHERE UserID=@dwUserID AND TypeID=11
		IF @@ROWCOUNT=0
		BEGIN
			INSERT RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,11,1,@lRewardGold)
		END	
		
		-- ������ʾ
		SET @strUpgradeDescribe = N'��ϲ����Ϊ'+CAST(@NowGrowLevelID AS NVARCHAR)+N'����ϵͳ������Ϸ�� '+CAST(@lRewardGold AS NVARCHAR)+N' ,Ԫ�� '+CAST(@lRewardIngot AS NVARCHAR)

		-- ���ñ���
		SET @GrowlevelID=@NowGrowLevelID		
	END
	ELSE
	BEGIN
		-- ���µȼ�
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo SET GrowLevelID=@NowGrowLevelID WHERE UserID=@dwUserID	
		
		-- ���ñ���
		SET @GrowlevelID=@NowGrowLevelID	
	END

	DECLARE @QuerylevelID INT
	SET @QuerylevelID=@GrowlevelID
	IF @GrowlevelID=0 SET @QuerylevelID=1

	-- ��һ�ȼ�	
	DECLARE	@UpgradeRewardGold BIGINT
	DECLARE	@UpgradeRewardMedal BIGINT
	DECLARE @UpgradeExperience BIGINT	
	SELECT @UpgradeExperience=Experience,@UpgradeRewardGold=RewardGold,@UpgradeRewardMedal=RewardMedal FROM RYPlatformDBLink.RYPlatformDB.dbo.GrowLevelConfig
	WHERE LevelID=@GrowlevelID+1
	
	-- ��������
	IF @UpgradeExperience IS NULL SET @UpgradeExperience=0
	IF @UpgradeRewardGold IS NULL SET @UpgradeRewardGold=0
	IF @UpgradeRewardMedal IS NULL SET @UpgradeRewardMedal=0

	-- ��ѯ��Ϸ��
	SELECT @Score=Score FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	
	-- ��ѯԪ��	
	SELECT @Ingot=UserMedal FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID

	-- �׳�����
	SELECT @GrowlevelID AS CurrLevelID,@Experience AS Experience,@UpgradeExperience AS UpgradeExperience, @UpgradeRewardGold AS RewardGold,
		   @UpgradeRewardMedal AS RewardMedal,@Score AS Score,@Ingot AS Ingot
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------