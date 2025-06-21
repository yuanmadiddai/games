
----------------------------------------------------------------------------------------------------

USE RYPlatformDB
GO

----д˽�˷�������Ϣ
IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GR_WritePersonalRoomJoinInfo]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GR_WritePersonalRoomJoinInfo]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------------------
-- ��Ϸд��
CREATE PROC GSP_GR_WritePersonalRoomJoinInfo

	-- ϵͳ��Ϣ
	@dwUserID INT,								-- �û� I D
	@dwTableID INT,								-- �û� I D
	@szRoomID NVARCHAR(6)						-- ����ID
	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN
	
	-- �����ǰ������Ҳ�����
	DECLARE @dwExistUserID INT
	SELECT @dwExistUserID = UserID FROM PersonalRoomScoreInfo WHERE RoomID = @szRoomID AND UserID = @dwUserID

	IF @dwExistUserID IS NULL
	BEGIN
		INSERT INTO PersonalRoomScoreInfo(UserID, RoomID, Score, WinCount, LostCount, DrawCount, FleeCount, WriteTime) 
												VALUES (@dwUserID, @szRoomID, 0,0, 0, 0, 0, GETDATE())  
	END

END

RETURN 0

GO