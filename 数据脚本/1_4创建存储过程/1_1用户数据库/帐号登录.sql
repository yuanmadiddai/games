
----------------------------------------------------------------------------------------------------

USE RYAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_EfficacyAccounts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyAccounts]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyAccounts]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyOtherPlatform]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyOtherPlatform]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_EfficacyLogonVisitor]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_EfficacyLogonVisitor]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_MB_EfficacyLogonVisitor]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_MB_EfficacyLogonVisitor]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- �ʺŵ�¼
CREATE PROC GSP_GP_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- �û��ʺ�
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@nNeeValidateMBCard BIT,					-- �ܱ�У��
	@strPassPortID NVARCHAR(19),				-- ֤��У��
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @AgentID INT
DECLARE @PlayTimeCount INT

-- �Ƹ�����
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME
DECLARE @ProtectID INT
DECLARE @PasswordID INT

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND (EnjoinOverDate>GETDATE() OR EnjoinOverDate IS NULL)
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END
 
	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE	@MachineSerial NCHAR(32)
	DECLARe @strLastLogonIP NCHAR(15)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,@InsurePass=InsurePass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,@strLastLogonIP=LastLogonIP,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount,@PasswordID=PasswordID,@ProtectID=ProtectID,@AgentID=AgentID
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 1
	END	
	
	-- ��ѯ����
	IF EXISTS (SELECT * FROM AccountsAgent WHERE @UserID = UserID and Nullity=0)
	BEGIN
		SET @strErrorDescribe=N'���Ǵ������ʺţ����ɵ�¼��'
		RETURN 1
	END		
	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END

	-- �ܱ�У��
	IF @nNeeValidateMBCard=1 AND @PasswordID<>0
	BEGIN
		SELECT @PasswordID AS PasswordID
		RETURN 18
	END

	-- ֤��У��
	IF @strLastLogonIP <> @strClientIP
	BEGIN
		DECLARE @RegisterPassPortID NVARCHAR(19)
		SELECT @RegisterPassPortID=PassportID FROM AccountsProtect WHERE UserID=@UserID
		IF @strPassPortID <> @RegisterPassPortID
		BEGIN
			IF @strPassPortID<>N''
			BEGIN
				SET @strErrorDescribe=N'����������֤��������ע��ʱ��д�Ĳ�һ�£�'		
				RETURN 1		
			END

			RETURN 19
		END				
	END

	-- �̶�����
	IF @MoorMachine=2
	BEGIN
		SET @MoorMachine=1
		SET @strErrorDescribe=N'�����ʺųɹ�ʹ���˹̶�������¼���ܣ�'
		UPDATE AccountsInfo SET MoorMachine=@MoorMachine, LastLogonMachine=@strMachineID WHERE UserID=@UserID
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 AND @AgentID=0
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GlobalSpreadInfo WHERE ID=1
		IF @GrantTime IS NULL
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END	
		
		IF @GrantScore>0 AND @GrantTime>0 AND @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- ��ѯ��Ϸ��
	SELECT @Beans=Currency FROM RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID		
	
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @Beans IS NULL SET @Beans=0

	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--�ж�����Ƿ��ڷ����У�����ڷ����в����¶�̬����
	DECLARE @LockServerID INT
	SELECT @LockServerID = ServerID FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerID IS NOT NULL
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1,LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			LastLogonMachine=@strMachineID 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
			-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1,LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID 
		WHERE UserID=@UserID
	END

	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ��¼��־
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1
	
	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID and Nullity=0) SET @IsAgent=1
		
	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS Ingot, 
		@Experience AS Experience, @Score AS Score, @Insure AS Insure, @Beans AS Beans, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate, @MoorMachine AS MoorMachine, @PasswordID as PasswordID, @InsureEnabled AS InsureEnabled, @PlatformID AS LogonMode,@IsAgent AS IsAgent
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ʺŵ�¼
CREATE PROC GSP_MB_EfficacyAccounts
	@strAccounts NVARCHAR(31),					-- �û��ʺ�
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT

-- �Ƹ�����
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)
DECLARE @RoomCard BIGINT

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN
	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@MachineSerial NCHAR(32)	
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,@InsurePass=InsurePass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine,@MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount
	FROM AccountsInfo(NOLOCK) WHERE Accounts=@strAccounts

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 1
	END	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END
		
	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID

	-- ��ѯ��Ϸ��
	SELECT @Beans=Currency FROM RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID		
	
	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @Beans IS NULL SET @Beans=0

	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

		--�ж�����Ƿ��ڷ����У�����ڷ����в����¶�̬����
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END


	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ��¼��־
	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1
	
	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID and Nullity=0) SET @IsAgent=1
	
	-- ��������ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0

	-- ��ȡ��������
	SET @RoomCard = 0
	SELECT @RoomCard=RoomCard FROM RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard WHERE UserID=@UserID
	IF @RoomCard IS null
	SET @RoomCard = 0

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS Ingot,		
		@Experience AS Experience,@Score AS Score, @Insure AS Insure, @Beans AS Beans, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, @PlatformID AS LogonMode ,@IsAgent AS IsAgent,
		@LockServerID AS LockServerID, @RoomCard AS RoomCard, @wKindID AS KindID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������¼
CREATE PROC GSP_MB_EfficacyOtherPlatform
	@cbPlatformID TINYINT,						-- ƽ̨���
	@strUserUin NVARCHAR(32),			        -- �û�Uin
	@strNickName NVARCHAR(32),					-- �û��ǳ�
	@strCompellation NVARCHAR(16),				-- ��ʵ����
	@cbGender TINYINT,							-- �û��Ա�
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN

	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@MachineSerial NCHAR(32)
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,	@InsurePass=InsurePass,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount
	FROM AccountsInfo(NOLOCK) WHERE UserUin=@strUserUin AND PlatformID=@cbPlatformID

    DECLARE @strTempName NVARCHAR(31)
	-- ע���û�
	IF @UserID IS NULL
	BEGIN
	
		-- ����˺�
		SET @strTempName=@strNickName+CONVERT(nvarchar(8),REPLACE(newid(),'-',''))
				
		-- ��ѯ�ǳ�
		IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strTempName)
		BEGIN
			SET @strErrorDescribe=N'���ǳ��ѱ�ע�ᣬ�뻻��һ�ǳƳ����ٴ�ע�ᣡ'
			RETURN 30
		END
		
		-- �����˺�
		DECLARE @strTemp NVARCHAR(31)
		SET @strTemp=CONVERT(NVARCHAR(31),REPLACE(NEWID(),'-','_'))
		
		-- ע���û�
		INSERT AccountsInfo (Accounts,NickName,RegAccounts,PlatformID,UserUin,LogonPass,InsurePass,Gender,FaceID,
			GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine)
		VALUES (@strTemp,@strTempName,@strTemp,@cbPlatformID,@strUserUin,N'd1fd5495e7b727081497cfce780b6456',N'',0,0,
			1,@strClientIP,N'',@strMachineID,@strClientIP,N'',@strMachineID)
		
		-- ��¼��־
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

		-- ��ѯ�û�
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
			@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
			@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
			@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount
		FROM AccountsInfo(NOLOCK) WHERE UserUin=@strUserUin AND PlatformID=@cbPlatformID

		-- �����ʶ
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			SET @GameID=0
			SET @strErrorDescribe=N'�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡'
		END
		ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

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
			SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
		
			-- �����ж�
			IF @GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount
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
				INSERT SystemGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, N'', @GrantScoreCount, 1)
			END
			
			-- ��ѯ���
			DECLARE @CurrScore BIGINT
			DECLARE @CurrInsure BIGINT
			DECLARE @CurrMedal INT
			SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo  WHERE UserID=@UserID
			SELECT @CurrMedal=UserMedal FROM AccountsInfo WHERE UserID=@UserID
			
			IF @CurrScore IS NULL SET @CurrScore=0
			IF @CurrInsure IS NULL SET @CurrInsure=0
			IF @CurrMedal IS NULL SET @CurrMedal=0	
			
			-- ���ͽ��
			INSERT RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP) VALUES (@UserID, @GrantScoreCount, @strClientIP, @strClientIP) 
			
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

		-- ���ͷ���
		DECLARE @GrantRoomCardCount AS BIGINT
		SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantRoomCardCount'
		INSERT RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard (UserID, RoomCard) VALUES (@UserID, @GrantRoomCardCount) 
		-- �����仯��¼
		--INSERT INTO RYRecordDB..RecordRoomCard(SourceUserID, SourceRoomCard, RoomCard, TargetUserID, TargetRoomCard, TradeType, TradeRemark, RoomID, ClientIP, CollectDate)
		--VALUES (@UserID, 0, @GrantRoomCardCount, 0, 0, 1, 'ע�����ͷ���', '', @strClientIP, GETDATE())
		----------------------------------------------------------------------------------------------------
		----------------------------------------------------------------------------------------------------

	END	
	ELSE
	BEGIN		

	    DECLARE @strRealNickName NVARCHAR(31)
	    SET @strRealNickName = SUBSTRING(@NickName,0,LEN(@NickName)-7)
		-- �޸��ǳ�
		IF @strRealNickName<>@strNickName
		BEGIN
			-- ����˺�
			
			SET @strTempName=@strNickName+CONVERT(NVARCHAR(8),REPLACE(newid(),'-',''))	
				
			-- ��ѯ�ǳ�
			IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strTempName)
			BEGIN
				SET @strErrorDescribe=N'���ǳ��ѱ�ʹ�ã��뻻��һ�ǳ��ٴγ��Ե�¼��'
				RETURN 30
			END
			ELSE
			BEGIN
				SET @NickName=@strTempName
				UPDATE AccountsInfo SET NickName=@strTempName WHERE UserID=@UserID
			END
		END
	END
	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	
	-- ��ѯ��Ϸ��
	SELECT @Beans=Currency FROM RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID		

	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @Beans IS NULL SET @Beans=0

	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--�ж�����Ƿ��ڷ����У�����ڷ����в����¶�̬����
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP, LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END


	-- ��¼��־
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1

	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID and Nullity=0) SET @IsAgent=1
	
	-- ��������ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0

	DECLARE @RoomCard BIGINT
	SET @RoomCard = 0
	SELECT @RoomCard=RoomCard FROM RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard WHERE UserID=@UserID

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS Ingot, 
		@Experience AS Experience,@Score AS Score, @Insure AS Insure, @Beans AS Beans, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, @PlatformID AS LogonMode,@IsAgent AS IsAgent ,@IsAgent AS IsAgent,
		@RoomCard AS RoomCard,@LockServerID AS LockServerID, @wKindID AS KindID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ο͵�¼
