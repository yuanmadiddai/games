
----------------------------------------------------------------------------------------------------

USE RYPlatformDB
GO
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGamePropertyTypeItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGamePropertyTypeItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGamePropertyRelatItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGamePropertyRelatItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGamePropertyItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGamePropertyItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadGamePropertySubItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadGamePropertySubItem]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_BuyProperty]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_BuyProperty]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadUserGameBuff]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadUserGameBuff]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadUserGameTrumpet]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadUserGameTrumpet]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QuerySendPresent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QuerySendPresent]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserSendPresentByID]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserSendPresentByID]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_UserSendPresentByNickName]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_UserSendPresentByNickName]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_GetSendPresent]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_GetSendPresent]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_PropertQuerySingle]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_PropertQuerySingle]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GP_LoadGamePropertyTypeItem
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	SELECT * FROM GamePropertyType(NOLOCK) WHERE Nullity=0 AND TagID = 0
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ع�ϵ
CREATE PROC GSP_GP_LoadGamePropertyRelatItem
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	SELECT * FROM GamePropertyRelat(NOLOCK) WHERE  TagID = 0
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ص���
CREATE PROC GSP_GP_LoadGamePropertyItem
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ���ص���
	SELECT * FROM GameProperty(NOLOCK) WHERE Nullity=0 AND Kind <> 11
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ���ص���
CREATE PROC GSP_GP_LoadGamePropertySubItem
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	-- ���ص���
	SELECT * FROM GamePropertySub(NOLOCK) 
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


