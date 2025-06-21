
----------------------------------------------------------------------------------------------------

USE RYAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_RegisterAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_RegisterAccounts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_RegisterAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_RegisterAccounts]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �ʺ�ע��
CREATE PROC GSP_GP_RegisterAccounts
	@strAccounts NVARCHAR(31),					-- �û��ʺ�
	@strNickName NVARCHAR(31),					-- �û��ǳ�
	@dwSpreaderGameID INT,						-- �����ʶ
	@strLogonPass NCHAR(32),					-- ��¼����
	@wFaceID SMALLINT,							-- ͷ���ʶ
	@cbGender TINYINT,							-- �û��Ա�
	@strPassPortID NVARCHAR(18),				-- ���֤��
	@strCompellation NVARCHAR(16),				-- ��ʵ����
	@dwAgentID INT,								-- �����ʶ	
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @Gender TINYINT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @MoorMachine TINYINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @DynamicPass NCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)

-- ������Ϣ
DECLARE @GameID INT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @SpreaderID INT
DECLARE @AgentID INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT
DECLARE @EnjoinRegister AS INT

-- ִ���߼�
BEGIN
	-- ע����ͣ
	SELECT @EnjoinRegister=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
		RETURN 1
	END

	-- ��¼��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч������
	IF EXISTS (SELECT [String] FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strAccounts)>0 AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL))
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ������������ĵ�¼�ʺ������������ַ�����������ʺ������ٴ������ʺţ�'
		RETURN 4
	END

	-- Ч���ǳ�
	IF EXISTS (SELECT [String] FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickname)>0 AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL))
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ���������������Ϸ�ǳ������������ַ�����������ǳ������ٴ������ʺţ�'
		RETURN 4
	END

	-- Ч���ַ
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ��ע�Ṧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 5
	END
	
	-- Ч�����
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�����ע�Ṧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 6
	END
 
 	-- У��Ƶ��
 	DECLARE @LimitRegisterIPCount INT
 	DECLARE @CurrRegisterCountIP INT
 	SET @LimitRegisterIPCount = 0
 	SET @CurrRegisterCountIP = 0
 	SELECT @LimitRegisterIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'LimitRegisterIPCount'
 	SELECT @CurrRegisterCountIP = COUNT(RegisterIP) FROM AccountsInfo WHERE RegisterIP=@strClientIP AND DateDiff(hh,RegisterDate,GetDate())<24
 	IF @LimitRegisterIPCount <>0
 	BEGIN
 		IF @LimitRegisterIPCount<=@CurrRegisterCountIP 
		BEGIN
			SET @strErrorDescribe = N'��Ǹ��֪ͨ�������Ļ�����ǰע�ᳬ���������ƣ�'
			RETURN 10
		END		 	
 	END 

 	
 	-- У��Ƶ��
  	DECLARE @LimitRegisterMachineCount INT
 	DECLARE @CurrRegisterCountMachine INT
 	SET @LimitRegisterMachineCount = 0
 	SET @CurrRegisterCountMachine = 0	
 	SELECT @LimitRegisterMachineCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'LimitRegisterMachineCount'
 	SELECT @CurrRegisterCountMachine = COUNT(RegisterMachine) FROM AccountsInfo WHERE RegisterMachine=@strMachineID AND DateDiff(hh,RegisterDate,GetDate())<24
 	IF @LimitRegisterMachineCount <>0
 	BEGIN 
  		IF @LimitRegisterMachineCount<=@CurrRegisterCountMachine 
		BEGIN
			SET @strErrorDescribe = N'��Ǹ��֪ͨ�������Ļ�����ǰע�ᳬ���������ƣ�'
			RETURN 10
		END	
	END
	
	-- ��ѯ�ʺ�
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts)
	BEGIN
		SET @strErrorDescribe=N'���ʺ��ѱ�ע�ᣬ�뻻��һ�ʺų����ٴ�ע�ᣡ'
		RETURN 7
	END

	-- ��ѯ�ǳ�
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	BEGIN
		SET @strErrorDescribe=N'���ǳ��ѱ�ע�ᣬ�뻻��һ�ǳƳ����ٴ�ע�ᣡ'
		RETURN 7
	END

	-- ���ƹ�Ա
	IF @dwSpreaderGameID<>0
	BEGIN
		-- ���ƹ�Ա
		SELECT @SpreaderID=UserID,@AgentID=AgentID FROM AccountsInfo(NOLOCK) WHERE GameID=@dwSpreaderGameID
		
		-- �������
		IF @SpreaderID IS NULL
		BEGIN
			SET @strErrorDescribe=N'������д���Ƽ��˲����ڻ�����д����������ٴ�ע�ᣡ'
			RETURN 8			
		END
	END
	ELSE SET @SpreaderID=0
	
	IF @SpreaderID=0
	BEGIN
			--��ѯ����
			SELECT @SpreaderID = UserID FROM AccountsAgent  where AgentID = @dwAgentID
				
	END

	SET @dwAgentID = 0	

	-- ע���û�
	INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,DynamicPass,DynamicPassTime,SpreaderID,PassPortID,Compellation,Gender,FaceID,
		GameLogonTimes,LastLogonIP,LastLogonMachine,RegisterIP,RegisterMachine,RegisterOrigin,AgentID)
	VALUES (@strAccounts,@strNickName,@strAccounts,@strLogonPass,CONVERT(nvarchar(32),REPLACE(newid(),'-','')),GetDate(),@SpreaderID,
		@strPassPortID,@strCompellation,@cbGender,@wFaceID,1,@strClientIP,@strMachineID,@strClientIP,@strMachineID,0,@dwAgentID)

	-- �����ж�
	IF @@ERROR<>0
	BEGIN
		SET @strErrorDescribe=N'�ʺ��Ѵ��ڣ��뻻��һ�ʺ����ֳ����ٴ�ע�ᣡ'
		RETURN 8
	END

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName,@DynamicPass=DynamicPass,@UnderWrite=UnderWrite, @FaceID=FaceID,
		@CustomID=CustomID, @Gender=Gender, @UserMedal=UserMedal, @Experience=Experience, @LoveLiness=LoveLiness, @MemberOrder=MemberOrder,
		@MemberOverDate=MemberOverDate, @MoorMachine=MoorMachine
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- �����ʶ
	SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
	IF @GameID IS NULL 
	BEGIN
		SET @GameID=0
		SET @strErrorDescribe=N'�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡'
	END
	ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

	-- ��������
	INSERT	IndividualDatum(UserID,QQ,EMail,SeatPhone,MobilePhone,DwellingPlace,PostalCode,CollectDate,UserNote) VALUES(@UserID,N'',N'',N'',N'',N'',N'',GetDate(),N'')
	
	-- �ƹ����,�����̲�����
	IF @SpreaderID<>0 AND @AgentID=0
	BEGIN
		DECLARE @RegisterGrantScore INT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'ע��'
		SELECT @RegisterGrantScore = RegisterGrantScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GlobalSpreadInfo
		IF @RegisterGrantScore IS NULL
		BEGIN
			SET @RegisterGrantScore=5000
		END
		INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@RegisterGrantScore,1,@UserID,@Note)		
	END

	-- ��¼��־
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

	--���ֹ���
	INSERT RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP,RegisterMachine) VALUES (@UserID, 0, @strClientIP, @strClientIP,@strMachineID) 

	----------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------
	-- ע������

	-- ��ȡ����
	DECLARE @GrantIPCount AS BIGINT
	DECLARE @GrantScoreCount AS BIGINT
	SELECT @GrantIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantIPCount'
	SELECT @GrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantScoreCount'

	-- ��������
	IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0 AND @GrantIPCount IS NOT NULL AND @GrantIPCount>0
	BEGIN
		-- ���ʹ���
		DECLARE @GrantCount AS BIGINT
		DECLARE @GrantMachineCount AS BIGINT
		DECLARE @GrantRoomCardCount AS BIGINT
		SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
		SELECT @GrantMachineCount=GrantCount FROM SystemMachineGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterMachine=@strMachineID
		SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantRoomCardCount'

		-- �����ж�
		IF (@GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount) OR (@GrantMachineCount IS NOT NULL AND @GrantMachineCount>=@GrantIPCount)
		BEGIN
			SET @GrantScoreCount=0
		END
	END

	-- ���ͽ��
	IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0
	BEGIN
		-- ���¼�¼
		UPDATE SystemGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterIP=@strClientIP

		-- �����¼
		IF @@ROWCOUNT=0
		BEGIN
			INSERT SystemGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, @strMachineID, @GrantScoreCount, 1)
		END

		-- ���¼�¼
		UPDATE SystemMachineGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterMachine=@strMachineID

		-- �����¼
		IF @@ROWCOUNT=0
		BEGIN
			INSERT SystemMachineGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, @strMachineID, @GrantScoreCount, 1)
		END

		-- ��ѯ���
		DECLARE @CurrScore BIGINT
		DECLARE @CurrInsure BIGINT
		DECLARE @CurrMedal INT
		SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo  WHERE UserID=@UserID
		SELECT @CurrMedal=UserMedal FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@UserID
	
		-- ���ͽ��
		UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET Score = Score+@GrantScoreCount WHERE UserID = @UserID AND RegisterIP = @strClientIP AND RegisterMachine = @strMachineID
	
		-- ��ˮ��
		INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
		VALUES (@UserID,@CurrScore,@CurrInsure,@GrantScoreCount,1,@strClientIP,GETDATE())	
		
	    -- ��ͳ��
	    UPDATE RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@GrantScoreCount WHERE UserID=@UserID AND TypeID=1
		IF @@ROWCOUNT=0
		BEGIN
			INSERT RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@UserID,1,1,@GrantScoreCount)
		END	
			
	END

	-- ���ͷ�������
	IF @GrantRoomCardCount IS NOT NULL AND @GrantRoomCardCount>0 AND @GrantIPCount IS NOT NULL AND @GrantIPCount>0
	BEGIN
		-- ���ʹ���
		--DECLARE @GrantCount AS BIGINT
		--DECLARE @GrantMachineCount AS BIGINT
		SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
		SELECT @GrantMachineCount=GrantCount FROM SystemMachineGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterMachine=@strMachineID
	
		-- �����ж�
		IF (@GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount) OR (@GrantMachineCount IS NOT NULL AND @GrantMachineCount>=@GrantIPCount)
		BEGIN
			SET @GrantRoomCardCount=0
		END
	END


	--�жϱ����͵�����Ƿ��Ǵ����û�
	DECLARE @Nullity INT
	DECLARE @GetSpreaderID INT
	SELECT @GetSpreaderID=SpreaderID FROM AccountsInfo WHERE UserID=@USerID
	SELECT @Nullity=Nullity FROM AccountsAgent WHERE UserID=@GetSpreaderID

	IF EXISTS(SELECT AgentID FROM AccountsAgent WHERE UserID=@GetSpreaderID  AND @Nullity = 0)
	BEGIN
		SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantAgentRoomCardCount'
	END

	-- ���ͷ���
	IF @GrantRoomCardCount IS NOT NULL AND @GrantRoomCardCount>0
	BEGIN
		-- ���ͷ���
		INSERT RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard (UserID, RoomCard) VALUES (@UserID, @GrantRoomCardCount) 
		-- �����仯��¼
		--INSERT INTO RYRecordDB..RecordRoomCard(SourceUserID, SourceRoomCard, RoomCard, TargetUserID, TargetRoomCard, TradeType, TradeRemark, RoomID, ClientIP, CollectDate)
		--VALUES (@UserID, 0, @GrantRoomCardCount, 0, 0, 1, 'ע�����ͷ���', '', @strClientIP, GETDATE())

	END
	----------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- ��ѯ��Ϸ��
	SELECT @Beans=Currency FROM RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID	

	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
    IF @Beans IS NULL SET @Beans=0

	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID) SET @IsAgent=1
	
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@DynamicPass AS DynamicPass,@UnderWrite AS UnderWrite,
		@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS Ingot, @Beans AS Beans, @Experience AS Experience,
		@Score AS Score, @Insure AS Insure, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, @MemberOverDate AS MemberOverDate,
		@MoorMachine AS MoorMachine,0 AS InsureEnabled,@IsAgent AS IsAgent

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ʺ�ע��
CREATE PROC GSP_MB_RegisterAccounts
	@strAccounts NVARCHAR(31),					-- �û��ʺ�
	@strNickName NVARCHAR(31),					-- �û��ǳ�
	@dwSpreaderGameID INT,						-- �����ʶ
	@strLogonPass NCHAR(32),					-- ��¼����
	@wFaceID SMALLINT,							-- ͷ���ʶ
	@cbGender TINYINT,							-- �û��Ա�
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NCHAR(32),					-- ������ʶ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@cbDeviceType TINYINT,						-- ע����Դ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @Gender TINYINT
DECLARE @FaceID SMALLINT
DECLARE @CustomID INT
DECLARE @MoorMachine TINYINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @DynamicPass NCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)
DECLARE @RoomCard bigint

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @SpreaderID INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT
DECLARE @EnjoinRegister AS INT

