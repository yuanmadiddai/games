
----------------------------------------------------------------------------------------------------

USE RYAccountsDB
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
	@wServerID	SMALLINT,						-- �����ʶ
	@dwBatchID	INT,							-- ���α�ʶ
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
	-- ����У��
	IF @wServerID=0 OR @dwBatchID=0 OR @dwAndroidCount=0 
	BEGIN
		RETURN 1
	END

	-- ��ʼ����
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED
	BEGIN TRAN

	-- ��������
	UPDATE AndroidLockInfo SET AndroidStatus=0,ServerID=0,BatchID=0 
	WHERE DATEDIFF(hh,LockDateTime,GetDate())>=12	

	-- ��ѯ����
	SELECT UserID,LogonPass INTO #TempAndroids FROM AndroidLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP (@dwAndroidCountMember0) UserID FROM AndroidLockInfo(NOLOCK) WHERE AndroidStatus=0 AND ServerID=0 AND MemberOrder=0 ORDER BY NEWID()) 
	

	-- ��ѯ����
	INSERT INTO #TempAndroids(UserID,LogonPass) SELECT UserID,LogonPass FROM AndroidLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP (@dwAndroidCountMember1) UserID FROM AndroidLockInfo(NOLOCK) WHERE AndroidStatus=0 AND ServerID=0 AND MemberOrder=1 ORDER BY NEWID()) 
	

	-- ��ѯ����
	INSERT INTO #TempAndroids(UserID,LogonPass) SELECT UserID,LogonPass FROM AndroidLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP (@dwAndroidCountMember2) UserID FROM AndroidLockInfo(NOLOCK) WHERE AndroidStatus=0 AND ServerID=0 AND MemberOrder=2 ORDER BY NEWID()) 
	
	
	-- ��ѯ����
	INSERT INTO #TempAndroids(UserID,LogonPass) SELECT UserID,LogonPass FROM AndroidLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP (@dwAndroidCountMember3) UserID FROM AndroidLockInfo(NOLOCK) WHERE AndroidStatus=0 AND ServerID=0 AND MemberOrder=3 ORDER BY NEWID()) 
	
	
	-- ��ѯ����
	INSERT INTO #TempAndroids(UserID,LogonPass) SELECT UserID,LogonPass FROM AndroidLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP (@dwAndroidCountMember4) UserID FROM AndroidLockInfo(NOLOCK) WHERE AndroidStatus=0 AND ServerID=0 AND MemberOrder=4 ORDER BY NEWID()) 
	

	-- ��ѯ����
	INSERT INTO #TempAndroids(UserID,LogonPass) SELECT UserID,LogonPass FROM AndroidLockInfo(NOLOCK)
	WHERE UserID IN (SELECT TOP (@dwAndroidCountMember5) UserID FROM AndroidLockInfo(NOLOCK) WHERE AndroidStatus=0 AND ServerID=0 AND MemberOrder=5 ORDER BY NEWID()) 
	
					
	-- ����״̬
	UPDATE AndroidLockInfo SET AndroidStatus=1,BatchID=@dwBatchID,ServerID=@wServerID,LockDateTime=GetDate() 
	FROM AndroidLockInfo a,#TempAndroids b WHERE a.UserID = b.UserID

	-- ��������
	COMMIT TRAN
	SET TRANSACTION ISOLATION LEVEL READ COMMITTED	

	-- ��ѯ����
	SELECT * FROM #TempAndroids

	-- ������ʱ��	
	IF OBJECT_ID('tempdb..#TempAndroids') IS NOT NULL DROP TABLE #TempAndroids
END

RETURN 0

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

	-- ����״̬
	IF @wBatchID<>0
	BEGIN
		UPDATE AndroidLockInfo SET AndroidStatus=0,ServerID=0,BatchID=0,LockDateTime=GetDate()  
		WHERE AndroidStatus<>0 AND BatchID=@wBatchID AND ServerID=@wServerID
	END
	ELSE
	BEGIN
		UPDATE AndroidLockInfo SET AndroidStatus=0,ServerID=0,BatchID=0,LockDateTime=GetDate()  
		WHERE AndroidStatus<>0 AND BatchID<>0 AND ServerID=@wServerID
	END

END

RETURN 0

----------------------------------------------------------------------------------------------------