-- ���ѵ���
CREATE PROC GSP_GP_BuyProperty
	@dwUserID INT,								-- �û���ʶ
	@dwPropertyID INT,							-- ���߱�ʶ
	@dwItemCount INT,							-- ������Ŀ
	@cbConsumeType SMALLINT,					-- ��������
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @GoldPrice AS BIGINT
	DECLARE @CashPrice AS DECIMAL(18,2)
	DECLARE @UserMedalPrice AS BIGINT
	DECLARE @LoveLinessPrice AS BIGINT
	DECLARE @Discount AS SMALLINT
	DECLARE @PropertyName AS NVARCHAR(31)

	DECLARE @ConsumeGold AS BIGINT
	DECLARE @ConsumeUserMedal AS BIGINT
	DECLARE @ConsumeCash AS DECIMAL(18,2)
	DECLARE @ConsumeLoveLiness AS BIGINT

	DECLARE @Gold AS BIGINT
	DECLARE @UserMedal AS BIGINT
	DECLARE @Cash AS DECIMAL(18,2)
	DECLARE @LoveLiness AS BIGINT
	
	DECLARE @BuyResultsGold AS BIGINT
	DECLARE @Nullity BIT
	DECLARE @CurrMemberOrder SMALLINT	
	
	SELECT @Nullity=Nullity, @CurrMemberOrder=MemberOrder FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword

	-- �û��ж�
	IF @CurrMemberOrder IS NULL
	BEGIN
		SET @strErrorDescribe=N'�ʺŲ����ڣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END

	-- ��ʼ��Ϣ
	SET @ConsumeGold=0
	SET @ConsumeUserMedal=0
	SET @ConsumeCash=0
	SET @ConsumeLoveLiness=0
	SET @BuyResultsGold = 0

	-- �����ж�
	SELECT @PropertyName=Name, @GoldPrice=Gold, @CashPrice=Cash, @UserMedalPrice=UserMedal, @LoveLinessPrice=Loveliness,@BuyResultsGold=BuyResultsGold	FROM GameProperty(NOLOCK) WHERE Nullity=0 AND ID=@dwPropertyID
	IF @PropertyName IS NULL
	BEGIN
		SET @strErrorDescribe=N'����ĵ�����Ʒ�����ڻ�������ά���У�'
		RETURN 3
	END
	
	IF @dwItemCount = 0
	BEGIN
		SET @strErrorDescribe=N'����ĵ�����Ʒ�����ڻ�������ά���У�'
		RETURN 3
	END
	
	--��Ա�ۿ�
	SELECT @Discount=ShopRate FROM RYAccountsDBLink.RYAccountsDB.dbo.MemberProperty where MemberOrder=@CurrMemberOrder
	IF @Discount is null
	BEGIN
		SET @Discount=100
	END
	
	-- �������
	IF @cbConsumeType=1
	BEGIN
			
		-- ���ü���
		IF @CurrMemberOrder=0
		BEGIN
			SET @ConsumeGold=@GoldPrice*@dwItemCount
		END		
		ELSE
		BEGIN
			SET @ConsumeGold=@GoldPrice*@dwItemCount*@Discount/100
		END
		
		-- ��ȡ����
		SELECT @Gold=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID

		-- �����ж�
		IF @Gold<@ConsumeGold OR @Gold IS NULL
		BEGIN
			-- ������Ϣ
			SET @strErrorDescribe=N'������Ϸ�Ҳ��㣬��ѡ�������Ĺ���ʽ���������д����㹻����Ϸ�Һ��ٴγ��ԣ�'
			RETURN 4
		END

		-- ���п۷�
		UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET InsureScore=InsureScore-@ConsumeGold WHERE UserID=@dwUserID

	END

	--Ԫ������
	IF @cbConsumeType=2
	BEGIN

		-- ���ü���
		IF @CurrMemberOrder=0 
		BEGIN
			SET @ConsumeUserMedal=@UserMedalPrice*@dwItemCount
		END
		ELSE 
		BEGIN
			SET @ConsumeUserMedal=@UserMedalPrice*@dwItemCount*@Discount/100
		END
		
		-- ��ȡԪ��
		SELECT @UserMedal=UserMedal FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
				
		-- �����ж�
		IF @UserMedal<@ConsumeUserMedal OR @UserMedal IS NULL
		BEGIN

			-- ������Ϣ
			SET @strErrorDescribe=N'Ԫ�����㣬��ѡ�������Ĺ���ʽ���ٴγ��ԣ�'
			RETURN 5
		END

		-- ���п۷�
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo SET UserMedal=UserMedal-@ConsumeUserMedal WHERE UserID=@dwUserID

	END
	
	--��Ϸ������
	IF @cbConsumeType=3
	BEGIN
	
		-- ���ü���
		IF @CurrMemberOrder=0 
		BEGIN
			SET @ConsumeCash=@CashPrice*@dwItemCount
		END
		ELSE
		BEGIN
			SET @ConsumeCash=@CashPrice*@dwItemCount*@Discount/100.00
		END

		-- ��ȡ��Ϸ��
		SELECT @Cash=Currency FROM RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@dwUserID
		
		-- �����ж�
		IF @Cash<@ConsumeCash OR @Cash IS NULL
		BEGIN
		
			-- ������Ϣ
			SET @strErrorDescribe=N'��Ϸ�����㣬��ѡ�������Ĺ���ʽ���ٴγ��ԣ�'
			RETURN 6
		END

		-- ���п۷�
		UPDATE RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo SET Currency=@Cash-@ConsumeCash WHERE UserID=@dwUserID
		
	END
	--����ֵ����
	IF @cbConsumeType=4
	BEGIN

		-- ���ü���
		IF @CurrMemberOrder=0 
		BEGIN
			SET @ConsumeLoveLiness=@LoveLinessPrice*@dwItemCount
		END
		ELSE
		BEGIN
			SET @ConsumeLoveLiness=@LoveLinessPrice*@dwItemCount*@Discount/100
		END

		-- ��ȡ����ֵ
		SELECT @LoveLiness=LoveLiness FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
		-- �����ж�
		IF @LoveLiness<@ConsumeLoveLiness OR @LoveLiness IS NULL
		BEGIN
			-- ������Ϣ
			SET @strErrorDescribe=N'����ֵ���㣬��ѡ�������Ĺ���ʽ���ٴγ��ԣ�'
			RETURN 7
		END

		-- ���п۷�
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo SET LoveLiness=LoveLiness-@ConsumeLoveLiness WHERE UserID=@dwUserID


	END

    IF @BuyResultsGold <> 0
	BEGIN
	
		DECLARE @DateID INT
		SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
		-- ��ѯ���
		DECLARE @CurrScore BIGINT
		DECLARE @CurrInsure BIGINT
		DECLARE @CurrMedal INT
		SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo  WHERE UserID=@dwUserID
		SELECT @CurrMedal=UserMedal FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
		
		-- ���ͽ��
		SET @BuyResultsGold = @BuyResultsGold * @dwItemCount
		UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET InsureScore = InsureScore+@BuyResultsGold WHERE UserID = @dwUserID  
	
		-- ��ˮ��
		INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
		VALUES (@dwUserID,@CurrScore,@CurrInsure,@BuyResultsGold,17,@strClientIP,GETDATE())	
		
	    -- ��ͳ��
	    UPDATE RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@BuyResultsGold WHERE UserID=@dwUserID AND TypeID=17
		IF @@ROWCOUNT=0
		BEGIN
			INSERT RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,17,1,@BuyResultsGold)
		END	
		
	END


	--����ǹ���ķ������ߣ�����Ҫ�ı䷿����
	IF @dwPropertyID = 501
	BEGIN
		UPDATE RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard SET RoomCard = RoomCard + @dwItemCount  WHERE UserID=@dwUserID 
		IF @@ROWCOUNT =0
		BEGIN
			INSERT RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard(UserID,RoomCard)	VALUES(@dwUserID,@dwItemCount )
		END
	END 
	ELSE
	BEGIN
		--���뱳��
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage SET GoodShowID=0, GoodsCount=GoodsCount+ @dwItemCount, PushTime=GETDATE() WHERE UserID=@dwUserID AND GoodsID=@dwPropertyID
		IF @@ROWCOUNT = 0
		BEGIN
			INSERT RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsCount,PushTime)	VALUES(@dwUserID, @dwPropertyID,0,@dwItemCount,GETDATE())
		END
	END

	--�����¼
	INSERT RYRecordDBLink.RYRecordDB.dbo.RecordBuyProperty(UserID,PropertyID,PropertyName,Cash,Gold,UserMedal,LoveLiness,PropertyCount,MemberDiscount,BuyCash,BuyGold,BuyUserMedal,BuyLoveLiness,ClinetIP,CollectDate)
	VALUES(@dwUserID,@dwPropertyID,@PropertyName,@CashPrice,@GoldPrice,@UserMedalPrice,@LoveLinessPrice,@dwItemCount,@Discount,@ConsumeCash,@ConsumeGold,@ConsumeUserMedal,@ConsumeLoveLiness,@strClientIP,GETDATE())

	
	SELECT @Gold=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID
	SELECT @UserMedal=UserMedal,@LoveLiness=LoveLiness,@CurrMemberOrder=MemberOrder FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	SELECT @Cash=Currency FROM RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@dwUserID
	
	IF @Gold IS NULL SET @Gold=0
	IF @UserMedal IS NULL SET @UserMedal=0
	IF @Cash IS NULL SET @Cash=0
	IF @LoveLiness IS NULL SET @LoveLiness=0		
	SET @strErrorDescribe=N'��ϲ��������' + @PropertyName + N' ��' + CAST(@dwItemCount AS NVARCHAR) +N' ' + N'�ɹ���'
	
	--�����¼
	SELECT @dwPropertyID AS PropertyID,@dwItemCount AS ItemCount,@Gold AS Gold, @UserMedal AS UserMedal, @Cash AS Cash, @LoveLiness AS LoveLiness,@CurrMemberOrder AS MemberOrder
	RETURN 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

