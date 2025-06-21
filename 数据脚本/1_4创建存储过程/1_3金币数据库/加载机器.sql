
----------------------------------------------------------------------------------------------------

USE RYTreasureDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_LoadAndroidUser') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_LoadAndroidUser
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'dbo.GSP_GR_UnLockAndroidUser') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE dbo.GSP_GR_UnLockAndroidUser
GO

SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO

----------------------------------------------------------------------------------------------------

-- ���ػ���
CREATE PROC GSP_GR_LoadAndroidUser
	@wServerID	SMALLINT,					-- �����ʶ
	@dwBatchID	INT,						-- ���α�ʶ
	@dwAndroidCount INT,						-- ������Ŀ
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
	DECLARE	@return_value int

	EXEC @return_value = RYAccountsDBLink.RYAccountsDB.dbo.GSP_GR_LoadAndroidUser
		 @wServerID = @wServerID,
		 @dwBatchID = @dwBatchID,
		 @dwAndroidCount = @dwAndroidCount,
		 @dwAndroidCountMember0=@dwAndroidCountMember0,
		 @dwAndroidCountMember1=@dwAndroidCountMember1,
		 @dwAndroidCountMember2=@dwAndroidCountMember2,
		 @dwAndroidCountMember3=@dwAndroidCountMember3,
		 @dwAndroidCountMember4=@dwAndroidCountMember4,
		 @dwAndroidCountMember5=@dwAndroidCountMember5
		 

	RETURN @return_value
END

GO

----------------------------------------------------------------------------------------------------

-- ��������
CREATE PROC GSP_GR_UnlockAndroidUser
	@wServerID	SMALLINT,					-- �����ʶ	
	@wBatchID	SMALLINT					-- ���α�ʶ	
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN

	-- ��������	
	DECLARE	@return_value int

	EXEC @return_value = RYAccountsDBLink.RYAccountsDB.dbo.GSP_GR_UnlockAndroidUser 
		 @wServerID=@wServerID,
		 @wBatchID=@wBatchID

	RETURN @return_value

END

GO
----------------------------------------------------------------------------------------------------