CREATE PROC GSP_GP_EfficacyLogonVisitor
	@cbLogonMode TINYINT,						-- ��½��ʽ
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN

	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,	@InsurePass=InsurePass,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount
	FROM AccountsInfo(NOLOCK) WHERE RegisterMachine=@strMachineID AND PlatformID=@cbLogonMode

	-- ע���û�
	IF @UserID IS NULL
	BEGIN	

		-- ��ʱ�˺�
		DECLARE @strTemp NVARCHAR(31)
		SET @strTemp=CONVERT(NVARCHAR(31),REPLACE(NEWID(),'-','_'))
		
		-- ע���û�
		INSERT AccountsInfo (Accounts,NickName,RegAccounts,PlatformID,UserUin,LogonPass,InsurePass,Gender,FaceID,
			GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine)
		VALUES (@strTemp,@strTemp,@strTemp,@cbLogonMode,@strMachineID,N'd1fd5495e7b727081497cfce780b6456',N'',0,0,
			1,@strClientIP,N'',@strMachineID,@strClientIP,N'',@strMachineID)

		-- ����˺�
		DECLARE @MaxUserID INT
		SELECT @MaxUserID=SCOPE_IDENTITY()
		DECLARE @strTempName NVARCHAR(31)
		SET @strTempName=N'�ο�'+CONVERT(NVARCHAR(31),@MaxUserID)
		
		--�����˺��ǳ�
		UPDATE AccountsInfo SET Accounts=@strTempName,NickName=@strTempName,RegAccounts=@strTempName WHERE UserID=@MaxUserID

		-- ��¼��־
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

		-- ��ѯ�û�
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
			@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
			@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
			@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount
		FROM AccountsInfo(NOLOCK) WHERE RegisterMachine=@strMachineID AND PlatformID=@cbLogonMode

		-- �����ʶ
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			SET @GameID=0
			SET @strErrorDescribe=N'�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡'
		END
		ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

		-- ���ͷ���
		DECLARE @GrantRoomCardCount AS BIGINT
		SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantRoomCardCount'
		INSERT RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard (UserID, RoomCard) VALUES (@UserID, @GrantRoomCardCount) 
		-- �����仯��¼
		--INSERT INTO RYRecordDB..RecordRoomCard(SourceUserID, SourceRoomCard, RoomCard, TargetUserID, TargetRoomCard, TradeType, TradeRemark, RoomID, ClientIP, CollectDate)
		--VALUES (@UserID, 0, @GrantRoomCardCount, 0, 0, 1, 'ע�����ͷ���', '', @strClientIP, GETDATE())

