USE RYPlatformDB
GO

-- ��������
TRUNCATE TABLE TaskInfo

set IDENTITY_INSERT TaskInfo ON
insert TaskInfo(TaskID,TaskName,TaskDescription,TaskType,UserType,KindID,MatchID,Innings,StandardAwardGold,StandardAwardMedal,MemberAwardGold,MemberAwardMedal,TimeLimit,InputDate) values(	1	,	N'��������ʤ����'	,	N'��ʤӮ���'	,	4	,	3	,	200	,	0	,	1	,	1000	,	100	,	2000	,	200	,	1800	,	'2014-04-01 19:40:55.203'	)
insert TaskInfo(TaskID,TaskName,TaskDescription,TaskType,UserType,KindID,MatchID,Innings,StandardAwardGold,StandardAwardMedal,MemberAwardGold,MemberAwardMedal,TimeLimit,InputDate) values(	2	,	N'������1������'	,	N'������1�֣������������'	,	2	,	3	,	200	,	0	,	1	,	1000	,	100	,	2000	,	200	,	1800	,	'2016-01-14 15:11:00.000'	)
insert TaskInfo(TaskID,TaskName,TaskDescription,TaskType,UserType,KindID,MatchID,Innings,StandardAwardGold,StandardAwardMedal,MemberAwardGold,MemberAwardMedal,TimeLimit,InputDate) values(	3	,	N'������Ӯ1������'	,	N'������Ӯ1�֣������������'	,	1	,	3	,	200	,	0	,	1	,	1000	,	100	,	2000	,	200	,	1800	,	'2016-01-14 15:11:00.000'	)
insert TaskInfo(TaskID,TaskName,TaskDescription,TaskType,UserType,KindID,MatchID,Innings,StandardAwardGold,StandardAwardMedal,MemberAwardGold,MemberAwardMedal,TimeLimit,InputDate) values(	4	,	N'թ��'	,	N'��թ����Ϸ�����������Ϸ'	,	1	,	3	,	6	,	0	,	2	,	10000	,	20	,	10	,	1200	,	500	,	'2016-02-26 14:36:22.003'	)
insert TaskInfo(TaskID,TaskName,TaskDescription,TaskType,UserType,KindID,MatchID,Innings,StandardAwardGold,StandardAwardMedal,MemberAwardGold,MemberAwardMedal,TimeLimit,InputDate) values(	5	,	N'թ��Ӯ����'	,	N'��թ����Ϸ��ʤ����'	,	1	,	3	,	6	,	0	,	2	,	20000	,	10	,	200000	,	20	,	600	,	'2016-02-26 14:37:27.847'	)

set IDENTITY_INSERT TaskInfo OFF