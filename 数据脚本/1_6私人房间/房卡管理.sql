
----------------------------------------------------------------------------------------------------

USE RYTreasureDB
GO



IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_ExchangeRoomCardByScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_ExchangeRoomCardByScore]
GO




----------------------------------------------------------------------------------------------------

-- �һ���Ϸ��
CREATE PROC GSP_GR_ExchangeRoomCardByScore

	-- �û���Ϣ
	@dwUserID INT,								-- �û���ʶ
	@ExchangeRoomCard INT,						-- �һ�����	

	-- ϵͳ��Ϣ	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strNotifyContent NVARCHAR(127) OUTPUT		-- �����Ϣ

WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ��Ա
	DECLARE @Nullity BIT
	DECLARE @UserRoomCard INT	
	SELECT @Nullity=Nullity FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	SELECT @UserRoomCard=RoomCard FROM UserRoomCard WHERE UserID=@dwUserID

	-- �û��ж�
	IF @UserRoomCard IS NULL
	BEGIN
		SET @strNotifyContent=N'��û�з����빺��'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strNotifyContent=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END

	-- Ԫ���ж�
	IF @UserRoomCard < @ExchangeRoomCard
	BEGIN
		SET @strNotifyContent=N'���ķ������㣬������öһ����������ԣ�'
		RETURN 3		
	END

	-- �һ�����
	DECLARE @ExchangeRate INT
	--SELECT @ExchangeRate=StatusValue FROM RYAccountsDBLink.RYAccountsDB.dbo.SystemStatusInfo WHERE StatusName=N'GeneralRoomCardExchRate'
	SELECT @ExchangeRate=UseResultsGold FROM RYPlatformDBLink.RYPlatformDB.dbo.GameProperty WHERE ID=501

	-- ϵͳ����
	IF @ExchangeRate IS NULL
	BEGIN
		SET @strNotifyContent=N'��Ǹ�������һ�ʧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4			
	END

	-- ������Ϸ��
	DECLARE @ExchangeScore BIGINT
	SET @ExchangeScore = @ExchangeRate*@ExchangeRoomCard

	-- ��ѯ����
	DECLARE @InsureScore BIGINT
	SELECT @InsureScore=InsureScore FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��������
	IF @InsureScore IS NULL
	BEGIN
		-- ���ñ���
		SET @InsureScore=0

		-- ��������
		INSERT INTO GameScoreInfo (UserID, LastLogonIP, LastLogonMachine, RegisterIP, RegisterMachine)
		VALUES (@dwUserID, @strClientIP, @strMachineID, @strClientIP, @strMachineID)		
	END	

	-- ��������
	UPDATE GameScoreInfo SET InsureScore=InsureScore+@ExchangeScore WHERE UserID=@dwUserID	
	
	-- ����Ԫ��
	SET @UserRoomCard=@UserRoomCard-@ExchangeRoomCard
	UPDATE UserRoomCard SET RoomCard=@UserRoomCard WHERE UserID=@dwUserID			

	-- ��ѯ��Ϸ��
	DECLARE @CurrScore BIGINT
	SELECT @CurrScore=Score FROM GameScoreInfo WHERE UserID=@dwUserID

	-- ��ѯ��Ϸ��
	DECLARE @CurrRoomCard decimal(18, 2)
	SELECT @CurrRoomCard=RoomCard FROM UserRoomCard WHERE UserID=@dwUserID	

	-- ������Ϸ��
	IF @CurrRoomCard IS NULL SET @CurrRoomCard=0


	--��ȡ��Һ���Ϸ��
	DECLARE @lGold  BIGINT
	DECLARE @Currency  DECIMAL
	SELECT  @lGold = Score FROM GameScoreInfo WHERE UserID = @dwUserID
	SELECT  @Currency = Currency FROM UserCurrencyInfo WHERE UserID = @dwUserID
	IF @Currency IS NULL SET @Currency=0

	INSERT INTO RYRecordDB..RecordRoomCard(SourceUserID, SBeforeCard, RoomCard, TargetUserID, TBeforeCard, TypeID, Currency, Gold, Remarks, ClientIP, CollectDate)
	VALUES (@dwUserID, @UserRoomCard, @ExchangeRoomCard, 0, 0, 2, @Currency, @lGold, '�һ�����', @strClientIP, GETDATE())
		

	-- �ɹ���ʾ
	SET @strNotifyContent=N'��ϲ������Ϸ�Ҷһ��ɹ���'

	-- �����¼
	SELECT @CurrRoomCard AS RoomCard,@CurrScore AS CurrScore

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


----------------------------------------------------------------------------------------------------