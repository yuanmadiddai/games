
----------------------------------------------------------------------------------------------------

USE RYAccountsDB
GO



IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RealAuth]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RealAuth]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

-- ʵ����֤
CREATE PROC GSP_GP_RealAuth
	@dwUserID INT,								-- �û� I D	
	@strPassword NCHAR(32),						-- �û�����
	@strCompellation NVARCHAR(16),				-- ��ʵ����
	@strPassPortID NVARCHAR(18),				-- ֤������
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ��Ϣ
	DECLARE @Password NCHAR(32)
	SELECT @Password=LogonPass FROM AccountsInfo WHERE UserID=@dwUserID

	-- �û��ж�
	IF @Password IS NULL
	BEGIN 
		SET @strErrorDescribe=N'�û���Ϣ�����ڣ���֤ʧ�ܣ�'
		RETURN 1
	END

	-- �����ж�
	IF @Password<>@strPassword
	BEGIN 
		SET @strErrorDescribe=N'�ʺ����벻��ȷ����֤ʧ�ܣ�'
		RETURN 3
	END

	--����֤
	IF LEN(@strPassPortID) = 0 OR LEN(@strCompellation) = 0
	BEGIN
		SET @strErrorDescribe=N'֤���������ʵ��������ȷ����֤ʧ�ܣ�'
		RETURN 4	
	END	
	
	--�ظ���֤
	IF EXISTS (SELECT UserID FROM RYRecordDBLink.RYRecordDB.dbo.RecordAuthentPresent WHERE UserID=@dwUserID)
	BEGIN
		SET @strErrorDescribe=N'�Ѿ���֤����֤ʧ�ܣ�'
		RETURN 5		
	END
	
	-- ʵ���޸�
	UPDATE AccountsInfo SET  Compellation=@strCompellation, PassPortID=@strPassPortID 	WHERE UserID=@dwUserID
	
	-- �ɹ��ж�
	IF @@ROWCOUNT=0
	BEGIN
		SET @strErrorDescribe=N'ִ��ʵ����֤��������ϵ�ͻ��������ģ�'
		RETURN 6
	END
	
	-- ʵ����¼
	INSERT INTO RYRecordDBLink.RYRecordDB.dbo.RecordAuthentPresent(UserID,PassPortID,Compellation,IpAddress) VALUES(@dwUserID,@strPassPortID,@strCompellation,@strClientIP)	

	--��ѯ����
	DECLARE @AuthentPresentCount AS BIGINT
	SELECT @AuthentPresentCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'AuthentPresent'
	IF @AuthentPresentCount IS NULL or @AuthentPresentCount=0
	BEGIN
		SET @strErrorDescribe=N'ʵ����֤�ɹ���'
		RETURN 0		
	END	
	
	-- ��ѯ���
	DECLARE @CurrScore BIGINT
	DECLARE @CurrInsure BIGINT
	DECLARE @CurrMedal INT
	SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo  WHERE UserID=@dwUserID
	SELECT @CurrMedal=UserMedal FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@dwUserID
	
	
	--���Ž���
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	-- ���ͽ��
	UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET Score=Score+@AuthentPresentCount WHERE UserID=@dwUserID	
	
	-- ��ˮ��
	INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
	VALUES (@dwUserID,@CurrScore,@CurrInsure,@AuthentPresentCount,8,@strClientIP,GETDATE())
	
	-- ��ͳ��
	UPDATE RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@AuthentPresentCount WHERE UserID=@dwUserID AND TypeID=8
	IF @@ROWCOUNT=0
	BEGIN
		INSERT RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@dwUserID,8,1,@AuthentPresentCount)
	END	
	-- ��Ϸ��Ϣ
	DECLARE @SourceScore BIGINT
	SELECT @SourceScore=Score FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@dwUserID	
	SELECT @SourceScore AS SourceScore
	SET @strErrorDescribe=N'ʵ����֤�ɹ����������'+ CAST(@AuthentPresentCount AS NVARCHAR)  +N'��'			 							
	RETURN 0

END

GO

----------------------------------------------------------------------------------------------------