--		----------------------------------------------------------------------------------------------------
--		----------------------------------------------------------------------------------------------------
--		-- ע������
--
--		-- ��ȡ����
--		DECLARE @GrantIPCount AS BIGINT
--		DECLARE @GrantScoreCount AS BIGINT
--		SELECT @GrantIPCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantIPCount'
--		SELECT @GrantScoreCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantScoreCount'
--
--		-- ��������
--		IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0 AND @GrantIPCount IS NOT NULL AND @GrantIPCount>0
--		BEGIN
--			-- ���ʹ���
--			DECLARE @GrantCount AS BIGINT
--			SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
--		
--			-- �����ж�
--			IF @GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount
--			BEGIN
--				SET @GrantScoreCount=0
--			END
--		END
--
--		-- ���ͽ��
--		IF @GrantScoreCount IS NOT NULL AND @GrantScoreCount>0
--		BEGIN
--			-- ���¼�¼
--			UPDATE SystemGrantCount SET GrantScore=GrantScore+@GrantScoreCount, GrantCount=GrantCount+1 WHERE DateID=@DateID AND RegisterIP=@strClientIP
--
--			-- �����¼
--			IF @@ROWCOUNT=0
--			BEGIN
--				INSERT SystemGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, N'', @GrantScoreCount, 1)
--			END
--
--			-- ���ͽ��
--			INSERT RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP) VALUES (@UserID, @GrantScoreCount, @strClientIP, @strClientIP) 
--		END
--
--		----------------------------------------------------------------------------------------------------
--		----------------------------------------------------------------------------------------------------

	END	
	

	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine <> 0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ���ֻ��ն˽��е�¼��'
		RETURN 1
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	
	-- ��ѯ��Ϸ��
	SELECT @Beans=Currency FROM RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID		

	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @Beans IS NULL SET @Beans=0

	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--�ж�����Ƿ��ڷ����У�����ڷ����в����¶�̬����
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID
		WHERE UserID=@UserID
	END


	-- ��¼��־
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1

	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID and Nullity=0) SET @IsAgent=1
	
	DECLARE @RoomCard BIGINT
	SET @RoomCard = 0
	SELECT @RoomCard=RoomCard FROM RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard WHERE UserID=@UserID

	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS Ingot, 
		@Experience AS Experience,@Score AS Score, @Insure AS Insure, @Beans AS Beans, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, @cbLogonMode AS LogonMode ,@IsAgent AS IsAgent,@RoomCard AS RoomCard

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- �ο͵�¼
CREATE PROC GSP_MB_EfficacyLogonVisitor
	@cbLogonMode TINYINT,						-- ��½��ʽ
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strMachineID NVARCHAR(32),					-- ������ʶ
	@strMobilePhone NVARCHAR(11),				-- �ֻ�����
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ������Ϣ
DECLARE @UserID INT
DECLARE @CustomID INT
DECLARE @FaceID SMALLINT
DECLARE @Accounts NVARCHAR(31)
DECLARE @NickName NVARCHAR(31)
DECLARE @PlatformID TINYINT
DECLARE @UserUin NVARCHAR(32)
DECLARE @UnderWrite NVARCHAR(63)
DECLARE @SpreaderID INT
DECLARE @PlayTimeCount INT

