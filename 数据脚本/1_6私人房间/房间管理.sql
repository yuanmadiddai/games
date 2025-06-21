
----------------------------------------------------------------------------------------------------

USE RYPlatformDB
GO


IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_DeleteGameRoom]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_DeleteGameRoom]
GO



---����˽�˷������
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_InsertPersonalRoomParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_InsertPersonalRoomParameter]
GO

--�޸�˽�˷������
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GS_ModifyPersonalRoomParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GS_ModifyPersonalRoomParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_DissumePersonalRoom]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_DissumePersonalRoom]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE  PROCEDURE dbo.GSP_GS_DeleteGameRoom
	@wServerID INT,								-- �����ʶ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���ҷ���
	DECLARE @ServerID INT
	SELECT @ServerID=ServerID FROM GameRoomInfo(NOLOCK) WHERE ServerID=@wServerID

	-- ����ж�
	IF @ServerID IS NULL
	BEGIN
		SET @strErrorDescribe=N'��Ϸ���䲻���ڻ����Ѿ���ɾ���ˣ������޸�ʧ�ܣ�'
		RETURN 1
	END

	-- ɾ������
	DELETE GameRoomInfo WHERE ServerID=@wServerID

	-- ɾ��˽�˷������
	DELETE PersonalRoomInfo WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------------------

-- ���뷿��
CREATE  PROCEDURE dbo.GSP_GS_InsertPersonalRoomParameter

	-- ��������
	@wServerID INT,								-- �����ʶ
	@wKindID INT,								-- ����ID
	@cbCardOrBean TINYINT,						-- ʹ�÷���������Ϸ��
	@lFeeBeanOrRoomCard BIGINT,					-- ���ķ�������Ϸ��������
	@cbIsJoinGame TINYINT,						-- �Ƿ������Ϸ
	@cbMinPeople TINYINT,						-- ������Ϸ����С����
	@cbMaxPeople TINYINT,						-- ������Ϸ���������

	-- ��������
	@lPersonalRoomTax  BIGINT,					-- ˽�˷�����Ϸ˰��
	@lMaxCellScore BIGINT,						-- ���׷�
	@wCanCreateCount SMALLINT,					-- ���Դ�������󷿼���Ŀ
	@wPlayTurnCount INT,						-- �����ܹ�������Ϸ��������

	-- ��������
	@wPlayTimeLimit	INT,						-- �����ܹ�������Ϸ�����ʱ��
	@wTimeAfterBeginCount	INT,				-- һ����Ϸ��ʼ��೤ʱ����ɢ����
	@wTimeOffLineCount	INT,					-- ��ҵ��߶೤ʱ����ɢ����
	@wTimeNotBeginGame	INT,					-- ������º�೤ʱ���δ��ʼ��Ϸ��ɢ����
	@wTimeNotBeginAfterCreateRoom	INT,		-- ���䴴����೤ʱ���δ��ʼ��Ϸ��ɢ����
	@cbCreateRight	TINYINT,					-- ����˽�˷���Ȩ��
		-- �����Ϣ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ����˽�˷������
	INSERT INTO PersonalRoomInfo (ServerID,  KindID, CardOrBean, FeeBeanOrRoomCard, IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax, MaxCellScore, CanCreateCount, 
				PlayTurnCount, PlayTimeLimit, TimeAfterBeginCount, TimeOffLineCount,TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight)
	VALUES (@wServerID,@wKindID,  @cbCardOrBean, @lFeeBeanOrRoomCard, @cbIsJoinGame, @cbMinPeople, @cbMaxPeople, @lPersonalRoomTax, @lMaxCellScore, @wCanCreateCount, @wPlayTurnCount, 
	@wPlayTimeLimit, @wTimeAfterBeginCount, @wTimeOffLineCount, @wTimeNotBeginGame, @wTimeNotBeginAfterCreateRoom, @cbCreateRight)


END

RETURN 0

GO


