
----------------------------------------------------------------------------------------------------

USE RYAccountsDB
GO

IF EXISTS (SELECT * FROM DBO.SYSOBJECTS WHERE ID = OBJECT_ID(N'[dbo].[GSP_GP_LoadAgentGameKindItem]') and OBJECTPROPERTY(ID, N'IsProcedure') = 1)
DROP PROCEDURE [dbo].[GSP_GP_LoadAgentGameKindItem]
GO


SET QUOTED_IDENTIFIER ON 
GO

SET ANSI_NULLS ON 
GO



----------------------------------------------------------------------------------------------------

-- �����б�
CREATE PROC GSP_GP_LoadAgentGameKindItem
	@dwUserID INT,								-- �û���ʶ
	@dwDeviceID INT,							-- �豸��ʶ	
	@strErrorDescribe NVARCHAR(127) OUTPUT		-- �����Ϣ
WITH ENCRYPTION AS

-- ��������
SET NOCOUNT ON

-- ִ���߼�
BEGIN 

	-- ��ѯ��Ϣ
	DECLARE @AgentID INT
	SET @AgentID =0
	SELECT @AgentID = AgentID FROM AccountsAgent WHERE UserID IN (SELECT SpreaderID FROM AccountsInfo WHERE @dwUserID = UserID )
	
	SELECT KindID ,SortID FROM AccountsAgentGame(NOLOCK) WHERE AgentID = @AgentID AND DeviceID = @dwDeviceID
END

GO


