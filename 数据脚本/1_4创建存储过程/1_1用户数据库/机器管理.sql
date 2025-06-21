
----------------------------------------------------------------------------------------------------

USE RYAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidGetParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidGetParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidAddParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidAddParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidModifyParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidModifyParameter]
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_AndroidDeleteParameter]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_AndroidDeleteParameter]
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ��ȡ����
CREATE PROC GSP_GP_AndroidGetParameter
	@wServerID INT								-- �����ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE ServerID=@wServerID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ��Ӳ���
CREATE PROC GSP_GP_AndroidAddParameter
	@wServerID INT,								-- �����ʶ
	@dwServiceMode INT,							-- ����ģʽ
	@dwAndroidCount INT,						-- ������Ŀ
	@dwEnterTime INT,							-- ����ʱ��
	@dwLeaveTime INT,							-- �뿪ʱ��
	@dwEnterMinInterval INT,					-- ������
	@dwEnterMaxInterval INT,					-- ������
	@dwLeaveMinInterval	INT,					-- �뿪���
	@dwLeaveMaxInterval	INT,					-- �뿪���
	@lTakeMinScore	BIGINT,						-- Я������
	@lTakeMaxScore BIGINT,						-- Я������
	@dwSwitchMinInnings INT,					-- ��������
	@dwSwitchMaxInnings INT,					-- ��������
	@dwAndroidCountMember0 INT,					-- ��ͨ��Ա
	@dwAndroidCountMember1 INT,					-- һ����Ա
	@dwAndroidCountMember2 INT,					-- ������Ա
	@dwAndroidCountMember3 INT,					-- ������Ա	
	@dwAndroidCountMember4 INT,					-- �ļ���Ա		
	@dwAndroidCountMember5 INT					-- �弶��Ա			
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������
	INSERT AndroidConfigure(ServerID,ServiceMode,AndroidCount,EnterTime,LeaveTime,EnterMinInterval,EnterMaxInterval,LeaveMinInterval,
			LeaveMaxInterval,TakeMinScore,TakeMaxScore,SwitchMinInnings,SwitchMaxInnings,AndroidCountMember0,AndroidCountMember1,AndroidCountMember2
           ,AndroidCountMember3,AndroidCountMember4,AndroidCountMember5)
	VALUES(@wServerID,@dwServiceMode,@dwAndroidCount,@dwEnterTime,@dwLeaveTime,@dwEnterMinInterval,@dwEnterMaxInterval,@dwLeaveMinInterval,
			@dwLeaveMaxInterval,@lTakeMinScore,@lTakeMaxScore,@dwSwitchMinInnings,@dwSwitchMaxInnings,@dwAndroidCountMember0,@dwAndroidCountMember1,@dwAndroidCountMember2
           ,@dwAndroidCountMember3,@dwAndroidCountMember4,@dwAndroidCountMember5)

	-- ��ѯ����	
	DECLARE @dwBatchID INT
	SET @dwBatchID=SCOPE_IDENTITY()
	
	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE BatchID=@dwBatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------


-- �޸Ĳ���
CREATE PROC GSP_GP_AndroidModifyParameter
	@dwDatchID INT,								-- ���α�ʶ
	@dwServiceMode INT,							-- ����ģʽ
	@dwAndroidCount INT,						-- ������Ŀ
	@dwEnterTime INT,							-- ����ʱ��
	@dwLeaveTime INT,							-- �뿪ʱ��
	@dwEnterMinInterval INT,					-- ������
	@dwEnterMaxInterval INT,					-- ������
	@dwLeaveMinInterval	INT,					-- �뿪���
	@dwLeaveMaxInterval	INT,					-- �뿪���
	@lTakeMinScore	BIGINT,						-- Я������
	@lTakeMaxScore	BIGINT,						-- Я������
	@dwSwitchMinInnings INT,					-- ��������
	@dwSwitchMaxInnings INT,					-- ��������
	@dwAndroidCountMember0 INT,					-- ��ͨ��Ա
	@dwAndroidCountMember1 INT,					-- һ����Ա
	@dwAndroidCountMember2 INT,					-- ������Ա
	@dwAndroidCountMember3 INT,					-- ������Ա	
	@dwAndroidCountMember4 INT,					-- �ļ���Ա		
	@dwAndroidCountMember5 INT					-- �弶��Ա			
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ���²���
	UPDATE AndroidConfigure SET ServiceMode=@dwServiceMode,AndroidCount=@dwAndroidCount,EnterTime=@dwEnterTime,LeaveTime=@dwLeaveTime,
		EnterMinInterval=@dwEnterMinInterval,EnterMaxInterval=@dwEnterMaxInterval,LeaveMinInterval=@dwLeaveMinInterval,
		LeaveMaxInterval=@dwLeaveMaxInterval,TakeMinScore=@lTakeMinScore,TakeMaxScore=@lTakeMaxScore,SwitchMinInnings=@dwSwitchMinInnings,
		SwitchMaxInnings=@dwSwitchMaxInnings,AndroidCountMember0=@dwAndroidCountMember0,AndroidCountMember1=@dwAndroidCountMember1,AndroidCountMember2=@dwAndroidCountMember2,
		AndroidCountMember3=@dwAndroidCountMember3,AndroidCountMember4=@dwAndroidCountMember4,AndroidCountMember5=@dwAndroidCountMember5
	WHERE BatchID=@dwDatchID
	
	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE BatchID=@dwDatchID

END

RETURN 0

GO

----------------------------------------------------------------------------------------------------

-- ɾ������
CREATE PROC GSP_GP_AndroidDeleteParameter
	@dwBatchID INT								-- ���α�ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��ѯ����
	SELECT * FROM AndroidConfigure WHERE BatchID=@dwBatchID

	-- ɾ������
	DELETE AndroidConfigure WHERE BatchID=@dwBatchID
END

RETURN 0

GO

----------------------------------------------------------------------------------------------------