-- ����Buff
CREATE PROC GSP_GP_LoadUserGameBuff
	@dwUserID	INT						-- �û� I D
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	--ɾ������
	DELETE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfoPoperty WHERE UserID=@dwUserID and dateadd(second,UseResultsValidTime,UseTime) < GETDATE()
	--��Ч��ѯ
	SELECT * FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfoPoperty  WHERE UserID=@dwUserID and dateadd(second,UseResultsValidTime,UseTime) > GETDATE()
	RETURN 0
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

-- ����Buff
CREATE PROC GSP_GP_LoadUserGameTrumpet
	@dwUserID	INT						-- �û� I D
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	DECLARE @TrumpetCount INT 
	DECLARE @TyphonCount INT 
	SET @TrumpetCount = 0
	SET @TyphonCount = 0
	SELECT @TrumpetCount=GoodsCount FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=307
	SELECT @TyphonCount=GoodsCount FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=306
	SELECT @TrumpetCount as TrumpetCount, @TyphonCount as TyphonCount
END

RETURN 0

GO
----------------------------------------------------------------------------------------------------


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--���͵���
CREATE PROC GSP_GP_QuerySendPresent
	@dwUserID				INT			-- �û�ID
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	SELECT * FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsSendPresent AS a, GameProperty AS b 
	WHERE a.ReceiverUserID=@dwUserID and a.PropID = b.ID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--���͵��� ������ϷID
