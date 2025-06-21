
----------------------------------------------------------------------------------------------------

USE RYGameScoreDB
GO


----д˽�˷�������Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WritePersonalGameScore]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WritePersonalGameScore]
GO



SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO


----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- ��Ϸд��
CREATE PROC GSP_GR_WritePersonalGameScore

	-- ϵͳ��Ϣ
	@dwUserID INT,								-- �û� I D
	@szRoomID NVARCHAR(6),						-- ����ID
	@dwDBQuestID INT,							-- �����ʶ
	@dwInoutIndex INT,							-- ��������

	-- ����ɼ�
	@lVariationScore BIGINT,					-- �û�����
	@lVariationGrade BIGINT,					-- �û��ɼ�
	@lVariationInsure BIGINT,					-- �û�����
	@lVariationRevenue BIGINT,					-- ��Ϸ˰��
	@lVariationWinCount INT,					-- ʤ������
	@lVariationLostCount INT,					-- ʧ������
	@lVariationDrawCount INT,					-- �;�����
	@lVariationFleeCount INT,					-- ������Ŀ
	@lVariationUserMedal INT,					-- �û�����
	@lVariationExperience INT,					-- �û�����
	@lVariationLoveLiness INT,					-- �û�����
	@dwVariationPlayTimeCount INT,				-- ��Ϸʱ��

	-- ������Ϣ
	@cbTaskForward TINYINT,						-- �������

	-- ������Ϣ
	@wKindID INT,								-- ��Ϸ I D
	@wServerID INT,								-- ���� I D
	@strClientIP NVARCHAR(15),					-- ���ӵ�ַ
	@dwRoomHostID INT,							-- ���� ID
	@dwPersonalTax INT							-- ˽�˷�˰��
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- �����ǰ������Ҳ�����
	DECLARE @dwExistUserID INT
	SELECT @dwExistUserID = UserID FROM RYPlatformDBLink.RYPlatformDB.dbo.PersonalRoomScoreInfo WHERE RoomID = @szRoomID AND UserID = @dwUserID

	IF @dwExistUserID IS NULL
	BEGIN
		INSERT INTO RYPlatformDB..PersonalRoomScoreInfo(UserID, RoomID, Score, WinCount, LostCount, DrawCount, FleeCount, WriteTime) 
												VALUES (@dwUserID, @szRoomID, @lVariationScore,@lVariationWinCount, @lVariationLostCount, @lVariationDrawCount, @lVariationFleeCount, GETDATE()) 
	END
	ELSE
	BEGIN
		-- �û�����
		UPDATE RYPlatformDBLink.RYPlatformDB.dbo.PersonalRoomScoreInfo SET Score=Score+@lVariationScore, WinCount=WinCount+@lVariationWinCount, LostCount=LostCount+@lVariationLostCount,
			DrawCount=DrawCount+@lVariationDrawCount, FleeCount=FleeCount+@lVariationFleeCount, WriteTime=GETDATE() 
		WHERE UserID=@dwUserID
	END

	--DECLARE @TaxCount INT
	--DECLARE @InsureScore BIGINT
	--DECLARE @SourceScore BIGINT
	--SELECT  @SourceScore = Score FROM GameScoreInfo WHERE UserID = @dwRoomHostID
	--SET @TaxCount = 0
	--IF @lVariationRevenue = 0
	--BEGIN
	--	IF @lVariationScore > 0
	--	BEGIN
	--		SET @TaxCount = @lVariationScore * @dwPersonalTax/1000
	--		UPDATE GameScoreInfo SET Score = Score + @TaxCount  WHERE UserID = @dwRoomHostID
	--	END
	--END
	--ELSE
	--BEGIN
	--	SET @TaxCount = @lVariationRevenue
	--	UPDATE GameScoreInfo SET Score = Score + @lVariationRevenue WHERE UserID = @dwRoomHostID
	--END

	-- ȫ����Ϣ
	IF @lVariationExperience>0 OR @lVariationLoveLiness<>0 OR @lVariationUserMedal>0
	BEGIN
		UPDATE RYAccountsDBLink.RYAccountsDB.dbo.AccountsInfo SET Experience=Experience+@lVariationExperience, LoveLiness=LoveLiness+@lVariationLoveLiness,
			UserMedal=UserMedal+@lVariationUserMedal
		WHERE UserID=@dwUserID
	END

	-- �������
	IF @cbTaskForward=1
	BEGIN
		exec RYPlatformDBLink.RYPlatformDB.dbo.GSP_GR_TaskForward @dwUserID,@wKindID,0,@lVariationWinCount,@lVariationLostCount,@lVariationDrawCount	
	END

END

RETURN 0

GO







