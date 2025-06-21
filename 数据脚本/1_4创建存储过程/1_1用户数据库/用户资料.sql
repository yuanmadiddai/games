
----------------------------------------------------------------------------------------------------

USE RYAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_QueryUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_QueryUserIndividual]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_ModifyUserIndividual]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_ModifyUserIndividual]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ѯ����
CREATE PROC GSP_GP_QueryUserIndividual
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @LogonPass AS NCHAR(32)

	-- ��ѯ�û�
	SELECT @LogonPass=LogonPass FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����û����벻��ȷ��������Ϣ��ѯʧ�ܣ�'
		RETURN 1
	END

	-- ��������
	DECLARE @UserID INT
	DECLARE @QQ NVARCHAR(16)
	DECLARE @EMail NVARCHAR(33)
	DECLARE @UserNote NVARCHAR(256)
	DECLARE @SeatPhone NVARCHAR(32)
	DECLARE @MobilePhone NVARCHAR(16)
	DECLARE @Compellation NVARCHAR(16)
	DECLARE @DwellingPlace NVARCHAR(128)
	DECLARE @PassPortID NVARCHAR(18)
	DECLARE	@Spreader NVARCHAR(31)
	
	SET @QQ=N''	
	SET @EMail=N''	
	SET @UserNote=N''	
	SET @SeatPhone=N''	
	SET @MobilePhone=N''	
	SET @Compellation=N''	
	SET @DwellingPlace=N''	
	SET @PassPortID=N''		
	SET @Spreader=N''

	-- ��ѯ�û�
	SELECT @UserID=UserID, @QQ=QQ, @EMail=EMail, @UserNote=UserNote, @SeatPhone=SeatPhone,@MobilePhone=MobilePhone, @DwellingPlace=DwellingPlace FROM IndividualDatum(NOLOCK) WHERE UserID=@dwUserID

	-- ��ѯ����
	DECLARE @SpreaderID INT	
	SELECT @Compellation=Compellation,@PassPortID = PassPortID,@SpreaderID = SpreaderID FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID
	
	-- ��ѯ�ƹ�
	SELECT @Spreader = GAMEID FROM AccountsInfo  WHERE UserID=@SpreaderID

	-- �����Ϣ
	SELECT @dwUserID AS UserID,
	       @Compellation AS Compellation,
	       @PassPortID AS PassPortID, 
	       @QQ AS QQ, 
	       @EMail AS EMail, 
	       @SeatPhone AS SeatPhone,
		   @MobilePhone AS MobilePhone,
		   @DwellingPlace AS DwellingPlace, 
		   @UserNote AS UserNote,
		   @Spreader AS Spreader

	RETURN 0

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GP_ModifyUserIndividual
	@dwUserID INT,								-- �û� I D
	@strPassword NCHAR(32),						-- �û�����
	@cbGender TINYINT,							-- �û��Ա�
	@strNickName NVARCHAR(32),					-- �û��ǳ�
	@strUnderWrite NVARCHAR(63),				-- ����ǩ��
	@strCompellation NVARCHAR(16),				-- ��ʵ����
	@strPassPortID NVARCHAR(18),				-- ֤������	
	@strQQ NVARCHAR(16),						-- Q Q ����
	@strEMail NVARCHAR(33),						-- �����ʵ�
	@strSeatPhone NVARCHAR(32),					-- �̶��绰
	@strMobilePhone NVARCHAR(16),				-- �ƶ��绰
	@strDwellingPlace NVARCHAR(128),			-- ��ϸ��ַ
	@strUserNote NVARCHAR(256),					-- �û���ע	
	@strSpreader NVARCHAR(31),					-- �Ƽ��ʺ�				
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	DECLARE @UserID INT
	DECLARE @NickName NVARCHAR(31)
	DECLARE @Nullity BIT
	DECLARE @StunDown BIT
	DECLARE @LogonPass AS NCHAR(32)
	-- ��ұ���
	DECLARE @SourceScore BIGINT

	-- ��ѯ�û�
	SELECT @UserID=UserID, @NickName=NickName, @LogonPass=LogonPass, @Nullity=Nullity, @StunDown=StunDown
	FROM AccountsInfo(NOLOCK) WHERE UserID=@dwUserID

	-- ��ѯ�û�
	IF @UserID IS NULL
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��ԣ�'
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
	
	-- �����ж�
	IF @LogonPass<>@strPassword
	BEGIN
		SET @strErrorDescribe=N'�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��'
		RETURN 3
	END

	-- Ч���ǳ�
	IF (SELECT COUNT(*) FROM ConfineContent(NOLOCK) WHERE CHARINDEX(String,@strNickName)>0)>0
	BEGIN
		SET @strErrorDescribe=N'�����������Ϸ�ǳ������������ַ�����������ǳ������ٴ��޸ģ�'
		RETURN 4
	END

	-- �����ж�
	IF EXISTS (SELECT UserID FROM AccountsInfo(NOLOCK) WHERE NickName=@strNickName AND UserID<>@UserID)
	BEGIN
		SET @strErrorDescribe=N'���ǳ��ѱ��������ʹ���ˣ�������ǳ������ٴ��޸ģ�'
		RETURN 4
	END
	
	-- ���ƹ�Ա
	DECLARE @SpreaderID INT	
	IF @strSpreader<>''
	BEGIN
		-- ���ƹ�Ա
		DECLARE @CurrSpreaderID INT
		SET @CurrSpreaderID = CONVERT(INT,@strSpreader)

		SELECT @SpreaderID=UserID FROM AccountsInfo(NOLOCK) WHERE GameID = @CurrSpreaderID 
		
		-- �������
		IF @SpreaderID IS NULL
		BEGIN
			SET @SpreaderID=0
			SET @strErrorDescribe=N'������д���ƹ�Ա�����ڻ�����д������������ύ��'
			RETURN 5
		END
		
		-- ��ѯ�ظ�
		
		IF EXISTS (SELECT SpreaderID FROM AccountsInfo WHERE @SpreaderID=SpreaderID and UserID=@dwUserID)
		BEGIN
			SET @SpreaderID=0
			SET @strErrorDescribe=N'�Ѿ��ύ�ƹ�Ա���ύʧ�ܣ�'
			RETURN 5
		END
		
	END
	ELSE SET @SpreaderID=0
	
	-- �ƹ����
	IF @SpreaderID<>0
	BEGIN
		DECLARE @RegisterGrantScore INT
		DECLARE @Note NVARCHAR(512)
		SET @Note = N'�ƹ�Ա'
		SELECT @RegisterGrantScore = RegisterGrantScore FROM RYTreasureDBLink.RYTreasureDB.dbo.GlobalSpreadInfo
		IF @RegisterGrantScore IS NULL
		BEGIN
			SET @RegisterGrantScore=5000
		END
		INSERT INTO RYTreasureDBLink.RYTreasureDB.dbo.RecordSpreadInfo(
			UserID,Score,TypeID,ChildrenID,CollectNote)
		VALUES(@SpreaderID,@RegisterGrantScore,1,@UserID,@Note)	
		
		-- �Ƽ���¼
		UPDATE AccountsInfo SET SpreaderID = @SpreaderID WHERE UserID=@dwUserID		
	END
	
	
	-- �޸�����
	UPDATE AccountsInfo SET NickName=@strNickName, UnderWrite=@strUnderWrite, Gender=@cbGender	WHERE UserID=@dwUserID	
		
	-- �ǳƼ�¼
	IF @NickName<>@strNickName
	BEGIN
		INSERT INTO RYRecordDBLink.RYRecordDB.dbo.RecordAccountsExpend(UserID,ReAccounts,ClientIP)
		VALUES(@dwUserID,@strNickName,@strClientIP)
	END

	-- �޸�����
	UPDATE IndividualDatum SET QQ=@strQQ, EMail=@strEMail, SeatPhone=@strSeatPhone,
		MobilePhone=@strMobilePhone, DwellingPlace=@strDwellingPlace, UserNote=@strUserNote WHERE UserID=@dwUserID
		
	-- �޸�����
	IF @@ROWCOUNT=0
	INSERT IndividualDatum (UserID, QQ, EMail, SeatPhone, MobilePhone, DwellingPlace, UserNote)
	VALUES (@dwUserID, @strQQ, @strEMail, @strSeatPhone, @strMobilePhone, @strDwellingPlace, @strUserNote)

	-- ������Ϣ
	IF @@ERROR=0 SET @strErrorDescribe=N'�������Ѿ����������������ϣ�'
		
	RETURN 0

END

RETURN 0

GO