----------------------------------------------------------------------------------------------------
-- �޸�˽�˷������
CREATE  PROCEDURE dbo.GSP_GS_ModifyPersonalRoomParameter

	-- ��������
	@wServerID INT,								-- �����ʶ
	@wKindID INT,								-- �����ʶ
	@cbCardOrBean TINYINT,						-- ʹ�÷���������Ϸ��
	@lFeeBeanOrRoomCard BIGINT,					-- ���ķ�������Ϸ��������
	@cbIsJoinGame TINYINT,						-- �Ƿ������Ϸ
	@cbMinPeople TINYINT,						-- ������Ϸ����С����
	@cbMaxPeople TINYINT,						-- ������Ϸ���������

	-- ��������
	@lPersonalRoomTax  BIGINT,					-- ��ʼ�η���
	@lMaxCellScore BIGINT,						-- ���׷�
	@wCanCreateCount int,						-- ���Դ�������󷿼���Ŀ
	@wPlayTurnCount INT,						-- �����ܹ�������Ϸ��������

	-- ��������
	@wPlayTimeLimit	INT,						-- �����ܹ�������Ϸ�����ʱ��
	@wTimeAfterBeginCount	INT,				-- һ����Ϸ��ʼ��೤ʱ����ɢ����
	@wTimeOffLineCount	INT,					-- ��ҵ��߶೤ʱ����ɢ����
	@wTimeNotBeginGame	INT,					-- ���䴴���೤ʱ���δ��ʼ��Ϸ��ɢ����
	@wTimeNotBeginAfterCreateRoom	INT,		-- ���䴴����೤ʱ���δ��ʼ��Ϸ��ɢ����
	@cbCreateRight	TINYINT,					-- ����˽�˷���Ȩ��

	-- �����Ϣ
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	DECLARE @dwServerID INT
	SELECT  @dwServerID = ServerID FROM PersonalRoomInfo WHERE KindID = @wKindID
	IF @dwServerID IS NULL
	BEGIN
		-- ����˽�˷������
		INSERT INTO PersonalRoomInfo (ServerID,  KindID, CardOrBean, FeeBeanOrRoomCard, IsJoinGame, MinPeople, MaxPeople, PersonalRoomTax, MaxCellScore, CanCreateCount, 
					PlayTurnCount, PlayTimeLimit, TimeAfterBeginCount, TimeOffLineCount,TimeNotBeginGame, TimeNotBeginAfterCreateRoom, CreateRight)
		VALUES (@wServerID,@wKindID,  @cbCardOrBean, @lFeeBeanOrRoomCard, @cbIsJoinGame, @cbMinPeople, @cbMaxPeople, @lPersonalRoomTax, @lMaxCellScore, @wCanCreateCount, @wPlayTurnCount, 
		@wPlayTimeLimit, @wTimeAfterBeginCount, @wTimeOffLineCount, @wTimeNotBeginGame, @wTimeNotBeginAfterCreateRoom, @cbCreateRight)
	END
	ELSE
	BEGIN
		--����˽�˷������
		UPDATE PersonalRoomInfo SET CardOrBean = @cbCardOrBean,FeeBeanOrRoomCard = @lFeeBeanOrRoomCard, IsJoinGame = @cbIsJoinGame, MinPeople = @cbMinPeople, MaxPeople = @cbMaxPeople,PersonalRoomTax = @lPersonalRoomTax, MaxCellScore = @lMaxCellScore,
									 PlayTurnCount = @wPlayTurnCount, PlayTimeLimit = @wPlayTimeLimit, TimeAfterBeginCount = @wTimeAfterBeginCount, CanCreateCount = @wCanCreateCount,
									TimeOffLineCount = @wTimeOffLineCount,TimeNotBeginGame = @wTimeNotBeginGame, TimeNotBeginAfterCreateRoom = @wTimeNotBeginAfterCreateRoom, CreateRight = @cbCreateRight
		WHERE KindID = @wKindID
	END

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- ��ɢ˽�˷�
CREATE PROC GSP_GR_DissumePersonalRoom
	@RoomID nvarchar(6),						-- ���� ID
	@lTaxCount varchar(MAX),
	@cbPersonalRoomInfo varbinary(MAX),
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN	
	DECLARE @tDissumDate datetime
	DECLARE @TaxRatio INT
	SET @TaxRatio = 0
	DECLARE @RoomTaxRatio INT
	DECLARE @ServerID INT
	DECLARE @TaxAgency	INT 
	SET @RoomTaxRatio = 0
	SET @ServerID = 0

	-- ���ʱ��,�����ʶ
	select @tDissumDate = DissumeDate, @ServerID = ServerID, @TaxAgency = TaxAgency from RYPlatformDB..StreamCreateTableFeeInfo where RoomID = @RoomID

	-- ��ȡ��Ӧ�����˰�ձ���
	select @RoomTaxRatio = RevenueRatio from RYPlatformDB..GameRoomInfo where ServerID = @ServerID

	if @TaxAgency is null
	begin
		set @TaxAgency = 0
	end

	if @RoomTaxRatio is null or @RoomTaxRatio = 0
	begin
		set @TaxAgency = 1
	end

	declare @TaxRevenue int
	set @TaxRevenue = 0
	if @RoomTaxRatio is null or @RoomTaxRatio = 0
	begin
		set @RoomTaxRatio = @lTaxCount
	end
	else
	begin
		set @RoomTaxRatio =  @lTaxCount * @TaxAgency/@RoomTaxRatio
	end


	-- д�봴����¼
	UPDATE  RYPlatformDB..StreamCreateTableFeeInfo   SET  DissumeDate = GETDATE(),TaxCount = @lTaxCount, RoomScoreInfo = @cbPersonalRoomInfo  WHERE RoomID = @RoomID




	if @tDissumDate is null
	SET  @tDissumDate = GETDATE()

	--����
	DECLARE @temp TABLE
	(
	UserID INT
	);

	-- ��Դ���е����ݲ��뵽�������
	INSERT INTO @temp(UserID)
	SELECT UserID FROM PersonalRoomScoreInfo WHERE RoomID = @RoomID
	ORDER BY UserID;

	-- ��������
	DECLARE
	@UserID AS INT,
	@firstname AS NVARCHAR(10),
	@lastname AS NVARCHAR(20);
     
	WHILE EXISTS(SELECT UserID FROM @temp)
	BEGIN
	-- Ҳ����ʹ��top 1
	SET ROWCOUNT 1
	SELECT @UserID= UserID FROM @temp;
	DELETE FROM RYTreasureDBLink.RYTreasureDB.dbo.GameScoreLocker where UserID = @UserID;
	SET ROWCOUNT 0 
	DELETE FROM @temp WHERE UserID=@UserID;
	END

	select @tDissumDate AS DissumeDate
												
END

RETURN 0
GO

-----------------------------------------------