CREATE PROC GSP_GP_UserSendPresentByID
	@dwUserID				INT,			-- �������û�ID
	@dwReceiverGameID		INT,			-- ��������ϷID
	@dwPropID				INT,			-- ���͵ĵ���ID
	@dwPropCount			INT,			-- ���͵�����
	@strClientIP			NVARCHAR(15),	-- �����ߵ�IP 
	@strErrorDescribe NVARCHAR(127) OUTPUT	-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	--��ѯ�û�
	DECLARE @dwReceiverUserID INT
	SELECT @dwReceiverUserID=UserID FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE GameID=@dwReceiverGameID
	IF @dwReceiverUserID is null
	BEGIN
		set @strErrorDescribe = N'���Ҳ�����������Ϣ'
		RETURN 1
	END

	if @dwPropCount <= 0
	BEGIN
		set @strErrorDescribe = N'����ĵ���������Ϣ����'
		RETURN 2
	END
	
	IF @dwReceiverUserID=@dwUserID
	BEGIN
		set @strErrorDescribe = N'�����Լ����͸��Լ�'
		RETURN 3
	END
	
	--���ҵ���
	DECLARE @UserPropCount INT
	SELECT @UserPropCount=GoodsCount FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage where UserID=@dwUserID and GoodsID=@dwPropID
	IF @UserPropCount is null or @UserPropCount < @dwPropCount
	BEGIN
		set @strErrorDescribe = N'�����ߵı���û���㹻�ĸõ��ߣ���������'
		RETURN 4
	END
	
	--��������
	UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage set GoodsCount=GoodsCount-@dwPropCount where UserID=@dwUserID and GoodsID=@dwPropID
	
	--����״̬
	INSERT RYAccountsDBLink.RYAccountsDB.dbo.AccountsSendPresent(UserID, ReceiverUserID, PropID, PropCount, SendTime, PropStatus, ClientIP) 
	VALUES (@dwUserID, @dwReceiverUserID, @dwPropID, @dwPropCount, GETDATE(), 0, @strClientIP)
	
	--���ͼ�¼
	INSERT RYRecordDBLink.RYRecordDB.dbo.RecordUserSendPresent(UserID,ReceiverUserID,PropID,PropCount,SendTime,PropStatus,ClientIP)
	VALUES(@dwUserID,@dwReceiverUserID,@dwPropID,@dwPropCount,GETDATE(),0,@strClientIP)
	
	set @strErrorDescribe = N'���ͳɹ�'
	
	SELECT @dwReceiverUserID AS RecvUserID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--���͵��� ���� �ǳ�
CREATE PROC GSP_GP_UserSendPresentByNickName
	@dwUserID				INT,			-- �������û�ID
	@strReceiverNickName	NVARCHAR(16),	-- �������ǳ�
	@dwPropID				INT,			-- ���͵ĵ���ID
	@dwPropCount			INT,			-- ���͵�����
	@strClientIP			NVARCHAR(16),	-- �����ߵ�IP 
	@strErrorDescribe NVARCHAR(64) OUTPUT	-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	--��ѯ�û�
	DECLARE @dwReceiverUserID INT
	SELECT @dwReceiverUserID=UserID FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE NickName=@strReceiverNickName
	IF @dwReceiverUserID is null
	BEGIN
		set @strErrorDescribe = N'���Ҳ�����������Ϣ'
		RETURN 1
	END
	
	if @dwPropCount <= 0
	BEGIN
		set @strErrorDescribe = N'����ĵ���������Ϣ����'
		RETURN 2
	END
	
	IF @dwReceiverUserID=@dwUserID
	BEGIN
		set @strErrorDescribe = N'�����Լ����͸��Լ�'
		RETURN 3
	END
	
	--���������߱�����û�и������ĵ���
	DECLARE @UserPropCount INT
	SELECT @UserPropCount=GoodsCount FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage where UserID=@dwUserID and GoodsID=@dwPropID
	IF @UserPropCount is null or @UserPropCount < @dwPropCount
	BEGIN
		set @strErrorDescribe = N'�����ߵı���û���㹻�ĸõ��ߣ���������'
		RETURN 4
	END
	
	--���������߱����ĵ���
	UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage set GoodsCount=GoodsCount-@dwPropCount where UserID=@dwUserID and GoodsID=@dwPropID
	
	--д�û�����״̬
	INSERT RYAccountsDBLink.RYAccountsDB.dbo.AccountsSendPresent(UserID, ReceiverUserID, PropID, PropCount, SendTime, PropStatus, ClientIP) 
	VALUES (@dwUserID, @dwReceiverUserID, @dwPropID, @dwPropCount, GETDATE(), 0, @strClientIP)
	
	--���ͼ�¼
	INSERT RYRecordDBLink.RYRecordDB.dbo.RecordUserSendPresent(UserID,ReceiverUserID,PropID,PropCount,SendTime,PropStatus,ClientIP)
	VALUES(@dwUserID,@dwReceiverUserID,@dwPropID,@dwPropCount,GETDATE(),0,@strClientIP)
	
	set @strErrorDescribe = N'���ͳɹ�'
	
	SELECT @dwReceiverUserID AS RecvUserID
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