-- ���ֱ���
DECLARE @Score BIGINT
DECLARE @Insure BIGINT
DECLARE @Beans decimal(18, 2)

-- ��չ��Ϣ
DECLARE @GameID INT
DECLARE @Gender TINYINT
DECLARE @UserMedal INT
DECLARE @Experience INT
DECLARE @LoveLiness INT
DECLARE @MemberOrder SMALLINT
DECLARE @MemberOverDate DATETIME
DECLARE @MemberSwitchDate DATETIME

-- ��������
DECLARE @EnjoinLogon AS INT

-- ִ���߼�
BEGIN

	-- ϵͳ��ͣ
	SELECT @EnjoinLogon=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SELECT @strErrorDescribe=StatusString FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'EnjoinLogon'
		RETURN 2
	END

	-- Ч���ַ
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineAddress(NOLOCK) WHERE AddrString=@strClientIP AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�������ڵ� IP ��ַ�ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 4
	END
	
	-- Ч�����
	SELECT @EnjoinLogon=EnjoinLogon FROM ConfineMachine(NOLOCK) WHERE MachineSerial=@strMachineID AND GETDATE()<EnjoinOverDate
	IF @EnjoinLogon IS NOT NULL AND @EnjoinLogon<>0
	BEGIN
		SET @strErrorDescribe=N'��Ǹ��֪ͨ����ϵͳ��ֹ�����Ļ����ĵ�¼���ܣ�����ϵ�ͻ����������˽���ϸ�����'
		RETURN 7
	END

	DECLARE @DateID INT
	SET @DateID=CAST(CAST(GETDATE() AS FLOAT) AS INT)

	-- ��ѯ�û�
	DECLARE @Nullity TINYINT
	DECLARE @StunDown TINYINT
	DECLARE @LogonPass AS NCHAR(32)
	DECLARE @InsurePass AS NCHAR(32)
	DECLARE @MoorMachine AS TINYINT
	DECLARE	@MachineSerial NCHAR(32)
	SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @PlatformID=PlatformID, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass,
		@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,	@InsurePass=InsurePass,
		@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
		@MoorMachine=MoorMachine, @MachineSerial=LastLogonMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount
	FROM AccountsInfo(NOLOCK) WHERE RegisterMachine=@strMachineID AND PlatformID=@cbLogonMode

	-- ע���û�
	IF @UserID IS NULL
	BEGIN	

		-- ��ʱ�˺�
		DECLARE @strTemp NVARCHAR(31)
		SET @strTemp=CONVERT(NVARCHAR(31),REPLACE(NEWID(),'-','_'))
		
		-- ע���û�
		INSERT AccountsInfo (Accounts,NickName,RegAccounts,PlatformID,UserUin,LogonPass,InsurePass,Gender,FaceID,
			GameLogonTimes,LastLogonIP,LastLogonMobile,LastLogonMachine,RegisterIP,RegisterMobile,RegisterMachine)
		VALUES (@strTemp,@strTemp,@strTemp,@cbLogonMode,@strMachineID,N'd1fd5495e7b727081497cfce780b6456',N'',0,0,
			1,@strClientIP,N'',@strMachineID,@strClientIP,N'',@strMachineID)

		-- ����˺�
		DECLARE @MaxUserID INT
		SELECT @MaxUserID=SCOPE_IDENTITY()
		DECLARE @strTempName NVARCHAR(31)
		SET @strTempName=N'�ο�'+CONVERT(NVARCHAR(31),@MaxUserID)
		
		--�����˺��ǳ�
		UPDATE AccountsInfo SET Accounts=@strTempName,NickName=@strTempName,RegAccounts=@strTempName WHERE UserID=@MaxUserID

		-- ��¼��־
		UPDATE SystemStreamInfo SET GameRegisterSuccess=GameRegisterSuccess+1 WHERE DateID=@DateID
		IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameRegisterSuccess) VALUES (@DateID, 1)

		-- ��ѯ�û�
		SELECT @UserID=UserID, @GameID=GameID, @Accounts=Accounts, @NickName=NickName, @UserUin=UserUin, @UnderWrite=UnderWrite, @LogonPass=LogonPass, @InsurePass=InsurePass,
			@FaceID=FaceID, @CustomID=CustomID, @Gender=Gender, @Nullity=Nullity, @StunDown=StunDown, @UserMedal=UserMedal, @Experience=Experience,
			@LoveLiness=LoveLiness, @MemberOrder=MemberOrder, @MemberOverDate=MemberOverDate, @MemberSwitchDate=MemberSwitchDate,
			@MoorMachine=MoorMachine,@SpreaderID=SpreaderID,@PlayTimeCount=PlayTimeCount
		FROM AccountsInfo(NOLOCK) WHERE RegisterMachine=@strMachineID AND PlatformID=@cbLogonMode

		-- �����ʶ
		SELECT @GameID=GameID FROM GameIdentifier(NOLOCK) WHERE UserID=@UserID
		IF @GameID IS NULL 
		BEGIN
			SET @GameID=0
			SET @strErrorDescribe=N'�û�ע��ɹ�����δ�ɹ���ȡ��Ϸ ID ���룬ϵͳ�Ժ󽫸������䣡'
		END
		ELSE UPDATE AccountsInfo SET GameID=@GameID WHERE UserID=@UserID

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
			SELECT @GrantCount=GrantCount FROM SystemGrantCount(NOLOCK) WHERE DateID=@DateID AND RegisterIP=@strClientIP
		
			-- �����ж�
			IF @GrantCount IS NOT NULL AND @GrantCount>=@GrantIPCount
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
				INSERT SystemGrantCount (DateID, RegisterIP, RegisterMachine, GrantScore, GrantCount) VALUES (@DateID, @strClientIP, N'', @GrantScoreCount, 1)
			END

			-- ��ѯ���
			DECLARE @CurrScore BIGINT
			DECLARE @CurrInsure BIGINT
			DECLARE @CurrMedal INT
			SELECT @CurrScore=Score,@CurrInsure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo  WHERE UserID=@UserID
			SELECT @CurrMedal=UserMedal FROM AccountsInfo WHERE UserID=@UserID
			
			IF @CurrScore IS NULL SET @CurrScore=0
			IF @CurrInsure IS NULL SET @CurrInsure=0
			IF @CurrMedal IS NULL SET @CurrMedal=0			
			
			-- ���ͽ��
			INSERT RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo (UserID, Score, RegisterIP, LastLogonIP) VALUES (@UserID, @GrantScoreCount, @strClientIP, @strClientIP) 

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

		-- ���ͷ���
		DECLARE @GrantRoomCardCount AS BIGINT
		SELECT @GrantRoomCardCount=StatusValue FROM SystemStatusInfo(NOLOCK) WHERE StatusName=N'GrantRoomCardCount'
		INSERT RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard (UserID, RoomCard) VALUES (@UserID, @GrantRoomCardCount) 
		-- �����仯��¼
		--INSERT INTO RYRecordDB..RecordRoomCard(SourceUserID, SourceRoomCard, RoomCard, TargetUserID, TargetRoomCard, TradeType, TradeRemark, RoomID, ClientIP, CollectDate)
		--VALUES (@UserID, 0, @GrantRoomCardCount, 0, 0, 1, 'ע�����ͷ���', '', @strClientIP, GETDATE())
		----------------------------------------------------------------------------------------------------
		----------------------------------------------------------------------------------------------------

	END	
	
	-- ����
	IF EXISTS(SELECT * FROM AccountsVisitor WHERE VisitorMachine=@strMachineID )
	BEGIN
		SET @strErrorDescribe=N'��¼ʧ�ܣ����ʺ��Ѿ��󶨣�'
		return 10
	END
	
	-- �ʺŽ�ֹ
	IF @Nullity<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ���ʱ���ڶ���״̬������ϵ�ͻ����������˽���ϸ�����'
		RETURN 2
	END	

	-- �ʺŹر�
	IF @StunDown<>0
	BEGIN
		SET @strErrorDescribe=N'�����ʺ�ʹ���˰�ȫ�رչ��ܣ��������¿�ͨ����ܼ���ʹ�ã�'
		RETURN 2
	END	
	
	-- �̶�����
	IF @MoorMachine=1
	BEGIN
		IF @MachineSerial<>@strMachineID
		BEGIN
			SET @strErrorDescribe=N'�����ʺ�ʹ�ù̶�������¼���ܣ�������ʹ�õĻ���������ָ���Ļ�����'
			RETURN 1
		END
	END

	-- �ƹ�Ա���
	IF @SpreaderID<>0 
	BEGIN
		DECLARE @GrantTime	INT
		DECLARE @GrantScore	BIGINT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'��Ϸʱ�����һ���Խ���'

		SELECT @GrantTime=PlayTimeCount,@GrantScore=PlayTimeGrantScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GlobalSpreadInfo
		WHERE ID=1
		IF @GrantTime IS NULL OR @GrantTime=0
		BEGIN
			SET @GrantTime = 108000 -- 30Сʱ
			SET @GrantScore = 200000
		END			
		IF @PlayTimeCount>=@GrantTime
		BEGIN
			-- ��ȡ�����Ϣ
			DECLARE @RecordID INT
			SELECT @RecordID=RecordID FROM RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo
			WHERE UserID = @SpreaderID AND ChildrenID = @UserID AND TypeID = 2
			IF @RecordID IS NULL
			BEGIN
				INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo(
					UserID,Score,TypeID,ChildrenID,CollectNote)
				VALUES(@SpreaderID,@GrantScore,2,@UserID,@Note)	
			END		
		END
	END

	-- ��ѯ���
	SELECT @Score=Score, @Insure=InsureScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreInfo WHERE UserID=@UserID
	
	-- ��ѯ��Ϸ��
	SELECT @Beans=Currency FROM RYTreasureDBLink.RYTreasureDB.dbo.UserCurrencyInfo WHERE UserID=@UserID		

	-- ���ݵ���
	IF @Score IS NULL SET @Score=0
	IF @Insure IS NULL SET @Insure=0
	IF @Beans IS NULL SET @Beans=0

	-- ��Ա�ȼ�
	IF @MemberOrder<>0 AND GETDATE()>@MemberSwitchDate
	BEGIN
		DECLARE @UserRight INT	
		SET @UserRight=0
		
		-- ɾ����Ա
		DELETE AccountsMember WHERE UserID=@UserID AND MemberOverDate<=GETDATE()

		-- ������Ա
		SELECT @MemberOverDate=MAX(MemberOverDate), @MemberOrder=MAX(MemberOrder), @MemberSwitchDate=MIN(MemberOverDate)
			FROM AccountsMember(NOLOCK) WHERE UserID=@UserID

		-- ���ݵ���
		IF @MemberOrder IS NULL 
		BEGIN
			SET @MemberOrder=0
			SET @UserRight=512
		END
		IF @MemberOverDate IS NULL SET @MemberOverDate='1980-1-1'
		IF @MemberSwitchDate IS NULL SET @MemberSwitchDate='1980-1-1'

		-- ��������
		UPDATE AccountsInfo SET MemberOrder=@MemberOrder, MemberOverDate=@MemberOverDate, MemberSwitchDate=@MemberSwitchDate,
			UserRight=UserRight&~@UserRight WHERE UserID=@UserID
	END

	--�ж�����Ƿ��ڷ����У�����ڷ����в����¶�̬����
	DECLARE @LockServerIDCheck INT
	SELECT @LockServerIDCheck = ServerID FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker WHERE UserID = @UserID
	IF @LockServerIDCheck IS NOT NULL
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END
	ELSE
	BEGIN
		-- ������Ϣ
		UPDATE AccountsInfo SET GameLogonTimes=GameLogonTimes+1, LastLogonDate=GETDATE(), LastLogonIP=@strClientIP,DynamicPassTime=GETDATE(),
			DynamicPass=CONVERT(nvarchar(32),REPLACE(newid(),'-','')),LastLogonMachine=@strMachineID, LastLogonMobile=@strMobilePhone 
		WHERE UserID=@UserID
	END


	-- ��¼��־
	UPDATE SystemStreamInfo SET GameLogonSuccess=GameLogonSuccess+1 WHERE DateID=@DateID
	IF @@ROWCOUNT=0 INSERT SystemStreamInfo (DateID, GameLogonSuccess) VALUES (@DateID, 1)

	-- ��̬����
	DECLARE @szDynamicPass  nchar(32)
	SELECT @szDynamicPass=DynamicPass FROM AccountsInfo WHERE UserID=@UserID

	-- ���б�ʶ
	DECLARE @InsureEnabled TINYINT
	SET @InsureEnabled=0
	IF @InsurePass <> '' SET @InsureEnabled=1

	-- �����ʶ
	DECLARE @IsAgent TINYINT
	SET @IsAgent =0
	IF EXISTS (SELECT * FROM AccountsAgent WHERE UserID=@UserID and Nullity=0) SET @IsAgent=1

	IF @strErrorDescribe IS NULL
	BEGIN
		SET @strErrorDescribe=N'�ο͵�¼�ɹ���'
	END

	-- ��������ID
	DECLARE @LockServerID INT
	DECLARE @wKindID INT
	SELECT @LockServerID=ServerID, @wKindID=KindID FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker WHERE UserID=@UserID
	IF @LockServerID IS NULL SET @LockServerID=0
	IF @wKindID IS NULL SET @wKindID=0

	DECLARE @RoomCard BIGINT
	SET @RoomCard = 0
	SELECT @RoomCard=RoomCard FROM RYTreasureDBLink.RYTreasureDB.dbo.UserRoomCard WHERE UserID=@UserID



	-- �������
	SELECT @UserID AS UserID, @GameID AS GameID, @Accounts AS Accounts, @NickName AS NickName,@szDynamicPass AS DynamicPass,
		@UnderWrite AS UnderWrite,@FaceID AS FaceID, @CustomID AS CustomID, @Gender AS Gender, @UserMedal AS Ingot, 
		@Experience AS Experience,@Score AS Score, @Insure AS Insure, @Beans AS Beans, @LoveLiness AS LoveLiness, @MemberOrder AS MemberOrder, 
		@MemberOverDate AS MemberOverDate,@MoorMachine AS MoorMachine, @InsureEnabled AS InsureEnabled, @cbLogonMode AS LogonMode ,@IsAgent AS IsAgent,
		@LockServerID AS LockServerID, @RoomCard AS RoomCard, @wKindID AS KindID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------