-- ִ���߼�
BEGIN
	-- ע����ͣ
	SELECT @EnjoinRegister=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinRegister'
		RETURN 1
	END

	-- ��¼��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч������
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strAccounts)>0)>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ������������ĵ�¼�ʺ������������ַ�����������ʺ������ٴ������ʺţ�'
		RETURN 4
	END

	-- Ч���ǳ�
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ���������������Ϸ�ǳ������������ַ�����������ǳ������ٴ������ʺţ�'
		RETURN 4
	END

	-- Ч���ַ
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ��ע�Ṧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 5
	END
	
	-- Ч�����
	SELECT @EnjoinRegister=EnjoinRegister FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinRegister IS NOT NULL AND @EnjoinRegister<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ�����ע�Ṧ�ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 6
	END

 	-- У��Ƶ��
 	DECLARE @LimitRegisterIPCount INT
 	DECLARE @CurrRegisterCountIP INT
 	SET @LimitRegisterIPCount = 0
 	SET @CurrRegisterCountIP = 0
 	SELECT @LimitRegisterIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'LimitRegisterIPCount'
 	SELECT @CurrRegisterCountIP = COUNT(RegisterIP) FROM AccountsInfo WHERE RegisterIP=@strClientIP AND DateDiff(hh,RegisterDate,GetDate())<24
 	IF @LimitRegisterIPCount <>0
 	BEGIN
 		IF @LimitRegisterIPCount<=@CurrRegisterCountIP 
		BEGIN
			SET @strErrorDescribe = N'��Ǹ��֪ͨ�������Ļ�����ǰע�ᳬ���������ƣ�'
			RETURN 10
		END		 	
 	END 

 	
 	-- У��Ƶ��
  	DECLARE @LimitRegisterMachineCount INT
 	DECLARE @CurrRegisterCountMachine INT
 	SET @LimitRegisterMachineCount = 0
 	SET @CurrRegisterCountMachine = 0	
 	SELECT @LimitRegisterMachineCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'LimitRegisterMachineCount'
 	SELECT @CurrRegisterCountMachine = COUNT(RegisterMachine) FROM AccountsInfo WHERE RegisterMachine=@strMachineID AND DateDiff(hh,RegisterDate,GetDate())<24
 	IF @LimitRegisterMachineCount <>0
 	BEGIN 
  		IF @LimitRegisterMachineCount<=@CurrRegisterCountMachine 
		BEGIN
			SET @strErrorDescribe = N'��Ǹ��֪ͨ�������Ļ�����ǰע�ᳬ���������ƣ�'
			RETURN 10
		END	
	END
	
	-- ��ѯ�ʺ�
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts)
	BEGIN
		SET @strErrorDescribe=N'���ʺ��ѱ�ע�ᣬ�뻻��һ�ʺų����ٴ�ע�ᣡ'
		RETURN 7
	END

	-- ��ѯ�ǳ�
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName)
	BEGIN
		SET @strErrorDescribe=N'���ǳ��ѱ�ע�ᣬ�뻻��һ�ǳƳ����ٴ�ע�ᣡ'
		RETURN 7
	END

	-- ���ƹ�Ա
	IF 	@dwSpreaderGameID<>0
	BEGIN
		-- ���ƹ�Ա
		SELECT @SpreaderID=UserID FROM AccountsInfo(NOLOCK) WHERE GameID=@dwSpreaderGameID
		
		-- �������
		IF @SpreaderID IS NULL
		BEGIN
			SET @strErrorDescribe=N'������д���Ƽ��˲����ڻ�����д����������ٴ�ע�ᣡ'
			RETURN 8			
		END
	END
	ELSE SET @SpreaderID=0
	
	-- ע���û�
	INSERT AccountsInfo (Accounts,NickName,RegAccounts,LogonPass,DynamicPass,SpreaderID,Gender,FaceID,GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine,RegisterOrigin,AgentID)
	VALUES (@strAccounts,@strNickName,@strAccounts,@strLogonPass,CONVERT(nvarchar(32),REPLACE(newid(),'-','')),@SpreaderID,@cbGender,@wFaceID,1,@strClientIP,@strMobilePhone,@strMachineID,@strClientIP,@strMobilePhone,@strMachineID,@cbDeviceType,0)

	-- �����ж�
	IF @@ERROR<>0
	BEGIN
		SET @strErrorDescribe=N'���������ԭ���ʺ�ע��ʧ�ܣ��볢���ٴ�ע�ᣡ'
		RETURN 8
	END

	-- ��ѯ�û�
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName,@DynamicPass=DynamicPass,@UnderWrite=UnderWrite, @FaceID=FaceID,
		@CustomID=CustomID, @Gender=Gender, @UserMedal=UserMedal, @Experience=Experience, @LoveLiness=LoveLiness, @MemberOrder=MemberOrder,
		@MemberOverDate=MemberOverDate, @MoorMachine=MoorMachine
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- �����ʶ
	SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
	IF @GameID IS NULL 
	BEGIN
		SET @GameID=0
		SET @strErrorDescribe=N'�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡'
	END
	ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

	-- �ƹ����
	IF @SpreaderID<>0
	BEGIN
		DECLARE @RegisterGrantScore INT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'ע��'
		SELECT @RegisterGrantScore = RegisterGrantScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GlobalSpreadInfo
		IF @RegisterGrantScore IS NULL
		BEGIN
			SET @RegisterGrantScore=5000
		END
		INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@RegisterGrantScore,1,@UserID,@Note)		
	END

	-- ��¼��־
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

	--���ֹ���
	INSERT RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP,RegisterMachine) VALUES (@UserID, 0, @strClientIP, @strClientIP,@strMachineID) 

	----------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------
	-- ע������

	-- ��ȡ����
	DECLARE @GrantIPCount AS BIGINT
	DECLARE @GrantScoreCount AS BIGINT
	DECLARE @GrantRoomCardCount AS BIGINT
	SELECT @GrantIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantIPCount'
	SELECT @GrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantScoreCount'
	SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantRoomCardCount'

	-- ��������
	IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0 AND @GrantIPCount IS NOT NULL AND @GrantIPCount>0
	BEGIN
		-- ���ʹ���
		DECLARE @GrantCount AS BIGINT
		DECLARE @GrantMachineCount AS BIGINT
		SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
		SELECT @GrantMachineCount=GrantCount FROM SystemMachineGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterMachine=@strMachineID
	
		-- �����ж�
		IF (@GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount) OR (@GrantMachineCount IS NOT NULL AND @GrantMachineCount>=@GrantIPCount)
		BEGIN
			SET @GrantScoreCount=0
		END
	END

	-- ���ͽ��
	IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0
	BEGIN
		-- ���¼�¼
		UPDATE SystemGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterIP=@strClientIP

		-- �����¼
		IF @@ROWCOUNT=0
		BEGIN
			INSERT SystemGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, @strMachineID, @GrantScoreCount, 1)
		END

		-- ���¼�¼
		UPDATE SystemMachineGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterMachine=@strMachineID

		-- �����¼
		IF @@ROWCOUNT=0
		BEGIN
			INSERT SystemMachineGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, @strMachineID, @GrantScoreCount, 1)
		END

		-- ��ѯ���
		DECLARE @CurrScore BIGINT
		DECLARE @CurrInsure BIGINT
		DECLARE @CurrMedal INT
		SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo  WHERE UserID=@UserID
		SELECT @CurrMedal=UserMedal FROM RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo WHERE UserID=@UserID
		
		-- ���ͽ��
		UPDATE RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo SET Score = Score+@GrantScoreCount WHERE UserID = @UserID AND RegisterIP = @strClientIP AND RegisterMachine = @strMachineID
		
		-- ��ˮ��
		INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordPresentInfo(UserID,PreScore,PreInsureScore,PresentScore,TypeID,IPAddress,CollectDate)
		VALUES (@UserID,@CurrScore,@CurrInsure,@GrantScoreCount,1,@strClientIP,GETDATE())			
		
	    -- ��ͳ��
	    UPDATE RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo SET DateID=@DateID, PresentCount=PresentCount+1,PresentScore=PresentScore+@GrantScoreCount WHERE UserID=@UserID AND TypeID=1
		IF @@ROWCOUNT=0
		BEGIN
			INSERT RYTreasureDBLink.RYTreasureDB.dbo.StreamPresentInfo(DateID,UserID,TypeID,PresentCount,PresentScore) VALUES(@DateID,@UserID,1,1,@GrantScoreCount)
		END		
	END


		-- ���ͷ�������
	IF @GrantRoomCardCount IS NOT NULL AND @GrantRoomCardCount>0 AND @GrantIPCount IS NOT NULL AND @GrantIPCount>0
	BEGIN
		-- ���ʹ���
		--DECLARE @GrantCount AS BIGINT
		--DECLARE @GrantMachineCount AS BIGINT
		SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
		SELECT @GrantMachineCount=GrantCount FROM SystemMachineGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterMachine=@strMachineID
	
		-- �����ж�
		IF (@GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount) OR (@GrantMachineCount IS NOT NULL AND @GrantMachineCount>=@GrantIPCount)
		BEGIN
			SET @GrantRoomCardCount=0
		END
	END

	--�жϱ����͵�����Ƿ��Ǵ����û�
	DECLARE @Nullity INT
	DECLARE @GetSpreaderID INT
	SELECT @GetSpreaderID=SpreaderID FROM AccountsInfo WHERE UserID=@USerID
	SELECT @Nullity=Nullity FROM AccountsAgent WHERE UserID=@GetSpreaderID

	IF EXISTS(SELECT AgentID FROM AccountsAgent WHERE UserID=@GetSpreaderID AND @Nullity = 0)
	BEGIN
		SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantAgentRoomCardCount'
	END

	-- ���ͷ���
	IF @GrantRoomCardCount IS NOT NULL AND @GrantRoomCardCount>0
	BEGIN
		-- ���ͷ���
		INSERT RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard (UserID, RoomCard) VALUES (@UserID, @GrantRoomCardCount) 
		-- �����仯��¼
		--INSERT INTO RYRecordDB..RecordRoomCard(SourceUserID, SourceRoomCard, RoomCard, TargetUserID, TargetRoomCard, TradeType, TradeRemark, RoomID, ClientIP, CollectDate)
		--VALUES (@UserID, 0, @GrantRoomCardCount, 0, 0, 1, 'ע�����ͷ���', '', @strClientIP, GETDATE())

	END
	--	SET @strErrorDescribe=N'���������ԭ��3��'
	--RETURN 1
	----------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- ��ѯ��Ϸ��
	SELECT @Beans=Currency FROM RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID	

	-- ��ѯ����
	SELECT @RoomCard=RoomCard FROM RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard WHERE UserID=@UserID	
	IF @RoomCard IS null
	SET @RoomCard = 0

	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Beans IS NULL SET @Beans=0
	IF @Insure IS NULL SET @Insure=0    

	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID) SET @IsAgent=1

	-- ��������ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID = KindID FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@DynamicPass AS DynamicPass,@UnderWrite AS UnderWrite,
		@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS Ingot, @Beans AS Beans, @Experience AS Experience,
		@Score AS Score, @Insure AS Insure, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, @MemberOverDate AS MemberOverDate,
		@MoorMachine AS MoorMachine,0 AS InsureEnabled,@IsAgent AS IsAgent,@LockServerID AS LockServerID, @RoomCard  AS RoomCard, @wKindID AS KindID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------