--��ȡ����
SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--���͵���
CREATE PROC GSP_GP_GetSendPresent
	@dwUserID				INT,			-- �û�ID
	@szPassword			NVARCHAR(33),		-- �û�����
	@strClientIP		NVARCHAR(15)		-- �ͻ�IP 
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	DECLARE @Password NVARCHAR(33)
	SELECT @Password = LogonPass FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo where UserID=@dwUserID
	--��ѯ����
	IF @Password is null
		RETURN 1
	
	--�������
	IF @szPassword != @Password
		RETURN 2
	
	DECLARE @PresentCount INT
	SELECT @PresentCount = COUNT(*) FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsSendPresent
	WHERE ReceiverUserID=@dwUserID and PropStatus=0
	--û����Ʒ
	IF @PresentCount = 0
		RETURN 3
	
	--���뱳��
	DECLARE @PropID INT
	DECLARE @PropCount INT
	DECLARE @ItemCount INT
	DECLARE auth_cur CURSOR FOR
	SELECT PropID,PropCount FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsSendPresent AS a, GameProperty AS b 
	WHERE a.ReceiverUserID=@dwUserID and a.PropID = b.ID and a.PropStatus=0
	OPEN auth_cur
	FETCH NEXT FROM auth_cur INTO @PropID,@PropCount
	WHILE (@@fetch_status=0)
	BEGIN
		SELECT @ItemCount=COUNT(*) FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=@PropID
		IF @ItemCount = 0
		BEGIN
			INSERT RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage(UserID,GoodsID,GoodShowID,GoodsSortID,GoodsCount,PushTime)
			VALUES(@dwUserID,@PropID,0,0,@PropCount,GETDATE())
		END
		ELSE
		BEGIN
			UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage SET GoodsCount=GoodsCount+@PropCount
			WHERE UserID=@dwUserID and GoodsID=@PropID
		END
		FETCH NEXT FROM auth_cur INTO @PropID,@PropCount
	END
	close auth_cur
	deallocate auth_cur
	
	--����״̬
	UPDATE RYRecordDBLink.RYRecordDB.dbo.RecordUserSendPresent SET ReceiveTime=GETDATE(), ReceiverClientIP=@strClientIP, PropStatus=1 
	WHERE  ReceiverUserID = @dwUserID and PropStatus=0
	
	--��ѯ����
	SELECT * FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsSendPresent AS a, GameProperty AS b 
	WHERE a.ReceiverUserID=@dwUserID and a.PropID = b.ID and a.PropStatus=0
	
	--ɾ��״̬
	DELETE RYAccountsDBLink.RYAccountsDB.dbo.AccountsSendPresent WHERE  ReceiverUserID=@dwUserID and PropStatus=0
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

--���͵���
CREATE PROC GSP_GP_PropertQuerySingle
	@dwUserID INT,								-- �û���ʶ
	@dwPropertyID INT,							-- ���߱�ʶ
	@strPassword NCHAR(32)						-- �û�����
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	DECLARE @Nullity INT
	SELECT @Nullity=Nullity FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID AND LogonPass=@strPassword

	-- �û��ж�
	IF @Nullity IS NULL
	BEGIN
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		RETURN 2
	END
	DECLARE @GoodsCount INT
	SET @GoodsCount = 0
	SELECT @GoodsCount = GoodsCount	FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsPackage WHERE UserID=@dwUserID and GoodsID=@dwPropertyID
	
	SELECT @dwPropertyID AS PropertyID,@GoodsCount AS ItemCount
	
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------